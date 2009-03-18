#include "features.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include <vector>
#include "CfgReader.h"
#define PI 3.14159265

#define HAARAMOUNT 147
#define HOGAMOUNT 64*9 //64*9
#define RIHOGAMOUNT 64*9 //64*9
#define HCORNERAMOUNT 1
//#define TYPECOUNT 5

Features::Features(){

	featureType = (FeatureType)CfgReader::getInt("featureType");
	switch(featureType){
	case HAAR:
	case HAARTILT:
		numOfFeatures = HAARAMOUNT;
		break;
	case HAAR_HAARTILT:
		numOfFeatures = HAARAMOUNT*2;
		break;
	case HOGN:
	case HOGUN:
	case HOGRI:
		numOfFeatures = HOGAMOUNT;
		break;
	case HAAR_HOG:  
		numOfFeatures = HAARAMOUNT + HOGAMOUNT;
		break;
	case HCORNER:
		numOfFeatures = HCORNERAMOUNT;
		break;
	case HAAR_HCORNER:
		numOfFeatures = HAARAMOUNT + HCORNERAMOUNT;
		break;
	case HOG_HCORNER:
		numOfFeatures = HOGAMOUNT + HCORNERAMOUNT;
		break;
	case HAAR_HOG_HCORNER:
		numOfFeatures = HAARAMOUNT + HOGAMOUNT + HCORNERAMOUNT;
		break;
	default:
		std::cout<<"error, unrecognized feature type put int"<<std::endl;
		numOfFeatures = -1;
		break;
	}
	if(numOfFeatures%2==0){
		needToPadFeature=false;
	}else{
		needToPadFeature=true;
		numOfFeatures = numOfFeatures+1;
	}


	//	featureType = HAAR;
	//	featureType = HOG;
	
	//featureType = HAARHOG;
	//featureType = HCORNER;
	//	featureType = ALL; //haar hog hcorner
	//featureType = ALL2;  //haar hog hcorner rihog
	//featureType = ALL3;  //haar rihog hcorner
	//featureType = HAARCORNER;
	//featureType = HOGCORNER;
	//featureType = RIHOG;

  readHaars();
}

Features::~Features(){
	//should probably deallocate the features...
}

bool Features::amountOfFeaturesRounded(){
	
	return needToPadFeature;

}

int Features::amountOfFeatures(){
	
	return numOfFeatures;
}


void Features::getFeatures(const IplImage *im, const IplImage *imTilt, CvMat *data, int item, const IplImage *realImg){

	switch(featureType){
	case HAAR:
		getHaarFeatures(im, data, item,0);
		break;
	case HAARTILT:
		getHaarFeatures(imTilt, data, item,HAARAMOUNT/2);
		break;
	case HAAR_HAARTILT:
		getHaarFeatures(im, data, item,0);
		getHaarFeatures(imTilt, data, item,HAARAMOUNT/2);
		break;
	case HOGN:
		getNormHOGFeatures(realImg,data,item,0);
	case HOGUN:
		getUnNormHOGFeatures(realImg,data,item,0);
	case HOGRI:
		getRINormHOGFeatures(realImg,data,item,0);
 		break;
	case HAAR_HOG:
		getHaarFeatures(im, data, item,0);
		getNormHOGFeatures(realImg,data,item,HAARAMOUNT);
		break;
	case HCORNER:
		getHarrisCornerCount(realImg,data,item,0);
		break;
	case HAAR_HCORNER:
		getHarrisCornerCount(realImg,data,item,0);
		getHaarFeatures(im, data, item,HCORNERAMOUNT);
		break;
	case HOG_HCORNER:
		getHarrisCornerCount(realImg,data,item,0);
		getNormHOGFeatures(realImg,data,item,HCORNERAMOUNT);
		break;
	case HAAR_HOG_HCORNER:
		getHaarFeatures(im, data, item,0);
		getNormHOGFeatures(realImg,data,item,HAARAMOUNT);
		getHarrisCornerCount(realImg,data,item,HAARAMOUNT+HOGAMOUNT);
		break;
	default:
		std::cout<<"error, unrecognized feature type put int"<<std::endl;
		break;
	}

	// if the amount of features is uneven then make sure last element is set to zero
	if(amountOfFeaturesRounded()){
		//	std::cout<<"rounded features"<<std::endl;
		//		*( (float*)CV_MAT_ELEM_PTR( *data, item, amountOfFeatures() - 1 ) ) = 0.0f;
		cvSetReal2D(data,item,amountOfFeatures()-1,0.0f);
	}

}

