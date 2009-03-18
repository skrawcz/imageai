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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "classer.h"
#include "CfgReader.h"

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

		std::cout << "wefgsfdsf" << std::endl;

    return true;
}

// saveState
// Writes classifier configuration to the given file
bool CClassifier::saveState(const char *filename)
{
    assert(filename != NULL);
    
    if(tree == NULL)	
			return false;


    return Classer::printToXML(filename, tree);
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

		// feature vector of image plus one in size to work with boost
		//		CvMat *imageData = cvCreateMat(1, Features::amountOfFeatures(), CV_32F);
		CvMat *imageData = cvCreateMat(1, featureSet->amountOfFeatures(), CV_32F);


		CObject obj;

		double highestPercent = 0.01;
		double percent[5];
		double threshold = 1.5;//CfgReader::getDouble("threshold");


		//iterate through candidate frames
		for (int x = 0; x <=320; x = x+8){
			for (int y = 0; y<=240; y = y+8){
				for (int w = 64; w<=320; w = w+8){
					for (int h = 64 ; h<=240; h = h+8){
						//check if we are out of frame & for milestone only take squares
						if( (x+w <= gray->width) && (y+h <= gray->height) && h == w) {
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


							featureSet->getFeatures(integralImage, imageData, 0,resizedImage);

							if(tree){
								

								//classifiedImage =
								tree->classify(imageData, percent);

								for(int k=0; k<5;k++){
									
									if(percent[k]> threshold){
										std::cout << percent[k] << std::endl;										
										obj.rect = cvRect(x,y,w,h);
										obj.label = Features::imageTypeToString(Features::ImageType(k));

										objects->push_back(obj);
									}
								}

								/*test this image
								if (classifiedImage != Features::OTHER && percent > highestPercent){
						
									highestPercent = percent;
									//std::cout << classifiedImage << std::endl;
									obj.rect = cvRect(x,y,w,h);
									obj.label = Features::imageTypeToString(classifiedImage);
									
								}
								*/
							}
							

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
			

		return true;

}
        
// train
// Trains the classifier to recognize the objects given in the
// training file list.
bool CClassifier::train(TTrainingFileList& fileList)
{



		// create haars
    featureSet = new Features();	


    IplImage *image, *smallImage, *integralo, *gray;

		// figure out if a subset of images is wanted
		int subset = CfgReader::strToInt(CfgReader::getValue("useSubset"));
		std::cout << "Training on a subset of about: " << subset  << std::endl;
		srand ( time(NULL) );
		int count = 0;

		// create cv matrix that has a row of features for every image
		//CvMat *imageData = cvCreateMat((int)fileList.files.size(),
		//Features::amountOfFeatures(), CV_32FC1);
		std::cout<<"there are this many features "<<featureSet->amountOfFeatures()<<std::endl;

		CvMat *imageData = cvCreateMat((int)(fileList.files.size()/subset), featureSet->amountOfFeatures(), CV_32FC1);

		// keep track of type of image
		CvMat *imageTypes = cvCreateMat((int)(fileList.files.size()/subset), 1, CV_32SC1);


    std::cout << "Processing images..." << std::endl;
		//grey scale image 
    smallImage = cvCreateImage(cvSize(64, 64), IPL_DEPTH_8U, 1);
		//integral image
		integralo = cvCreateImage(cvSize(65, 65), IPL_DEPTH_32S, 1);

    for (int i = 0;i < (int)fileList.files.size(); i++) { 
								 //i < 20;++i){

			// show progress
			if (i % 1000 == 0) {
					showProgress(i, fileList.files.size());
			}


			// doing this we simply avoid some of the "other" images 
			if(subset == 1 || Features::stringToImageType(fileList.files[i].label) != 5 || (rand() % subset == 0 &&  imageData->rows - count > 284)) {


				count++;

				if(count >= imageData->rows)
					break;
			
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

				// resize to 64 x 64
				cvResize(gray, smallImage);

				// create integral image
				cvIntegral(smallImage, integralo);
			

				featureSet->getFeatures(integralo, imageData, count, smallImage);
				//			featureSet->getHOGFeatures(smallImage,imageData,i);
				//featureSet->getHOGFeatures(smallImage,imageData,i);

				cvSetReal1D( imageTypes, count, Features::stringToImageType(fileList.files[i].label) );

				//std::cout << Features::stringToImageType(fileList.files[i].label) << std::endl;

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

    std::cout << std::endl;

		if(tree != NULL)
			delete tree;

		std::cout << "making tree"<<std::endl;
		tree = Classer::create(imageData, imageTypes);
		std::cout << "finished with tree" << std::endl;


		cvReleaseMat(&imageData);
		cvReleaseMat(&imageTypes);
		

    return true;
}


