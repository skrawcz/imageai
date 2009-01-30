#include "decisionTree.h"


DecisionTree::DecisionTree(){



}

DecisionTree::DecisionTree(std::ifstream &in){




}
DecisionTree::~DecisionTree(){

	for(unsigned i=0;i<children.size();++i)
			delete children[i];


}
void DecisionTree::print(std::ofstream &out){


	// print data.


	for(int i=0;i<children.size();++i)
			children[i]->print(out);



}
