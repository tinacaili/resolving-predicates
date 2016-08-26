//Lab 2 Megan Bacani and Tina Li
//number class header that is a child of parent and has a value of unsigned num

#ifndef Number_H
#define Number_H

#include "stdafx.h"
#include "Token.h"
#include <string>

using namespace std;

struct Number : public Token{
	unsigned int num;
	Number(string & str);
};

#endif