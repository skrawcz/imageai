
#include "classer.h"

#include "boostTree.h"
//#include "multipleDecisionTree.h"
#include "CXMLParser.h"
#include "CfgReader.h"

#include <iostream>
#include <fstream>
#include <sstream>



Classer::ClassifierType Classer::classifierType = Classer::SINGLE;

Classer * Classer::create(CvMat *examples, CvMat *imageTypes, Features::ImageType t){


	findClassifierTypeFromCFG();




	if(classifierType == BOOST){



 		return new BoostTree(examples, imageTypes);



	}

	/*

	// create a vector of attributes, ie the different haar features.
	std::vector<bool> attribs;

	for(int i=0;i<Features::amountOfFeatures();++i)
		attribs.push_back(true);

	if(classifierType == SINGLE){

		t = findTreeTypeFromCFG();

		return new DecisionTree(examples, imageTypes, attribs, -1, Features::OTHER, 0, t);
	}else{
		return new MultipleDecisionTree(examples, imageTypes, attribs);
	}
	*/


	return NULL;

}

Classer * Classer::createFromXML(const char* filename){

	std::ifstream ifs ( filename, std::ifstream::in );
	std::string current;

	// figure out what type of classifier we are using
	CXMLParser::getNextValue(ifs, current);
	classifierType = (ClassifierType)atoi(current.c_str());


	if(classifierType == BOOST){



	}

/*
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
*/

	return NULL;

}

bool Classer::printToXML(const char *filename, Classer *t){		


	t->print(filename);



	return true;
}

Features::ImageType Classer::findTreeTypeFromCFG(){

	std::string type;
	type = CfgReader::getValue("treeType");
	std::cout << "Type of single tree is: " << Features::stringToImageType(type) << std::endl;	
	return Features::stringToImageType(type);

}


void Classer::findClassifierTypeFromCFG(){
	// should come from config file
	std::string type;
	type = CfgReader::getValue("classifierType");


	std::cout << "Type of tree to create is: " << type << std::endl;

	if(type.find("multiple") != std::string::npos){
		classifierType = MULTIPLE;
	}else if(type.find("boost") != std::string::npos){
		classifierType = BOOST;
	}else{// if(type.find("single")){
		classifierType = SINGLE;
	}
}










