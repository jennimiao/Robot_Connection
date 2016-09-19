#include <iostream>
#include <string>
#include <fstream>
#include "MakeBlock.h"
#include "pin.h"
using namespace std;

MakeBlock::MakeBlock(string componentname)
{
	name_Object = componentname;//different component name for different child classes
	fstream in(configfile, ios::in);
	pin temp;//to store the pin information temporarily
	char buffer[256];
	if (!in.is_open())
	{
		cout << "error opening file!" << endl;
		exit(1);
	}
	while (!in.eof())//goes to the component part
	{
		in.getline(buffer, 100);
		string tempo(buffer + 6);
		if (tempo == componentname)
		{
			break;
		}
	}
	in.getline(buffer, 100);//read type
	type = buffer + 6;
	in.getline(buffer, 100);//read input_need
	char c = *(buffer + 7);
	input_need = c - '0';
	in.getline(buffer, 100);//read output_need
	c = *(buffer + 8);
	output_need = c - '0';
	while (!in.eof())//read pin information
	{
		in.getline(buffer, 100);
		if (strcmp(buffer, "EndComponent") == 0)
		{
			break;
		}
		if (strcmp(buffer + 1, "Pin") == 0)
		{
			in.getline(buffer, 100);
			if (strcmp(buffer + 7, "Input") == 0)
			{
				in.getline(buffer, 100);
				temp.type_Pins = buffer + 17;
				in.getline(buffer, 100);
				temp.name = buffer + 7;
				in.getline(buffer, 100);
				sscanf(buffer + 11, "%lf %lf %lf", &temp.position[0], &temp.position[1], &temp.position[2]);
				input_Pins.push_back(temp);
			}
			else
			{
				in.getline(buffer, 100);
				temp.type_Pins = buffer + 17;
				in.getline(buffer, 100);
				temp.name = buffer + 7;
				in.getline(buffer, 100);
				sscanf(buffer + 11, "%lf %lf %lf", &temp.position[0], &temp.position[1], &temp.position[2]);
				output_Pins.push_back(temp);
			}
		}
	}
	while (!in.eof())//load connection map
	{
		in.getline(buffer, 100);
		if (strcmp(buffer, "ConnectionMap") == 0)
		{
			break;
		}
	}
	while (!in.eof())
	{
		in.getline(buffer, 100);
		if (strcmp(buffer, "EndConnectionMap") == 0)
		{
			break;
		}
		if (strcmp(buffer, "") == 0) continue;
		char ss1[32], ss2[32];
		sscanf(buffer + 6, "%s %s", ss1, ss2);
		string s1(ss1);
		string s2(ss2);
		for (unsigned int i = 0; i <= input_Pins.size() - 1; i++)//input
		{
			if (input_Pins.size() == 0) break;
			if (input_Pins[i].name == s1)
			{
				input_Pins[i].connectable_Pins.push_back(s2);
			}
			else
			{
				if (input_Pins[i].name == s2)
				{
					input_Pins[i].connectable_Pins.push_back(s1);
				}
			}
		}
		for (unsigned int i = 0; i <= output_Pins.size() - 1; i++)
		{
			if (output_Pins.size() == 0) break;
			if (output_Pins[i].name == s1)
			{
				output_Pins[i].connectable_Pins.push_back(s2);
			}
			else
			{
				if (output_Pins[i].name == s2)
				{
					output_Pins[i].connectable_Pins.push_back(s1);
				}
			}
		}
		in.getline(buffer, 100);
		if (strcmp(buffer, "EndConnectionMap") == 0)
		{
			break;
		}
	}
	in.close();
}

MakeBlock::MakeBlock()
{
	configfile = "config.txt";
	No = 0;
}

string MakeBlock::get_Name()
{
	return name_Object;
}

