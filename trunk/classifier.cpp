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
#include "classer.h"

#include "classifier.h"

#include <math.h>

using namespace std;

// CClassifier class ---------------------------------------------------------

// default constructor
CClassifier::CClassifier()
{
    // initalize the random number generator (for sample code)
    rng = cvRNG(-1);
    parameters = NULL;
		tree = NULL;
		

    // CS221 TO DO: add initialization for any member variables   
}
    
// destructor
CClassifier::~CClassifier()
{
    if (parameters != NULL) {
			cvReleaseMat(&parameters);
    }

    if(tree != NULL)
			delete tree;
}

// loadState
// Configure the classifier from the given file.
bool CClassifier::loadState(const char *filename)
{

    assert(filename != NULL);

		readHaars();

		if(tree)
			delete tree;

		tree = Classer::createFromXML(filename);

		//saveState("bongo.xml");

    return true;
}

// saveState
// Writes classifier configuration to the given file
bool CClassifier::saveState(const char *filename)
{
    assert(filename != NULL);
    
    if(tree == NULL)	
			return false;

		
		ofstream ofs ( filename );

		
		tree->printToXML(ofs, 0);

    ofs.close();
    return true;
}

// run
// Runs the classifier over the given frame and returns a list of
// objects found (and their location).
bool CClassifier::run(const IplImage *frame, CObjectList *objects)
{

		assert((frame != NULL) && (objects != NULL));

		//convert to gray scale
		IplImage *gray;
		gray = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
		cvCvtColor(frame,gray,CV_BGR2GRAY);


		HaarOutput *haarOut;
		CObject obj;

		double highestPercent = 0.01;

		//iterate through candidate frames
		for (int x = 0; x <=320; x = x+8){
			for (int y = 0; y<=240; y = y+8){
				for (int w = 64; w<=320; w = w+8){
					for (int h = 64 ; h<=240; h = h+8){
						//check if we are out of frame & for milestone only take squares
						if( (x+w <= gray->width) && (y+h <= gray->height) && (w==h)) {
							//clip the image to the right size
							CvRect region = cvRect(x,y,w,h);
							IplImage *clippedImage = cvCreateImage(cvSize(region.width, region.height), 
																										 gray->depth, gray->nChannels);
							cvSetImageROI(gray,region);
							cvCopyImage(gray, clippedImage);
							cvResetImageROI(gray);
							
								
							//scale image to 64x64
							IplImage *resizedImage = cvCreateImage(cvSize(64,64), 
																										 clippedImage->depth, 
																										 clippedImage->nChannels);
							cvResize(clippedImage, resizedImage);
					
							//compute integral image
							IplImage *integralImage = cvCreateImage(cvSize(resizedImage->width+1, 
																														 resizedImage->height+1), 
																											IPL_DEPTH_32S, 1);
							cvIntegral(resizedImage, integralImage);

							ImageType classifiedImage = OTHER;

							//create haarOutput object that contains type and haar values of image
							haarOut = new HaarOutput;
				
							// save haar features
							applyHaar(integralImage, haarOut);

							

							if(tree){
								double percent = 0.0;

								classifiedImage = tree->classify(haarOut, &percent);

								//test this image
								if (classifiedImage == MUG && percent > highestPercent){
						
									highestPercent = percent;
									
									obj.rect = cvRect(x,y,w,h);
									obj.label = "mug";
									
								}
							}
							delete haarOut;
							

							cvReleaseImage(&clippedImage);
							cvReleaseImage(&resizedImage);
							cvReleaseImage(&integralImage);
						}
					}
				}
			}
		}
		cvReleaseImage(&gray);

		if(highestPercent > .9)
			objects->push_back(obj);

		return true;

}
        
