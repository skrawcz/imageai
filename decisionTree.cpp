

#include "decisionTree.h"
#include <math.h>

#include <algorithm>


CClassifier::ImageType DecisionTree::treeType = CClassifier::MUG;

DecisionTree::DecisionTree(std::vector<CClassifier::HaarOutput*> examples, std::vector<bool> attribs, 
													 float percent, CClassifier::ImageType type) 

: majorityPercent(percent) , 
	majorityType(type),
	isLeaf(false)

{

	if(examples.size() == 0){
		isLeaf = true;

	}

	else if(sameClassification(examples)){

		isLeaf = true;
		
		// value something like 100 % of something :S
		majorityPercent = 1;
		majorityType = examples.at(0)->type;


	}

	else if(isAttribsEmpty(attribs)){

		isLeaf = true;
		setMajorityValues(examples);

	// now its established that we are not a leaf but a beautiful treeeee
	}else{
		int bestAttribute;
		double bestThreshold;
		chooseAttribute(examples, attribs, bestAttribute, bestThreshold);




	}
}

DecisionTree::DecisionTree(std::ifstream &in){




}
DecisionTree::~DecisionTree(){

	for(unsigned i=0;i<children.size();++i)
			delete children[i];


}
void DecisionTree::print(std::ofstream &out, int level){

	std::string tabStr;

	for(int i=0;i<=level;++i)
		tabStr.append("\t");

	out << tabStr << "<node>\n";
	out << tabStr << "\t<isLeaf>" << isLeaf << "</isLeaf>\n";




	for(unsigned i=0;i<children.size();++i)
			children[i]->print(out, level + 1);

	out << tabStr << "</node>\n";

}

void DecisionTree::chooseAttribute(const std::vector<CClassifier::HaarOutput*> &examples,const std::vector<bool> &attribs, int &bestAttribute, double &bestThreshold){
		int PositiveVals [HAARAMOUNT][THRESHOLDVALS][2];
		int NegativeVals [HAARAMOUNT][THRESHOLDVALS][2];
		int minAttr;
		float minThr, minEntropy, tempEntropy;

		//initialize all to 0
		for (int attr=0; attr<HAARAMOUNT; attr++){
			for (int thr=0; thr<THRESHOLDVALS; thr++){
				PositiveVals[attr][thr][2] = 0;
				NegativeVals[attr][thr][2] = 0;
			}
		}

		//start iterator
		std::vector<CClassifier::HaarOutput*>::const_iterator it = examples.begin();

		//increment counts for each 
		while(it != examples.end()){
			for (int attr=0; attr<HAARAMOUNT; attr++){
				for (int thr=0; thr<THRESHOLDVALS; thr++){

					if ((*it)->haarVals[attr] > (0.1 + 0.1*thr)){
						if ((*it)->type == treeType){
							PositiveVals[attr][thr][1]++;
						}else{
							NegativeVals[attr][thr][1]++;
						}
					}else{
						if ((*it)->type == treeType){
							PositiveVals[attr][thr][0]++;
						}else{
							NegativeVals[attr][thr][0]++;
						}
					}

				}
			}
		}

	//min entropy initialized to 0
	//store min entropy attr and threshold
	minAttr = 0;
	minThr = 0;
	minEntropy = 1;
	int mlp, mln, mlpUp, mlpDown, mlnUp, mlnDown;

	for (int attr=0; attr<HAARAMOUNT; attr++){
		for (int thr=0; thr<THRESHOLDVALS; thr++){
			mlpUp = PositiveVals[attr][thr][1];
			mlnUp = NegativeVals[attr][thr][1];
			
			mlpDown = PositiveVals[attr][thr][0];
			mlnDown = NegativeVals[attr][thr][0];

			mlp = mlpDown + mlnUp;
			mln = mlnDown + mlnUp;

				//calculate entropy
			tempEntropy = (mlp+mln)*EntropyFunc(mlp/(mlp+mln)) 
									 -(mlpUp + mlnUp)*EntropyFunc(mlpUp/(mlpUp+mlnUp))
									 -(mlpDown + mlnDown)*EntropyFunc(mlpDown/(mlpDown+mlnDown));

			if (tempEntropy < minEntropy){
				minEntropy = tempEntropy;
				minThr = thr;
				minAttr = attr;
			}
		}
	}
	bestAttribute = minAttr;
	bestThreshold = (0.1 + 0.1*minThr);
}

bool DecisionTree::sameClassification(const std::vector<CClassifier::HaarOutput*> &examples){

	std::vector<CClassifier::HaarOutput*>::const_iterator it = examples.begin();

	// check if first one is treetype or not
	bool isNotTreeType = ((*it)->type != treeType);

	// loop through examples, if any one has a different type than the first one its not the same classification
	while(it != examples.end()){

		if(isNotTreeType){
			if((*it)->type == treeType)
				return false;
		}else if((*it)->type != treeType){
			return false;

		}

		++it;

	}
	
	return true;

}

bool DecisionTree::isAttribsEmpty(const std::vector<bool> &attribs){

	// if any attribute (ie haar feature) is true its not empty
	for(int i=0;i<attribs.size();++i){
		if(attribs[i]) return false;
	}
	
	return true;

}

void DecisionTree::setMajorityValues(const std::vector<CClassifier::HaarOutput*> &examples){

	int positive = 0, negative = 0;

	std::vector<CClassifier::HaarOutput*>::const_iterator it = examples.begin();

	// find the amount of the sought type versus the other types
	while(it != examples.end()){

		if((*it)->type == treeType)
			++positive;
		else
			++negative;

		++it;
	}

	int max = std::max(positive, negative);
	
	// find the most common types percentage and save it.
	if(max != 0)
		majorityPercent = float(max) / float(positive + negative);

	if(max == positive)
		majorityType = treeType;

	else
		majorityType = CClassifier::OTHER;


}

double DecisionTree::EntropyFunc(double p){
	return -p * log(p)/log(2) - (1-p)*log(1-p)/log(2);
}





















































