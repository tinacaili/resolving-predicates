//Lab 2 Megan Bacani and Tina Li
//header class for a label object that is a child to token
#ifndef Label_H
#define Label_H

#include "stdafx.h"
#include "Token.h"
#include <string>

using namespace std;

struct Label : public Token{
	Label(string & str);
	
};

#endif