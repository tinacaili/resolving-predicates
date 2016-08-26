//Lab 2 Megan Bacani and Tina Li

//Scanner.cpp defines the methods, operators, and member variables of the scanner class 

//include files and defining the namespace
#include "stdafx.h"
#include "Scanner.h"
#include "Token.h"
#include "Number.h"
#include "Label.h"
#include <fstream>
#include <algorithm>
#include <memory>

using namespace std;

int zero_scanner = 0;
int success_scanner = 0;
string blank = "";

shared_ptr<Token> tkn; 

//method that returns a boolean true if the ifs stream is open or if the string in the ss stream is done reading, false otherwise 
bool Scanner::isValid(){
	if (readingFile){
		if (is.eof()){
			return false;
		}
		else {
			return true;
		}
	}
	else{
		if (str.length() > str_counter){
			return true;
		}
		else{
			return false;
		}
	}
}

//private method that uses an iterator to iterator through the string to check if the string is all alphabetical characters
bool Scanner::isAlphabet(string s){
	//creating iterator
	string::iterator it;
	int i = zero_scanner;
	bool ans = true;
	//iterates through string 
	for (it = s.begin(); it < s.end(); it++){
		if (!(*it >= 'a' && *it <= 'z' || *it >= 'A' && *it <= 'Z')){
			ans = false;
		}
	}
	return ans;
}

//private method that uses an iterator to iterator through the string to check if the string is '('
bool Scanner::isLeft(string s){
	//creating iterator
	bool ans = true;
	if (s.compare("(") != zero_scanner)
		ans = false;
	return ans;
}

//private method that uses an iterator to iterator through the string to check if the string is ')'
bool Scanner::isRight(string s){
	bool ans = true;
	if (s.compare(")") != zero_scanner)
		ans = false;
	return ans;
}

//private method that uses an iterator to iterator through the string to check if the string is all digits
bool Scanner::isNumeric(string s){
	return find_if_not(s.begin(), s.end(), (int(*)(int))isdigit) == s.end();
}


//one uppercase letter
bool Scanner::isUnbound(string s){
	bool ans = true;
	if (s.length() == 1){
		if (!isupper(*s.begin())){
			ans = false;
		}
	}
	else{
		ans = false;
	}

	return ans;
}

//one lowercase letter
bool Scanner::isBound(string s){
	bool ans = true;
	if (s.length() == 1){
		if (!islower(*s.begin())){
			ans = false;
		}
	}
	else{
		ans = false;
	}
	
	return ans;
}

//set_string is called to indicate to scanner that it should be scanning a string
int Scanner::set_string(string s){
	readingFile = false;
	str = s;
	str_counter = 0;
	return success_scanner;
}

//method that overrides the >> operator, parses the string by word and checks if each word is a valid token
Scanner& Scanner::operator>>(shared_ptr<Token> & t){
	//defining strings
	string s = blank;

	//reads each string 
	if (readingFile){
		//istream is a filestream
		is >> s;
	}
	else{
		//istream is a stringstream
		is >> s;
		str_counter++;
	}
	

	//checks if the input is a token, assigns it to the correct enum and unknown otherwise 
	if (isAlphabet(s)){
		shared_ptr<Label> l(new Label(s));
		t = l;
		//if is bound, set the subtype of the token to bound
		if (isBound(s)){
			t->set_subtype(Token::BOUND);
		}
		//if is bound, set the subtype of the token to unbound
		else if (isUnbound(s)){
			t->set_subtype(Token::UNBOUND);
		}
	}
	else if (isLeft(s)){
		shared_ptr<Token> tkn(new Token(Token::LEFTPAREN, s));
		t = tkn;
	}
	else if (isRight(s)){
		shared_ptr<Token> tkn(new Token(Token::RIGHTPAREN, s));
		t = tkn;
	}
	else if (isNumeric(s)){
		shared_ptr<Number> n(new Number(s));
		t = n;
	}
	else{
		shared_ptr<Token> tkn(new Token(Token::UNKNOWN, s));
		t = tkn;
	}

	return *this;
}

