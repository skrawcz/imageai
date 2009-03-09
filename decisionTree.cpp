#include "features.h"

#include "decisionTree.h"
#include <math.h>
#include <stdlib.h>
#include <algorithm>

#include "CXMLParser.h"
#include "highgui.h"


DecisionTree::DecisionTree(CvMat *examples, CvMat *imageTypes, std::vector<bool> attribs, 
													 float percent, Features::ImageType type, int depth, Features::ImageType treeType) 

: Classer(),
	majorityPercent(percent) , 
	majorityType(type),
	treeType(treeType),
	isLeaf(false)
	

{

	Features::ImageType tmpType;

	//std::cout << "in decision tree constructor" << std::endl;
	if(doneClassifying(imageTypes) || depth > 10){

		isLeaf = true;
		

	}else if(sameClassification(imageTypes, tmpType)){
		isLeaf = true;
		
		// value something like 100 % of something :S
		majorityPercent = 1;

		if(tmpType == treeType)
			majorityType = treeType;
		else
			majorityType = Features::OTHER;


	}

	else if(isAttribsEmpty(attribs)){
		isLeaf = true;
		setMajorityValues(examples);

	// now its established that we are not a leaf but a beautiful treeeee
	}else{
		double infogain;
		infogain = chooseAttribute(examples, imageTypes, attribs, attribute, threshold);
	
		setMajorityValues(examples);


		if(infogain <= 10){
			isLeaf = true;
		}else{
		

			CvMat *above = cvCreateMat(imageTypes->rows, 1, CV_32S);
			CvMat *below = cvCreateMat(imageTypes->rows, 1, CV_32S);
	
			//splitting examples on best feature 
			for(int i=0; i < imageTypes->rows;++i){
						
				if(CV_MAT_ELEM( *examples, float, i, attribute ) <= threshold){
					*( (int*)CV_MAT_ELEM_PTR( *below, i, 0 ) ) = CV_MAT_ELEM( *imageTypes, int, i, 0 );
					*( (float*)CV_MAT_ELEM_PTR( *above, i, 0 ) ) = -1;

				}else{
					*( (int*)CV_MAT_ELEM_PTR( *above, i, 0 ) ) = CV_MAT_ELEM( *imageTypes, int, i, 0 );
					*( (int*)CV_MAT_ELEM_PTR( *below, i, 0 ) ) = -1;
				}
			}
			//finished splitting examples on best feature

			attribs.at(attribute) = false;
	
			DecisionTree *child = new DecisionTree(examples, below, attribs, majorityPercent, majorityType, ++depth, treeType);
			//reached the end of first child 
		
			children.push_back(child);

			child = new DecisionTree(examples, above, attribs, majorityPercent, majorityType, ++depth, treeType);

			children.push_back(child);

		}
	}
}

