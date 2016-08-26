// cse425_lab3.cpp : Defines the entry point for the console application.
// Megan Bacani and Tina Li
#include "stdafx.h"
#include "Scanner.h"
#include "Token.h"
#include "Label.h"
#include "Number.h"
#include <iostream>
#include <string>
#include <istream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <vector>
#include <memory>
#include <set>
#include <list>
#include <map>
#include <ctime>
#include <cstdlib>

//enums for errors 
enum error_num{
	value_not_num = 1,
	ifsNotOpen = 2,
	ofsNotOpen = 3,
	isNotPred = 1,
	var_not_bound,
	cannot_resolve,
	incorrect_args,
	unknown_error
};
//variables to avoid hard coding 
string blankStr = "";
int numArgs = 2;
int success = 0;
int firstArg = 1;
int secondArg = 2;
int fileName = 0;
int zero = 0;
int two = 2;
unsigned int offset = 1;
int hcDisplayFull = 81;
int hcDisplayStart = 0;
int hcDisplayEnd = 81;
bool skip_all = false;
bool has_head = false;

using namespace std;

//global lists/maps/strings for symbol tables 
list<string> labels;
list<string> nums;
map<string, int> bound;
list<string> unbound;
vector<string> printhc;
//struct of predicate to store all the information that is now associated with a predicate so our data structures containing our predicates don't get out of control
struct Predicate{
	vector<Token> args;
	string name;
	bool isBody;
	Predicate(string n, vector<Token> a, bool b){
		args = a;
		name = n;
		isBody = b;
	}
};
multimap<int, Predicate> symbol_table; //map of a symbol table



//operator overloading of Predicate into ostream to make our lives easier and the code cleaner
ostream& operator<< (ostream& os, const Predicate & pred){
	os << "( " << pred.name << " ";
	for (Token arg : pred.args){
		os << arg.input << " ";
	}
	os << ")";
	return os;
}

//prints the horn clause with the display parameters put into the up, down, and initial arguments
int print_hc(){
	//there aren't that many hornclauses to display, these will adjust the end and start lines in the hornclause arguments
	if (hcDisplayEnd > (int) printhc.size() && hcDisplayStart < zero){
		hcDisplayEnd = printhc.size();
		hcDisplayStart = zero;
	}
	//if the end display index is too large
	if (hcDisplayEnd > (int) printhc.size()){
		hcDisplayEnd = printhc.size();
		if ((hcDisplayStart = printhc.size() - hcDisplayFull) < zero) hcDisplayStart = zero;
	}
	//if the beginning index is negative
	if (hcDisplayStart < zero){
		hcDisplayStart = zero;
		hcDisplayEnd = hcDisplayFull;
		if ((hcDisplayEnd = hcDisplayFull) > (int) printhc.size()) hcDisplayEnd = printhc.size();
	}
	//printing it out
	for (int i = hcDisplayStart; i < hcDisplayEnd; i++){
		cout << (i + offset) << ": " << printhc[i] << endl;
	}
	return success;
}

//method to find predicate in a file based on grammar given and prints out a correct predicate if found
int predicate(Scanner scan, shared_ptr<Token> & t, string & hc, int & counter){
	//variables used in method
	bool isCheckingArgs = false;
	bool isCheckingSym = false;
	string pred = blankStr;


	//checking for left parent of predicate
	if (t->token_type == Token::LEFTPAREN){
		//add the token to the predicate
		pred += t->input + " ";
		scan >> t;
		counter++;
		//check for a label or number as an the name
		if (t->token_type == Token::LABEL){

			pred += t->input + " ";
			isCheckingSym = true;
			//check for the args
			while (isCheckingSym){
				scan >> t;

				counter++;
				//checking if token was a label, and then what kind of label it is (bound, unbound)
				if (t->token_type == Token::LABEL){
					if (t->get_subtype() == Token::BOUND || t->get_subtype() == Token::UNBOUND){

						pred += t->input + " ";
					}
					else{
						return isNotPred;
					}
				} //checks if token is a number
				else if (t->token_type == Token::NUMBER){
					pred += t->input + " ";

				}
				else{
					isCheckingSym = false;
				}
			}
			//check for right paren to end predicate 
			if (t->token_type == Token::RIGHTPAREN){

				//add to pred and add pred to hc, return 1 to say the predicate was correct
				pred += t->input + " ";
				hc += pred;
				return success;
			}
			else{
				return isNotPred;
			}
		}
		//if there was not an argument, only found ( with no label following  
		else {
			if (t->token_type == Token::LEFTPAREN){
				skip_all = true;
			}
			return isNotPred;

		}
	}
	else{
		return isNotPred;
	}

	return isNotPred; //not predicate
}


