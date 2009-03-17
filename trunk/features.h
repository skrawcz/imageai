#ifndef FEATURES_H
#define FEATURES_H

#include "cv.h"

#include <iostream>
#include <vector>

#define HAARAMOUNT 57
#define TYPECOUNT 6

class Features{

public:
  Features();
  ~Features();

	// add more types ! :) 
	enum FeatureType{
		HAAR
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

	void getHOGFeatures(const IplImage *im, CvMat* data, int item);

	void getFeatures(const IplImage *im, CvMat *data, int item);

	static int amountOfFeatures(){

		// Make uneven number as the boost functions dont work otherwise
		return HAARAMOUNT + (HAARAMOUNT + 1)%2;

	}

	static bool amountOfFeaturesRounded(){
		return (HAARAMOUNT + 1)%2;
	}


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


	void getHaarFeatures(const IplImage *im, CvMat *data, int item);


};

#endif
