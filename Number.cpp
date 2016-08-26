//Lab 2 Megan Bacani and Tina Li
//number object that is a child of token
#include "stdafx.h"
#include "Token.h"
#include "Number.h"
#include <istream>
#include <sstream>
#include <string>

using namespace std;
//number contains a number token type is auto set to  number
Number::Number(string & str):Token(token_kind::NUMBER, str){
	istringstream iss(str); 
	iss >> num;
	if (iss.fail()){
		cout << "No. Fix me." << endl;
	}
}
