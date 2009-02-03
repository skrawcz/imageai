

#include "decisionTree.h"
#include <math.h>
#include <stdlib.h>
#include <algorithm>

#include "CXMLParser.h"
#include "highgui.h"

CClassifier::ImageType DecisionTree::treeType = CClassifier::MUG;

DecisionTree::DecisionTree(std::vector<CClassifier::HaarOutput*> examples, std::vector<bool> attribs, 
													 float percent, CClassifier::ImageType type) 

: majorityPercent(percent) , 
	majorityType(type),
	isLeaf(false)

{
	//std::cout << "in decision tree constructor" << std::endl;
	if(examples.size() == 0){
		isLeaf = true;
		std::cout << "examples.size == 0 " << std::endl;
		
	}

	else if(sameClassification(examples)){
		//std::cout << "in sameClassification " << std::endl;
		isLeaf = true;
		
		// value something like 100 % of something :S
		majorityPercent = 1;
		majorityType = examples.at(0)->type;


	}

	else if(isAttribsEmpty(attribs)){
		std::cout << "in the attributes are empty" << std::endl;
		isLeaf = true;
		setMajorityValues(examples);

	// now its established that we are not a leaf but a beautiful treeeee
	}else{
		//std::cout << "in else part " << std::endl;
		double infogain;
		infogain = chooseAttribute(examples, attribs, attribute, threshold);
	
		setMajorityValues(examples);

		if(infogain == 0){
			isLeaf = true;
		}else{
		
			std::vector<CClassifier::HaarOutput*>::iterator it = examples.begin();
			std::vector<CClassifier::HaarOutput*> above, below;
	
			//std::cout << "splitting examples on best feature " << std::endl;
			while(it != examples.end()){
						
					if((*it)->haarVals[attribute] <= threshold){
				//if((*it)->haarVals[attribute] > threshold){
					below.push_back(*it);
				}else{
					above.push_back(*it);
				}
				++it;
			}
			//problem with above and below size
			//std::cout << "finished splitting examples on best feature " <<std::endl;
			//std::cout << "size above = " << above.size() << " size below = " <<	below.size() << std::endl;

			attribs.at(attribute) = false;
	
			DecisionTree *child = new DecisionTree(below, attribs, majorityPercent, majorityType);
			//std::cout << "reached the end of first child " << std::endl;
		
			children.push_back(child);

			child = new DecisionTree(above, attribs, majorityPercent, majorityType);

			children.push_back(child);
			//	std::cout << "reached the end" << std::endl;
		}
	}
}

DecisionTree::DecisionTree(std::ifstream &in, bool isNode){

	isLeaf = !isNode;

	std::string current;

	// if it is a node
	if(isNode){

		// extract values  atoi and atof turn them into numbers
		CXMLParser::getNextValue(in, current);
		attribute = atoi(current.c_str());

		CXMLParser::getNextValue(in, current);
		threshold = atof(current.c_str());

		// get first line of possible children or just end of node
		getline(in,current);

		//std::cout << current << std::endl;

		// while the data stream doesnt contain the end of this object
		while(current.find("</node>") == std::string::npos){

			
			// create new node or leaf depending on what we find
			if(current.find("<node>") != std::string::npos){
				
				children.push_back(new DecisionTree(in, true));
			}else{
				children.push_back(new DecisionTree(in, false));
			}

			// grab the next line
			getline(in,current);
			//std::cout << current << std::endl;

		}


	}else{

		// extract values  atoi and atof turn them into numbers
		CXMLParser::getNextValue(in, current);
		majorityType = (CClassifier::ImageType)atoi(current.c_str());

		CXMLParser::getNextValue(in, current);
		majorityPercent = atoi(current.c_str());

		// kill the end </leaf>
		getline(in, current);

	}
}
DecisionTree::~DecisionTree(){

	for(unsigned i=0;i<children.size();++i)
			delete children[i];


}
void DecisionTree::print(std::ofstream &out, int level){

	std::string tabStr;

	for(int i=0;i<=level;++i)
		tabStr.append("\t");

	if(isLeaf){
		out << tabStr << "<Leaf>\n";
		out << tabStr << "\t<majorityType>" << majorityType << "</majorityType>\n";
		out << tabStr << "\t<majorityPercent>" << majorityPercent << "</majorityPercent>\n";
		out << tabStr << "</leaf>\n";
	}else{
		out << tabStr << "<node>\n";
		out << tabStr << "\t<attribute>" << attribute << "</attribute>\n";
		out << tabStr << "\t<threshold>" << threshold << "</threshold>\n";
	
		for(unsigned i=0;i<children.size();++i)
				children[i]->print(out, level + 1);

		out << tabStr << "</node>\n";
	}

}

