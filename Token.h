//Lab 0 Megan Bacani and Tina Li
//Token.h gives declarations the methods, operators, and member variables of the token class 

#ifndef Token_H
#define Token_H

//include files and defining the namespace
#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <istream>
#include <ostream>
#include <string>

using namespace std;

struct Token{
public:
	//enum to define the token kind, assigned in scanner.cpp 
	enum token_kind{
		AND,
		COMMA,
		LABEL,
		LEFTPAREN,
		NUMBER,
		RIGHTPAREN,
		SEPARATOR,
		BOUND,
		UNBOUND,
		UNKNOWN
	};
	//member variable to assign the enum to for each token
	token_kind token_type;

	//input to parse for token type
	string input;

	//constructors that initializes the token type and the input
	Token();
	Token(token_kind type, const string &str);
	
	//subtype of a label token can be bound or unbound
	token_kind subtype;
	//sets the subtype
	int set_subtype(token_kind token);
	//gets the subtype
	token_kind get_subtype();
};


#endif