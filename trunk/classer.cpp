
#include "classer.h"

#include "boostTree.h"
#include "randomTree.h"
//#include "multipleDecisionTree.h"
#include "CXMLParser.h"
#include "CfgReader.h"

#include <iostream>
#include <fstream>
#include <sstream>


/*
 * This class is the parent class for our classifiers.
 * 
 *
 *
 *
 */

Classer::ClassifierType Classer::classifierType = Classer::SINGLE;

//this creates a new classifier from scratch
Classer * Classer::create(CvMat *examples, CvMat *imageTypes, Features::ImageType t){


	findClassifierTypeFromCFG();


	if(classifierType == BOOST){



 		return new BoostTree(examples, imageTypes);



	}else if(classifierType = RANDOM){
		
		return new RandomTree(examples,imageTypes);

	}

	return NULL;

}

//this loads a classifier in
Classer * Classer::createFromXML(const char* filename){

	findClassifierTypeFromCFG();

	if(classifierType == BOOST){

		return new BoostTree(filename);

	}else if(classifierType = RANDOM){
		
		return new RandomTree(filename);

	}

	return NULL;

}

//this saves the trees
bool Classer::printToXML(const char *filename, Classer *t){		


	t->print(filename);

	return true;
}

//this find the tree type for boosting.
Features::ImageType Classer::findTreeTypeFromCFG(){

	std::string type;
	type = CfgReader::getValue("treeType");
	std::cout << "Type of single tree is: " << Features::stringToImageType(type) << std::endl;	
	return Features::stringToImageType(type);

}

//this method finds the classifier type from the config file
void Classer::findClassifierTypeFromCFG(){
	
	std::string type;
	type = CfgReader::getValue("classifierType");

	if(type.find("multiple") != std::string::npos){
		classifierType = MULTIPLE;
	}else if(type.find("boost") != std::string::npos){
		classifierType = BOOST;
	}else if(type.find("random") != std::string::npos){
		classifierType = RANDOM;
	}else{// if(type.find("single")){
		classifierType = SINGLE;
	}
}










