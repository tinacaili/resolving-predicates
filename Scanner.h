//Lab 2 Megan Bacani and Tina Li
//Scanner.h gives declarations the methods, operators, and member variables of the scanner class to be defined in Scanner.cpp
#ifndef Scanner_H
#define Scanner_H

//include files and defining the namespace
#include "stdafx.h"
#include "Token.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <istream>
#include <ostream>
#include <string>
#include <memory>

using namespace std;

class Scanner{
private:
	//declares input file stream member variable and private methods
	istream & is;
	string str;
	unsigned int str_counter;
	bool readingFile;
	bool isAlphabet(string s);
	bool isLeft(string s);
	bool isRight(string s);
	bool isNumeric(string s);
	bool isComma(string s);
	bool isSeparator(string s);
	bool isAnd(string s);
	bool isUnbound(string s);
	bool isBound(string s);
public:
	//constructor for scanner class that takes in a reference to ifstream and assigned it to ifs
	Scanner(istream & i) : is(i), readingFile(true){};
	//set_string is called to indicate to scanner that it should be scanning a string
	int set_string(string s);
	//boolean method to check if valid
	bool isValid();

	//overwriting the >> operator 
	Scanner& operator>>(shared_ptr<Token> & t);

};

#endif