// train
// Trains the classifier to recognize the objects given in the
// training file list.
bool CClassifier::train(TTrainingFileList& fileList)
{



		// create haars
		readHaars();

    // example code for loading and resizing image files--
    // you may find this useful for the milestone    
    IplImage *image, *smallImage, *integralo, *gray;
		HaarOutput *haarOut;

		std::vector<HaarOutput*> haarOutVec;

    cout << "Processing images..." << endl;
		//grey scale image 
    smallImage = cvCreateImage(cvSize(64, 64), IPL_DEPTH_8U, 1);
		//integral image
		integralo = cvCreateImage(cvSize(65, 65), IPL_DEPTH_32S, 1);	

		bool flagM = true;
		bool flagO = true;

    for (int i = 0; i < (int)fileList.files.size(); i++) {
			// show progress
			if (i % 1000 == 0) {
					showProgress(i, fileList.files.size());
			}

			// skip non-mug and non-other images (milestone only)
			if ((fileList.files[i].label == "mug" && flagM) ||
			(fileList.files[i].label == "other" && flagO)) {
				

				/*  USED FOR LIMITING INPUTS...COMMENTED OUT FOR SUBMISSION
				if(fileList.files[i].label == "mug" && c > 25){
					flagM = false;
				}
				if(fileList.files[i].label == "other" && c > 50){
					flagO = false;
				}
			  c++;//incrementing counter+
 				if(c > 100){//if the counter is more then break
 					break;
 				}*/
				
				// load the image
				image = cvLoadImage(fileList.files[i].filename.c_str(), 0);
				if (image == NULL) {
					cerr << "ERROR: could not load image "
							 << fileList.files[i].filename.c_str() << endl;
					continue;
				}

				//this checks to see if the channel is 1, if not
				//it converts it to gray scale
				//otherwise makes gray point to the same image
				if(image->nChannels != 1){
					gray = cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1);
					cvCvtColor(image,gray,CV_BGR2GRAY);
					//releasing old image as we don't need it now
					cvReleaseImage(&image);
				}else{
					gray = image;
				}

				//Image display code, not needed for submission
				//could display image
				//cvNamedWindow("WindowName",CV_WINDOW_AUTOSIZE);//creating view
				//window - put outside loop
				//	cvShowImage("WindowName",gray); //display on screen
				//	cvWaitKey(1); //wait for key press
				//remember to releaseImage...
				//cvDestroyWindow("WindowName");//destroying view window - put
				//outside loop

			  // resize to 64 x 64
			  cvResize(gray, smallImage);

				// create integral image
				cvIntegral(smallImage, integralo);
				
				//create haarOutput object that contains type and haar values of image
				haarOut = new HaarOutput;

				// save image type
				if(fileList.files[i].label == "mug"){
					haarOut->type = MUG;
				}
				else{
					haarOut->type = OTHER;
				}
				
				// save haar features
				applyHaar(integralo, haarOut);

				// add to struct of features.
				haarOutVec.push_back(haarOut);

			  // free memory
				//releasing gray (if the image was grayscale to begin with this
			  //should point to the same thing and still work
				cvReleaseImage(&gray);
			}
    }
		//	cvDestroyWindow("WindowName");//destroying view window - put
				//outside loop
    // free memory
    cvReleaseImage(&smallImage);
		cvReleaseImage(&integralo);

    cout << endl;

		if(tree != NULL)
			delete tree;

		cout << "making tree"<<endl;
		tree = Classer::create(haarOutVec);
		cout << "finished with tree" << endl;
		
		// clear out haar feature data
		for(unsigned i=0;i<haarOutVec.size();++i)
			delete haarOutVec[i];

    return true;
}

// creates set of haar values for an image.
void CClassifier::applyHaar(const IplImage *im, HaarOutput *haary){

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



		}
		++it;
		
		out = (z - 2*t)/z;
		haary->haarVals[i] = fabs(out);
		//std::cout << "haar value = " << fabs(out) << std::endl;
		++i;
	}

}

// reads haar presets from file
void CClassifier::readHaars(){


	haars.clear();
	std::string tmp;
	ifstream ifs ( "haarfeatures.txt" , ifstream::in );

	// get all the haars
  while (getline(ifs,tmp))
		haars.push_back(strToHaar(tmp));
	
	ifs.close();
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
