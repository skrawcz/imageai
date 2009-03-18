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
#define PI 3.14159265

#define HAARAMOUNT 57//*2
#define HOGAMOUNT 64*9 //64*9
#define HCORNERAMOUNT 1
//#define TYPECOUNT 5

Features::Features(){

	//we should probably make this some value we pass in
	featureType = HAAR;
	//featureType = HOG;
	//featureType = HAARHOG;
	//featureType = HCORNER;
	//featureType = ALL;

	//featureType = HAACORNER;
	//featureType = HOGCORNER;

	
  readHaars();
}

Features::~Features(){
	//should probably deallocate the features...
}

bool Features::amountOfFeaturesRounded(){
	//		return (HAARAMOUNT)%2;
	switch(featureType){
	case HAAR:
		return (HAARAMOUNT)%2;
		//break;
			
	case HOG:
		return (HOGAMOUNT)%2;
		//break;
			
	case HAARHOG:
		return (HAARAMOUNT + HOGAMOUNT)%2;
		//	break;
			
	case HCORNER:
		return (HCORNERAMOUNT)%2;
		
	case HOGCORNER:
		return (HOGAMOUNT + HCORNERAMOUNT)%2;
		
	case HAARCORNER:
		return (HAARAMOUNT + HCORNERAMOUNT)%2;

	case ALL:
		return (HAARAMOUNT + HOGAMOUNT+ HCORNERAMOUNT)%2;
	}
	return false;
}

int Features::amountOfFeatures(){

	// Make uneven number as the boost functions dont work otherwise
	//		return HAARAMOUNT + (HAARAMOUNT)%2;
	switch(featureType){
	case HAAR:
		return HAARAMOUNT +(HAARAMOUNT)%2;
		break;
			
	case HOG:
		return HOGAMOUNT + (HOGAMOUNT)%2;;
		break;
			
	case HAARHOG:
		return HAARAMOUNT + HOGAMOUNT +(HAARAMOUNT+HOGAMOUNT)%2;
		break;

	case HCORNER:
		return HCORNERAMOUNT + (HCORNERAMOUNT)%2;
		
	case HOGCORNER:
		return HOGAMOUNT + HCORNERAMOUNT+  (HOGAMOUNT + HCORNERAMOUNT)%2;
		
	case HAARCORNER:
		return HAARAMOUNT + HCORNERAMOUNT+ (HAARAMOUNT + HCORNERAMOUNT)%2;

	case ALL:
		return HAARAMOUNT + HOGAMOUNT+ HCORNERAMOUNT+ (HAARAMOUNT + HOGAMOUNT+ HCORNERAMOUNT)%2;
	default:
		std::cout<<"there was an error in the amount of features"
						 <<" there is no such feature type";
		return 0;
		break;
	}


}


