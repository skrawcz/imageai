

#include "decisionTree.h"


DecisionTree::DecisionTree(std::vector<CClassifier::HaarOutput*> examples, std::vector<bool> attribs, float majority) 

: majorityPercent(majority) , 
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


}

DecisionTree::DecisionTree(std::ifstream &in){




}
DecisionTree::~DecisionTree(){

	for(unsigned i=0;i<children.size();++i)
			delete children[i];


}
void DecisionTree::print(std::ofstream &out){


	// print data.


	for(unsigned i=0;i<children.size();++i)
			children[i]->print(out);



}

int DecisionTree::chooseAttribute(const std::vector<CClassifier::HaarOutput*> &examples,const std::vector<bool> &attribs){


	return 0;

}

bool DecisionTree::sameClassification(const std::vector<CClassifier::HaarOutput*> &examples){

	std::vector<CClassifier::HaarOutput*>::const_iterator it = examples.begin();

	CClassifier::ImageType type = (*it)->type;

	while(it != examples.end()){

		if((*it)->type != type)
			return false;

		++it;

	}
	
	return true;

}












