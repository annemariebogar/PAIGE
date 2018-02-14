/* 	MLSA Multilingual Software Analysis
	This program is part of the MLSA package under development at Fordham University Department of Computer and Information Science.
 	Cparse.cpp processes strings found in the C/C++ Island Grammar to print out a CSV with all function calls that include their ID, scope (the function they are found in), name, and all arguments
 	Author: Anne Marie Bogar
 	Date: February 14, 2018
 	this code can be copied or used and is without warrenty or support, but this header needs to be copied along with the program FU2017
 	Input: N/A
 	Output: csv file
		call_id(string),scope(string),function_name(string),argument1(string),argument2(string)...
 	for arguments, all literals are encased in <> (ex: <5>)
 	function calls that are arguments of another function call have their id next to () in csv line of the other function call
               ex: 6,GLOBAL,file.js,function1,function2()4
*/

#include <iostream>
#include <string>
#include <typeinfo>
#include <vector>
#include "Cparse.h"
#include "Call.h"
using namespace std;

//default constructor -> set starting values for member variables
Cparse::Cparse()
{
	arg = false;
	memarg = false;
	sub = "";
	mem = "";
}

//destructor -> delete all Call pointers and Callblock pointers
Cparse::~Cparse()
{
	for(int k=0; k<callList.size(); k++){
		delete callList[k];
	}
	for(int j=0; j<calltab.size(); j++){
		delete calltab[j];
	}
}

//checks whether the AST is in an argument block or a member argument block
//if in block, do not add argument unless it is the name of a subscript variable
bool Cparse::checkarg(int tabcount, bool tick, bool memcall)
{
	if(arg){
		//checks whether the argument block has finished
		if((argtab > tabcount)||((argtab == tabcount)&&(!tick))){
			arg = false;
			//cout << "no longer in arg\n";
			return true;
		} else if(sub == "[]"){
			//cout << "sub is true\n";
			return true;
		}
		else if(memcall && !memarg){
			//cout << "memcall && !memarg\n";
			return true;
		}
		else {
			//cout << "arg is true and sub is false\n";
			return false;
		}
	}
	if(memarg){
		//checks whether the member argument block has finished
		//cout << "checking memarg\n";
		if((memtab > tabcount)||((memtab == tabcount)&&(!tick))){
			memarg = false;
			//cout << "memarg done\n";
			return true;
		} else {
			//cout << "memarg is true\n";
			return false;
		}
	}
	//cout << "not memarg or arg\n";
	return true;
}

//the AST is in an argument block
void Cparse::markarg(int tabcount)
{
	arg = true;
	argtab = tabcount;
}

//the AST is in a member argument block
void Cparse::markmemarg(int tabcount)
{
	memarg = true;
	memtab = tabcount;
}

//checks whether a call block has finished
//if the indent is smaller than or equal to the call indent and there is no tick, the call block has finished
void Cparse::checkblock(int tabcount, bool tick)
{
	int tabs = calltab.size();
	for(int k = 0; k < tabs; k++){
		if((calltab.back()->indent > tabcount)||((calltab.back()->indent == tabcount)&&(!tick))){
			delete calltab.back();
			calltab.pop_back();
		}
	}
	//if(!calltab.empty())
		//cout << calltab.size() << endl;
}

//checks whether the AST is inside a call block or an argument block
bool Cparse::checkcall(int tabcount, bool tick, bool memcall=false)
{
	//bool j;
	checkblock(tabcount, tick);
	//cout << "after checkblock in checkcall\n";
	if(calltab.empty()){
		//cout << "there's no call\n";
		//cout << "call tab size = " << calltab.size() << endl;
		memarg = false;
		arg = false;
		//cout << "changed arg bool\n";
		//cout << "calltab is empty\n";
		return false;
	} else {
		//cout << "there IS a call\n";
		bool j = checkarg(tabcount, tick, memcall);
		//if(!j){
		//	cout << "checkarg returned false\n";
		//}
		return j;
	}
}

//adds a new Call object to the list and a new Callblock object depending on the type of call
void Cparse::addCall(int type, int tabcount, bool tick, string id)
{
	//cout << "in addCall\n";
	checkcall(tabcount, tick);
	//cout << "checked the block :)\n";
	if(type && memarg){
		//cout << "adding mem call AFTER other mem call\n";
		memarg = false;
		delete calltab.back();
		calltab.pop_back();
	} else {
		if(!calltab.empty() && !arg)
			callList.back()->addArg(id);
	}
	//cout << "adding call: " << id << endl;
	calltab.push_back(new Callblock(type, tabcount));
	//cout << to_string(calltab.size()) << " " << id << endl;
	callList.push_back(new Call(function, id));
	mem = "";
	//cout << "call added!!\n";
	//if(type == 1)
	//	cout << callList.back()->print() << endl;
}

//adds an argument to the last Call object in the list
bool Cparse::addArgument(int tabcount, bool tick, string a)
{
	if(checkcall(tabcount, tick)){
		callList.back()->addArg(a);
		return true;
	}
	//cout << "not adding: " << a;
	//if(!callList.empty())
	//	cout << " from: " << callList.back()->print() << endl;
	return false;
}

//adds a BINOP or UNOP argument
//marks argument block
void Cparse::addOp(int tabcount, bool tick, string a)
{
	if(addArgument(tabcount, tick, a))
		markarg(tabcount);
}

//either saves member attribute name for later or adds member function name to latest Call object
void Cparse::addMem(int tabcount, bool tick, string attr)
{
	//cout << "in addMem\n";
	if(checkcall(tabcount, tick, true)){
		//cout << "in if(checkcall)\n";
		if((calltab.back()->type == 1) && !callList.back()->hasName()){
			addCallName("OBJ."+attr);
			markmemarg(tabcount);
			//cout << callList.back()->print() << endl;
		} else{
			mem = "."+attr+mem;
		}
	} /*else {
		cout << "didn't add mem name: ";
		if(!callList.empty())
			cout << callList.back()->print() << endl;
	}*/
	//cout << "not in a call\n";
}

//marks argument block and signals subscript type
void Cparse::addSub(int tabcount, bool tick)
{
	if(checkcall(tabcount, tick)){
		sub = "[]"; 
		markarg(tabcount);
	}
}

//saves current function name
void Cparse::addFunction(int tabcount, bool tick, string f)
{
	checkblock(tabcount, tick);
	function = f;
}

//adds name of the function call to the last Call object in list
void Cparse::addCallName(string name)
{
	if(!calltab.empty())
		//if(name == " ")
			//name = "**Undetermined Function**";
		callList.back()->setName(name);

}

//adds variable argument
//potential to be a member variable, so adds mem (if no mem, the value is "" and nothing happens)
void Cparse::addVar(int tabcount, bool tick, string a)
{
	if(addArgument(tabcount, tick, a+mem))
		mem = "";
}

//adds Parameter variable argument
//potential to be subscript variable, so adds sub (if no sub, value is "" and nothing happens)
void Cparse::addParmVar(int tabcount, bool tick, string a)
{
	if(addArgument(tabcount, tick, a+sub))
		sub = "";
}

//any Call without a name is tagged as **Undetermined_Function**
void Cparse::cleanUp()
{
	for(int k=0; k<callList.size(); k++){
		if(!callList[k]->hasName())
			callList[k]->setName("**Undetermined_Function**");
	}
}

//prints out the list of Calls
void Cparse::print()
{
	for(int k=0; k<callList.size(); k++){
		cout << callList[k]->print() << endl;
	}
}