//method parse that parses the string and looks for multiple predicates and 
int parse_body(Scanner scan, string output){
	//created vector to store all horn clauses
	bool foundPred = false;
	bool isCheckingPred = false;
	bool foundAnd = false;
	vector<string> hornclauses;
	ofstream ofs(output);

	
		shared_ptr<Token> t = make_shared<Token>();

		//booleans for parse method
		bool isFirst = true;
		bool moveOn = false;
		bool skip = false;
		bool skip_body = false;

		//checks if the ifs is still valid and not end of file
		while (scan.isValid()){
			int counter = zero;
			string hc = blankStr;
			//to check if its the first predicate or if moving on 
			if (isFirst || moveOn){
				scan >> t;
				counter++;
			}

			//setting variables
			int origCounter = counter;
			moveOn = false;
			isFirst = false;
			bool isHC = false;
			has_head = false;
			string witht = blankStr;
			string temp = "";
			//check for ( to start the HC

			if (t->token_type == Token::LEFTPAREN){
				temp += t->input + " ";
				if (!skip){ //keeps track of if you need to skip (i.e. in case of an incorrect HC and then a correct one following immediately after
					scan >> t;
				}
				else{
					skip = false;
				}
				counter++;


				isCheckingPred = true;
				while (isCheckingPred){

					//checks for an infinite number of predicates in body 
					if (predicate(scan, t, temp, counter))
					{
						isCheckingPred = false;
						skip_body = true;
						if (skip_all){
							if (has_head){
								skip_all = false;
							}
							else{ //skip if it doesn't have a head 
								skip = true;
							}
						}
					}
					else{ //if not a predicate, it had a head and start looking again 
						has_head = true;
						scan >> t;
						counter++;
					}
				}
 				if (!skip_all){ //check for new HC, look for ( first
					if (t->token_type == Token::LEFTPAREN){
						bool hasBody = false;
						temp += t->input + " ";
						isCheckingPred = true;
						hasBody = true;

						//continuously look for predicates 
						while (isCheckingPred){
							if (!skip_body){
								scan >> t;
							}
							else{
								skip_body = false;
							}

							counter++;

							shared_ptr<Token> tempscanned = t;
							//checks for an infinite number of predicates in body 
							if (predicate(scan, t, temp, counter))
							{
								isCheckingPred = false;
							}
						}
						//look for ) to end body
						if (t->token_type == Token::RIGHTPAREN){
							temp += t->input + " ";
							scan >> t;

							counter++;
							//look for ) to end HC
							if (t->token_type == Token::RIGHTPAREN){
								temp += t->input + " ";
							}
							else{
								hc = "failed";
							}
						}
						else{ //if the next token was a (, skip = true so you can start looking for a new HC 
							if (t->token_type == Token::LEFTPAREN){
								skip = true;
							}
							hc = "failed";
						}

					}
					else if (t->token_type == Token::RIGHTPAREN){
						temp += t->input + " ";
					}
					else{ //didn't get any part of a valid hornclause
						hc = "failed";
						if (t->token_type == Token::LEFTPAREN){
							skip = true;
						}
					}

				}
				else{ 
					hc = "failed";
				}
				skip_all = false;
			}
			else{
				hc = "failed";
				moveOn = true;
			}

			//checks if the HC didn't fail, then add it to the hc 
			if (hc.compare("failed") != zero){
				hc = temp;
			}
			else{
				hc = "";
			}
			//checking to see if anything was added to the hc variable
			if (hc.compare(blankStr) != zero){
				//push valid hc into vector
				hornclauses.push_back(hc);
				moveOn = true;
			}
			isFirst = false;
		}

		//open ofstream and write hornclauses to output file
		for (string hornclause : hornclauses){
			ofs << hornclause << endl;
		}
		return success;
}

//method parse that sees if you can parse from a file and looks for predicates by calling parse_body
int parse(string input, string output, bool isString){
	stringstream ss;
	ifstream ifs;
	//if input is an actual string to parse, read it as a string
	if (isString){
		ss = stringstream(input);
		Scanner scan(ss);
		scan.set_string(input);
		parse_body(scan, output);
	}
	//if input is a file to parse, open the file to read
	else{
		ifs = ifstream(input);
		if (ifs.is_open()){
			Scanner scan(ifs);
			parse_body(scan, output);
		}
		//if file doesn't open, then it's bad news for everyone
		else return ifsNotOpen;
	}
	//return 0 to indicate success 
	return success;
}