vector<MakeBlock> load_Assembly(string assemblyfile)
{
	//first, load the components from assemblyfile
	fstream assembly(assemblyfile, ios::in);
	vector<MakeBlock> components;//the components added
	char buffer[128];

	if (!assembly.is_open())
	{
		cout << "error opening file!" << endl;
		exit(1);
	}
	while (!assembly.eof())
	{
		assembly.getline(buffer, 100);
		if (strcmp(buffer, "AddComponent") == 0)
		{
			break;
		}
	}
	while (!assembly.eof())
	{
		assembly.getline(buffer, 100);
		if (strcmp(buffer, "EndAddComponent") == 0)
		{
			break;
		}
		char num = *(buffer + 5);
		int tnum = num - '0';//number of the components of the same kind
		string tname(buffer + 7);//name of the components
		MakeBlock temp(tname);
		for (int i = 1; i <= tnum; i++)
		{
			temp.No = i;
			components.push_back(temp);
		}
	}

	//then, build connections
	int flag1 = 0, flag2 = 0;//flag1 & flag2 shows if the components to be assembles have already been added to the component list
	while (!assembly.eof())
	{
		assembly.getline(buffer, 100);
		if (strcmp(buffer, "ConnectComponents") == 0)
		{
			break;
		}
	}
	cout << char(4) << " Assemble " << assemblyfile << endl << endl;
	while (!assembly.eof())
	{
		assembly.getline(buffer, 100);
		if (strcmp(buffer, "EndConnectComponents") == 0)
		{
			break;
		}
		char c1[32], c2[32];
		int d1, d2;
		sscanf(buffer + 9, "%s %d %s %d", c1, &d1, c2, &d2);
		string s1(c1);
		string s2(c2);
		flag1 = 0;//start to check the existence of components s1, s2
		flag2 = 0;
		int seq1, seq2;//the position of s1, s2 in the vector
		for (unsigned int i = 0; i <= components.size() - 1; i++)
		{
			if (components.size() == 0) break;
			if (components[i].name_Object == s1 && components[i].No == d1)
			{
				flag1 = 1;
				seq1 = i;
			}
			if (components[i].name_Object == s2 && components[i].No == d2)
			{
				flag2 = 1;
				seq2 = i;
			}
			if (flag1 == 1 && flag2 == 1)
			{
				break;
			}
		}
		if (flag1 == 1 && flag2 == 1)
		{
			build_Connection(&components[seq1], &components[seq2]);
		}
		else
		{
			if (flag1 == 0)
			{
				cout << "Error: " << s1 << " has not be added to the design." << endl << endl;
			}
			if (flag2 == 0)
			{
				cout << "Error: " << s2 << " has not be added to the design." << endl << endl;
			}
		}
	}

	for (unsigned int i = 0; i <= components.size() - 1; i++)//suggest other connections to the user
	{
		components[i].suggest_Connection_OnlyNecessary();
	}
	cout << endl << endl;
	return components;
}

void suggest_Components(vector<MakeBlock> components)
{
	char buffer[128];
	string name;//temporary variable
	cout << char(4) << " Suggestion based on component properties: " << endl << endl;
	//first check if orionboard is added
	int orion = 0;
	for (unsigned int i = 0; i <= components.size() - 1; i++)
	{
		if (components.size() == 0) break;
		if (components[i].name_Object == "MakeBlockOrionBoard")
		{
			orion = 1;
		}
	}
	if (orion == 0)
	{
		cout << "Warning: MakeBlockOrionBoard is missing." << endl << endl;
	}

	//second check sensor/actuator
	//third check connector
	int sensor = 0;
	int actuator = 0;
	for (unsigned int i = 0; i <= components.size() - 1; i++)
	{
		if (components.size() == 0) break;
		if (components[i].type == "Sensor")
		{
			sensor = 1;
		}
		if (components[i].type == "Actuator")
		{
			actuator = 1;
		}
	}
	if (sensor == 0)
	{
		cout << "Sensors are recommended, such as: " << endl;
		fstream in(components[0].configfile, ios::in);
		if (!in.is_open())
		{
			cout << "error opening file!" << endl;
			exit(1);
		}
		while (!in.eof())
		{
			in.getline(buffer, 100);
			if (strcmp(buffer, "ConnectionMap") == 0) break;
			if (strcmp(buffer, "Component") == 0)
			{
				in.getline(buffer, 100);
				name = buffer + 6;
				in.getline(buffer, 100);
				if (strcmp(buffer + 6, "Sensor") == 0)
				{
					cout << full_Path_single(name, "\t" + name);
				}
			}
		}
		in.close();
		cout << endl;
	}
	if (actuator == 0)
	{
		cout << "Actuators are recommended, such as: " << endl;
		fstream in(components[0].configfile, ios::in);
		if (!in.is_open())
		{
			cout << "error opening file!" << endl;
			exit(1);
		}
		while (!in.eof())
		{
			in.getline(buffer, 100);
			if (strcmp(buffer, "ConnectionMap") == 0) break;
			if (strcmp(buffer, "Component") == 0)
			{
				in.getline(buffer, 100);
				name = buffer + 6;
				in.getline(buffer, 100);
				if (strcmp(buffer + 6, "Actuator") == 0)
				{
					cout << full_Path_single(name, "\t" + name);
				}
			}
		}
		in.close();
		cout << endl;
	}

	//fourth check battery
	vector<string> need_battery;//the components which need additional battery
	int need = 0;//to see if those components are added
	fstream config(components[0].configfile, ios::in);
	if (!config.is_open())
	{
		cout << "error opening file!" << endl;
		exit(1);
	}
	while (!config.eof())
	{
		config.getline(buffer, 100);
		if (strcmp(buffer, "RequireBattery") == 0) break;
	}
	while (!config.eof())
	{
		config.getline(buffer, 100);
		if (strcmp(buffer, "EndRequireBattery") == 0) break;
		need_battery.push_back(buffer + 1);
	}
	config.close();
	for (unsigned int i = 0; i <= components.size() - 1; i++)//traversal the components
	{
		if (components.size() == 0) break;
		for (unsigned int j = 0; j <= need_battery.size() - 1; j++)
		{
			if (need_battery.size() == 0) break;
			if (components[i].name_Object == need_battery[j])
			{
				need = 1;
				break;
			}
		}
		if (need == 1) break;
	}
	if (need == 1)//if a battery is needed, check if a battery is already added to the design
	{
		int battery = 0;
		for (unsigned int i = 0; i <= components.size() - 1; i++)
		{
			if (components.size() == 0) break;
			if (components[i].name_Object == "Battery")
			{
				battery = 1;
				break;
			}
		}
		if (battery == 0)
		{
			cout << "Warning: a battery is required. " << endl << endl;
		}
	}
	if (orion == 0 && sensor == 0 && actuator == 0 && need == 0)
	{
		cout << "   to be continued..." << endl << endl;
	}
	cout << endl << endl;
}

