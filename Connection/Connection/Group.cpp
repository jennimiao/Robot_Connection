#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Group.h"
#include "Design.h"
#include "MakeBlock.h"
using namespace std;
#define MAX 1000 //the initial value when finding the minimum DOI

vector<Group> load_Group(string filename)
{
	vector<Group> groups;
	char buffer[128];
	fstream in(filename, ios::in);//load the group information from the file
	if (!in.is_open())
	{
		cout << "Error opening file!" << endl << endl;
		exit(1);
	}
	while (!in.eof())//go to "GroupMap"
	{
		in.getline(buffer, 100);
		if (strcmp(buffer, "GroupMap") == 0)
		{
			break;
		}
	}
	while (!in.eof())
	{
		in.getline(buffer, 100);
		if (strcmp(buffer, "EndGroupMap") == 0)
		{
			break;
		}
		if (strcmp(buffer, "Group") == 0)
		{
			Group temp_group;
			in.getline(buffer, 100);//reading name
			temp_group.name = buffer + 6;
			in.getline(buffer, 100);
			if (strcmp(buffer, "\tCertain") == 0)//if has a certain list
			{
				while (1)
				{
					in.getline(buffer, 100);
					if (strcmp(buffer, "\tEndCertain") == 0)
					{
						break;
					}
					temp_group.certainList.push_back(buffer + 2);
				}
			}
			while (1)
			{
				if (strcmp(buffer, "\tCom") == 0)//load the component list
				{
					vector<string> temp_part;
					//Part temp_part;
					while (1)
					{
						in.getline(buffer, 100);
						if (strcmp(buffer, "\tEndCom") == 0)
						{
							break;
						}
						for (int i = 0; i <= *(buffer + 2) - '1'; i++)
						{
							temp_part.push_back(buffer + 4);
						}
					}
					temp_group.parts.push_back(temp_part);
				}
				in.getline(buffer, 100);
				if (strcmp(buffer, "EndGroup") == 0)
				{
					break;
				}
			}
			groups.push_back(temp_group);
		}
	}
	return groups;
}

ostream & operator << (ostream &out, Group x)
{
	out << "Group: " << x.name<< endl;
	if (x.certainList.size() != 0)
	{
		out << "Certain list:" << endl;
		for (unsigned int i = 0; i <= x.certainList.size() - 1; i++)
		{
			cout << '\t' << x.certainList[i] << endl;
		}
	}
	for (unsigned int i = 0; i <= x.parts.size() - 1; i++)
	{
		out << "Composition " << i + 1 << ": " << endl;
		for (unsigned int j = 0; j <= x.parts[i].size() - 1; j++)
		{
			if (x.parts[i].size() == 0) break;
			out << '\t' << x.parts[i][j] << endl;
		}
		}
	return out;
}

ostream & operator << (ostream &out, vector<Group> x)
{
	for (unsigned int i = 0; i <= x.size() - 1; i++)
	{
		if (x.size() == 0) break;
		out << x[i] << endl << endl;
	}
	return out;
}