//helper method to sort labels and numbers lexically
int sort_hc(){
	//sorting both labels and nums
	labels.sort();
	nums.sort();
	unbound.sort();

	//make sure each is unique
	labels.unique();
	nums.unique();
	unbound.unique();




	//return success when lists have been correctly sorted and checked as unique 
	return success;
}

//method that compares the values of two tokens and returns true only if the number values are the same
bool compare_values(Token num, Token bnd){
	//comparing values of tokens 

	//converts tokens to ints 
	int value_num = stoi(num.input);
	int value_bnd = bound.find(bnd.input)->second;

	//return true if equal, false otherwise
	if (value_num == value_bnd){
		return true;
	}
	return false;
}

//method that makes a substitution across entire two vectors 
int replacer_subs_replacee(vector<string> & list_subs, Token replacer, Token replacee, vector<Token> & vec_replacer, vector<Token> & vec_replacee, vector<Token>::iterator it_replacer, vector<Token>::iterator it_replacee)
{

	list_subs.push_back(replacer.input + "/" + replacee.input);
	//makes substitution across the vector 
	for (it_replacee; it_replacee != vec_replacee.end(); it_replacee++){
		if ((*it_replacee).input.compare(replacee.input) == zero){
			(*it_replacee) = replacer;
		}
	}
	//make substitution across the other vector
	for (it_replacer; it_replacer != vec_replacer.end(); it_replacer++){
		if ((*it_replacer).input.compare(replacee.input) == zero){
			(*it_replacer) = replacer;
		}
	}

	//return success that it successfully completed 
	return success;
}

//checks to see if string is a number
int isNum(string s){
	return find_if_not(s.begin(), s.end(), (int(*)(int))isdigit) == s.end();
}

//checks to see if string is an unbound variable
bool isUnbound(string s){
	bool ans = true;

	//isn't unbound if the length isn't 1 and isn't uppercas
	if (s.length() == offset){
		if (!isupper(*s.begin())){
			ans = false;
		}
	}
	else{
		ans = false;
	}

	return ans;
}


//focused on adding the predicate and bound (with values set initially at 0)
int predicateOut(Scanner scan, shared_ptr<Token> & t, string & outhc, int hcCount, bool isBody)
{

	//variable for checking symbols 
	bool isCheckingSym = false;
	string symbolKey;
	vector<Token> symbolArgs;

	//checking for left paren to start predicate
	if (t->token_type == Token::LEFTPAREN){
		outhc += t->input + " ";
		scan >> t;

		//first token following a ( should be a label
		if (t->token_type == Token::LABEL)
		{
			outhc += t->input + " ";
			labels.push_back(t->input); //push back the label
			symbolKey = t->input;

			//don't need to put a left parent around symbols, so don't add anything if found LP
			isCheckingSym = true; //prep to start checking for symbols
			while (isCheckingSym)
			{
				scan >> t;

				//check for a label or number as a symbol
				if (t->token_type == Token::LABEL || t->token_type == Token::NUMBER)
				{
					outhc += t->input + " ";
					if (t->token_type == Token::LABEL){
						if (t->get_subtype() == Token::BOUND){
							if (bound.find(t->input) == bound.end()){
								bound.insert(pair<string, int>(t->input, zero));
							}
						}
						else{
							//push back into unbound
							unbound.push_back(t->input);
						}
						//push back to labels and symbolArgs table for reference later
						labels.push_back(t->input);
						symbolArgs.push_back(*t);
					}
					else {
						//push back into according symbol tables 
						nums.push_back(t->input);
						symbolArgs.push_back(*t);
					}
				}
				//if it wasn't a label or number, don't check for symbols anymore
				else
				{
					isCheckingSym = false;
				}
			}

			//already scanned, checked if it wasn’t a label, so it should be the end of the predicate which is a right paren 
			if (t->token_type == Token::RIGHTPAREN)
			{
				outhc += t->input + " ";
				//don't add right paren of token to close symbols 
				//insert into symbol table
				symbol_table.insert(pair<int, Predicate>(hcCount, Predicate(symbolKey, symbolArgs, isBody)));
				return success; //return value 0 for output predicate being correctly written 
			}
		}
	}
	//if wasn't valid hc, return isNotPred value; 
	return isNotPred;
}

