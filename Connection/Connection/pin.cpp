#include <iostream>
#include <string>
#include <fstream>
#include "MakeBlock.h"
#include "pin.h"
using namespace std;

pin::pin()
{
	name = "";
	position[0] = 0.0;
	position[1] = 0.0;
	position[2] = 0.0;
	occupied = 0;
	connected_Pin = "";
	type_Pins = "";
}

string pin::get_name()
{
	return name;
}

vector<string> pin::get_connectable()
{
	return connectable_Pins;
}

int pin::get_occupied()
{
	return occupied;
}

string pin::get_connected()
{
	return connected_Pin;
}

string pin::get_type()
{
	return type_Pins;
}

vector<string> connectable_Components(vector<pin> pins)
{
	int flag = 0;//to see if the current candidate has already been added to the list
	vector<string> sug_pin;//the possible pins to be connected
	vector<string> sug_com;//the component which sug_pin belong to

	for (unsigned int i = 0; i <= pins.size() - 1; i++)//traversal of pins
	{
		if (pins.size() == 0) break;
		if (pins[i].occupied == 0)//if a pin is availbale, add the connectable pins to sug_pin
		{
			for (unsigned int j = 0; j <= pins[i].connectable_Pins.size() - 1; j++)
			{
				if (pins[i].connectable_Pins.size() == 0) break;
				int same = 0;//check if the same candidate already exists
				for (unsigned int k = 0; k <= sug_pin.size() - 1; k++)
				{
					if (sug_pin.size() == 0) break;
					if (sug_pin[k] == pins[i].connectable_Pins[j])
					{
						same = 1;
						break;
					}
				}
				if (same == 0)
				{
					sug_pin.push_back(pins[i].connectable_Pins[j]);
				}
			}
		}
	}
	//Now we have a list of possible pins to be added. 
	//Next we have to transfer them into components to be more user-friendly
	string com_name;//temporary component name;
	int seq;
	for (unsigned int i = 0; i <= sug_pin.size() - 1; i++)
	{
		if (sug_pin.size() == 0) break;
		seq = sug_pin[i].find("-");
		com_name = sug_pin[i].substr(0, seq);
		int same = 0;//check if the component is already added
		for (unsigned int j = 0; j <= sug_com.size() - 1; j++)
		{
			if (sug_com.size() == 0) break;
			if (sug_com[j] == com_name)
			{
				same = 1;
				break;
			}
		}
		if (same == 0)
		{
			sug_com.push_back(com_name);
		}
	}
	return sug_com;
}

int connected_Pins(vector<pin> pins)
{
	int count = 0;//how many pins are already connected to another pin
	for (unsigned int i = 0; i <= pins.size() - 1; i++)
	{
		if (pins.size() == 0) break;
		if (pins[i].occupied == 1)
		{
			count++;
		}
	}
	return count;
}
