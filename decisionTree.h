#ifndef DECISIONTREE_H
#define DECISIONTREE_H


#include <iostream>
#include <fstream>
#include <vector>

#include "classifier.h"

#define THRESHOLDVALS 9

class DecisionTree{


public:

	// just create
	DecisionTree(std::vector<CClassifier::HaarOutput*> examples, std::vector<bool> attribs, 
							 float percent, CClassifier::ImageType type);

	// create from xml file
	DecisionTree(std::ifstream &in);

	~DecisionTree();

	//DecisionTree* learn(){return NULL;};

	void print(std::ofstream &out, int level);

	double chooseAttribute(const std::vector<CClassifier::HaarOutput*> &examples,const std::vector<bool> &attribs, int &bestAttribute, double &bestThreshold);

	static void setTreeType(CClassifier::ImageType t) { treeType = t; }


private:

	bool sameClassification(const std::vector<CClassifier::HaarOutput*> &examples);
	bool isAttribsEmpty(const std::vector<bool> &attribs);
	void setMajorityValues(const std::vector<CClassifier::HaarOutput*> &examples);
	long double EntropyFunc(long double p);


	std::vector<DecisionTree*> children;

	float majorityPercent;
	CClassifier::ImageType majorityType;
	bool isLeaf;
	

	static CClassifier::ImageType treeType;

};


#endif
