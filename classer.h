#ifndef CLASSER_H
#define CLASSER_H


#include <iostream>
#include <fstream>
#include <vector>


#include "classifier.h"

class Classer{


public:

	static Classer * create(const std::vector<CClassifier::HaarOutput*> &examples);

	static Classer * createFromXML(const char* filename);

	virtual void printToXML(std::ofstream &out, int level) = 0;
	virtual CClassifier::ImageType classify(CClassifier::HaarOutput *haary, double *percent) = 0;

	virtual ~Classer() {};


	// set get the type of tree
	static void setTreeType(CClassifier::ImageType t) { treeType = t; }
	CClassifier::ImageType getTreeType() { return treeType; }


protected:

	// in future should print things like if boosted tree etc.
	void printTypeData(std::ofstream &out){

		out << "<treeType>" << treeType << "</treeType>\n";
	};

	// can only be created by its children
	Classer() {};
	
	static CClassifier::ImageType treeType;

};


#endif
