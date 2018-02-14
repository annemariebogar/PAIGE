/* 	MLSA Multilingual Software Analysis
	This program is part of the MLSA package under development at Fordham University Department of Computer and Information Science.
 	Call.cpp holds all the information for the function calls in the AST, including ID, scope (the function they are found in), name, and all arguments
 	Author: Anne Marie Bogar
 	Date: February 14, 2018
 	this code can be copied or used and is without warrenty or support, but this header needs to be copied along with the program FU2017
 	Input: N/A
 	Output: N/A
*/

#include <iostream>
#include <string>
#include <vector>
#include "Call.h"
using namespace std;

//constructor -> sets scope and id, declares name as ""
Call::Call(string s, string i)
{
	scope = s;
	id = i;
	name = "";
}

//sets the name of the Call
void Call::setName(string n)
{
	name = n;
}

//adds an argument to the Call's argument vector
void Call::addArg(string a)
{
	args.push_back(a);
}

//returns true if name is not ""
bool Call::hasName()
{
	if(name != ""){
		return true;
	}
	return false;
}

//returns a string of the Call in CSV form
//ID,scope,name[,arg1][,arg2][...]
string Call::print()
{
	string p = id+","+scope+","+name;
	for(int k=0; k<args.size(); k++){
		p += ","+args[k];
	}
	return p;
}