double DecisionTree::chooseAttribute(const
std::vector<CClassifier::HaarOutput*> &examples,const std::vector<bool>
&attribs, int &bestAttribute, double &bestThreshold){

	  //std::cout << "in choose Attribute"<<std::endl;
		
		int PositiveVals [HAARAMOUNT][THRESHOLDVALS][2];
		int NegativeVals [HAARAMOUNT][THRESHOLDVALS][2];
		int maxAttr;
    long double maxThr, maxInfoGain, tempInfoGain;
		
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

				//stefan's attempt at figuring out what's wrong
				if(!attribs.at(attr)){
					//std::cout <<"get thres count: skipping attrib #"<<attr<<" value = "<< attribs.at(attr)<<std::endl;
					continue;
				}
				
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
		
	maxAttr = -1;
	maxThr = -1;
	maxInfoGain = -1;
	long double mlp, mln, mlpUp, mlpDown, mlnUp, mlnDown;

	for (int attr=0; attr<HAARAMOUNT; ++attr){
		//std::cout <<"calculating max info gain = "<<std::endl;

		//if attribute is already used skip it
		if(!attribs.at(attr)){
			continue;
		}

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
			long double ml1,mlpUp1,mlpDwn1;

			//Default values to 0 if it'll make the entropy function angry
			if(mlp ==0.0 || mln == 0){
				ml1 = 0.0;
			}
			else{
				ml1 = (mlp+mln)*EntropyFunc(mlp/(mlp+mln));
			}
			if(mlnUp==0.0|| mlpUp ==0.0){
				mlpUp1 = 0.0;
			}
			else{
				mlpUp1 = (mlpUp + mlnUp)*EntropyFunc(mlpUp/(mlpUp+mlnUp));
			}
			if(mlnDown==0.0 || mlpDown ==0.0){
				mlpDwn1 =0.0;
			}
			else{
				mlpDwn1 = (mlpDown + mlnDown)*EntropyFunc(mlpDown/(mlpDown+mlnDown));
			}

			/*
			std::cout <<"ml1 ="<<ml1<<std::endl;
			std::cout <<"mlpUp1 ="<<mlpUp1<<std::endl;
			std::cout <<"mlpUp = "<<mlpUp<<" mlpUp+mlnUp = "<<mlpUp+mlnUp<<std::endl;
			std::cout <<"mlpDwn1 ="<<mlpDwn1<<std::endl;
			*/
			tempInfoGain = ml1 - mlpUp1 - mlpDwn1;
			
			if ( tempInfoGain > maxInfoGain){
				//std::cout <<"Reset max info gain" << std::endl;
				maxInfoGain = tempInfoGain;
				maxThr = thr;
				maxAttr = attr;
			}
		}
	}

	bestAttribute = maxAttr;
	bestThreshold = (0.1 + 0.1*maxThr);
	//std::cout<<"attr ="<<bestAttribute<<" thres ="<<bestThreshold<<" max gain= "<<maxInfoGain<<std::endl;
	return maxInfoGain;
}

bool DecisionTree::sameClassification(const std::vector<CClassifier::HaarOutput*> &examples){

	std::vector<CClassifier::HaarOutput*>::const_iterator it = examples.begin();

	// check if first one is treetype or not
	bool isNotTreeType = ((*it)->type != treeType);

		
			
		

	// loop through examples, if any one has a different type than the first one its not the same classification
	if(isNotTreeType){

		while(it != examples.end()){

			if((*it)->type == treeType)
				return false;

			++it;

		}
	}else{

		while(it != examples.end()){

			if((*it)->type != treeType){
				return false;
	
			}

			++it;

		}
	}
	
	return true;

}

CClassifier::ImageType DecisionTree::classify(CClassifier::HaarOutput *haary){
	
	//std::cout << "in classify"<< std::endl;
	if(!isLeaf){
		//if(haary->haarVals[attribute] > threshold){
		if(haary->haarVals[attribute] <= threshold){

			return children.at(0)->classify(haary);

		}else{

			return children.at(1)->classify(haary);

		}
	}else{
		//std::cout << "majority percent = "<<majorityPercent;
		//std::cout << " majority type = "<<majorityType << std::endl;
		return majorityType;
	}

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
	else
		majorityPercent = 0.0;

	if(max == positive)
		majorityType = treeType;
	
	//if (majorityPercent >= 0.5)
	//	majorityType = treeType;
	else
		majorityType = CClassifier::OTHER;


}

long double DecisionTree::EntropyFunc(long double p){
	return -p * log(p)/M_LN2 - (1-p)*log(1-p)/M_LN2;
	//return -p * log(p)/log(2) - (1-p)*log(1-p)/log(2);
}





















































