#pragma once
#include <string>
#include <vector>
#include "pin.h"
using namespace std;

class MakeBlock//the parent class of all the MakeBlock components
{
protected:
	string name_Object;//name of the component
	string type;//type of the component(Sensor, Actuator, Connector, Controller, Power....)
	vector<pin> input_Pins;//head of the list of input pins
	vector<pin> output_Pins;//head of the list of output pins
	int input_need;//the minimum number of necessary inputs
	int output_need;//the minimum number of necessary outputs
	int No;//when there are more than one components of the same type, a sequence number is needed to label them.
	string configfile = "config.txt";

public:
	//construction function
	MakeBlock(string componentname);
	MakeBlock();

	//access the members
	string get_Name();

	//basic functionalities
	friend vector<MakeBlock> load_Assembly(string assemblyfile);//load the components from the assembly file & config file and return the head
	friend void suggest_Components(vector<MakeBlock> components);//suggestion based on properties
	friend void suggest_Components(vector<MakeBlock> components, string type);//suggestion based on properties
	friend void demo_Connection(string name);//show how to connect a component based on name

	void friend build_Connection(MakeBlock *x, MakeBlock *y);//build a connection between 2 components, picking ports in sequence
	void friend build_Connection(MakeBlock *x, string x_pin, MakeBlock *y, string y_pin);//build a connection between 2 components, through the specified pins
	void suggest_Connection_NecessaryAndOptional();//suggest possible components to be connected to x
	void suggest_Connection_OnlyNecessary();//suggest possible components to be connected to x
	friend string full_Path_single(string comname, string path);//return 1 possible connection path from the component to OrionBoard
	friend vector<string> full_Path_all(string comname, vector<string> path);//return all the possible connection paths

	//output
	friend ostream & operator << (ostream &out, MakeBlock x);//output x
	friend ostream & operator << (ostream &out, vector<MakeBlock> components);//output vector of components

	//aid functions of suggest_components_by_probabilities and suggest_components_by_completeness
	friend vector<MakeBlock> load_all_candidates();//load all the components from the configfile
	int test_Connection(MakeBlock another);//test if another makeblock components can be connected to the current one
	friend vector<MakeBlock> delete_connectors(vector<MakeBlock> model);//delete the connectors from the component list
	friend vector<string> MakeBlock2string(vector<MakeBlock> model);//invert the makeblock model to strings
};


