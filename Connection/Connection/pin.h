#pragma once
using namespace std;
#include <string>
#include <vector>

class pin //The class of pins on the components.
{
protected:
	string name;//name of the pin
	double position[3]; //3D coordinate of the pin on the component
	vector<string> connectable_Pins;//the list of pins which can be connected to this pin
	int occupied;//occupied = 0 means the pin is currently available
	string connected_Pin; // if occupied = 1, con_Pin is the name of the connected pin
	string type_Pins;//the connection type of the pin(RJ25, 3PINS, 2PINS...)

public:
	friend class MakeBlock;
	pin();
	string get_name();
	vector<string> get_connectable();
	int get_occupied();
	string get_connected();
	string get_type();
	void friend build_Connection(MakeBlock *x, MakeBlock *y);//build a connection between 2 components, picking ports in sequence
	void friend build_Connection(MakeBlock *x, string x_pin, MakeBlock *y, string y_pin);//build a connection between 2 components, through the specified pins
	friend int connected_Pins(vector<pin> pins);//given the head of the pin list, show how many pins are already connected
	friend vector<string> connectable_Components(vector<pin> pins);//given the head of the pin list, return the connectable components
	friend vector<MakeBlock> load_all_candidates();
};