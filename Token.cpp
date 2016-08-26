//Lab 0 Megan Bacani and Tina Li
// Token.cpp : Defines Token constructor. Token constructor is here because without, it's just an empty .cpp file.. which is kind of awkward.
//
#include "stdafx.h"
#include "Token.h"

int success_token = 0;
//creates constructor for Token
Token::Token(){
	token_type = token_kind::UNKNOWN;
	subtype = token_kind::UNKNOWN;
	input = "";
}


//creates constructor that sets the type and input of the token 
Token::Token(token_kind type, const string &str)
{
	token_type = type; 
	input = str; 
}

//sets the subtype of a label token to bound or unbound if it's that
int Token::set_subtype(token_kind token){
	subtype = token;
	return success_token;
}

//gets the subtype of a label token
Token::token_kind Token::get_subtype(){
	return subtype;
}