//method to build symbol tables and the hornclause database and adds values to them 
int build_body(Scanner scan){
	shared_ptr<Token> t = make_shared<Token>();
	//checks if the ifs is still valid and not end of file
	while (scan.isValid()){
		string finalstr = "";
		bool foundBody = false;
		bool isBody = false;
		bool isHead = true;
		bool skip = true;
		scan >> t;
		//should start with LEFT Paren
		if (t->token_type == Token::LEFTPAREN){
			finalstr += t->input + " ";
			//EXTRA CREDIT checking for multiple predicates in the head 
			while (isHead){
				scan >> t;
				if (predicateOut(scan, t, finalstr, printhc.size() + offset, isBody)){
					isHead = false; //scanning for head
				}
			}
			//check for token right parent to follow the head, means there will be a body if found
			if (t->token_type == Token::LEFTPAREN){
				isBody = true;
				foundBody = true;
			}

			//checking for predicates in the body until there are no more predicates
			while (isBody){
				if (skip){
					skip = false;
				}
				else{
					scan >> t;
				}
				int pred = predicateOut(scan, t, finalstr, printhc.size() + offset, isBody);
				if (pred){
					isBody = false;
				}
			}
			//find the right parent of the body
			if (foundBody){
				if (t->token_type == Token::RIGHTPAREN){
					finalstr += t->input + " ";

				}
				scan >> t;
			}
			//find the right parent of the hornclause
			if (t->token_type == Token::RIGHTPAREN){
				finalstr += t->input + " ";
				printhc.push_back(finalstr);

			}
		}
	}


	//sort all of the labels and nums found in predicates 
	sort_hc();

	
	return success;
}

//determines whether the istream of scanner should be using a filestream or stringstream to read the input string based on the value of isString
int build(string input, bool isString){
	stringstream ss;
	ifstream ifs;
	//if input is an actual string to parse, read it as a string
	if (isString){
		ss = stringstream(input);
		Scanner scan(ss);
		scan.set_string(input);
		build_body(scan);
	}
	//if input is a file to parse, open the file to read
	else{
		ifs = ifstream(input);
		if (ifs.is_open()){
			Scanner scan(ifs);
			build_body(scan);
		}
		//if file doesn't open, then it's bad news for everyone
		else return ifsNotOpen;
	}

	return success;
}

