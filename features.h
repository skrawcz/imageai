#ifndef FEATURES_H
#define FEATURES_H

#include "cv.h"

#include <iostream>
#include <vector>

//#define HAARAMOUNT 57
//#define HOGAMOUNT 64
#define TYPECOUNT 5

class Features{

public:
  Features();
  ~Features();

	// add more types ! :) 
	enum FeatureType{
		HAAR, HOG, HAARHOG, HCORNER,ALL
	};

	// make very sure other is last!
  enum ImageType{
	  MUG, SCISSORS, STAPLER, CLOCK, KEYBOARD, OTHER
  };

  enum HaarType{
    hH, hV, hD, hTL, hTR, hBL, hBR
  };

  struct HaarFeature{
    int x,y,w,h;
    HaarType t;
  };

	

	void getFeatures(const IplImage *im, CvMat *data, int item, const	IplImage *realImg);
	
	int amountOfFeatures();
	bool amountOfFeaturesRounded();
	/*
	static int amountOfFeatures(){

		// Make uneven number as the boost functions dont work otherwise
		//		return HAARAMOUNT + (HAARAMOUNT)%2;
		switch(featureType){
		case HAAR:
			return HAARAMOUNT + HOGAMOUNT +(HAARAMOUNT)%2;
			break;
			
		case HOG:
			return HOGAMOUNT;
			break;
			
		case HAARHOG:
			return HAARAMOUNT + HOGAMOUNT +(HAARAMOUNT)%2;
			break;
		}
		return HAARAMOUNT + HOGAMOUNT +(HAARAMOUNT)%2;

	}

	static bool amountOfFeaturesRounded(){
		//		return (HAARAMOUNT)%2;
	switch(featureType){
		case HAAR:
			return (HAARAMOUNT)%2;
			break;
			
		case HOG:
			return (HOGAMOUNT)%2;
			break;
			
		case HAARHOG:
			return (HAARAMOUNT + HOGAMOUNT)%2;
			break;
		}
	return false;
	}*/


	static ImageType stringToImageType(const std::string &val){

		if(val.find("other") != std::string::npos){
			return OTHER;
		}else if(val.find("mug") != std::string::npos){
			return MUG;
		}else if(val.find("scissors") != std::string::npos){
			return SCISSORS;
		}else if(val.find("stapler") != std::string::npos){
			return STAPLER;
		}else if(val.find("clock") != std::string::npos){
			return CLOCK;
		}else if(val.find("keyboard") != std::string::npos){
			return KEYBOARD;
		}

		return OTHER;

	}

	static std::string imageTypeToString(ImageType val){

		std::string out = "other";

		if(val == MUG){
			out = "mug";
		}else if(val == SCISSORS){
			out = "scissors";
		}else if(val == STAPLER){
			out = "stapler";
		}else if(val == CLOCK){
			out = "clock";
		}else if(val == KEYBOARD){
			out = "keyboard";
		}

		return out;

	}

private:
  std::vector<HaarFeature> haars; 

  HaarFeature strToHaar(const std::string &in);
  void readHaars();
  //void applyHaar(const IplImage *im, HaarOutput *haary);

	FeatureType featureType;


	void getHaarFeatures(const IplImage *im, CvMat *data, int item, int startIndex);
	void getHOGFeatures(const IplImage *im, CvMat *data, int item, int startIndex);
	float* makeHistogram(std::vector<float> vect,std::vector<float>	mags,float normConst);

	void getHarrisCornerCount(const IplImage *im, CvMat *data, int item, int startIndex);
};

#endif
