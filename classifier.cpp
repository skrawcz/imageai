/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    classifier.cpp
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
** DESCRIPTION:
**  See classifier.h
**
*****************************************************************************/

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "classifier.h"

using namespace std;

// CClassifier class ---------------------------------------------------------

// default constructor
CClassifier::CClassifier()
{
    // initalize the random number generator (for sample code)
    rng = cvRNG(-1);
    parameters = NULL;

    // CS221 TO DO: add initialization for any member variables   
}
    
// destructor
CClassifier::~CClassifier()
{
    if (parameters != NULL) {
	cvReleaseMat(&parameters);
    }

    // CS221 TO DO: free any memory allocated by the object
}

// loadState
// Configure the classifier from the given file.
bool CClassifier::loadState(const char *filename)
{
    assert(filename != NULL);
    
    // CS221 TO DO: replace this with your own configuration code
    
    return true;
}

// saveState
// Writes classifier configuration to the given file
bool CClassifier::saveState(const char *filename)
{
    assert(filename != NULL);
    
    // CS221 TO DO: replace this with your own configuration code
    
    return true;
}

// run
// Runs the classifier over the given frame and returns a list of
// objects found (and their location).
bool CClassifier::run(const IplImage *frame, CObjectList *objects)
{
    assert((frame != NULL) && (objects != NULL));
    
    // CS221 TO DO: replace this with your own code

    // Example code which returns up to 10 random objects, each object
    // having a width and height equal to half the frame size.
    const char *labels[5] = {
	"mug", "stapler", "keyboard", "clock", "scissors"
    }; 
    int n = cvRandInt(&rng) % 10;
    while (n-- > 0) {
        CObject obj;
        obj.rect = cvRect(0, 0, frame->width / 2, frame->height / 2);
        obj.rect.x = cvRandInt(&rng) % (frame->width - obj.rect.width);
        obj.rect.y = cvRandInt(&rng) % (frame->height - obj.rect.height);
	obj.label = string(labels[cvRandInt(&rng) % 5]);
	objects->push_back(obj);        
    }

    return true;
}
        
// train
// Trains the classifier to recognize the objects given in the
// training file list.
bool CClassifier::train(TTrainingFileList& fileList)
{
    // CS221 TO DO: replace with your own training code

    // example code to show you number of samples for each object class
    cout << "Classes:" << endl;
    for (int i = 0; i < (int)fileList.classes.size(); i++) {
			
			cout << fileList.classes[i] << " (";
			int count = 0;

			for (int j = 0; j < (int)fileList.files.size(); j++) {
	    	if (fileList.files[j].label == fileList.classes[i]) {
					count += 1;
	    	}
			}
			cout << count << " samples)" << endl;
    }
    cout << endl;


		// create haars
		readHaars();

    // example code for loading and resizing image files--
    // you may find this useful for the milestone    
    IplImage *image, *smallImage, *integralo;

    cout << "Processing images..." << endl;
    smallImage = cvCreateImage(cvSize(64, 64), IPL_DEPTH_8U, 1);
		integralo = cvCreateImage(cvSize(65, 65), IPL_DEPTH_32S, 1);	

    for (int i = 0; i < (int)fileList.files.size(); i++) {
			// show progress
			if (i % 1000 == 0) {
					showProgress(i, fileList.files.size());
			}

			// skip non-mug and non-other images (milestone only)
			if ((fileList.files[i].label == "mug") ||
					(fileList.files[i].label == "other")) {
					
				// load the image
				image = cvLoadImage(fileList.files[i].filename.c_str(), 0);
				if (image == NULL) {
					cerr << "ERROR: could not load image "
							 << fileList.files[i].filename.c_str() << endl;
					continue;
				}

				

			  // resize to 64 x 64
			  cvResize(image, smallImage);

				// create integral image
				cvIntegral(smallImage, integralo);

				applyHaar(integralo);

			  // CS221 TO DO: extract features from image here

			  // free memory
			  cvReleaseImage(&image);
			}
    }

    // free memory
    cvReleaseImage(&smallImage);
		cvReleaseImage(&integralo);
    cout << endl;

    // CS221 TO DO: train you classifier here

    return true;
}

