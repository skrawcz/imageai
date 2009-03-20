#ifndef BOOSTTREE_H
#define BOOSTTREE_H

#include <iostream>

#include "classer.h"

#include "cv.h"
#include "cxcore.h"
#include "ml.h"

#include <sstream>


class Features;

class BoostTree : public Classer{


public:

	// just create
	BoostTree(CvMat *examples, CvMat *imageTypes);

	// create from xml file
	BoostTree(const char *filename);

	~BoostTree();

	Features::ImageType classify(CvMat *imageData, double *percent, int type = -1);

	void print(const char *filename) {

		for(int j = 0; j < TYPECOUNT; j++ ){
			std::string egon(filename);

			std::stringstream out;
			out << j << ".xml";
			egon.append(out.str());

			boost.at(j)->save( egon.c_str() );
		}

	};



private:

	std::vector<CvBoost *> boost;
	std::vector<CvMat *> weak_responses;

};


#endif