void suggest_Components(vector<MakeBlock> components, string type)
{
	char buffer[128];
	string name;//temporary variable
	int sensor = 0;//if sensor is missing
	int actuator = 0;//if actuator is missing
	for (unsigned int i = 0; i <= components.size() - 1; i++)
	{
		if (components.size() == 0) break;
		if (components[i].type == "Sensor")
		{
			sensor = 1;
		}
		if (components[i].type == "Actuator")
		{
			actuator = 1;
		}
	}
	if (type == "Sensor")
	{
		cout << char(4) << " Suggestion of sensors: " << endl << endl;
		if (sensor == 0)
		{
			cout << "Sensors are recommended, such as: " << endl;
			fstream in(components[0].configfile, ios::in);
			if (!in.is_open())
			{
				cout << "error opening file!" << endl;
				exit(1);
			}
			while (!in.eof())
			{
				in.getline(buffer, 100);
				if (strcmp(buffer, "ConnectionMap") == 0) break;
				if (strcmp(buffer, "Component") == 0)
				{
					in.getline(buffer, 100);
					name = buffer + 6;
					in.getline(buffer, 100);
					if (strcmp(buffer + 6, "Sensor") == 0)
					{
						cout << full_Path_single(name, "\t" + name);//the whole path from the sensor to the controller
					}
				}
			}
			in.close();
			cout << endl << endl << endl;
		}
		else
		{
			cout << "There is already a sensor in the design, and further recommendations are: " << endl;
			fstream in(components[0].configfile, ios::in);
			if (!in.is_open())
			{
				cout << "error opening file!" << endl;
				exit(1);
			}
			while (!in.eof())
			{
				in.getline(buffer, 100);
				if (strcmp(buffer, "ConnectionMap") == 0) break;
				if (strcmp(buffer, "Component") == 0)
				{
					in.getline(buffer, 100);
					name = buffer + 6;
					in.getline(buffer, 100);
					if (strcmp(buffer + 6, "Sensor") == 0)
					{
						cout << full_Path_single(name, "\t" + name);//the whole path from the sensor to the controller
					}
				}
			}
			in.close();
			cout << endl << endl << endl;
		}
	}
	if (type == "Actuator")
	{
		cout << char(4) << " Suggestion of actuators: " << endl << endl;
		if (actuator == 0)
		{
			cout << "Actuators are recommended, such as: " << endl;
			fstream in(components[0].configfile, ios::in);
			if (!in.is_open())
			{
				cout << "error opening file!" << endl;
				exit(1);
			}
			while (!in.eof())
			{
				in.getline(buffer, 100);
				if (strcmp(buffer, "ConnectionMap") == 0) break;
				if (strcmp(buffer, "Component") == 0)
				{
					in.getline(buffer, 100);
					name = buffer + 6;
					in.getline(buffer, 100);
					if (strcmp(buffer + 6, "Actuator") == 0)
					{
						cout << full_Path_single(name, "\t" + name);//the whole path from the actuator to the controller
					}
				}
			}
			in.close();
			cout << endl << endl << endl;
		}
		else
		{
			cout << "There is already a actuator in the design, and further recommendations are: " << endl;
			fstream in(components[0].configfile, ios::in);
			if (!in.is_open())
			{
				cout << "error opening file!" << endl;
				exit(1);
			}
			while (!in.eof())
			{
				in.getline(buffer, 100);
				if (strcmp(buffer, "ConnectionMap") == 0) break;
				if (strcmp(buffer, "Component") == 0)
				{
					in.getline(buffer, 100);
					name = buffer + 6;
					in.getline(buffer, 100);
					if (strcmp(buffer + 6, "Actuator") == 0)
					{
						cout << full_Path_single(name, "\t" + name);//the whole path from the actuator to the controller
					}
				}
			}
			in.close();
			cout << endl << endl << endl;
		}
	}
}

