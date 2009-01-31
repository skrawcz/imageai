

#include "decisionTree.h"

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
		
		haarType = chooseAttribute(examples, attribs);




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

float DecisionTree::chooseAttribute(const std::vector<CClassifier::HaarOutput*> &examples,const std::vector<bool> &attribs){


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

bool DecisionTree::isAttribsEmpty(const std::vector<bool> &attribs){


	for(int i=0;i<attribs.size();++i){
		if(attribs[i]) return false;
	}
	
	return true;

}

void DecisionTree::setMajorityValues(const std::vector<CClassifier::HaarOutput*> &examples){

	int sought = 0, other = 0;

	std::vector<CClassifier::HaarOutput*>::const_iterator it = examples.begin();

	while(it != examples.end()){

		if((*it)->type == treeType)
			++sought;
		else
			++other;

		++it;
	}

	int max = std::max(other, sought);
	
	if(max != 0)
		majorityPercent = float(max) / float(sought + other);

	if(max == other)
		majorityType = CClassifier::OTHER;

	else if(max == mug)
		majorityType = CClassifier::MUG;

	else if(max == scissors)
		majorityType = CClassifier::SCISSORS;

	else if(max == clock)
		majorityType = CClassifier::CLOCK;

	else if(max == stapler)
		majorityType = CClassifier::STAPLER;

	else if(max == keyboard)
		majorityType = CClassifier::KEYBOARD;

}





















































