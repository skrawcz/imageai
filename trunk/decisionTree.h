#ifndef DECISIONTREE_H
#define DECISIONTREE_H


#include <iostream>
#include <fstream>
#include <vector>

#include "classifier.h"

class DecisionTree{


public:

	// just create
	DecisionTree(std::vector<CClassifier::HaarOutput*> examples, std::vector<bool> attribs, float majority);

	// create from xml file
	DecisionTree(std::ifstream &in);

	~DecisionTree();

	//DecisionTree* learn(){return NULL;};

	void print(std::ofstream &out);

	int chooseAttribute(const std::vector<CClassifier::HaarOutput*> &examples,const std::vector<bool> &attribs);


private:

	bool sameClassification(const std::vector<CClassifier::HaarOutput*> &examples);


	std::vector<DecisionTree*> children;

	float majorityPercent;
	int haarType;
	bool isLeaf;
	CClassifier::ImageType majorityType;



};


#endif