string full_Path_single(string component, string path)
{
	int check = 0;//to check if com could be directly connected to OrionBoard
	MakeBlock com(component);
	vector<string> connectable = connectable_Components(com.input_Pins);//the connectable components of com
	for (unsigned int i = 0; i <= connectable.size() - 1; i++)
	{
		if (connectable.size() == 0) break;
		if (connectable[i] == "MakeBlockOrionBoard")
		{
			check = 1;
			break;
		}
	}
	if (check == 1)//if com can reach OrionBoard directly, end the recursion
	{
		path += " - MakeBlockOrionBoard\n";
		return path;
	}
	else
	{
		if (connectable.size() == 0)//error report
		{
			cout << "Error: missing input candidates of " << com.name_Object << endl;
			return path;
		}
		else//recurse to add connectors to the path
		{
			path += " - ";
			path += connectable[0];
			path = full_Path_single(connectable[0], path);
			return path;
		}
	}
}

vector<string> full_Path_all(string comname, vector<string> path)
{
	int check = 0;//to check if the component could be directly connected to OrionBoard
	string temp_path = "";//temorary variable
	string cur_path = "";//temporary variable
	MakeBlock com(comname);//MakeBlock object based on comname
	vector<string> connectable = connectable_Components(com.input_Pins);//the connectable components of com
	vector<int> visited(path.size(), 0);
	if (connectable.size() == 0)
	{
		cout << "Error: missing input candidates of " << com.name_Object << endl;
	}
	for (unsigned int i = 0; i <= path.size() - 1; i++)//if the end of the path includes comname, add all the possible connections
	{
		if (path.size() == 0) break;
		if (visited[i] == 1) break;
		if (path[i].find(comname) != -1)
		{
			visited[i] = 1;
			cur_path = path[i];
			path[i] += " - ";
			path[i] += connectable[0];
			if (connectable[0] != "MakeBlockOrionBoard")
			{
				path = full_Path_all(connectable[0], path);
			}
			for (unsigned int j = 1; j <= connectable.size() - 1; j++)//all the connectable components
			{
				visited.push_back(1);
				temp_path = cur_path + " - ";
				temp_path += connectable[j];
				path.push_back(temp_path);
				if (connectable[j] != "MakeBlockOrionBoard")
				{
					path = full_Path_all(connectable[j], path);
				}
			}
		}
	}
	return path;
}

void build_Connection(MakeBlock *x, MakeBlock *y)
{
	int test = 0;//to see if the connection is possible after traversal

	for (unsigned int i = 0; i <= x->input_Pins.size() - 1; i++)//traversal of x_in
	{
		if (x->input_Pins.size() == 0) break;
		for (unsigned int j = 0; j <= x->input_Pins[i].connectable_Pins.size() - 1; j++)//traversal of the connectable pins of x_in
		{
			if (x->input_Pins[i].connectable_Pins.size() == 0) break;
			for (unsigned int k = 0; k <= y->output_Pins.size() - 1; k++)//traversal of y_out to see if they match
			{
				if (y->output_Pins.size() == 0) break;
				if (x->input_Pins[i].connectable_Pins[j] == y->output_Pins[k].name)
				{
					if (x->input_Pins[i].occupied == 0 && y->output_Pins[k].occupied == 0)//check the availability of x_in and y_out
					{
						test = 1;
						x->input_Pins[i].occupied = 1;
						x->input_Pins[i].connected_Pin = y->output_Pins[k].name;
						y->output_Pins[k].occupied = 1;
						y->output_Pins[k].connected_Pin = x->input_Pins[i].name;
						break;
					}
				}
			}
			if (test == 1) break;
		}
		if (test == 1) break;
	}

	if (test == 1)
	{
		cout << "Succeeded to connect " << x->name_Object << " and " << y->name_Object << "." << endl << endl;
		return;
	}

	for (unsigned int i = 0; i <= y->input_Pins.size() - 1; i++)//traversal of y_in
	{
		if (y->input_Pins.size() == 0) break;
		for (unsigned int j = 0; j <= y->input_Pins[i].connectable_Pins.size() - 1; j++)//traversal of the connectable pins of y->input_Pins
		{
			if (y->input_Pins[i].connectable_Pins.size() == 0) break;
			for (unsigned int k = 0; k <= x->output_Pins.size() - 1; k++)//traversal of x_out to see if they match
			{
				if (x->output_Pins.size() == 0) break;
				if (y->input_Pins[i].connectable_Pins[j] == x->output_Pins[k].name)
				{
					if (y->input_Pins[i].occupied == 0 && x->output_Pins[k].occupied == 0)//check the availability of y->input_Pins and x_out
					{
						test = 1;
						y->input_Pins[i].occupied = 1;
						y->input_Pins[i].connected_Pin = x->output_Pins[k].name;
						x->output_Pins[k].occupied = 1;
						x->output_Pins[k].connected_Pin = y->input_Pins[i].name;
						break;
					}
				}
			}
			if (test == 1) break;
		}
		if (test == 1) break;
	}

	if (test == 0)
	{
		cout << "Failed to connect " << x->name_Object << " and " << y->name_Object << ", use test_Connection to see details." << endl << endl;
		return;
	}
	else
	{
		cout << "Succeeded to connect " << x->name_Object << " and " << y->name_Object << "." << endl << endl;
		return;
	}
}

