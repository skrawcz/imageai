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
#include "decisionTree.h"

#include "classifier.h"

#include "CXMLParser.h"
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

		if(tree)
			delete tree;

		ifstream ifs ( filename, ifstream::in );

		std::string current;

		// figure out what type of things the tree classifies
		CXMLParser::getNextValue(ifs, current);

		CClassifier::ImageType treeType = (CClassifier::ImageType)atoi(current.c_str());

		getline(ifs, current);

		if(current.find("node") != std::string::npos)
			tree = new DecisionTree(ifs, true);
		else
			tree = new DecisionTree(ifs, false);

		tree->setTreeType(treeType);

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

		ofs << "<treeType>" << tree->getTreeType() << "</treeType>\n";
		tree->print(ofs, 0);

    ofs.close();
    return true;
}

// run
// Runs the classifier over the given frame and returns a list of
// objects found (and their location).
bool CClassifier::run(const IplImage *frame, CObjectList *objects)
{
    assert((frame != NULL) && (objects != NULL));
    
    // CS221 TO DO: replace this with your own code

		//convert to gray scale
		IplImage *gray;
		gray = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
		cvCvtColor(frame,gray,CV_BGR2GRAY);

		HaarOutput *haarOut;

		for (int x = 0; x <=320; x = x+8){
			for (int y = 0; y<=240; y = y+8){
				for (int w = 64; w<=320; w = w+8){
					for (int h = 64 ; h<=240; h = h+8){
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
							//	cvReleaseImage(&clippedImage); done below when I show
							//the image
							
							/*if(w>200){
								//could display image
								cvNamedWindow("WindowName",CV_WINDOW_AUTOSIZE);//creating view
								//window - put outside loop
								cvShowImage("WindowName",resizedImage); //display on screen
								cvWaitKey(0); //wait for key press
								//remember to releaseImage...
								cvDestroyWindow("WindowName");//destroying view window - put
								//outside loop
							}*/
					
							//compute integral image
							IplImage *integralImage = cvCreateImage(cvSize(resizedImage->width+1, 
																														 resizedImage->height+1), 
																											IPL_DEPTH_32S, 1);
							cvIntegral(resizedImage, integralImage);
							//cvReleaseImage(&resizedImage);

							ImageType classifiedImage = OTHER;

							//create haarOutput object that contains type and haar values of image
							haarOut = new HaarOutput;
				
							// save haar features
							applyHaar(integralImage, haarOut);

							if(tree){
								//tree->print(
								//saveState("check.txt");
								//exit(-1);
								classifiedImage = tree->classify(haarOut);

								//could display image
								//cvNamedWindow("WindowName",CV_WINDOW_AUTOSIZE);//creating view
								//window - put outside loop
								//cvShowImage("WindowName",resizedImage); //display on screen
								//cvWaitKey(0); //wait for key press
								//remember to releaseImage...
								//cvDestroyWindow("WindowName");//destroying view window - put
								//outside loop
								//exit(-1);
							}
							delete haarOut;
							
							
							//test this image
							if (classifiedImage == MUG){
								CObject obj;
								obj.rect = cvRect(x,y,w,h);
								obj.label = "mug";
								objects->push_back(obj);
								
								//std::cout << "classified image = "<< classifiedImage <<std::endl;
								/*
								//could display image
								cvNamedWindow("WindowName",CV_WINDOW_AUTOSIZE);//creating view
								//window - put outside loop
								cvShowImage("WindowName",resizedImage); //display on screen
								cvWaitKey(0); //wait for key press
								//remember to releaseImage...
								cvDestroyWindow("WindowName");//destroying view window - put
								//outside loop
								*/
								
							}else{
								//CObject obj;
								//obj.rect = cvRect(x,y,w,h);
								//obj.label = "Other";
								//objects->push_back(obj);
								//cvDestroyWindow("WindowName");//destroying view window - put
								//outside loop
								//could display image
								//cvNamedWindow("WindowName",CV_WINDOW_AUTOSIZE);//creating view
								//window - put outside loop
								//cvShowImage("WindowName",clippedImage); //display on screen
								//cvWaitKey(0); //wait for key press
								//cvShowImage("WindowName",resizedImage);
								//	cvWaitKey(0);
								//remember to releaseImage...
								//cvDestroyWindow("WindowName");
							}
							//if(x%16==0 && y%16==0){
							//	CObject obj;
							//	obj.rect = cvRect(x,y,w,h);
							//	obj.label = "";
							//	objects->push_back(obj);
							//
							//}
							cvReleaseImage(&clippedImage);
							cvReleaseImage(&resizedImage);
							cvReleaseImage(&integralImage);
							//exit(-1);
						}
					}
					//cvWaitKey(0);
					//exit(-1);
					
				}
			}
		}
		cvReleaseImage(&gray);

		return true;


