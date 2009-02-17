#ifndef DECISIONTREE_H
#define DECISIONTREE_H


#include <iostream>
#include <fstream>
#include <vector>

#include "classifier.h"
#include "features.h"
#include "classer.h"



#define THRESHOLDVALS 9

class DecisionTree : public Classer{


public:

	// just create
	DecisionTree(std::vector<Features::HaarOutput*> examples, std::vector<bool> attribs, 
							 float percent,Features::ImageType type, int depth);

	// create from xml file
	DecisionTree(std::ifstream &in, bool isNode);

	~DecisionTree();

	Features::ImageType classify(Features::HaarOutput *haary, double *percent);

	void printToXML(std::ofstream &out, int level);

	double chooseAttribute(const std::vector<Features::HaarOutput*> &examples,const std::vector<bool> &attribs, int &bestAttribute, double &bestThreshold);


private:

	bool sameClassification(const std::vector<Features::HaarOutput*> &examples);
	bool isAttribsEmpty(const std::vector<bool> &attribs);
	void setMajorityValues(const std::vector<Features::HaarOutput*> &examples);
	long double EntropyFunc(long double p);


	std::vector<DecisionTree*> children;

	float majorityPercent;
	Features::ImageType majorityType;
	bool isLeaf;
	int attribute;
	double threshold;
	

};


#endif