void demo_Connection(string name)
{
	vector<string> path(1, name);//initialize the paths
	path = full_Path_all(name, path);//recursion
	cout << char(4) << " Demonstration of component " << name << ":" << endl << endl;
	cout << "The possible connection paths of component " << name << " are:" << endl;
	for (unsigned int i = 0; i <= path.size() - 1; i++)
	{
		if (path.size() == 0) break;
		cout << path[i] << endl;
	}
	cout << endl << endl << endl;
}

void build_Connection(MakeBlock *x, string x_pin, MakeBlock *y, string y_pin)
{
	int test = 0;//to see if the connection is done after traversal
	int find_xpin = 0;//to see if x_pin is found
	int find_ypin = 0;//to see if y_pin is found
	int seq_xpin;//the position of x_pin
	int seq_ypin;//the position of y_pin

	for (unsigned int i = 0; i <= x->input_Pins.size() - 1; i++)//traversal of pin x_in
	{
		if (x->input_Pins.size() == 0) break;
		if (x->input_Pins[i].name == x_pin)
		{
			find_xpin = 1;
			seq_xpin = i;
			break;
		}
	}
	if (find_xpin == 1)
	{
		for (unsigned int j = 0; j <= y->output_Pins.size() - 1; j++)//traversal of pin y_out
		{
			if (y->output_Pins.size() == 0) break;
			if (y->output_Pins[j].name == y_pin)
			{
				find_ypin = 1;
				seq_ypin = j;
				break;
			}
		}
		if (find_ypin == 1)
		{
			if (x->input_Pins[seq_xpin].occupied == 0 && y->output_Pins[seq_ypin].occupied == 0)
			{
				for (unsigned int k = 0; k <= x->input_Pins[seq_xpin].connectable_Pins.size() - 1; k++)//traversal of te to see if the connection if possible
				{
					if (x->input_Pins[seq_xpin].connectable_Pins.size() == 0) break;
					if (x->input_Pins[seq_xpin].connectable_Pins[k] == y_pin)
					{
						test = 1;
					}
				}
				if (test == 1)
				{
					x->input_Pins[seq_xpin].occupied = 1;//renew the connection status of x_in and y_out
					x->input_Pins[seq_xpin].connected_Pin = y_pin;
					y->output_Pins[seq_ypin].occupied = 1;
					y->output_Pins[seq_ypin].connected_Pin = x_pin;
				}
			}
		}
	}

	if (test == 1)
	{
		cout << "Succeeded to connect " << x->name_Object << " and " << y->name_Object << "." << endl << endl;
		/*if (connected_Pins(x, x->input_Pins) < x->input_need || connected_Pins(x, x->output_Pins) < x->output_need)
		{
			suggest_Connection_OnlyNecessary(x);
		}
		if (connected_Pins(y, y->input_Pins) < y->input_need || connected_Pins(y, y->output_Pins) < y->output_need)
		{
			suggest_Connection_OnlyNecessary(y);
		}*/
		return;
	}

	find_xpin = 0;
	find_ypin = 0;
	test = 0;
	for (unsigned int i = 0; i <= y->input_Pins.size() - 1; i++)//traversal of pin y_in
	{
		if (y->input_Pins.size() == 0) break;
		if (y->input_Pins[i].name == y_pin)
		{
			find_ypin = 1;
			seq_ypin = i;
			break;
		}
	}
	if (find_ypin == 1)
	{
		for (unsigned int j = 0; j <= x->output_Pins.size() - 1; j++)//traversal of pin x_out
		{
			if (x->output_Pins.size() == 0) break;
			if (x->output_Pins[j].name == x_pin)
			{
				find_xpin = 1;
				seq_xpin = j;
				break;
			}
		}
		if (find_xpin == 1)
		{
			if (y->input_Pins[seq_ypin].occupied == 0 && x->output_Pins[seq_xpin].occupied == 0)
			{
				for (unsigned int k = 0; k <= y->input_Pins[seq_ypin].connectable_Pins.size() - 1; k++)//traversal of te to see if the connection if possible
				{
					if (y->input_Pins[seq_ypin].connectable_Pins.size() == 0) break;
					if (y->input_Pins[seq_ypin].connectable_Pins[k] == x_pin)
					{
						test = 1;
					}
				}
				if (test == 1)
				{
					y->input_Pins[seq_ypin].occupied = 1;//renew the connection status of y_in and x_out
					y->input_Pins[seq_ypin].connected_Pin = x_pin;
					x->output_Pins[seq_xpin].occupied = 1;
					x->output_Pins[seq_xpin].connected_Pin = y_pin;
				}
			}
		}
	}

	if (test == 1)
	{
		cout << "Succeeded to connect " << x->name_Object << " and " << y->name_Object << "." << endl << endl;
		/*if (connected_Pins(x, x->input_Pins) < x->input_need || connected_Pins(x, x->output_Pins) < x->output_need)
		{
			suggest_Connection_warning(x);
		}
		if (connected_Pins(y, y->input_Pins) < y->input_need || connected_Pins(y, y->output_Pins) < y->output_need)
		{
			suggest_Connection_warning(y);
		}*/
		return;
	}
	else
	{
		cout << x_pin << " and " << y_pin << " cannot be connected, use test_Connection to see details." << endl << endl;
		return;
	}
}