/*
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

*/
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
    IplImage *image, *smallImage, *integralo, *gray;
		HaarOutput *haarOut;

		std::vector<HaarOutput*> haarOutVec;

    cout << "Processing images..." << endl;
		//grey scale image 
    smallImage = cvCreateImage(cvSize(64, 64), IPL_DEPTH_8U, 1);
		//integral image
		integralo = cvCreateImage(cvSize(65, 65), IPL_DEPTH_32S, 1);	

		int c = 0; //my counter to only load a certain amount of images
		bool flagM = true;
		bool flagO = true;
		//cvNamedWindow("WindowName",CV_WINDOW_AUTOSIZE);//creating view
				//window - put outside loop
    for (int i = 0; i < (int)fileList.files.size(); i++) {
			// show progress
			if (i % 1000 == 0) {
					showProgress(i, fileList.files.size());
			}

			// skip non-mug and non-other images (milestone only)
			if ((fileList.files[i].label == "mug" && flagM) ||
			(fileList.files[i].label == "other" && flagO)) {
				

				/*
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
				//cout << "loaded image" << endl;
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
					//std::cout << "mug" << endl;
				}
				else{
					haarOut->type = OTHER;
				}
				
				// save haar features
				applyHaar(integralo, haarOut);

				// add to struct of features.
				haarOutVec.push_back(haarOut);

			  // free memory
			  //cvReleaseImage(&image); potential memory bug maybe?
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

		// create a vector of attributes, ie the different haar features.
		std::vector<bool> attribs;

		for(unsigned i=0;i<HAARAMOUNT;++i)
			attribs.push_back(true);


		if(tree != NULL)
			delete tree;
		cout << "making tree"<<endl;
		tree = new DecisionTree(haarOutVec, attribs, -1, CClassifier::OTHER);
		cout << "finished with tree" << endl;
		
		// clear out haar feature data
		for(unsigned i=0;i<haarOutVec.size();++i)
			delete haarOutVec[i];
		/*
		//=====================================================
		//we should test our tree here
		int	correct =0;
		//grey scale image 
    smallImage = cvCreateImage(cvSize(64, 64), IPL_DEPTH_8U, 1);
		//integral image
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
				//std::cout<<"got here1"<< std::endl;
				if(image->nChannels != 1){
					//	std::cout <<"image was colour"<<std::endl;
					gray = cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1);
					cvCvtColor(image,gray,CV_BGR2GRAY);
					//releasing old image as we don't need it now
					cvReleaseImage(&image);

				}else{
					//std::cout<<"image was gray"<<std::endl;
					gray = image;
				}
				//std::cout<<"got here2"<< std::endl;
				
				 // resize to 64 x 64
			  cvResize(gray, smallImage);
				//std::cout<<"got here3"<< std::endl;
				// create integral image
				cvIntegral(smallImage, integralo);
				//std::cout<<"got here4"<< std::endl;
				//create haarOutput object that contains type and haar values of image
				haarOut = new HaarOutput;

				// save image type
				if(fileList.files[i].label == "mug"){
					haarOut->type = MUG;
					//std::cout << "mug" << endl;
				}
				else{
					haarOut->type = OTHER;
				}
				
				// save haar features
				applyHaar(integralo, haarOut);
				ImageType classifiedImage = OTHER;
				if(tree){
					classifiedImage = tree->classify(haarOut);
				}
					//test this image
				if (classifiedImage == haarOut->type){
					correct++;
				}
				delete haarOut;
				cvReleaseImage(&gray);
				
			}
						
		 }
		 cvReleaseImage(&smallImage);
		 cvReleaseImage(&integralo);
		 std::cout << "correct = "<< correct << std::endl;*/
		 //====================================================
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
