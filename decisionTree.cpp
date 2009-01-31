

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
void DecisionTree::print(std::ofstream &out, int level){

	std::string tabStr;

	for(int i=0;i<=level;++i)
		tabStr.append("\t");

	out << tabStr << "<node>\n";
	out << tabStr << "\t<isLeaf>" << isLeaf << "</isLeaf>";




	for(unsigned i=0;i<children.size();++i)
			children[i]->print(out, level + 1);

	out << tabStr << "</node>\n";

}

float DecisionTree::chooseAttribute(const std::vector<CClassifier::HaarOutput*> &examples,const std::vector<bool> &attribs){


	return 0;

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





















