void MakeBlock::suggest_Connection_NecessaryAndOptional()
{
	int input_connected = connected_Pins(input_Pins);//the number of input components already connected to x
	int output_connected = connected_Pins(output_Pins);//the number of output components already connected to x
	vector<string> com;//the connectable components suggested
	
	cout << "Suggested connections for " << name_Object << ":" << endl;
	if (input_connected < input_need)
	{
		cout << "Warning: There are not enough input components to " << name_Object << ". At least " << input_need - input_connected << " additional components should be connected." << endl;
		cout << "The possible candidates are:" << endl;
		com = connectable_Components(input_Pins);
		for (unsigned int i = 0; i <= com.size() - 1; i++)
		{
			if (com.size() == 0) break;
			cout << '\t' << com[i] << endl;
		}

		if (output_connected < output_need)
		{
			cout << "Warning: There are not enough output components to " << name_Object << ". At least " << output_need - output_connected << " additional components should be connected." << endl;
			cout << "The possible candidates are:" << endl;
			com = connectable_Components(output_Pins);
			for (unsigned int i = 0; i <= com.size() - 1; i++)
			{
				if (com.size() == 0) break;
				cout << '\t' << com[i] << endl;
			}
		}
		else
		{
			com = connectable_Components(output_Pins);
			if (com.size() != 0)
			{
				cout << "For output components, the possible candidates are:" << endl;
				for (unsigned int i = 0; i <= com.size() - 1; i++)
				{
					if (com.size() == 0) break;
					cout << '\t' << com[i] << endl;
				}
			}
		}
	}
	else
	{
		if (output_connected < output_need)
		{
			cout << "Warning: There are not enough output components to " << name_Object << ". At least " << output_need - output_connected << " additional components should be connected." << endl;
			cout << "The possible candidates are:" << endl;
			com = connectable_Components(output_Pins);
			for (unsigned int i = 0; i <= com.size() - 1; i++)
			{
				if (com.size() == 0) break;
				cout << '\t' << com[i] << endl;
			}
		
			com = connectable_Components(input_Pins);
			if (com.size() != 0)
			{
				cout << "For input components, the possible candidates are:" << endl;
				for (unsigned int i = 0; i <= com.size() - 1; i++)
				{
					if (com.size() == 0) break;
					cout << '\t' << com[i] << endl;
				}
			}
		}
		else
		{
			com = connectable_Components(input_Pins);
			if (com.size() != 0)
			{
				cout << "For input components, the possible candidates are:" << endl;
				for (unsigned int i = 0; i <= com.size() - 1; i++)
				{
					if (com.size() == 0) break;
					cout << '\t' << com[i] << endl;
				}
			}

			com = connectable_Components(output_Pins);
			if (com.size() != 0)
			{
				cout << "For output components, the possible candidates are:" << endl;
				for (unsigned int i = 0; i <= com.size() - 1; i++)
				{
					if (com.size() == 0) break;
					cout << '\t' << com[i] << endl;
				}
			}
		}
	}
	cout << endl;
}

void MakeBlock::suggest_Connection_OnlyNecessary()
{
	int input_connected = connected_Pins(input_Pins);//the number of input components already connected to x
	int output_connected = connected_Pins(output_Pins);//the number of output components already connected to x
	vector<string> com;//the possible components to be connected
	if (input_connected < input_need)
	{
		cout << "Warning: There are not enough input components to " << name_Object << ". At least " << input_need - input_connected << " additional components should be connected." << endl;
		cout << "The possible candidates are:" << endl;
		com = connectable_Components(input_Pins);
		for (unsigned int i = 0; i <= com.size() - 1; i++)
		{
			if (com.size() == 0) break;
			cout << '\t' << com[i] << endl;
		}
	}
	if (output_connected < output_need)
	{
		cout << "Warning: There are not enough output components to " << name_Object << ". At least " << output_need - output_connected << " additional components should be connected." << endl;
		cout << "The possible candidates are:" << endl;
		com = connectable_Components(output_Pins);
		for (unsigned int i = 0; i <= com.size() - 1; i++)
		{
			if (com.size() == 0) break;
			cout << '\t' << com[i] << endl;
		}
	}
	if (input_connected < input_need || output_connected < output_need) cout << endl;
}

