#ifndef CLASSER_H
#define CLASSER_H


#include <iostream>
#include <fstream>
#include <vector>

#include "features.h"

class Classer{


public:

	static Classer * create(CvMat *examples, CvMat *imageTypes, Features::ImageType t = Features::MUG);
	static Classer * createFromXML(const char* filename);
	static bool printToXML(const char *filename, Classer *t);

	virtual Features::ImageType classify(CvMat *imageData, double *percent) = 0;

	virtual ~Classer() {};

	enum ClassifierType{
		SINGLE,
		MULTIPLE,
		BOOST,
		RANDOM
	};



protected:

	virtual void print(const char *filename) = 0;

	// in future should print things like if boosted tree etc.
	void printGeneralInfo(std::ofstream &out){

		out << "<classifierType>" << classifierType << "</classifierType>\n";
	};

	static ClassifierType classifierType;



	// can only be created by its children
	Classer() {};

private:

	static void findClassifierTypeFromCFG();
	static Features::ImageType findTreeTypeFromCFG();

};


#endif