//method that looks through a pair of vectors and substitutes based on instructions given in lab until vectors are unified
int substitution(vector<Token> first_args, vector<Token> second_args, vector<Predicate> first_preds, vector<Predicate> second_preds, vector<string> list_subs, string method, int index){

	//creating new vectors to put tokens into 
	vector<Token> first_vec, second_vec;

	//making a copy copy just to make sure they're copied. Better safe than sorry
	first_vec = first_args;
	second_vec = second_args;

	//iterators for our working copy
	vector<Token>::iterator it_first = first_vec.begin();
	vector<Token>::iterator it_second = second_vec.begin();


	//while loop to check for different constants in the same place(substitution should not happen at all if not_two_const is false)
	while (it_first != first_vec.end() && it_second != second_vec.end())
	{
		Token token_first = *it_first;
		Token token_compare = *it_second;

		//checks if both tokens are numbers
		if (token_first.token_type == Token::NUMBER){
			if (token_compare.token_type == Token::NUMBER){
				//if tokens aren't equal numbers, end
				if (token_compare.input.compare(token_first.input) != zero){
					return cannot_resolve; //two constants in the same place. why keep going
				}
			}
		}

		//increase iterators to keep stepping through vectors
		it_first++; it_second++;
	}


	//reset iterators to start at the front of the working copies to do substitutions 
	it_first = first_vec.begin();
	it_second = second_vec.begin();

	//loops to print out the two vectors prior to unifying them 
	

	//loop that compares vectors based on lab instructions to unify them 
	while (it_first != first_vec.end() && it_second != second_vec.end()){
		Token tfirst = *it_first;
		Token tsecond = *it_second;

		//makes a new token object for first and compare
		Token::token_kind first_kind, second_kind;

		//set the token kinds to their appropriate token because the bound/unbound tokens of the label token are stored elsewhere
		if (it_first->token_type == Token::LABEL)
			first_kind = it_first->get_subtype();
		else
			first_kind = it_first->token_type;
		if (it_second->token_type == Token::LABEL)
			second_kind = it_second->get_subtype();
		else
			second_kind = it_second->token_type;

		//vector pointers are incremented after every step unless they cannot be unified which returns an error message
		//first is unbound
		if (first_kind == Token::UNBOUND){
			//second is unbound meaning the first token will replace the second token if they're not the same
			if (second_kind == Token::UNBOUND){
				if (tfirst.input.compare(tsecond.input) != zero){
					replacer_subs_replacee(list_subs, tfirst, tsecond, first_vec, second_vec, it_first, it_second);
					it_first++; it_second++;
				}
				else{
					it_first++; it_second++;
				}
			}
			//second is unbound meaning the second token will replace the first token 
			else if (second_kind == Token::BOUND){
				replacer_subs_replacee(list_subs, tsecond, tfirst, second_vec, first_vec, it_second, it_first);
				it_first++; it_second++;
			}
			//second is a number meaning second will replace the first
			else{

				replacer_subs_replacee(list_subs, tsecond, tfirst, second_vec, first_vec, it_second, it_first);
				it_first++; it_second++;
			}
		}
		//first  is bound
		else if (first_kind == Token::BOUND){
			//second is bound meaning they cannot be unified
			if (second_kind == Token::BOUND){
				return cannot_resolve;
			}
			//second is unbound meaning first token will replace the second token
			else if (second_kind == Token::UNBOUND){
				replacer_subs_replacee(list_subs, tfirst, tsecond, first_vec, second_vec, it_first, it_second);
				it_first++; it_second++;
			}
			//second is number meaning second token will replace the first token if the value of the first token is equal to the value of the constant
			else{
				if (stoi(tsecond.input) == bound.find(tfirst.input)->second){
					replacer_subs_replacee(list_subs, tsecond, tfirst, second_vec, first_vec, it_second, it_first);
					it_first++; it_second++;
				}
				else{
					return cannot_resolve;
				}
			}
		}
		//first is number
		else {
			//second is bound meaning first will replace second if their values are the same
			if (second_kind == Token::BOUND){
				if (stoi(tfirst.input) == bound.find(tsecond.input)->second){
					replacer_subs_replacee(list_subs, tfirst, tsecond, first_vec, second_vec, it_first, it_second);
					it_first++; it_second++;
				}
				else{
					return cannot_resolve;
				}
			}
			//second is unbound meaning first will replace second
			else if (second_kind == Token::UNBOUND){
				replacer_subs_replacee(list_subs, tfirst, tsecond, first_vec, second_vec, it_first, it_second);
				it_first++; it_second++;
			}
			//second is number meaning if first and second values are the same, the vectors will move on 
			else{
				if (tfirst.input == tsecond.input){
					it_first++; it_second++;
				}
				else{
					return cannot_resolve;
				}
			}
		}

	}

	
	vector<string>::iterator ls;
	for (ls = list_subs.begin(); ls != list_subs.end(); ls++){
		string s = *ls;
		string delimiter = "/";
		string replacer = s.substr(zero, s.find(delimiter));
		string replacee = s.substr(s.find(delimiter) + offset, s.length());
		for (vector<Token>::iterator i = first_vec.begin(); i != first_vec.end(); i++){
			if ((*i).input.compare(replacee) == zero){
				//subs
				(*i).input = replacer;
			}
		}
	}
	


	//checking if first clause is fact and stores it to a vector to replace in the second clause
	bool fact = true;

	vector<Predicate> first_body;
	for (Predicate p : first_preds){
		if (p.isBody){
			fact = false;
			first_body.push_back(p);
		}

	}

	//if first is a fact, just remove the predicate in the second body
	if (fact){
		second_preds.erase(second_preds.begin() + index);
	}
	//if first is not a fact, replace predicate in second body with first body
	else{
		//goes through and sub the rest of the tokens in the first horn clause with the substitutions in the sub list if the body of the first clause is needed
		for (unsigned int k = zero; k < first_body.size(); k++){
			Predicate p = first_body[k];
			for (string s : list_subs){

				//separates sublist string by delimiter "/"
				string replacer = s.substr(zero, s.find("/"));
				string replacee = s.substr(s.find("/") + offset, s.length());
				for (unsigned int i = zero; i < p.args.size(); i++){
					if (p.args[i].input.compare(replacee) == zero){
						//subs appropriately in the vector of args in this predicate
						Token t;
						if (isNum(replacer) == offset){
							t = Token(Token::NUMBER, replacer);
							t.subtype = Token::UNKNOWN;
							p.args[i] = t;
						}
						else if (isUnbound(replacer)){
							t = Token(Token::LABEL, replacer);
							t.subtype = Token::UNBOUND;
							p.args[i] = t;
						}
						else{
							t = Token(Token::LABEL, replacer);
							t.subtype = Token::BOUND;
							p.args[i] = t;
						}
					}
				}
			}
			//saves the predicate back in for its previous
			first_body[k] = p;
		}
		//erases the target predicate...
		second_preds.erase(second_preds.begin() + index);
		//...and replaces it with the substitutions applied body of the first clause
		second_preds.insert(second_preds.begin() + index, first_body.begin(), first_body.end());
	}

	//applies substitutions process to second body to substitute the unsubstituted tokens in the second clause
	for (unsigned int k = zero; k < second_preds.size(); k++){
		Predicate p = second_preds[k];
		for (string s : list_subs){

			string replacer = s.substr(zero, s.find("/"));
			string replacee = s.substr(s.find("/") + offset, s.length());
			for (unsigned int i = zero; i < p.args.size(); i++){
				if (p.args[i].input.compare(replacee) == zero){
					//subs appropriately
					Token t;
					if (isNum(replacer) == offset){
						t = Token(Token::NUMBER, replacer);
						t.subtype = Token::UNKNOWN;
						p.args[i] = t;
					}
					else if (isUnbound(replacer)){
						t = Token(Token::LABEL, replacer);
						t.subtype = Token::UNBOUND;
						p.args[i] = t;
					}
					else{
						t = Token(Token::LABEL, replacer);
						t.subtype = Token::BOUND;
						p.args[i] = t;
					}
				}
			}
		}
		second_preds[k] = p;

	}

	unsigned int headNum = zero;
	unsigned int bodyNum = zero;

	for (Predicate p : second_preds){
		if (p.isBody){
			bodyNum++;
		}
		else headNum++;
	}
	//puts the newly made hornclause (in the correct format) through build to add it to the symbol tables and hornclause database
	if (headNum > offset){
		unsigned int counter = zero;
		stringstream buffer;
		string hcstring;
		buffer << "( ";
		for (Predicate p : second_preds){
			if (counter == bodyNum - offset){
				if (bodyNum > (unsigned int) zero)
					buffer << "( ";
			}
			buffer << p << " ";

			if (counter == second_preds.size() - offset && bodyNum > (unsigned int) zero)
				buffer << ") ";
			counter++;
		}
		buffer << ")";

		build(buffer.str(), true);
	}
	else{
		int counter = zero;
		stringstream buffer;
		string hcstring;
		buffer << "( ";
		for (Predicate p : second_preds){
			
			buffer << p << " ";
			if (counter == bodyNum - offset){
				if (bodyNum > (unsigned int) zero)
					buffer << "( ";
			}
			if (counter == second_preds.size() - offset && bodyNum > (unsigned int) zero)
				buffer << ") ";
			counter++;
		}
		buffer << ")";

		build(buffer.str(), true);
	}
	

	return success;
}