ostream & operator << (ostream &out, MakeBlock x)//reload of <<
{
	out << "Component name: " << x.name_Object << " No." << x.No << endl;
	if (x.input_Pins.size() != 0)
	{
		out << "Minimum input: " << x.input_need << endl;
		out << "Input pins: " << endl;
		for (unsigned int i = 0; i <= x.input_Pins.size() - 1; i++)
		{
			out << "\t" << x.input_Pins[i].get_name();
			if (x.input_Pins[i].get_occupied() == 0)
			{
				out << " - 0" << endl;
			}
			else
			{
				out << " - " << x.input_Pins[i].get_connected() << endl;
			}
		}
	}
	if (x.output_Pins.size() != 0)
	{
		out << "Minimum output: " << x.output_need << endl;
		out << "Output pins: " << endl;
		for (unsigned int i = 0; i <= x.output_Pins.size() - 1; i++)
		{
			out << "\t" << x.output_Pins[i].get_name();
			if (x.output_Pins[i].get_occupied() == 0)
			{
				out << " - 0" << endl;
			}
			else
			{
				out << " - " << x.output_Pins[i].get_connected() << endl;
			}
		}
	}
	out << endl << endl;
	return out;
}

ostream & operator << (ostream &out, vector<MakeBlock> components)//reload of <<
{
	if (components.size() == 0) return out;
	for (unsigned int i = 0; i <= components.size() - 1; i++)
	{
		out << components[i];
	}
	return out;
}

double probability(vector<MakeBlock> components, MakeBlock new_com, vector<vector<string>> designs)
{
	//first, check if new_com could be connected to the existing design
	int find_orion;//if the controller is added to the design
	unsigned int orion = 0;//the controller would be components[orion]
	for (orion = 0; orion <= components.size() - 1; orion++)
	{
		if (components[orion].get_Name() == "MakeBlockOrionBoard")
		{
			find_orion = 1;
			break;
		}
	}
	if (find_orion == 0)
	{
		cout << "error: MakeBlockOrionBoard is not added to the design." << endl;
		exit(1);
	}
	int connect = components[orion].test_Connection(new_com);
	if (connect == 0) return 0;//if the new component can not be connected to the design, apparently the probability is zero.

	//second, search the database for those containing the current components
	int total = 0;//the total designs containing the current components in the database
	int contain = 0;//the number of designs containing both the current components and the new component
	for (unsigned int i = 0; i <= designs.size() - 1; i++)
	{
		int notmatch = 0;
		for (unsigned int j = 0; j <= components.size() - 1; j++)
		{
			int test = 0;
			vector<int> match(designs[i].size(), 0);
			for (unsigned int k = 0; k <= designs[i].size() - 1; k++)
			{
				if (components[j].get_Name() == designs[i][k] && match[k] == 0)
				{
					match[k] = 1;
					test = 1;
					break;
				}
			}
			if (test == 0)
			{
				notmatch = 1;
				break;
			}
		}
		if (notmatch == 1) continue;//as long as 1 component is not matched, this design is not chosen
		int test = 0;
		for (unsigned int j = 0; j <= designs[i].size() - 1; j++)
		{
			if (designs[i][j] == new_com.get_Name())//if matched, add to both [total] and [contain]
			{
				total++;
				contain++;
				test = 1;
				break;
			}
		}
		if (test == 0)//if not matched, add to [total] only
		{
			total++;
		}
	}
	
	if (total == 0)
	{
		return 0;
	}
	else
	{
		double t = (double)contain / (double)total;
		return t;
	}
}

int MakeBlock::test_Connection(MakeBlock another)
{
	int connect = 0;
	if (another.name_Object == name_Object)//two same components can not be connected to each other
	{
		return 0;
	}
	for (unsigned int i = 0; i <= another.input_Pins.size() - 1; i++)//traversal the input pins
	{
		if (connect == 1 || another.input_Pins.size() == 0) break;
		for (unsigned int j = 0; j <= another.input_Pins[i].connectable_Pins.size() - 1; j++)
		{
			if (connect == 1 || another.input_Pins[i].connectable_Pins.size() == 0) break;
			if (another.input_Pins[i].connectable_Pins[j].find(name_Object) != -1)
			{
				for (unsigned int k = 0; k <= output_Pins.size() - 1; k++)
				{
					if (output_Pins[k].name == another.input_Pins[i].connectable_Pins[j] &&
						output_Pins[k].occupied == 0)
					{
						connect = 1;
						break;
					}
				}
			}
			else//if cannot be directly connected to orionboard, recursively check the connectors
			{
				int s = another.input_Pins[i].connectable_Pins[j].find("-");
				string name = another.input_Pins[i].connectable_Pins[j].substr(0, s);
				MakeBlock newcom(name);
				if (test_Connection(newcom) == 1) connect = 1;
			}
		}
	}
	for (unsigned int i = 0; i <= another.output_Pins.size() - 1; i++)//traversal the output pins
	{
		if (connect == 1 || another.output_Pins.size() == 0) break;
		for (unsigned int j = 0; j <= another.output_Pins[i].connectable_Pins.size() - 1; j++)
		{
			if (connect == 1 || another.output_Pins[i].connectable_Pins.size() == 0) break;
			if (another.output_Pins[i].connectable_Pins[j].find(name_Object) != -1)
			{
				for (unsigned int k = 0; k <= input_Pins.size() - 1; k++)
				{
					if (input_Pins[k].name == another.output_Pins[i].connectable_Pins[j] &&
						input_Pins[k].occupied == 0)
					{
						connect = 1;
						break;
					}
				}
			}
			else//if cannot be directly connected to orionboard, recursively check the connectors
			{
				int s = another.input_Pins[i].connectable_Pins[j].find("-");
				string name = another.input_Pins[i].connectable_Pins[j].substr(0, s);
				MakeBlock newcom(name);
				if (test_Connection(newcom) == 1) connect = 1;
			}
		}
	}
	return connect;
}

