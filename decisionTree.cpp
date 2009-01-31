

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
	std::cout << "in decision tree constructor" << std::endl;
	if(examples.size() == 0){
		isLeaf = true;
		//std::cout << "examples.size == 0 " << std::endl;
		
	}

	else if(sameClassification(examples)){
		//std::cout << "in sameClassification " << std::endl;
		isLeaf = true;
		
		// value something like 100 % of something :S
		majorityPercent = 1;
		majorityType = examples.at(0)->type;


	}

	else if(isAttribsEmpty(attribs)){
		//std::cout << "in the attributes are empty" << std::endl;
		isLeaf = true;
		setMajorityValues(examples);

	// now its established that we are not a leaf but a beautiful treeeee
	}else{
		//std::cout << "in else part " << std::endl;
		int bestAttribute;
		double bestThreshold;
		chooseAttribute(examples, attribs, bestAttribute, bestThreshold);
		setMajorityValues(examples);
		
		std::vector<CClassifier::HaarOutput*>::iterator it = examples.begin();
		std::vector<CClassifier::HaarOutput*> above, below;
	
		//std::cout << "splitting examples on best feature " << std::endl;
		while(it != examples.end()){
						
			if((*it)->haarVals[bestAttribute] < bestThreshold)
				below.push_back(*it);
			else
				above.push_back(*it);
			
			++it;
		}
		//problem with above and below size
		std::cout << "finished splitting examples on best feature " <<std::endl;
		std::cout << "size above = " << above.size() << " size below = " <<
			below.size() << std::endl;
		attribs.at(bestAttribute) = false;

		DecisionTree *child = new DecisionTree(above, attribs, majorityPercent, majorityType);
		std::cout << "reached the end of first child " << std::endl;
		children.push_back(child);

		child = new DecisionTree(below, attribs, majorityPercent, majorityType);

		children.push_back(child);
		std::cout << "reached the end" << std::endl;

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

void DecisionTree::chooseAttribute(const
std::vector<CClassifier::HaarOutput*> &examples,const std::vector<bool>
&attribs, int &bestAttribute, double &bestThreshold){

	//std::cout << "in choose Attribute"<<std::endl;
		int PositiveVals [HAARAMOUNT][THRESHOLDVALS][2];
		int NegativeVals [HAARAMOUNT][THRESHOLDVALS][2];
		int maxAttr;
		double maxThr, maxInfoGain, tempInfoGain;
		
		//std::cout << "initialising the array to all zeros "<<std::endl;
		//initialize all to 0
		for (int attr=0; attr<HAARAMOUNT; ++attr){
			for (int thr=0; thr<THRESHOLDVALS; ++thr){
				PositiveVals[attr][thr][1] = 0;
				NegativeVals[attr][thr][1] = 0;
				PositiveVals[attr][thr][0] = 0;
				NegativeVals[attr][thr][0] = 0;
			}
		}
		//	std::cout << "finished intialising all the array to zeros "<<std::endl;
		
		//start iterator
		std::vector<CClassifier::HaarOutput*>::const_iterator it = examples.begin();

		//	std::cout << "going through loop to get threshold value counts"<<std::endl;
		//increment counts for each 
		while(it != examples.end()){
			for (int attr=0; attr<HAARAMOUNT; ++attr){
				for (int thr=0; thr<THRESHOLDVALS; ++thr){

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
			++it;
		}

	//min entropy initialized to 0
	//store min entropy attr and threshold
	maxAttr = 0;
	maxThr = 0;
	maxInfoGain = 1;
	double mlp, mln, mlpUp, mlpDown, mlnUp, mlnDown;

	for (int attr=0; attr<HAARAMOUNT; ++attr){
		for (int thr=0; thr<THRESHOLDVALS; ++thr){

			// the number of positive values above the threshold
			mlpUp = PositiveVals[attr][thr][1];
			// # negative above threshold			
			mlnUp = NegativeVals[attr][thr][1];
			
			// <= threshold
			mlpDown = PositiveVals[attr][thr][0];
			mlnDown = NegativeVals[attr][thr][0];

			// total # positive
			mlp = mlpDown + mlpUp;

			// total # negative
			mln = mlnDown + mlnUp;

				//calculate Infoformation Gain
			tempInfoGain = (mlp+mln)*EntropyFunc(mlp/(mlp+mln)) 
									 -(mlpUp + mlnUp)*EntropyFunc(mlpUp/(mlpUp+mlnUp))
									 -(mlpDown + mlnDown)*EntropyFunc(mlpDown/(mlpDown+mlnDown));

			if ( tempInfoGain > maxInfoGain){
				maxInfoGain = tempInfoGain;
				maxThr = thr;
				maxAttr = attr;
			}
		}
	}
	bestAttribute = maxAttr;
	bestThreshold = (0.1 + 0.1*maxThr);
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
	return -p * log(p)/M_LN2 - (1-p)*log(1-p)/M_LN2;
}





















































