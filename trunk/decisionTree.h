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
	DecisionTree(CvMat *examples, CvMat *imageTypes, std::vector<bool> attribs, 
							 float percent,Features::ImageType type, int depth, Features::ImageType treeType);

	// create from xml file
	DecisionTree(std::ifstream &in, bool isNode, Features::ImageType treeType);

	~DecisionTree();

	Features::ImageType classify(CvMat *imageData, double &percent);



	double chooseAttribute(CvMat *examples, CvMat *imageTypes, const std::vector<bool> &attribs, int &bestAttribute, double &bestThreshold);

	void print(std::ofstream &out, int level);

private:

	bool sameClassification(CvMat *imageTypes, Features::ImageType &tmpType);
	bool isAttribsEmpty(const std::vector<bool> &attribs);
	bool doneClassifying(CvMat *imageTypes);

	void setMajorityValues(CvMat *imageTypes);
	long double EntropyFunc(long double p);


	std::vector<DecisionTree*> children;

	float majorityPercent;
	Features::ImageType majorityType, treeType;
	bool isLeaf;
	int attribute;
	double threshold;
	

};


#endif