DecisionTree::DecisionTree(std::ifstream &in, bool isNode, Features::ImageType treeType){

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


		// while the data stream doesnt contain the end of this object
		while(current.find("</node>") == std::string::npos){

			
			// create new node or leaf depending on what we find
			if(current.find("<node>") != std::string::npos){
				
				children.push_back(new DecisionTree(in, true, treeType));
			}else{
				children.push_back(new DecisionTree(in, false, treeType));
			}

			// grab the next line
			getline(in,current);
		}


	}else{

		// extract values  atoi and atof turn them into numbers
		CXMLParser::getNextValue(in, current);
		majorityType = (Features::ImageType)atoi(current.c_str());

		CXMLParser::getNextValue(in, current);
		majorityPercent = atof(current.c_str());

		// kill the end </leaf>
		getline(in, current);

	}
}
DecisionTree::~DecisionTree(){

	for(unsigned i=0;i<children.size();++i)
			delete children[i];


}
void DecisionTree::print(std::ofstream &out, int level){

	if(level == 0){
		printGeneralInfo(out);
		out << "<treeType>" << treeType << "</treeType>\n";
	}

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

double DecisionTree::chooseAttribute(CvMat *examples, CvMat *imageTypes, const std::vector<bool>
&attribs, int &bestAttribute, double &bestThreshold){

		// TODO  allocate dynamically or such
		int PositiveVals [HAARAMOUNT][THRESHOLDVALS][2];
		int NegativeVals [HAARAMOUNT][THRESHOLDVALS][2];
		int maxAttr;
    long double maxThr, maxInfoGain, tempInfoGain;
		
		//initialize all to 0
		for (int attr=0; attr<Features::amountOfFeatures(); ++attr){
			for (int thr=0; thr<THRESHOLDVALS; ++thr){
				PositiveVals[attr][thr][1] = 0;
				NegativeVals[attr][thr][1] = 0;
				PositiveVals[attr][thr][0] = 0;
				NegativeVals[attr][thr][0] = 0;
			}
		}

		//increment counts for each 
		for(int i=0; i < imageTypes->rows;++i){

			if(CV_MAT_ELEM( *imageTypes, int, i, 0 ) != -1){
				for (int attr=0; attr<Features::amountOfFeatures(); ++attr){

					//if attribute is already used skip it
					if(!attribs.at(attr)){
						continue;
					}
				
					for (int thr=0; thr<THRESHOLDVALS; ++thr){
					
						if (CV_MAT_ELEM( *examples, float, i, thr ) > (0.1 + 0.1*thr)){
							if (CV_MAT_ELEM( *imageTypes, int, i, 0 ) == treeType){
								PositiveVals[attr][thr][1]++;
							}else{
								NegativeVals[attr][thr][1]++;
							}
						}else{
							if (CV_MAT_ELEM( *imageTypes, int, i, 0 ) == treeType){
								PositiveVals[attr][thr][0]++;
							}else{
								NegativeVals[attr][thr][0]++;
							}
						}
					}
				}
			}
		}
		
	maxAttr = -1;
	maxThr = -1;
	maxInfoGain = -1;
	long double mlp, mln, mlpUp, mlpDown, mlnUp, mlnDown;

	for (int attr=0; attr<Features::amountOfFeatures(); ++attr){
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

			//Default values to 0 if it'll make the entropy function angry (i.e 0 or 1)
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

			tempInfoGain = ml1 - mlpUp1 - mlpDwn1;
			
			if ( tempInfoGain > maxInfoGain){
				maxInfoGain = tempInfoGain;
				maxThr = thr;
				maxAttr = attr;
			}
		}
	}

	bestAttribute = maxAttr;
	bestThreshold = (0.1 + 0.1*maxThr);
	return maxInfoGain;
}

bool DecisionTree::sameClassification(CvMat *imageTypes, Features::ImageType &tmpType){

	int i = 0;

	// check if first one is treetype or not
	while(CV_MAT_ELEM( *imageTypes, int, i, 0 ) == -1){

		++i;
	}

	bool isNotTreeType = (CV_MAT_ELEM( *imageTypes, int, i, 0 ) != treeType);

	tmpType = (Features::ImageType)CV_MAT_ELEM( *imageTypes, int, i, 0 );

	// loop through examples, if any one has a different type than the first one its not the same classification
	if(isNotTreeType){

		for(i=0; i < imageTypes->rows;++i){

			if(CV_MAT_ELEM( *imageTypes, int, i, 0 ) == treeType)
				return false;


		}
	}else{

	

		for(i=0; i < imageTypes->rows;++i){

			if(CV_MAT_ELEM( *imageTypes, int, i, 0 ) != treeType){
				return false;
	
			}


		}
	}
	
	return true;

}

bool DecisionTree::doneClassifying(CvMat *imageTypes){

	for(int i=0; i < imageTypes->rows;++i){

		if(CV_MAT_ELEM( *imageTypes, int, i, 0 ) != -1)
			return false;

	}

	return true;

}

Features::ImageType DecisionTree::classify(CvMat *imageData, double &percent){
	
	if(!isLeaf){
		if(CV_MAT_ELEM( *imageData, float, 0, attribute ) <= threshold){
			
			return children.at(0)->classify(imageData, percent);

		}else{

			return children.at(1)->classify(imageData, percent);

		}
	}else{
		//if(majorityType == treeType)
		//	std::cout << majorityPercent << std::endl;

		percent = majorityPercent;

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

void DecisionTree::setMajorityValues(CvMat *imageTypes){

	int positive = 0, negative = 0, val;


	// find the amount of the sought type versus the other types
	for(int i=0; i < imageTypes->rows;++i){

		val = CV_MAT_ELEM( *imageTypes, int, i, 0);

		if(val != -1){
			if(val == treeType)
				++positive;
			else
				++negative;
		}
	}

	int max = std::max(positive, negative);
	
	// find the most common types percentage and save it.
	if(max != 0)
		majorityPercent = float(max) / float(positive + negative);
	else
		majorityPercent = 0.0;


	if(max == positive)
		majorityType = treeType;
	else
		majorityType = Features::OTHER;


}

long double DecisionTree::EntropyFunc(long double p){
	return -p * log(p)/M_LN2 - (1-p)*log(1-p)/M_LN2;
}





















































