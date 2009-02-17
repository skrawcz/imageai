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

    if(featureSet != NULL)
      delete featureSet;
}

// loadState
// Configure the classifier from the given file.
bool CClassifier::loadState(const char *filename)
{

    assert(filename != NULL);

    featureSet = new Features();		
    //readHaars();

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

		
		std::ofstream ofs ( filename );

		
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


		Features::HaarOutput *haarOut;
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

							Features::ImageType classifiedImage = Features::OTHER;

							//create haarOutput object that contains type and haar values of image
							haarOut = new Features::HaarOutput;
				
							// save haar features
              featureSet->getHaarFeatures(integralImage, haarOut);							
              //applyHaar(integralImage, haarOut);

							

							if(tree){
								double percent = 0.0;

								classifiedImage = tree->classify(haarOut, &percent);

								
								//test this image
								if (classifiedImage != Features::OTHER && percent > highestPercent){
						
									highestPercent = percent;
									std::cout << classifiedImage << std::endl;
									obj.rect = cvRect(x,y,w,h);
									obj.label = Features::imageTypeToString(classifiedImage);
									
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

		//if(highestPercent > .9)
			objects->push_back(obj);

		return true;

}
        
// train
// Trains the classifier to recognize the objects given in the
// training file list.
bool CClassifier::train(TTrainingFileList& fileList)
{



		// create haars
    featureSet = new Features();	
		//readHaars();

    // example code for loading and resizing image files--
    // you may find this useful for the milestone    
    IplImage *image, *smallImage, *integralo, *gray;
		Features::HaarOutput *haarOut;

		std::vector<Features::HaarOutput*> haarOutVec;

    std::cout << "Processing images..." << std::endl;
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
				std::cerr << "ERROR: could not load image "
						 << fileList.files[i].filename.c_str() << std::endl;
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
			haarOut = new Features::HaarOutput;

			// save image type
			haarOut->type = Features::stringToImageType(fileList.files[i].label);
			
			
			// save haar features
			//applyHaar(integralo, haarOut);
      featureSet->getHaarFeatures(integralo, haarOut);			

			// add to struct of features.
			haarOutVec.push_back(haarOut);

		  // free memory
			//releasing gray (if the image was grayscale to begin with this
		  //should point to the same thing and still work
			cvReleaseImage(&gray);

    }
		//	cvDestroyWindow("WindowName");//destroying view window - put
				//outside loop
    // free memory
    cvReleaseImage(&smallImage);
		cvReleaseImage(&integralo);

    std::cout << std::endl;

		if(tree != NULL)
			delete tree;

		std::cout << "making tree"<<std::endl;
		tree = Classer::create(haarOutVec);
		std::cout << "finished with tree" << std::endl;
		
		// clear out haar feature data
		for(unsigned i=0;i<haarOutVec.size();++i)
			delete haarOutVec[i];

    return true;
}


