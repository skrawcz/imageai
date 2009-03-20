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
		gray = NULL;
		frameCount = 0;
		readStuff = false;
		//	frameJump = CfgReader::getInt("frameJump");
		


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

		if(gray != NULL)
			cvReleaseImage(&gray);
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
    //std::cout<<"in save state 1"<<std::endl;
    if(tree == NULL)	
			return false;
    //std::cout<<"in save state 2"<<std::endl;

    return Classer::printToXML(filename, tree);
}

// run
// Runs the classifier over the given frame and returns a list of
// objects found (and their location).
bool CClassifier::run(const IplImage *frame, CObjectList *objects)
{
	if(!readStuff){
		frameJump = CfgReader::getInt("frameJump");
		readStuff=true;
	}
		
		if(frameCount % frameJump == 0){// && frameCount > 150){
			assert((frame != NULL) && (objects != NULL));

			IplImage *oldGray = NULL;

			if(gray != NULL){

				oldGray = gray;//cvCreateImage(cvGetSize(gray),IPL_DEPTH_8U,1);
				//cvCopy(gray, oldGray);
			}

			//convert to gray scale
			gray = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
			cvCvtColor(frame,gray,CV_BGR2GRAY);

			double dx=0, dy=0;

			// calc optical flow stuff
			if(oldGray != NULL){

				CvMat *dxMat = cvCreateMat(gray->height, gray->width, CV_32FC1);
				CvMat *dyMat = cvCreateMat(gray->height, gray->width, CV_32FC1);

				cvCalcOpticalFlowLK(gray, oldGray, cvSize(5,5), dxMat, dyMat);

				dx = cvAvg(dxMat).val[0];
				dy = cvAvg(dyMat).val[0];

				cvReleaseMat(&dxMat);
				cvReleaseMat(&dyMat);
				cvReleaseImage(&oldGray);

				//std::cout << dx << " " << dy << std::endl;

			}

			// feature vector of image plus one in size to work with boost
			//		CvMat *imageData = cvCreateMat(1, Features::amountOfFeatures(), CV_32F);
			CvMat *imageData = cvCreateMat(1, featureSet->amountOfFeatures(), CV_32F);


			CObject obj;

			double percent[5];
			double threshold[5];
      threshold[Features::MUG] = CfgReader::getDouble("isObjectThreshold");
      threshold[Features::CLOCK] = threshold[Features::MUG];
      threshold[Features::SCISSORS] = threshold[Features::MUG] + 1.5;
      threshold[Features::STAPLER] = threshold[Features::MUG] + 0.2;
      threshold[Features::KEYBOARD] = threshold[Features::MUG] - 0.2;



			IplImage *resizedImage = cvCreateImage(cvSize(64,64), gray->depth, gray->nChannels);
			IplImage *integralImage = cvCreateImage(cvSize(65, 65), IPL_DEPTH_32S, 1);
			IplImage *IntegralImageSquare = cvCreateImage(cvSize(65, 65), IPL_DEPTH_64F, 1);
			IplImage *IntegralImageTilted = cvCreateImage(cvSize(65, 65), IPL_DEPTH_32S, 1);


			//iterate through candidate frames
			for (int x = 0; x <=320; x = x+8){
				for (int y = 0; y<=240; y = y+8){
					for (int w = 64; w<=320; w = w+8){
						for (int h = 64 ; h<=240; h = h+8){
							//check if we are out of frame & for milestone only take squares
							if( (x+w <= gray->width) && (y+h <= gray->height) && isRatio(w,h)) {
								//clip the image to the right size
								CvRect region = cvRect(x,y,w,h);
								IplImage *clippedImage = cvCreateImage(cvSize(region.width, region.height), 
																											 gray->depth, gray->nChannels);
								cvSetImageROI(gray,region);
								cvCopyImage(gray, clippedImage);
								cvResetImageROI(gray);

								
								//scale image to 64x64
								
								cvResize(clippedImage, resizedImage);
					
								//compute integral image
								cvIntegral(resizedImage, integralImage, IntegralImageSquare, IntegralImageTilted);

								featureSet->getFeatures(integralImage, IntegralImageTilted, imageData, 0,resizedImage);

								if(tree){

									//classifiedImage =
									tree->classify(imageData, percent);

									for(int k=0; k<5;k++){
									//std::cout << percent[k] << std::endl;	
										if(percent[k]> threshold[k]){
											//std::cout << percent[k] << std::endl;										
											obj.rect = cvRect(x,y,w,h);
											obj.label = Features::imageTypeToString(Features::ImageType(k));
											obj.score = percent[k];
											obj.type = k;
                      if (obj.type == Features::MUG || obj.type == Features::CLOCK){
                        if(w==h){
                          objects->push_back(obj);
                        }
                          
                      }else{
                        if(w!=h){
                          objects->push_back(obj);
                        }
                      }											
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

							
							//	std::cout<<"in loop"<<std::endl;
								cvReleaseImage(&clippedImage);
								
							}
						}
					}
				}
			}

			cvReleaseImage(&resizedImage);
			cvReleaseImage(&integralImage);
			cvReleaseImage(&IntegralImageSquare);

			
			CObject::boostScores(*objects, previousObjects, dx, dy);
			CObject::copyOverwrite(*objects, previousObjects);
			CObject::filterOverlap(*objects);
			//CObject::stefansOverlap(*objects,3);


			// save values
			CObject::copyOverwrite(*objects, tmpDisplayObjects);

		// skipping frame
		}else{

			// save previous objects
			CObject::copyOverwrite(tmpDisplayObjects, *objects);

		}

		frameCount++;			
		//std::cout<<"returning"<<std::endl;
		return true;

}

bool CClassifier::isRatio(int width, int height){
	double ratio = ((double)width)/((double)height);
	//if (ratio == 5/2 || ratio == 10/3 || ratio == 30/11 || ratio == 1){
	if (ratio == 30/11 || ratio == 1){
		return true;

	}else{
		return false;
	}

}


        
// train
// Trains the classifier to recognize the objects given in the
// training file list.
bool CClassifier::train(TTrainingFileList& fileList)
{



		// create haars
    featureSet = new Features();	


    IplImage *image, *smallImage, *integralo, *gray, *integraloTilto, *integraloSquaro;

		// figure out if a subset of images is wanted
		int subset = CfgReader::strToInt(CfgReader::getValue("useSubset"));
		//std::cout << "Training on a subset of about: " << subset  << std::endl;
		srand ( time(NULL) );
		int count = 0;

		// create cv matrix that has a row of features for every image
		//CvMat *imageData = cvCreateMat((int)fileList.files.size(),
		//Features::amountOfFeatures(), CV_32FC1);
		//std::cout<<"there are this many features "<<featureSet->amountOfFeatures()<<std::endl;

		CvMat *imageData = cvCreateMat((int)(fileList.files.size()/subset), featureSet->amountOfFeatures(), CV_32FC1);

		// keep track of type of image
		CvMat *imageTypes = cvCreateMat((int)(fileList.files.size()/subset), 1, CV_32SC1);


    //std::cout << "Processing images..." << std::endl;
		//grey scale image 
    smallImage = cvCreateImage(cvSize(64, 64), IPL_DEPTH_8U, 1);
		//integral image
		integralo = cvCreateImage(cvSize(65, 65), IPL_DEPTH_32S, 1);
		integraloTilto = cvCreateImage(cvSize(65, 65), IPL_DEPTH_32S, 1);
		integraloSquaro = cvCreateImage(cvSize(65, 65), IPL_DEPTH_64F, 1);

    for (int i = 0;i < (int)fileList.files.size(); i++) { 
								 //i < 20;++i){

			// show progress
			/*if (i % 1000 == 0) {
					showProgress(i, fileList.files.size());
			}*/


			// doing this we simply avoid some of the "other" images 

			if(subset == 1 || Features::stringToImageType(fileList.files[i].label) != 5 || (rand() % subset == 0 &&  imageData->rows - count > 284)) {
				//if(Features::stringToImageType(fileList.files[i].label) != 1)
				//	continue;
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
				cvIntegral(smallImage, integralo, integraloSquaro, integraloTilto);
			

				featureSet->getFeatures(integralo, integraloTilto, imageData, count, smallImage);
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

   // std::cout << std::endl;

		if(tree != NULL)
			delete tree;

		//std::cout << "making tree"<<std::endl;
		tree = Classer::create(imageData, imageTypes);
		//std::cout << "finished with tree" << std::endl;

		cvReleaseMat(&imageData);
		cvReleaseMat(&imageTypes);

		

    return true;
}