//call substitution to unify once it find a first head predicate to a second body predicate to unify
int unify(vector<Predicate> first, vector<Predicate> second){

	//make vector of list subs to pass into substitute method 
	vector<string> list_subs;

	//gets a vector the the predicates in the second body
	vector<Predicate>::iterator it;

	vector<Predicate> first_heads;
	int headNum = zero;

	//push back heads 
	for (it = first.begin(); it != first.end(); it++){
		if (!(it->isBody)){
			first_heads.push_back(*it);
			headNum++;
		}
	}
	//push back bodies
	vector<Predicate> second_body;
	for (it = second.begin(); it != second.end(); it++){
		if (it->isBody){
			second_body.push_back(*it);
		}
	}



	//goes through the second body predicates to find which ones can be unified with the first head
	for (vector<Predicate>::iterator it2 = first_heads.begin(); it2 != first_heads.end(); it2++){
		int index = zero;
		Predicate first_head = *it2;
		for (it = second_body.begin(); it != second_body.end(); it++){

			//checks to make sure names and number of args are the same and that number is not zero
			if (first_head.name == it->name && first_head.args.size() == it->args.size() && first_head.args.size() != zero){

				vector<Token> first_args = first_head.args;
				vector<Token> second_args = it->args;

				vector<Token>::iterator first_it = first_args.begin();
				vector<Token>::iterator second_it = second_args.begin();

				int counter_skip = zero;
				//checks to make sure the predicates aren't exactly the same
				while (first_it != first_args.end()){
					if (first_it->input.compare(second_it->input) == zero){
						counter_skip++;
					}
					first_it++; second_it++;
				}

				//skip unifying this if predicates are the same
				if (counter_skip != first_args.size()){
					//index + headNum is the position the target body predicate is in the list of args
					substitution(first_args, second_args, first, second, list_subs, it->name, index + headNum); //call substitute to unify two predicates 
				}
			}
			index++;
		}
	}

	return success;
}


