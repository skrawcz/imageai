#ifndef DECISIONTREE_H
#define DECISIONTREE_H


#include <iostream>
#include <fstream>
#include <vector>

#include "classer.h"

class Features;

#define THRESHOLDVALS 9

class DecisionTree : public Classer{


public:

	// just create
	DecisionTree(std::vector<Features::HaarOutput*> examples, std::vector<bool> attribs, 
							 float percent,Features::ImageType type, int depth, Features::ImageType treeType);

	// create from xml file
	DecisionTree(std::ifstream &in, bool isNode, Features::ImageType treeType);

	~DecisionTree();

	Features::ImageType classify(Features::HaarOutput *haary, double &percent);



	double chooseAttribute(const std::vector<Features::HaarOutput*> &examples,const std::vector<bool> &attribs, int &bestAttribute, double &bestThreshold);

	void print(std::ofstream &out, int level);

private:

	bool sameClassification(const std::vector<Features::HaarOutput*> &examples);
	bool isAttribsEmpty(const std::vector<bool> &attribs);
	void setMajorityValues(const std::vector<Features::HaarOutput*> &examples);
	long double EntropyFunc(long double p);


	std::vector<DecisionTree*> children;

	float majorityPercent;
	Features::ImageType majorityType, treeType;
	bool isLeaf;
	int attribute;
	double threshold;
	

};


#endif