void Features::getFeatures(const IplImage *im, const IplImage *imTilt, CvMat *data, int item, const IplImage *realImg){

	float* ptrToFeatures;
	if(featureType == HAAR){
		getHaarFeatures(im, data, item,0);
		//getHaarFeatures(imTilt, data, item,HAARAMOUNT/2);
	}else if(featureType == HOG){
		//getting the float array out and putting it into the matrix
		//ptrToFeatures=getHOGFeatures(realImg);
		//for(int i=0;i<HOGAMOUNT;i++){
		//		cvSetReal2D( data,item,i,ptrToFeatures[i]);
		//}
		getHOGFeatures(realImg,data,item,0);
	}else if(featureType == HAARHOG){
		//TODO: make the two feature vectors concatenate onto each other
		getHaarFeatures(im, data, item,0);
		getHaarFeatures(imTilt, data, item,HAARAMOUNT/2);
		//ptrToFeatures=getHOGFeatures(realImg);
		//for(int i=0;i<HAARAMOUNT+HOGAMOUNT;i++){
		//	cvSetReal2D( data,item,i,ptrToFeatures[i]);
		//}
		getHOGFeatures(realImg,data,item,HAARAMOUNT);
	}else if(featureType == HCORNER){
		getHarrisCornerCount(realImg,data,item,0);

	}else if(featureType == HAARCORNER){
		getHarrisCornerCount(realImg,data,item,0);
		getHaarFeatures(im, data, item,HCORNERAMOUNT);
		getHaarFeatures(imTilt, data, item,HCORNERAMOUNT + HAARAMOUNT/2);
	}else if(featureType == HOGCORNER){
		getHarrisCornerCount(realImg,data,item,0);
		getHOGFeatures(realImg,data,item,HCORNERAMOUNT);
	}else if(featureType == ALL){
		getHaarFeatures(im, data, item,0);
		getHaarFeatures(imTilt, data, item,HAARAMOUNT/2);
		getHOGFeatures(realImg,data,item,HAARAMOUNT);
		getHarrisCornerCount(realImg,data,item,HAARAMOUNT+HOGAMOUNT);
	}else{

		std::cout<<"Woops, something screwed up"<<std::endl;
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

void Features::getHOGFeatures(const IplImage *im, CvMat *data, int item,
															int startIndex){
	//float* Features::getHOGFeatures(const IplImage *im){




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
	

	
	float orientations[dstx->height * dstx->width];
	float magnitudes[dstx->height * dstx->width];
	int counter=0;
	//calculating the gradient magnitudes here
	for(int i=0; i< (dstx->height * dsty->width);i++){
		double ex = dxarray[i];
		double ey = dyarray[i];
		double mag = sqrt(pow(ex,2) + pow(ey,2));
		double o = atan2(ey,ex); //in radians -Pi to Pi
		double alpha = o*180/PI;
		double alpha_signed = alpha;
		//std::cout<<"x,y = "<<ex<<","<<ey<<" o=" << o<<std::endl;
		if(alpha < 0.0){
			alpha_signed = alpha+360.0;
		}
		magnitudes[i]=(float)mag;
		orientations[i]=(float)alpha_signed;
		//now need to bin it for each cell
		//std::cout<<"alpha = "<<alpha_signed<<std::endl;
		counter++;
	}
	//std::cout<<"counter = "<<counter<<std::endl;
	//std::cout<<"x,y = "<<dstx->height<<","<<dstx->width<<std::endl;
	//need to setup data arrays to store data to make histograms from
	//to capture strong edges, need to weight them by the magnitude

	int width=64;
	int height=64;
	int cellWidth=8;
	float normConst=0.0f;

	int numCells = width/cellWidth*width/cellWidth;
	float *cellOPtr[numCells];
	float *cellMPtr[numCells];
	//not doing overlaping anymore, just doing simple method
	//this points to the 15, as they are 8x8 and we
	//move them over ever 4  cells

	
	float *oCell;
	float *mCell;
	
	int idx;
	//going through each cell and making a float array of
	//the orientations and magnitudes for it
	//this is so we can compute histogram input
	for(int i=0; i< numCells;i++){
	
		int start = i*cellWidth+(i/cellWidth)*(cellWidth-1)*width;
			
		mCell = new float[cellWidth*cellWidth];
		oCell = new float[cellWidth*cellWidth];

		idx = 0;
		//std::cout<<"i = "<<i<<" start = "<<start<<std::endl;
		
		//		for(int j=start +width;j<start*width + cellWidth;++j){
		for(int j=0; j<cellWidth;j++){
			int rowStart = start + j*width;			
			//std::cout<<"rowStart= "<<rowStart<<std::endl;
			//std::cout<<"j = "<<j<<"< "<<start*width+cellWidth<<std::endl;
			for(int k=0; k<cellWidth; k++){
				int pos = rowStart+k;
				//				std::cout<<"pos ="<<pos<<std::endl;
				/*if(pos > 4095){
					std::cout<<"Crap - "<<pos<<std::endl;
				}else if(pos==4095){
					std::cout<<"hooray"<<std::endl;
					}*/
				mCell[idx] = magnitudes[pos];
				oCell[idx++] = orientations[pos];
				normConst = normConst+magnitudes[pos];

			}
			/*for(int k=start + j*height;k < start + j*height + cellWidth;++k){
				std::cout<<"k = "<<k<<std::endl;
				mCell[idx] = magnitudes[k];
				//std::cout<<"mag = "<<magnitudes[k];
				oCell[idx++] = orientations[k];
				//std::cout<<"orien = "<<orientations[k];
				//std::cout<<" "<<orientations[k];
				normConst = normConst+magnitudes[k];
				}*/
		}
		//		std::cout<<std::endl;

		cellOPtr[i]=oCell;
		cellMPtr[i]=mCell;
	}
	
	//was thinking of normalizing it somehow, but meh
	if (normConst==0){
		normConst=1;
	}else{
		normConst =1 ;///normConst;
	}
	//cells are now in a double array
	
	//std::cout<<"normConst = "<< 1/normConst*10000<<std::endl;
	std::vector<std::vector<float> > histInputVect;
	std::vector<std::vector<float> > histInputVectMags;
	//now need to make arrays for histogram input
	for(int q=0;q<numCells;q++){
		std::vector<float> inputs;	
		std::vector<float> mags;
		for(int j=0;j<64;j++){//64 as things are 8x8
			//std::cout<<"mag = "<<cellMPtr[q][j]<<" ";
			inputs.push_back(cellOPtr[q][j]);
			mags.push_back(cellMPtr[q][j]);
		}
		histInputVect.push_back(inputs);
		histInputVectMags.push_back(inputs);
		//	std::cout<<" size pushed back= "<<inputs.size();
	}
	
	float * inputs;
	float* floats[numCells];
	//making histogram input;
	for(int q=0;q<numCells;q++){
		std::vector<float> v= histInputVect.at(q);
		std::vector<float> mags=histInputVectMags.at(q);
		floats[q]=makeHistogram(v,mags,normConst);
	}

	//float* featureArray;
	//	featureArray = new float[numCells * 9];
	for(int q=0,idx=0;q<numCells;q++){
		for(int k=0;k<9;k++){
			float f = floats[q][k];
			
			//			featureArray[idx++]=f;//
			//std::cout<<"sIndex="<<startIndex<<std::endl;
			cvSetReal2D( data,item,startIndex++,f);
		}
	}
		//deallocate
	for(int i=0; i< numCells;i++){
		delete floats[i];
		delete cellOPtr[i];
		delete cellMPtr[i];
	}
	//std::cout<<"One image = ";
	for(int q=0;q<numCells;q++){
		//			std::cout<<"one hist =";
		for(int j=0;j<9;j++){
			//					std::cout<<" "<<floats[q][j];
		}
		//			std::cout<<std::endl;
	}
	
 
	//==== display debugging stuff here=======
  //making display windows
	/*cvNamedWindow("HOG IN image");
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
	*/
	//====== releasing stuff============
	cvReleaseImage(&dstx);
	cvReleaseImage(&dsty);
	cvReleaseImage(&smoothed);

	//killing display windows
	/*cvDestroyWindow("HOG IN image");
	cvDestroyWindow("Smoothed IN image");
	cvDestroyWindow("dx image");
	cvDestroyWindow("dy image");*/
	//	return featureArray;

}


float* Features::makeHistogram(std::vector<float> ovect,std::vector<float>
	mvect,float normConst){
	float* hist;
	int numBins=9;
	hist= new float[numBins];
	for(int i=0; i<numBins; i++){
		hist[i]=0;
	}
	//	std::cout<<"val = ";
	for(int i=0; i<ovect.size(); i++){
		float val = ovect.at(i);
		//	std::cout<<val<<" ";
		float mag = mvect.at(i)/normConst;
		if(val >= 0 && val <40){
			hist[0]=hist[0]+val*mag;
		}else if(val>=40 && val <80){
			//std::cout<<"other"<<std::endl;
			hist[1]=hist[1]+val*mag;
		}else if(val>=80 && val <120){
			//std::cout<<"other"<<std::endl;
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
	//	std::cout<<std::endl;
	return hist;
}


void Features::getHarrisCornerCount(const IplImage *im, CvMat *data, int item,
																	 int startIndex){
	int MAX_CORNERS = 40;
	//IplImage* gray= cvCreateImage(cvSize(im->width,im->height), IPL_DEPTH_8U, 1);
  //cvCvtColor(im, gray, CV_BGR2GRAY);

	//gray = cvCloneImage(im);
	CvSize src_size = cvGetSize (im);

	// create temporary images to store the two image derivatives
	//CvMat *eI = cvCreateMat (src_size.height, src_size.width, CV_32SC1);
	IplImage *eI=cvCreateImage(src_size,IPL_DEPTH_32F, 1);
	//CvMat *tI = cvCreateMat (src_size.height, src_size.width, CV_32SC1);
	IplImage *tI = cvCreateImage(src_size,IPL_DEPTH_32F, 1);

	//	cvSobel (gray, dx, 1, 0, APETURE_SIZE);
	//cvSobel (gray, dy, 0, 1, APETURE_SIZE);


	// Setup the buffers
	//harris_responce = cvCloneImage (image);

	// This array will store all corners found in the image
	//CvPoint2D32f *corners = (CvPoint2D32f*)cvAlloc (MAX_CORNERS * sizeof
	//(corners));
	CvPoint2D32f* corners = new CvPoint2D32f[ MAX_CORNERS ];

	int corner_count = MAX_CORNERS;
	double qual_level=0.1;
	double min_distance=9; //in number of pixels;

	//getting corners
  cvGoodFeaturesToTrack(im,eI,tI,corners,&corner_count,
															 qual_level, min_distance);
  
	
	cvSetReal2D( data,item,startIndex++,(float)corner_count);






//==== display debugging stuff here=======


	CvFont font;
	double hScale=1.0;
	double vScale=1.0;
	int    lineWidth=1;
	//cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC,
	//	hScale,vScale,0,lineWidth);
	//std::cout<<"num corners = "<<corner_count<<std::endl;
	for(int i=0;i < corner_count ; i++){
		//cvPutText (im,"C",corners[i], &font, cvScalar(255,255,0));
		//std::cout<<corners[i].x<<","<<corners[i].y<<std::endl;
	}


  //making display windows
	//	cvNamedWindow("HCorner IN image");
	//showing images
	//cvShowImage("HCorner IN image",im);
	//showing x and y gradient images
	
	//cvWaitKey( 0 );
	
	//====== releasing stuff============
	cvReleaseImage(&eI);
	cvReleaseImage(&tI);
	//killing display windows
	//cvDestroyWindow("HCorner IN image");
	delete corners;
	//	return featureArray;
}