void Features::getHaarFeatures(const IplImage *im, CvMat *data, int item,
															 int startIndex){

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
		if(z != 0){
			out = (z - 2*t)/z;
		}else{
			z = 0;
		}		

		//*( (float*)CV_MAT_ELEM_PTR( *data, item, i ) ) = fabs(out);
		cvSetReal2D( data,item,startIndex++,fabs(out));
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
//gets the normalized HOG features
void Features::getNormHOGFeatures(const IplImage *im, CvMat *data, int item,
															int startIndex){
	getHOGFeatures(im,data,item,startIndex,false,true);
}
//gets the unnormalized HOG features
void Features::getUnNormHOGFeatures(const IplImage *im, CvMat *data, int item,
															int startIndex){
	getHOGFeatures(im,data,item,startIndex,false,false);

}
//gets the normalized RI HOG features
void Features::getRINormHOGFeatures(const IplImage *im, CvMat *data, int item,
															int startIndex){
	getHOGFeatures(im,data,item,startIndex,true,true);
}


//Main method for computing the HOG features, according to arguements
//passed in.
void Features::getHOGFeatures(const IplImage *im, CvMat *data, int item,
															int startIndex,bool RI, bool normalize){
	
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
	//making temp storage
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
	}
	int width=64;
	int height=64;
	int cellWidth=8;
	int numCells = width/cellWidth*width/cellWidth;
	float *cellOPtr[numCells];
	float *cellMPtr[numCells];
	float *oCell;
	float *mCell;
	int idx;
	//going through each cell and making a float array of
	//the orientations and magnitudes for it
	//this is so we can compute histogram input
	for(int i=0; i< numCells;i++){
		//making storage
		mCell = new float[cellWidth*cellWidth];
		oCell = new float[cellWidth*cellWidth];
		//finding index of start of cell
		int start = i*cellWidth+(i/cellWidth)*(cellWidth-1)*width;
		idx = 0;
		//looping through the cell rows
		for(int j=0; j<cellWidth;j++){
			int rowStart = start + j*width;			
			for(int k=0; k<cellWidth; k++){
				//adding to storage
				int pos = rowStart+k;
				mCell[idx] = magnitudes[pos];
				oCell[idx++] = orientations[pos];
			}
		}
		cellOPtr[i]=oCell;
		cellMPtr[i]=mCell;
	}
	//cells are now in a double array
	
	std::vector<std::vector<float> > histInputVect;
	std::vector<std::vector<float> > histInputVectMags;
	//now need to make arrays for histogram input
	for(int q=0;q<numCells;q++){
		std::vector<float> inputs;	
		std::vector<float> mags;
		for(int j=0;j<(cellWidth*cellWidth);j++){//64 as things are 8x8

			inputs.push_back(cellOPtr[q][j]);
			mags.push_back(cellMPtr[q][j]);
		}
		histInputVect.push_back(inputs);
		histInputVectMags.push_back(inputs);
	}
	
	float * inputs;
	float* floats[numCells];
	//making histogram input;
	for(int q=0;q<numCells;q++){
		std::vector<float> v= histInputVect.at(q);
		std::vector<float> mags=histInputVectMags.at(q);
		floats[q]=makeHistogram(v,mags,normalize);
	}
	//Checking wther we are doing RI stuff
	if(RI){
		for(int q=0,idx=0;q<numCells;q++){	
		//finding highest peak in histogram
			float max=-1.0f;
			int indx =-1;
			for(int k=0;k<9;k++){
				float f = floats[q][k];
				if(f > max){
					max = f;
					indx = k;
				}
			}
			//we now know where the highest peak is, so we are making that
			//the first feature and wrapping the rest of the histogram around
			int cc=0;
			while(cc<9){
				float f = floats[q][indx++];
				cvSetReal2D( data,item,startIndex++,f);
				if(indx>8){
					indx =0;
				}
				cc++;
			}
		}
	}else{
		//if not doing RI stuff added the features
		for(int q=0,idx=0;q<numCells;q++){
			for(int k=0;k<9;k++){
				float f = floats[q][k];
				cvSetReal2D( data,item,startIndex++,f);
			}
		}
	}
		//deallocate
	for(int i=0; i< numCells;i++){
		delete floats[i];
		delete cellOPtr[i];
		delete cellMPtr[i];
	}

	//====== releasing stuff============
	cvReleaseImage(&dstx);
	cvReleaseImage(&dsty);
	cvReleaseImage(&smoothed);
 
}


