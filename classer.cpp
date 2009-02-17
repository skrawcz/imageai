
#include "classer.h"

#include "decisionTree.h"
#include "multipleDecisionTree.h"
#include "CXMLParser.h"

#include <iostream>
#include <fstream>
#include <sstream>

Features::ImageType Classer::treeType = Features::MUG;

Classer * Classer::create(const std::vector<Features::HaarOutput*> &examples){

	// should come from config file
	std::string type("SingleDecisionTree");

	// create a vector of attributes, ie the different haar features.
	std::vector<bool> attribs;

	for(unsigned i=0;i<HAARAMOUNT;++i)
		attribs.push_back(true);

	//if(type == "SingleDecisionTree"){

		return new DecisionTree(examples, attribs, -1, Features::OTHER, 0);
	//}else{
	//	return new MultipleDecisionTree(examples, attribs);
	//}

}

Classer * Classer::createFromXML(const char* filename){

	// should come from config file
	std::string type("SingleDecisionTree");

	std::ifstream ifs ( filename, std::ifstream::in );

	std::string current;

	//if(type == "SingleDecisionTree"){
		// figure out what type of things the tree classifies
		CXMLParser::getNextValue(ifs, current);
		treeType = (Features::ImageType)atoi(current.c_str());
		getline(ifs, current);

		std::cout << treeType << std::endl;

		if(current.find("node") != std::string::npos)
			return new DecisionTree(ifs, true);
		else
			return new DecisionTree(ifs, false);
	
	//}else{

	//	return new MultipleDecisionTree(ifs, false);
	//}

}
