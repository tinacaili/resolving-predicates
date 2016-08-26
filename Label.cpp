//Lab 2 Megan Bacani and Tina Li
//label class that is a child of token with type always being token_kind::label and value the string passed into it
#include "stdafx.h"
#include "Token.h"
#include "Label.h"
#include <string>

using namespace std;

Label::Label(string & str) :Token(token_kind::LABEL, str){
}