float* Features::makeHistogram(std::vector<float> ovect,std::vector<float>
	mvect,bool normalize){
	float* hist;
	int numBins=9;
	hist= new float[numBins];
	for(int i=0; i<numBins; i++){
		hist[i]=0;
	}
	for(int i=0; i<ovect.size(); i++){
		float val = ovect.at(i);
		float mag = mvect.at(i); ///normConst;
		if(val >= 0 && val <40){
			hist[0]=hist[0]+val*mag;
		}else if(val>=40 && val <80){
			hist[1]=hist[1]+val*mag;
		}else if(val>=80 && val <120){
			hist[2]=hist[2]+val*mag;
		}else if(val>=120 && val <160){
			hist[3]=hist[3]+val*mag;
		}else if(val>=160 && val <200){
			hist[4]=hist[4]+val*mag;
		}else if(val>=200 && val <240){
			hist[5]=hist[5]+val*mag;
		}else if(val>=240 && val <280){
			hist[6]=hist[6]+val*mag;
		}else if(val>=280 && val <320){
			hist[7]=hist[7]+val*mag;
		}else if(val>=320 && val <360){
			hist[8]=hist[8]+val*mag;
		}else{
			std::cout<<"error, degrees range exceeded bin!!! "<<val <<std::endl;
		}
	}
	//using the L2 norm.
	if(normalize){
		float sum=0.0;
		for(int i=0; i<numBins; i++){
			sum = sum + hist[i]*hist[i];
		}
		float normC= sqrt(sum);
		for(int i=0; i<numBins; i++){
			hist[i] = normC * hist[i];
		}
	}
	return hist;
}


void Features::getHarrisCornerCount(const IplImage *im, CvMat *data, int item,
																	 int startIndex){
	int MAX_CORNERS = 40;
	CvSize src_size = cvGetSize (im);

	// create temporary images to store the temp image stuff
	IplImage *eI=cvCreateImage(src_size,IPL_DEPTH_32F, 1);
	IplImage *tI = cvCreateImage(src_size,IPL_DEPTH_32F, 1);

	//making place to store corners
	CvPoint2D32f* corners = new CvPoint2D32f[ MAX_CORNERS ];

	//setting variables
	int corner_count = MAX_CORNERS;
	double qual_level=0.1;
	double min_distance=9; //in number of pixels;

  cvGoodFeaturesToTrack(im,eI,tI,corners,&corner_count,
															 qual_level, min_distance);
	//adding feature
  cvSetReal2D( data,item,startIndex++,(float)corner_count);

	//====== releasing stuff============
	cvReleaseImage(&eI);
	cvReleaseImage(&tI);
	delete corners;
}