// creates set of haar values for an image.
int CClassifier::applyHaar(const IplImage *im){

	std::vector<HaarFeature>::iterator it = haars.begin();

	double t,t2,z,out;
	int i = 0;

	// for every haars feature calculate value for image.
	// the input file just gives us the coordinates of the entire square
	// we need to compute where the black and white areas are
	while(it != haars.end()){
		
		t = 0.0;t2 = 0.0;

		// total
		z  = cvGetReal2D(im, it->y,				 		it->x);
		z -= cvGetReal2D(im, it->y,				 		it->x + it->w);
		z -= cvGetReal2D(im, it->y + it->h,	 	it->x);
		z += cvGetReal2D(im, it->y + it->h,	 	it->x + it->w);

		//this goes through the haar features and computes
		// the area computation has been checked by Stefan - written by Filip
		switch (it->t){
			case hH:

				// bottom part (y + h/2) + (y+h,x+w) - (y+h/2,x+w) - (y+h,x)
				// + (y+h/2,x)
				t  = cvGetReal2D(im, it->y + it->h/2,		it->x);
				// + (y+h,x+w)
				t += cvGetReal2D(im, it->y + it->h,	 		it->x + it->w);
				// - (y+h/2,x+w)
				t -= cvGetReal2D(im, it->y + it->h/2,		it->x + it->w);
				// - (y+h,x)
				t -= cvGetReal2D(im, it->y + it->h,	 		it->x);


			break;
			case hV:

				// right part    (y,x+w/2) + (y+h,x+w) - (y,x+w) - (y+h,x + w/2)
				// + (y,x+w/2)
				t  = cvGetReal2D(im, it->y,							it->x + it->w/2);
				// + (y+h,x+w)
				t += cvGetReal2D(im, it->y + it->h,	 		it->x + it->w);
				// - (y,x+w)
				t -= cvGetReal2D(im, it->y,							it->x + it->w);
				// - (y+h,x+w/2)
				t -= cvGetReal2D(im, it->y + it->h,	 		it->x + it->w/2);

			break;
			case hD:

				// top right part (y,x+w/2) + (y+h/2,x+w) - (y + h/2, x + w/2) - (y,x + w)
				t  = cvGetReal2D(im, it->y,							it->x + it->w/2);
				t += cvGetReal2D(im, it->y + it->h/2,	 	it->x + it->w);
				t -= cvGetReal2D(im, it->y + it->h/2,		it->x + it->w/2);
				t -= cvGetReal2D(im, it->y,	 						it->x + it->w);

				// bottom left part ( y+h, x + w/2) + (y + h/2, x) - (y+h,x) - (y + h/2, x + w/2)
				t2  = cvGetReal2D(im, it->y + it->h/2,	it->x);
				t2 += cvGetReal2D(im, it->y + it->h,	 	it->x + it->w/2);
				t2 -= cvGetReal2D(im, it->y + it->h,		it->x);
				t2 -= cvGetReal2D(im, it->y + it->h/2,	it->x + it->w/2);

				t += t2;

			break;
			case hTL:
				
				// top left part (y,x) + (y+h/2,x+w/2) - (y+h/2,x) - (y,x+w/2)
				t  = cvGetReal2D(im, it->y,							it->x);
				t += cvGetReal2D(im, it->y + it->h/2,	 	it->x + it->w/2);
				t -= cvGetReal2D(im, it->y + it->h/2,		it->x);
				t -= cvGetReal2D(im, it->y,						 	it->x + it->w/2);

			break;
			case hTR:

				// top right part (y,x+w/2) + (y+h/2,x+w) - (y + h/2, x + w/2) - (y,x + w)
				t  = cvGetReal2D(im, it->y,							it->x + it->w/2);
				t += cvGetReal2D(im, it->y + it->h/2,	 	it->x + it->w);
				t -= cvGetReal2D(im, it->y + it->h/2,		it->x + it->w/2);
				t -= cvGetReal2D(im, it->y,	 						it->x + it->w);

			break;
			case hBL:

				// bottom left part ( y+h, x + w/2) + (y + h/2, x) - (y+h,x) - (y + h/2, x + w/2)
				t  = cvGetReal2D(im, it->y + it->h/2,	it->x);
				t += cvGetReal2D(im, it->y + it->h,	 	it->x + it->w/2);
				t -= cvGetReal2D(im, it->y + it->h,		it->x);
				t -= cvGetReal2D(im, it->y + it->h/2,	it->x + it->w/2);

			break;
			case hBR:

				// bottom right part (y+h,x+w) + (y+h/2,x+w/2) - (y+h,x+w/2) - (y+h/2,x+w)
				t  = cvGetReal2D(im, it->y + it->h/2,	 	it->x + it->w/2);
				t += cvGetReal2D(im, it->y + it->h,	 		it->x + it->w);
				t -= cvGetReal2D(im, it->y + it->h,	 		it->x + it->w/2);
				t -= cvGetReal2D(im, it->y + it->h/2,	 	it->x + it->w);
			
			break;

			out = (z - 2*t)/z;

		}
		++it;
		++i;
	}


	return 0;
}

// reads haar presets from file
void CClassifier::readHaars(){


	haars.clear();
	std::string tmp;
	ifstream ifs ( "haarfeatures.txt" , ifstream::in );

	// get all the haars
  while (getline(ifs,tmp))
		haars.push_back(strToHaar(tmp));
	

}


// reads a string and turn it into a haar struct.
CClassifier::HaarFeature CClassifier::strToHaar(const std::string &in){

	HaarFeature out;

	std::stringstream s;
  s << in;

	s >> out.x;
	s >> out.y;
	s >> out.w;
	s >> out.h;

	std::string tmp;

	s >> tmp;

	// define the type
	if(tmp == "H")
		out.t = hH;
	else if(tmp == "V")
		out.t = hV;
	else if(tmp == "D")
		out.t = hD;
	else if(tmp == "TL")
		out.t = hTL;
	else if(tmp == "TR")
		out.t = hTR;
	else if(tmp == "BL")
		out.t = hBL;
	else if(tmp == "BR")
		out.t = hBR;

	return out;

}