//resolve method that resolves the hornclauses based on the number given to them
int resolve(unsigned int firstNum, unsigned int secondNum){
	if (firstNum > printhc.size() || secondNum > printhc.size() || firstNum == zero || secondNum == zero){
		hcDisplayEnd = hcDisplayFull;
		hcDisplayStart = zero;
		print_hc();
		cout << "Cannot resolve because not in symbol table" << endl;
		return cannot_resolve;
	}
	int size_hcvec = printhc.size();

	//vector of predicates
	vector<Predicate> first_pred_vec;
	vector<Predicate> second_pred_vec;

	pair<multimap<int, Predicate>::iterator, multimap<int, Predicate>::iterator> range = symbol_table.equal_range(firstNum);
	multimap<int, Predicate>::iterator map_it;

	//push back the predicates into the vectors based on the numbers given in the function
	for (map_it = range.first; map_it != range.second; map_it++){
		first_pred_vec.push_back(map_it->second);
	}

	range = symbol_table.equal_range(secondNum);
	for (map_it = range.first; map_it != range.second; map_it++){
		second_pred_vec.push_back(map_it->second);
	}

	//calling unify to the vector of predicates
	if (printhc[firstNum - offset].compare(printhc[secondNum - offset]) != zero){
		unify(first_pred_vec, second_pred_vec);
	}

	//display numbers for the console window 
	hcDisplayEnd = hcDisplayFull;
	hcDisplayStart = zero;
	print_hc();
	if (size_hcvec == printhc.size()){
		
		cout << "Horn clauses could not be resolved" << endl;
		return cannot_resolve;
	}
	return success;
}


//method that sets the value when the set command is typed into command line
int set_value(vector<string> args){
	string variable = args[zero];
	string value = args[offset];
	//checks that its a number
	if (!isNum(value)){
		cout << "Value must be a number" << endl;
		return value_not_num; 
	}

	//insert value into the bound map if a valid number 
	if (variable.length() == offset && islower(variable[zero])){
		//update value if bound variable is already there
		if (bound.find(variable) == bound.end()){
			bound.insert(pair<string, int>(variable, stoi(value)));
		}
		else{
			//add a new bound variable and value if not already in there 
			if (isNum(value)){
				bound[variable] = stoi(value);
			}
			else{
				cout << "Value must be a number" << endl;
			}
		}
	}
	else{
		cout << "Variable must be bound" << endl;
	}
	return success;
}