int degree_incompleteness(vector<string> components, vector<Group> groups)
{
	int min_single = MAX;//the minimum degree of incompleteness
	vector<string> temp_group_composition;//the composition of components found in step 1
	vector<vector<string>> complete_groups;//if any, record all the possible composition of complete groups
	if (components.size() == 0 || groups.size() == 0)
	{
		return 0;
	}

	//step 1: [brute-force] try to find all the complete groups, or an incomplete group with minimum doi for itself
	for (unsigned int i = 0; i <= components.size() - 1; i++)
	{
		for (unsigned int j = 0; j <= groups.size() - 1; j++)//traversal of groups
		{
			for (unsigned int k = 0; k <= groups[j].parts.size() - 1; k++)//traversal of different compositions of components in this group
			{
				vector<int> matched(components.size(), 0);//the components which are already matched
				for (unsigned int l = 0; l <= groups[j].parts[k].size() - 1; l++)//find if component[i] is in this part
				{
					if (groups[j].parts[k][l] == components[i])//if it's in this part, calculate the degree
					{
						vector<string> t;//temporary registor of temp_group_composition
						t.push_back(components[i]);
						matched[i] = 1;
						for (unsigned int m = 0; m <= groups[j].parts[k].size() - 1; m++)
						{
							if (m == l) continue;
							for (unsigned int n = 0; n <= components.size() - 1; n++)
							{
								if (matched[n] == 0 && groups[j].parts[k][m] == components[n])
								{
									matched[n] = 1;
									t.push_back(components[n]);
								}
							}
						}
						int incompleteness = groups[j].parts[k].size();
						for (unsigned int m = 0; m <= matched.size() - 1; m++)
						{
							if (matched[m] == 1)
							{
								incompleteness--;
							}
						}
						if (incompleteness < min_single)//if less than min_single, renew min_single and temp_group_composition
						{
							min_single = incompleteness;
							temp_group_composition = t;
						}
						if (incompleteness == 0)//if complete, add to the complete group list
						{
							complete_groups.push_back(t);
						}
						break;
					}
				}
			}
		}
	}

	//step 2: for complete groups, find one giving a minimum DOI to the remaining components
	//		  for incomplete group, recurse to find the DOI of the remaining components
	if (min_single == 0)//if at least one complete groups are found
	{
		int min = MAX;//minimum doi
		vector<string> best_complete_group;//the composition which gives the minimum doi
		for (unsigned int i = 0; i <= complete_groups.size() - 1; i++)//traversal different composition of complete groups
		{
			vector<int> match(components.size(), 0);
			for (unsigned int j = 0; j <= complete_groups[i].size() - 1; j++)
			{
				for (unsigned int k = 0; k <= components.size() - 1; k++)
				{
					if (complete_groups[i][j] == components[k] && match[k] == 0)
					{
						match[k] = 1;
						break;
					}
				}
			}
			vector<string> components_new;//the remaining components
			for (unsigned int j = 0; j <= components.size() - 1; j++)
			{
				if (match[j] == 0)
				{
					components_new.push_back(components[j]);
				}
			}
			int doi = degree_incompleteness(components_new, groups);//overall doi
			if (doi < min)
			{
				best_complete_group = complete_groups[i];
				min = doi;
			}
		}
		return min;
	}
	else//if no more complete groups
	{
		vector<string> components_new;//the remaining components
		vector<int> match(components.size(), 0);
		if (temp_group_composition.size() == components.size())//reach the end of recursion
		{
			return min_single;
		}
		for (unsigned int i = 0; i <= temp_group_composition.size() - 1; i++)
		{
			for (unsigned int j = 0; j <= components.size() - 1; j++)
			{
				if (temp_group_composition[i] == components[j] && match[j] == 0)//label the components which formed this complete groups
				{
					match[j] = 1;
					break;
				}
			}
		}
		for (unsigned int i = 0; i <= components.size() - 1; i++)
		{
			if (match[i] == 0)
			{
				components_new.push_back(components[i]);//push the remaining components back to components_new
			}
		}
		int doi = min_single + degree_incompleteness(components_new, groups);//the overall doi
		return doi;
	}
}

