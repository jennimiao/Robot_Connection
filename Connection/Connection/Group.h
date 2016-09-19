#pragma once
#include <vector>
#include <string>
#include "MakeBlock.h"
#include "Design.h"
using namespace std;

class Group
{
protected:
	string name;//name of the group
	vector<vector<string>> parts;//possible combinations of components
	vector<string> certainList;//the list of component names which features this group

public:
	//primary functions
	friend void suggest_components_by_probability(string designfilename, vector<MakeBlock> model);//suggest groups of components, based on the probalistic model
	friend void suggest_components_by_completeness(string designfilename, vector<MakeBlock> model);//suggest components to fill up the incomplete groups

	//aid functions of suggest_components_by_probability
	friend double similarity_groups(vector<MakeBlock> components, Group group, vector<vector<string>> *group_part);//the similarity between a candidate group and the current design
	friend double probability_groups(Group group, vector<Design> designs);//the probability of a certain group in the database
	friend vector<string> form_incompleteGroup(vector<MakeBlock> model, vector<string> com_remain, vector<Group> groups, vector<string> com_add);//suggest as few components as possible to form complete groups

	//aid functions of suggest_components_by_completeness
	friend vector<Group> load_Group(string filename);//load all the groups from file
	friend int degree_incompleteness(vector<string> components, vector<Group> groups);//return the degree of incompleteness
	friend int find_1completeGroup(vector<string> components, vector<Group> groups, vector<string> *group_name, vector<int> *part_number);//find complete groups, one at a time
	friend vector<string> delete_matched_components(vector<string> components, vector<Group> groups, vector<string> *group_name, vector<int> *part_num);//delete the components, which already formed a complete group, from the input component list
	//friend vector<vector<string>> suggest_coms(vector<vector<string>> groups_sug, vector<string> coms_remain, vector<Group> groups);//suggest additional components based on the suggested groups
	friend vector<string> missing_components(vector<Group> allgroups, vector<string> coms_remain, vector<string> all_com);//recursively find a combination of groups, which needs least additional components
	
	//output
	friend ostream & operator << (ostream &out, Group x);//overload of <<
	friend ostream & operator << (ostream &out, vector<Group> x);//overload of <<

};

//other aid functions
vector<string> break_string(string parts);//break the input long string into several substrings by ' '
vector<string> substract(vector<string> all, vector<string> useless);//substract elements in <useless> from <all>. elements in <useless> but absent in <all> are ignored.
vector<vector<string>> sort_components(vector<vector<string>> coms);//sort the suggested components by numbers