//main method 
int main(int argc, char* argv[])
{
	//checks for only 2 inputs (file name, input file, output file)
	if (argc > numArgs) {
		//prints usage if you give the wrong number of arguments
		cout << "usage: " << argv[fileName] << " [lines]" << endl;
		//returns number thats not 1 to reflect not a success
		return incorrect_args;
	}
	else {
		//two arguments means the one other one than the .exe file is the desirces lines shown on console window
		if (argc == two){
			if (isNum(argv[offset])){
				hcDisplayEnd = stoi(argv[offset]);
				hcDisplayFull = stoi(argv[offset]);
			}
			else{ //was not given a number as the second argument
				cout << "usage: [lines] should be an unsigned integer" << endl;
				return incorrect_args;
			}
		}

		while (true){ //infinitely runs to prompt user for commands 

			string commands;
			cout << ">> ";
			getline(cin, commands);
			stringstream ss(commands);
			string command;
			ss >> command;

			vector<string> args;
			string temp;
			while (ss >> temp){
				args.push_back(temp);
			}

			//process parses file given 
			if (command == "process"){
				if (args.size() != offset){
					cout << "usage: process <filename>" << endl;
				}
				else{ //if file can't be opened, print error
					if (parse(args[zero], "out.txt", false) == ifsNotOpen){
						cout << "File could not be opened" << endl;
						
					}
					else{ //output to a text file 
						build("out.txt", false);
					}
					//sets the lines of display
					hcDisplayStart = zero;
					hcDisplayEnd = hcDisplayFull;
					print_hc();
				}
			}

			//assert parses the line after assert as a hornclause
			if (command == "assert"){

				if (args.size() < offset){
					cout << "usage: assert <Horn clause> {<Horn clause>}" << endl;
					hcDisplayStart = zero;
					hcDisplayEnd = hcDisplayFull;
					print_hc();
				}
				else{
					//parse string 
					string str_assert = "";
					for (unsigned int i = zero; i < args.size(); i++){
						str_assert += args[i] + " ";
					}
					parse(str_assert, "out.txt", true);
					build("out.txt", false);
					//set display 
					hcDisplayStart = zero;
					hcDisplayEnd = hcDisplayFull;
					print_hc();
				}
			}

			//up scrolls up the screen the necessary numbers, and if no  number was specified go up a full page
			if (command == "up"){
				if (args.size() > offset){
					cout << "usage: up [lines]" << endl;
				}
				else if (args.size() == offset){
					if (isNum(args[zero])){ //if no size specified, go up a page
						int lines = stoi(args[zero]);
						if (lines > hcDisplayFull){
							lines = hcDisplayFull;
						}
						//display HC's based on lines and display start 
						hcDisplayStart -= lines;
						hcDisplayEnd = hcDisplayStart + hcDisplayFull;
						print_hc();
					}
					else{
						cout << "[lines] must be number" << endl;
					}
				}
				else{
					hcDisplayStart -= hcDisplayFull;
					hcDisplayEnd = hcDisplayStart + hcDisplayFull;
					print_hc();
				}
			}

			//down does similar actions as up, but scrolls down the page instead of up
			if (command == "down"){
				if (args.size() > offset){
					cout << "usage: down [lines]" << endl;
				}
				else if (args.size() == offset){
					if (isNum(args[zero])){ //scroll down a page length
						int lines = stoi(args[zero]);
						if (lines > hcDisplayFull){
							lines = hcDisplayFull;
						}
						hcDisplayStart += lines;
						hcDisplayEnd = hcDisplayStart + hcDisplayFull;
						print_hc();
					}
					else{
						cout << "[lines] must be number" << endl;
					}
				}
				else{
					hcDisplayStart += hcDisplayFull;
					hcDisplayEnd = hcDisplayStart + hcDisplayFull;
					print_hc();
				}
			}

			//resolve unifies two HC based on the numbers provided in the command line and creates a new HC
			if (command == "resolve"){
				if (args.size() != two){
					cout << "usage: resolve <Horn_clause_number> <Horn_clause_number>" << endl;
				}
				else{
					//if two arguments were given, check if they were both numbers 
					string int1 = args[zero];
					string int2 = args[offset];
					if (isNum(int1) && isNum(int2)){
						if (int1 == int2){ //can't be the same number
							hcDisplayStart = zero;
							hcDisplayEnd = hcDisplayFull;
							print_hc();
							cout << "Must input different numbers" << endl;
						}
						else{ //convert argument to a num, and call resolve on those two HC
							int currentNum = stoi(int1);
							int compareNum = stoi(int2);
							resolve(currentNum, compareNum);
						}
					}
					else{
						hcDisplayStart = zero;
						hcDisplayEnd = hcDisplayFull;
						print_hc();
						cout << "Must input unsigned integers" << endl;
					}
				}
			}

			//randomize sets a certain bound variable to be a random value
			if (command == "randomize"){
				srand(static_cast<unsigned int>(time(0)));
				if (args.size() == two || args.size() == offset){
					vector<string> params;
					params.push_back(args[zero]);
					if (args.size() == two){ //needs two arguments 
						if (isNum(args[offset])){
							//create random number that is below the number specified
							int random = rand() % stoi(args[offset]);
							//push in random value
							params.push_back(to_string(random));
							set_value(params);
						}
						else{
							cout << "<max> must be a number" << endl;
						}
					}
					else if (args.size() == offset){
						int random = rand();
						//if no number is specified, make the number completely random (no roof)
						params.push_back(to_string(random));
						set_value(params);
					}
				}
				else{
					cout << "usage: randomize <variable> [<max>]" << endl;
				}

			}

			//set sets a number to the bound variable
			if (command == "set"){
				if (args.size() != 2){ //must be the variable and the value 
					cout << "usage: set <variable> <value>" << endl;
				}
				else{
					set_value(args);
				}
			}

			//print prints out all bounds, predicates, and hornclauses 
			if (command == "print"){
				if (args.size() != zero){
					cout << "usage: print" << endl;
				}
				else{
					//print bounds symbol table
					cout << "BOUNDS: " << endl;
					map<string, int>::iterator it_map;
					for (it_map = bound.begin(); it_map != bound.end(); it_map++) {
						cout << it_map->first << " " << it_map->second << endl;
					}
					cout << endl;

					//print predicates symbol table 
					cout << "PREDICATES: (with their corresponding Hornclause number)" << endl;
					multimap<int, Predicate>::iterator it;
					for (it = symbol_table.begin(); it != symbol_table.end(); it++){
						cout << (*it).first << ": ";
						Predicate p = (*it).second;
						vector<Token> predVec = p.args;
						cout << "( " << p.name << " ";
						for (vector<Token>::iterator i = predVec.begin(); i != predVec.end(); i++){
							cout << i->input << " ";
						}
						cout << ") " << endl;
					}
					cout << endl;

					//print HC table 
					cout << "HORN CLAUSES:" << endl;
					hcDisplayStart = zero;
					hcDisplayEnd = hcDisplayFull;
					print_hc();

				}
			}

		}

		//return 0 if succesfful 
		return success;
	}
}