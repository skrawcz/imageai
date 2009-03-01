
#include "classer.h"

#include "decisionTree.h"
#include "multipleDecisionTree.h"
#include "CXMLParser.h"

#include <iostream>
#include <fstream>
#include <sstream>

Classer::ClassifierType Classer::classifierType = Classer::SINGLE;

Classer * Classer::create(const std::vector<Features::HaarOutput*> &examples, Features::ImageType t){

	
	findClassifierTypeFromCFG();

	// create a vector of attributes, ie the different haar features.
	std::vector<bool> attribs;

	for(unsigned i=0;i<HAARAMOUNT;++i)
		attribs.push_back(true);

	if(classifierType == SINGLE){

		t = findTreeTypeFromCFG();

		return new DecisionTree(examples, attribs, -1, Features::OTHER, 0, t);
	}else{
		return new MultipleDecisionTree(examples, attribs);
	}

}

Classer * Classer::createFromXML(const char* filename){

	std::ifstream ifs ( filename, std::ifstream::in );
	std::string current;

	// figure out what type of classifier we are using
	CXMLParser::getNextValue(ifs, current);
	classifierType = (ClassifierType)atoi(current.c_str());

	if(classifierType == SINGLE){
		
		// figure out what type of things the tree classifies
		CXMLParser::getNextValue(ifs, current);
		Features::ImageType treeType = (Features::ImageType)atoi(current.c_str());
		getline(ifs, current);

		if(current.find("node") != std::string::npos)
			return new DecisionTree(ifs, true, treeType);
		else
			return new DecisionTree(ifs, false, treeType);
	
	}else{

		return new MultipleDecisionTree(ifs, false);
	}

}

bool Classer::printToXML(const char *filename, Classer *t){		

	std::ofstream ofs ( filename );



	t->print(ofs, 0);
  

	ofs.close();


	return true;
}

Features::ImageType Classer::findTreeTypeFromCFG(){
	// should come from config file
	
	return Features::MUG;

}


void Classer::findClassifierTypeFromCFG(){
	// should come from config file
	std::string type;
	type = "multipledecisiontree";

	if(type.find("multiple") != -1){
		classifierType = MULTIPLE;
	}else{// if(type.find("single")){
		classifierType = SINGLE;
	}
}