int find_1completeGroup(vector<string> components, vector<Group> groups, vector<string> *group_name, vector<int> *part_number)
{
	int min = MAX;
	vector<string> temp_group_composition;//list of the components in the complete group
	string temp_groupname;//the group name
	int temp_partnumber;//the part number of the composition in the group
	for (unsigned int i = 0; i <= components.size() - 1; i++)//traversal of input components
	{
		for (unsigned int j = 0; j <= groups.size() - 1; j++)//traversal of groups
		{
			for (unsigned int k = 0; k <= groups[j].parts.size() - 1; k++)//traversal of different compositions of components in this group
			{
				vector<int> matched(components.size(), 0);//the components which are already matched
				for (unsigned int l = 0; l <= groups[j].parts[k].size() - 1; l++)//find if component[i] is in this part
				{
					if (groups[j].parts[k][l] == components[i])//if it's in this part, calculate the degree
					{
						vector<string> t;
						int incompleteness = groups[j].parts[k].size() - 1;
						t.push_back(components[i]);
						matched[i] = 1;
						for (unsigned int m = 0; m <= groups[j].parts[k].size() - 1; m++)
						{
							if (m == l) continue;
							for (unsigned int n = 0; n <= components.size() - 1; n++)
							{
								if (matched[n] == 0 && groups[j].parts[k][m] == components[n])
								{
									matched[n] = 1;
									t.push_back(components[n]);
									incompleteness--;
								}
							}
						}
						if (incompleteness == 0)
						{			//if forms an complete group, compare the degree of incompleteness of the remaining components
							vector<string> components_new;
							for (unsigned int m = 0; m <= components.size() - 1; m++)
							{
								if (matched[m] == 0)
								{
									components_new.push_back(components[m]);
								}
							}
							incompleteness = degree_incompleteness(components_new, groups);
							if (incompleteness < min)
							{
								min = incompleteness;//renew the min, and other information
								temp_group_composition = t;
								temp_groupname = groups[j].name;
								temp_partnumber = k;
							}
						}
					}
				}
			}
		}
	}
	if (min == MAX)//no complete groups are found
	{
		return -1;
	}
	else
	{
		group_name->push_back(temp_groupname);//store the group and composition information
		part_number->push_back(temp_partnumber);
		return min;
	}
}

vector<string> form_incompleteGroup(vector<MakeBlock> model, vector<string> com_remain, vector<Group> groups, vector<string> com_add)
{
	int min = MAX;
	vector<string> temp_group_composition;//the temporary composition of the optimal groups
	int group_index;//the index of the corresponding incomplete group
	int part_index;//the index of the corresponding part of the incomplete group
	int orion = 0;//the index of orionboard in the model
	for (unsigned int i = 0; i <= model.size() - 1; i++)
	{
		if (model[i].get_Name() == "MakeBlockOrionBoard")
		{
			orion = i;
			break;
		}
	}
	for (unsigned int i = 0; i <= com_remain.size() - 1; i++)
	{
		for (unsigned int j = 0; j <= groups.size() - 1; j++)
		{
			for (unsigned int k = 0; k <= groups[j].parts.size() - 1; k++)
			{
				vector<int> match(com_remain.size(), 0);
				for (unsigned int l = 0; l <= groups[j].parts[k].size() - 1; l++)
				{
					if (groups[j].parts[k][l] == com_remain[i])
					{
						vector<string> t;//temporary registor of temp_group_composition
						t.push_back(com_remain[i]);
						match[i] = 1;
						int availability = 1;//availability = 1 if the current part could be connected to the design
						for (unsigned int m = 0; m <= groups[j].parts[k].size() - 1; m++)
						{
							if (m == l) continue;
							MakeBlock temp(groups[j].parts[k][m]);
							if (model[orion].test_Connection(temp) == 0)
							{
								availability = 0;
								break;
							}
							for (unsigned int n = 0; n <= com_remain.size() - 1; n++)
							{
								if (match[n] == 0 && groups[j].parts[k][m] == com_remain[n])
								{
									match[n] = 1;
									t.push_back(com_remain[n]);
								}
							}
						}
						if (availability == 0)
						{
							break;
						}
						int incompleteness = groups[j].parts[k].size();//doi
						for (unsigned int m = 0; m <= match.size() - 1; m++)
						{
							if (match[m] == 1)
							{
								incompleteness--;
							}
						}
						if (incompleteness < min)//if less than min, renew min_single and temp_group_composition
						{
							min = incompleteness;
							temp_group_composition = t;
							group_index = j;
							part_index = k;
						}
						break;
					}
				}
			}
		}
	}

	for (unsigned int i = 0; i <= groups[group_index].parts[part_index].size() - 1; i++)//generate the additional components needed to fill up the incomplete group
	{
		int is = 0;
		for (unsigned int j = 0; j <= temp_group_composition.size() - 1; j++)
		{
			if (temp_group_composition[j] == groups[group_index].parts[part_index][i])
			{
				is = 1;
				break;
			}
		}
		if (is == 0)
		{
			com_add.push_back(groups[group_index].parts[part_index][i]);
		}
	}

	vector<int> match(com_remain.size(), 0);
	vector<string> com_new;//the remaining components in the inputs
	for (unsigned int i = 0; i <= temp_group_composition.size() - 1; i++)
	{
		for (unsigned int j = 0; j <= com_remain.size() - 1; j++)
		{
			if (temp_group_composition[i] == com_remain[j] && match[j] == 0)
			{
				match[j] = 1;
				break;
			}
		}
	}
	for (unsigned int i = 0; i <= com_remain.size() - 1; i++)
	{
		if (match[i] == 0)
		{
			com_new.push_back(com_remain[i]);
		}
	}

	if (com_new.size() != 0)
	{
		com_add = form_incompleteGroup(model, com_new, groups, com_add);//recursively add additional components until the end of the recursion
	}

	return com_add;
}

