#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Group.h"
#include "Design.h"
#include "MakeBlock.h"
using namespace std;
#define MAX 1000

string Design::get_name()
{
	return name;
}

vector<vector<string>> Design::get_parts()
{
	return parts;
}

vector<Design> load_Design(string filename)
{
	vector<Design> designs;
	char buffer[128];
	fstream in(filename, ios::in);//load the group information from the file
	if (!in.is_open())
	{
		cout << "Error opening file!" << endl << endl;
		exit(1);
	}
	while (!in.eof())//go to design map
	{
		in.getline(buffer, 100);
		if (strcmp(buffer, "DesignMap") == 0)
		{
			break;
		}
	}
	while (!in.eof())
	{
		in.getline(buffer, 100);
		if (strcmp(buffer, "Design") == 0)
		{
			Design temp_design;
			in.getline(buffer, 100);//loading name
			temp_design.name = buffer + 6;
			while (1)
			{
				in.getline(buffer, 100);
				if (strcmp(buffer, "EndDesign") == 0)//the end of the current design
				{
					break;
				}
				if (strcmp(buffer, "\tGroup") == 0)//continuously add the compositions of groups
				{
					vector<string> temp_part;
					while (1)
					{
						in.getline(buffer, 100);
						if (strcmp(buffer, "\tEndGroup") == 0)
						{
							break;
						}
						for (int i = 0; i <= *(buffer + 2) - '1'; i++)
						{
							temp_part.push_back(buffer + 4);
						}
					}
					temp_design.parts.push_back(temp_part);
				}
			}
			designs.push_back(temp_design);
		}
		if (strcmp(buffer, "EndDesignMap") == 0)
		{
			break;
		}
	}
	return designs;
}

ostream & operator << (ostream &out, Design x)
{
	out << "Design: " << x.name << endl;
	if (x.parts.size() == 1)
	{
		for (unsigned int i = 0; i <= x.parts[0].size() - 1; i++)
		{
			if (x.parts[0].size() == 0) break;
			out << "\t" << x.parts[0][i] << endl;
		}
	}
	else
	{
		for (unsigned int i = 0; i <= x.parts.size() - 1; i++)
		{
			out << "Composition " << i + 1 << ": " << endl;
			for (unsigned int j = 0; j <= x.parts[i].size() - 1; j++)
			{
				if (x.parts[i].size() == 0) break;
				out << '\t' << x.parts[i][j] << endl;
			}
		}
	}
	out << endl << endl;
	return out;
}

ostream & operator << (ostream &out, vector<Design> x)
{
	for (unsigned int i = 0; i <= x.size() - 1; i++)
	{
		if (x.size() == 0) break;
		out << x[i] << endl << endl;
	}
	return out;
}

