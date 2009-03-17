#include "features.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#define PI 3.14159265


Features::Features(){

	featureType = HAAR;

  readHaars();
}

Features::~Features(){
  
}

void Features::getFeatures(const IplImage *im, CvMat *data, int item){


	if(featureType == HAAR)
		getHaarFeatures(im, data, item);
	
	// if the amount of features is uneven then make sure last element is set to zero
	if(amountOfFeaturesRounded){

		*( (float*)CV_MAT_ELEM_PTR( *data, item, amountOfFeatures() - 1 ) ) = 0.0f;

	}

}

void Features::getHaarFeatures(const IplImage *im, CvMat *data, int item){

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
		*( (float*)CV_MAT_ELEM_PTR( *data, item, i ) ) = fabs(out);
		//std::cout << "haar value = " << fabs(out) << std::endl;
		++i;
	}
}

// reads haar presets from file
void Features::readHaars(){


	haars.clear();
	std::string tmp;
	std::ifstream ifs ( "haarfeatures.txt" , std::ifstream::in );

	// get all the haars
  while (getline(ifs,tmp))
		haars.push_back(strToHaar(tmp));
	
	ifs.close();
}


// reads a string and turn it into a haar struct.
Features::HaarFeature Features::strToHaar(const std::string &in){

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

void Features::getHOGFeatures(const IplImage *im, CvMat *data, int item){



	//making space to save gradient images
	IplImage *dstx = cvCreateImage(cvGetSize(im),IPL_DEPTH_8U,1);
	IplImage *dsty = cvCreateImage(cvGetSize(im),IPL_DEPTH_8U,1);
	IplImage *smoothed =cvCreateImage(cvGetSize(im),IPL_DEPTH_8U,1);
	cvSmooth(im   , smoothed  , CV_GAUSSIAN, 3, 3 );
	
	//getting the derivatives
	cvSobel(smoothed,dstx,1,0);
	cvSobel(smoothed,dsty,0,1);

	//need to get the data out of the iplimage format
	double dxarray[dstx->height * dstx->width];
	int count=0;
	for( int y=0; y<dstx->height; y++ ) {
		uchar* ptr = (uchar*) (dstx->imageData + y * dstx->widthStep);
		for( int x=0; x<dstx->width; x++ ) {
			dxarray[count++]= ptr[1*x+1];
		}
	}
	double dyarray[dsty->height*dsty->width];
	count=0;
	for( int y=0; y<dsty->height; y++ ) {
		uchar* ptr = (uchar*) (dsty->imageData + y * dsty->widthStep);
		for( int x=0; x<dsty->width; x++ ) {
			dyarray[count++]= ptr[1*x+1];
		}
	}
	
	//need to setup histograms here.
	int numBins[]={12};
	float range[]={0,360};
	float *ranges[]= {range};
	CvHistogram* hist;
	//cvMakeHistHeaderForArray(1, numBins, hist,dxarray, ranges);

	// = cvCreateHist(1,numBins,CV_HIST_ARRAY,ranges);
	
	float orientations[dstx->height * dstx->width];
	float magnitudes[dstx->height * dstx->width];
	//calculating the gradient magnitudes here
	for(int i=0; i< (dstx->height * dsty->width);i++){
		double ex = dxarray[i];
		double ey = dyarray[i];
		double mag = sqrt(pow(ex,2) + pow(ey,2));
		double o = atan2(ey,ex); //in radians -Pi to Pi
		double alpha = o*180/PI;
		double alpha_signed = alpha;
		if(alpha < 0.0){
			alpha_signed = alpha+360.0;
		}
		magnitudes[i]=(float)mag;
		orientations[i]=(float)alpha_signed;
		//now need to bin it for each cell
		//std::cout<<"alpha = "<<alpha_signed<<std::endl;
	}
	//need to setup data arrays to store data to make histograms from
	//to capture strong edges, need to weight them by the magnitude
	int numCells = 64;
	float *cellOPtr[numCells];
	float *cellMPtr[numCells];
	//not doing overlaping anymore, just doing simple method
	//this points to the 15, as they are 8x8 and we
	//move them over ever 4  cells

	int width=64;
	int height=64;
	int cellWidth=8;
	for(int i=0; i< numCells;i++){
		//one cell
		//1stcell start=0, next = 7, next =15
		int start=i*cellWidth;
		//start 0-7 = first row
		float oCell[64];
		float mCell[64];
		int idx=0;
		for(int row=0;row<cellWidth;row++){
			int rowStart=start + cellWidth*(row-1);
			for(int k=0;k<cellWidth;k++){
				oCell[idx]=orientations[rowStart+k];
				mCell[idx++]=magnitudes[rowStart+k];
			}
		}
		cellOPtr[i]=oCell;
		cellMPtr[i]=mCell;
	}
				
	for(int q=0;q<numCells;q++){
		std::cout<<"entry ";
		for(int j=0;j<64;j++){
				std::cout<<cellOPtr[q][j]<<" ";
		
		}
		std::cout<<std::endl;
	}

	

	//need to calculate the gradient orienation




	//==== display debugging stuff here=======
  //making display windows
	cvNamedWindow("HOG IN image");
	cvNamedWindow("Smoothed IN image");
	cvNamedWindow("dx image");
	cvNamedWindow("dy image");

	//showing images
	cvShowImage("HOG IN image",im);
	cvShowImage("Smoothed IN image",smoothed);
	//showing x and y gradient images
	cvShowImage("dx image",dstx);
	cvShowImage("dy image",dsty);
 
	
	cvWaitKey( 0 );
	//====== releasing stuff============
	cvReleaseImage(&dstx);
	cvReleaseImage(&dsty);
	cvReleaseImage(&smoothed);

	//killing display windows
	cvDestroyWindow("HOG IN image");
	cvDestroyWindow("Smoothed IN image");
	cvDestroyWindow("dx image");
	cvDestroyWindow("dy image");
}
