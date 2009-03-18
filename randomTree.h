#ifndef RANDOMTREE_H
#define RANDOMTREE_H

#include <iostream>

#include "classer.h"

#include "cv.h"
#include "cxcore.h"
#include "ml.h"

#include <sstream>


class Features;

class RandomTree : public Classer{


public:

	// just create
	RandomTree(CvMat *examples, CvMat *imageTypes);

	// create from xml file
	RandomTree(const char *filename);

	~RandomTree();

	Features::ImageType classify(CvMat *imageData, double *percent);

	void print(const char *filename) {

		forest.save( filename );


	};



private:

	CvRTrees forest;

};


#endif