vector<MakeBlock> load_all_candidates()
{
	MakeBlock x;//to get the config filename
	vector<MakeBlock> coms;//storing the candidates
	char buffer[128];
	fstream in(x.configfile, ios::in);
	if (!in.is_open())
	{
		cout << "error opening file!" << endl;
		exit(1);
	}
	while (!in.eof())//goes to the component part
	{
		in.getline(buffer, 100);
		if (strcmp(buffer, "ConnectionMap") == 0) break;
		if (strcmp(buffer, "Component") == 0)
		{
			MakeBlock t;
			in.getline(buffer, 100);//read name
			t.name_Object = buffer + 6;
			in.getline(buffer, 100);//read type
			t.type = buffer + 6;
			in.getline(buffer, 100);//read input_need
			char c = *(buffer + 7);
			t.input_need = c - '0';
			in.getline(buffer, 100);//read output_need
			c = *(buffer + 8);
			t.output_need = c - '0';

			while (!in.eof())//read pin information
			{
				pin temp;
				in.getline(buffer, 100);
				if (strcmp(buffer, "EndComponent") == 0)
				{
					break;
				}
				if (strcmp(buffer + 1, "Pin") == 0)
				{
					in.getline(buffer, 100);
					if (strcmp(buffer + 7, "Input") == 0)
					{
						in.getline(buffer, 100);
						temp.type_Pins = buffer + 17;
						in.getline(buffer, 100);
						temp.name = buffer + 7;
						in.getline(buffer, 100);
						sscanf(buffer + 11, "%lf %lf %lf", &temp.position[0], &temp.position[1], &temp.position[2]);
						t.input_Pins.push_back(temp);
					}
					else
					{
						in.getline(buffer, 100);
						temp.type_Pins = buffer + 17;
						in.getline(buffer, 100);
						temp.name = buffer + 7;
						in.getline(buffer, 100);
						sscanf(buffer + 11, "%lf %lf %lf", &temp.position[0], &temp.position[1], &temp.position[2]);
						t.output_Pins.push_back(temp);
					}
				}
			}
			coms.push_back(t);
		}
	}
	
	while (!in.eof())//load connection map
	{
		in.getline(buffer, 100);
		if (strcmp(buffer, "EndConnectionMap") == 0)
		{
			break;
		}
		if (strcmp(buffer, "") == 0) continue;
		char ss1[32], ss2[32];
		sscanf(buffer + 6, "%s %s", ss1, ss2);
		string s1(ss1);
		string s2(ss2);
		for (unsigned int j = 0; j <= coms.size() - 1; j++)//traversal the components to see if s1/s2 matches them
		{
			int test = 0;
			for (unsigned int i = 0; i <= coms[j].input_Pins.size() - 1; i++)//input
			{
				if (coms[j].input_Pins.size() == 0) break;
				if (coms[j].input_Pins[i].name == s1)
				{
					coms[j].input_Pins[i].connectable_Pins.push_back(s2);
					test = 1;
					break;
				}
				else
				{
					if (coms[j].input_Pins[i].name == s2)
					{
						coms[j].input_Pins[i].connectable_Pins.push_back(s1);
						test = 1;
						break;
					}
				}
			}
			if (test == 1) break;
		}
		for (unsigned int j = 0; j <= coms.size() - 1; j++)
		{
			int test = 0;
			for (unsigned int i = 0; i <= coms[j].output_Pins.size() - 1; i++)
			{
				if (coms[j].output_Pins.size() == 0) break;
				if (coms[j].output_Pins[i].name == s1)
				{
					coms[j].output_Pins[i].connectable_Pins.push_back(s2);
					test = 1;
					break;
				}
				else
				{
					if (coms[j].output_Pins[i].name == s2)
					{
						coms[j].output_Pins[i].connectable_Pins.push_back(s1);
						test = 1;
						break;
					}
				}
			}
			if (test == 1) break;
		}
		in.getline(buffer, 100);
		if (strcmp(buffer, "EndConnectionMap") == 0)
		{
			break;
		}
	}
	in.close();
	return coms;
}

vector<MakeBlock> delete_connectors(vector<MakeBlock> model)
{
	vector<MakeBlock> new_model;
	for (unsigned int i = 0; i <= model.size() - 1; i++)
	{
		if (model[i].type == "Connector")
		{
			continue;
		}
		else
		{
			new_model.push_back(model[i]);
		}
	}
	return new_model;
}

vector<string> MakeBlock2string(vector<MakeBlock> model)
{
	vector<string> components;
	for (unsigned int i = 0; i <= model.size() - 1; i++)
	{
		components.push_back(model[i].name_Object);
	}
	return components;
}
