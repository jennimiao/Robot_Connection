#include <iostream>
#include "MakeBlock.h"
#include "pin.h"
#include "Group.h"

int main()
{
	string designfile = "design.txt";
	string assemblyfile = "assembly_test.txt";

	vector<MakeBlock> model = load_Assembly(assemblyfile);//load components and build the connections

	suggest_components_by_completeness(designfile, model);//suggest new groups based on compatibility

	//suggest_Components(model, "Sensor");//suggestion based on actuators and sensors, including the full paths to OrionBoard
	//suggest_Components(model, "Actuator");
	//demo_Connection("DCMotor");//demonstration of all the possible paths to OrionBoard

	return 0;
}