vector<string> break_string(string parts)
{
	vector<string> coms;//breaking input string into separate parts, devided by ' '
	int p;
	while ((p = parts.find(' ')) != parts.npos)
	{
		coms.push_back(parts.substr(0, p));
		parts = parts.substr(p + 1, parts.length());
	}
	coms.push_back(parts);
	return coms;
}

vector<string> delete_matched_components(vector<string> components, vector<Group> groups, vector<string> *group_name, vector<int> *part_num)
{
	vector<string> components_new;//the result after deleting
	vector<int> match(components.size(), 0);
	for (unsigned int i = 0; i <= groups.size() - 1; i++)//traversal of groups to find the corresponding group
	{
		if (groups[i].name == (*group_name)[group_name->size() - 1])
		{
			for (unsigned int j = 0; j <= groups[i].parts[(*part_num)[group_name->size() - 1]].size() - 1; j++)
			{
				int test = 0;//check if all the components in this group are present in the input component series
				for (unsigned int k = 0; k <= components.size() - 1; k++)
				{
					if (groups[i].parts[(*part_num)[group_name->size() - 1]][j] == components[k] && match[k] == 0)
					{
						match[k] = 1;
						test = 1;
						break;
					}
				}
				if (test == 0)
				{
					cout << "Error: not an complete group!" << endl << endl;
					return components;
				}
			}
			break;
		}
	}

	for (unsigned int i = 0; i <= components.size() - 1; i++)
	{
		if (match[i] == 0)
		{
			components_new.push_back(components[i]);
		}
		//else
		//{
		//	cout << components[i] << ' ';
		//}
	}
	cout << endl;
	return components_new;
}

vector<string> missing_components(vector<Group> allgroups, vector<string> coms_remain, vector<string> all_com)
{			 //total recursion times: the product of the number of combination ways in all the groups
	vector<string> temp(1000, "");
	vector<string> all_com2;//a copy of all_com to protect it from changing
	if (allgroups.size() == 1)//the end of recursion
	{
		for (unsigned int i = 0; i <= allgroups[0].parts.size() - 1; i++)
		{         //traversal different compositions in the suggested group to find one with least missing components
			all_com2 = all_com;
			for (unsigned int j = 0; j <= allgroups[0].parts[i].size() - 1; j++)
			{
				all_com2.push_back(allgroups[0].parts[i][j]);//add the current components in this combination to all_com2
			}
			vector<string> t = substract(all_com2, coms_remain);//substract coms_remain from all_com2 to get the missing components
			if (t.size() < temp.size())
			{
				temp = t;//renew temp
			}
		}
	}
	else//still in the recursion loop
	{
		Group g = allgroups[allgroups.size() - 1];//from the last group
		allgroups.pop_back();
		for (unsigned int i = 0; i <= g.parts.size() - 1; i++)
		{			 //traversal different compositions in the suggested group to find one with least missing components
			all_com2 = all_com;
			for (unsigned int j = 0; j <= g.parts[i].size() - 1; j++)
			{
				all_com2.push_back(g.parts[i][j]); // add the current components in this combination to all_com2
			}
			vector<string> t = missing_components(allgroups, coms_remain, all_com2);//the missing comopnents to be added
			if (t.size() < temp.size())
			{
				temp = t;//renew temp
			}
		}
	}
	return temp;
}

