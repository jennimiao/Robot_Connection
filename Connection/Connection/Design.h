#pragma once
#include <vector>
#include <string>
#include "MakeBlock.h"
using namespace std;

class Design
{
protected:
	string name;//name of the design
	vector<vector<string>> parts;//possible combinations of groups

public:
	//accessing members
	string get_name();//return name
	vector<vector<string>> get_parts();//return parts

	//output
	friend ostream & operator << (ostream &out, Design x);//overload of <<
	friend ostream & operator << (ostream &out, vector<Design> x);//overload of << 

	//aid functions of suggest_components_by_probability and suggest_components_by_completeness
	friend vector<Design> load_Design(string filename);//load all the designs from file
}; 
