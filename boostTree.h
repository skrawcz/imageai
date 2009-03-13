#ifndef BOOSTTREE_H
#define BOOSTTREE_H

#include <iostream>

#include "classer.h"

#include "cv.h"
#include "cxcore.h"
#include "ml.h"

class Features;

class BoostTree : public Classer{


public:

	// just create
	BoostTree(CvMat *examples, CvMat *imageTypes);

	// create from xml file
	BoostTree(const char *filename);

	~BoostTree();

	Features::ImageType classify(CvMat *imageData, double &percent);

	void print(const char *filename) {

		boost->save( filename );

	};



private:

	CvBoost *boost;
	CvMat * weak_responses;

};


#endif