vector<string> substract(vector<string> all, vector<string> useless)
{
	vector<int> match(all.size(), 0);
	vector<string> result;//result = all - useless
	if (all.size() == 0 || useless.size() == 0)
	{
		return all;
	}
	for (unsigned int i = 0; i <= useless.size() - 1; i++)
	{
		for (unsigned int j = 0; j <= all.size() - 1; j++)
		{
			if (useless[i] == all[j] && match[j] == 0)
			{
				match[j] = 1;
				break;
			}
		}
	}
	for (unsigned int i = 0; i <= all.size() - 1; i++)
	{
		if (match[i] == 0)
		{
			result.push_back(all[i]);
		}
	}
	return result;
}

vector<vector<string>> sort_components(vector<vector<string>> coms)
{
	vector<vector<string>> coms_new;
	for (unsigned int i = 0; i <= coms.size() - 2; i++)//bubble sort, sort by the number of missing components
	{
		if (coms.size() == 1) break;
		for (unsigned int j = i + 1; j <= coms.size() - 1; j++)
		{
			if (coms[i].size() > coms[j].size())
			{
				vector<string> temp;
				temp = coms[i];
				coms[i] = coms[j];
				coms[j] = temp;
			}
		}
	}

	for (unsigned int i = 0; i <= coms.size() - 1; i++)//merge those with the same name, and add a number after each component
	{
		vector<int> match(coms[i].size(), 0);
		for (unsigned int j = 1; j <= coms[i].size() - 1; j++)
		{
			if (match[j] == -1) continue;
			match[j] = 1;
			for (unsigned int k = 1; k <= coms[i].size() - 1; k++)
			{
				if (coms[i][j] == coms[i][k] && k != j)
				{
					match[j]++;
					match[k] = -1;
				}
			}
		}
		vector<string> temp_com(1, coms[i][0]);
		for (unsigned int j = 1; j <= coms[i].size() - 1; j++)
		{
			if (match[j] == -1) continue;
			temp_com.push_back(coms[i][j] + " " + char((int)'0' + match[j]));
		}
		coms_new.push_back(temp_com);
	}
	return coms_new;
}

double similarity_groups(vector<MakeBlock> model, Group group, vector<vector<string>> *group_part)
{
	double simi = 0.0;
	vector<string> components = MakeBlock2string(model);//components in the current design
	vector<string> temp_part;//the optimal composition of component in this group
	int orion = 0;//the index of orionboard in the design
	for (unsigned int i = 0; i <= model.size() - 1; i++)
	{
		if (model[i].get_Name() == "MakeBlockOrionBoard")
		{
			orion = i;
			break;
		}
	}
	for (unsigned int i = 0; i <= group.parts.size() - 1; i++)//traversal all the parts in the groups to find one with the highest [simi]
	{
		vector<int> match(components.size(), 0);
		int count = 0;//number of components both in the design and in the candidate group
		for (unsigned int j = 0; j <= group.parts[i].size() - 1; j++)
		{
			int exist = 0;//if the component groups.parts[i][j] is present in the current design
			for (unsigned int k = 0; k <= components.size() - 1; k++)
			{
				if (group.parts[i][j] == components[k] && match[k] == 0)
				{
					match[k] = 1;
					count++;
					exist = 1;
					break;
				}
			}
			if (exist == 0)//if not in the current design, and it cannot be connected to the orionboard, the group would be useless
			{
				MakeBlock temp(group.parts[i][j]);
				if (model[orion].test_Connection(temp) == 0)
				{
					count = 0;
					break;
				}
			}
		}
		if (((double)count / (double)group.parts[i].size()) > simi)
		{
			simi = (double)count / (double)group.parts[i].size();
			temp_part = group.parts[i];
		}
	}
	group_part->push_back(temp_part);
	return simi;
}

