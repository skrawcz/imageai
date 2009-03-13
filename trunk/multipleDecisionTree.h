#ifndef MULTIPLEDECISIONTREE_H
#define MULTIPLEDECISIONTREE_H

#include <iostream>
#include <fstream>
#include <vector>

#include "classer.h"

class Features;
class DecisionTree;
#define THRESHOLDVALS 9

class MultipleDecisionTree : public Classer{


public:

	// just create
	MultipleDecisionTree(CvMat *examples, CvMat *imageTypes, std::vector<bool> attribs);

	// create from xml file
	MultipleDecisionTree(std::ifstream &in, bool isNode);

	~MultipleDecisionTree();

	Features::ImageType classify(CvMat *imageData, double &percent);

	void print(std::ofstream &out, int level);



private:

	
	std::vector<DecisionTree*> singleTrees;

};


#endif
