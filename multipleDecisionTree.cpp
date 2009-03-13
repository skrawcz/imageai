#include "features.h"
#include "decisionTree.h"

#include "multipleDecisionTree.h"


#include <math.h>
#include <stdlib.h>
#include <algorithm>

#include "CXMLParser.h"
#include "highgui.h"


MultipleDecisionTree::MultipleDecisionTree(CvMat *examples, CvMat *imageTypes, std::vector<bool> attribs) 

// TODO uncomment if you want to use this class
//Classer()

{


	for(int i=Features::MUG;i<Features::OTHER;++i){

		//singleTrees.push_back(new DecisionTree(examples, imageTypes, attribs, -1, Features::OTHER, 0, (Features::ImageType)i));


	}



}

MultipleDecisionTree::MultipleDecisionTree(std::ifstream &in, bool isNode){


	std::string current;

	CXMLParser::getNextValue(in, current);
		
	for(int i=Features::MUG;i<Features::OTHER;++i){


		// figure out what type of things the tree classifies
		CXMLParser::getNextValue(in, current);
		Features::ImageType treeType = (Features::ImageType)atoi(current.c_str());
		getline(in, current);

		//if(current.find("node") != std::string::npos)
		//	singleTrees.push_back(new DecisionTree(in, true, treeType));
		//else
		//	singleTrees.push_back(new DecisionTree(in, false, treeType));

	}

	CXMLParser::getNextValue(in, current);

}
MultipleDecisionTree::~MultipleDecisionTree(){

	//for(unsigned i=0;i<singleTrees.size();++i)
	//		delete singleTrees[i];


}
void MultipleDecisionTree::print(std::ofstream &out, int level){

	//printGeneralInfo(out);

	std::string tabStr;

	for(int i=0;i<=level;++i)
		tabStr.append("\t");

	
	out << "<treeSet>\n";

	
	for(int i=Features::MUG;i<Features::OTHER;++i){
		
			out << "\t<treeType>" << i << "</treeType>\n";
			//singleTrees[i]->print(out, level + 2);


	}

	out << "</treeSet>\n";


}


Features::ImageType MultipleDecisionTree::classify(CvMat *imageData, double &percent){
	
	percent = 0;

	double percentMax = 0, tmpPercent;
	Features::ImageType tmp, out = Features::OTHER;

	for(int i=Features::MUG;i<Features::OTHER;++i){
		
		tmpPercent = percent;

		//if(singleTrees[i]->classify(imageData, percent) != Features::OTHER){
		//	tmp = (Features::ImageType)i;	
//
		//}else{
		//	tmp = Features::OTHER;
		//	percent = tmpPercent;
		//}

		

		if(percent > percentMax){
			//std::cout << tmp << std::endl;
			out = tmp;
			percentMax = percent;
		}
	}

	percent = percentMax;

	return out;

}