double probability_groups(Group group, vector<Design> designs)
{
	int total = 0;//the total group numbers in all the designs
	int times = 0;//the total times this group appear
	for (unsigned int i = 0; i <= designs.size() - 1; i++)
	{
		for (unsigned int j = 0; j <= designs[i].get_parts().size() - 1; j++)
		{
			for (unsigned int k = 0; k <= designs[i].get_parts()[j].size() - 1; k++)
			{
				total++;
				if (designs[i].get_parts()[j][k] == group.name)
				{
					times++;
				}
			}
		}
	}
	return ((double)times / (double)total);
}

void suggest_components_by_probability(string designfilename, vector<MakeBlock> model)
{
	model = delete_connectors(model);//delete the connectors from the current component list, which have little to do with functions
	vector<MakeBlock> candidate = load_all_candidates();//load the candidates to be suggested
	candidate = delete_connectors(candidate);//delete the connectors from the candidate list
	vector<Group> groups = load_Group(designfilename);//load group information
	vector<Design> designs = load_Design(designfilename);//load design information

	vector<double> compatibility(groups.size(), 0);
	vector<vector<string>> *group_part = new vector<vector<string>>;//the composition of each group that gives the highest compatibility
	for (unsigned int i = 0; i <= groups.size() - 1; i++)
	{
		if (probability_groups(groups[i], designs) != 0)
		{
			compatibility[i] = similarity_groups(model, groups[i], group_part) / probability_groups(groups[i], designs);
		}
	}

	for (unsigned int i = 0; i <= groups.size() - 2; i++)//use bubble to sort the groups by their compatibilities
	{
		if (groups.size() == 1 || groups.size() == 0) break;
		for (unsigned int j = i + 1; j <= groups.size() - 1; j++)
		{
			if (compatibility[i] <compatibility[j])
			{
				double t1 = compatibility[i];
				compatibility[i] = compatibility[j];
				compatibility[j] = t1;
				vector<string> t2 = (*group_part)[i];
				(*group_part)[i] = (*group_part)[j];
				(*group_part)[j] = t2;
				Group t3 = groups[i];
				groups[i] = groups[j];
				groups[j] = t3;
			}
		}
	}

	if (compatibility[0] == 0)//if the highest compatibility is zero, then no suggestion could be made
	{
		cout << char(4) << " No suggestion availble." << endl << endl << endl;
	}
	else
	{
		cout << char(4) << " Suggest groups of components by probabilities:" << endl;
		for (unsigned int i = 0; i <= groups.size() - 1; i++)
		{
			if ((*group_part)[i].size() == 0) break;
			cout << endl << groups[i].name << " with group compatibility " << compatibility[i] << " based on the current design." << endl;
			for (unsigned int j = 0; j <= (*group_part)[i].size() - 1; j++)
			{
				cout << '\t' << (*group_part)[i][j] << endl;
			}
		}
		cout << endl << endl;
	}
}

void suggest_components_by_completeness(string designfilename, vector<MakeBlock> model)
{
	model = delete_connectors(model);//delete the connectors from the current component list, which have little to do with functions
	vector<string> components = MakeBlock2string(model);
	vector<MakeBlock> candidate = load_all_candidates();//load the candidates to be suggested
	candidate = delete_connectors(candidate);//delete the connectors from the candidate list
	vector<Group> groups = load_Group(designfilename);//load group information
	vector<Design> designs = load_Design(designfilename);//load design information

	vector<string> *group_name = new vector<string>;
	vector<int> *part_number = new vector<int>;
	int p = find_1completeGroup(components, groups, group_name, part_number);
	while (p != -1)
	{
		components = delete_matched_components(components, groups, group_name, part_number);
		p = find_1completeGroup(components, groups, group_name, part_number);
	}

	//suggest as few components as possible to form complete groups
	vector<string> coms_additional;
	coms_additional = form_incompleteGroup(model, components, groups, coms_additional);

	cout << char(4) << " Suggest components by the completeness of groups:" << endl << endl;
	for (unsigned int i = 0; i <= coms_additional.size() - 1; i++)
	{
		cout << coms_additional[i] << endl;
	}
	cout << endl << endl << endl;
}
