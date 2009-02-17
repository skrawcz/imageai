#ifndef FEATURES_H
#define FEATURES_H

#include "cv.h"

#include <iostream>
#include <vector>
#define HAARAMOUNT 57

class Features{

public:
  Features();
  ~Features();

  enum HaarType{
    hH, hV, hD, hTL, hTR, hBL, hBR
  };

	// make very sure other is last!
  enum ImageType{
	  MUG, SCISSORS, STAPLER, CLOCK, KEYBOARD, OTHER
  };

  struct HaarFeature{
    int x,y,w,h;
    HaarType t;
  };

  struct HaarOutput{
    ImageType type;
    double haarVals[HAARAMOUNT];
  };

  void getHaarFeatures(const IplImage *im, HaarOutput *haary);


	static ImageType stringToImageType(const std::string &val){

		if(val == "other"){
			return OTHER;
		}else if(val == "mug"){
			return MUG;
		}else if(val == "scissors"){
			return SCISSORS;
		}else if(val == "stapler"){
			return STAPLER;
		}else if(val == "clock"){
			return CLOCK;
		}else if(val == "keyboard"){
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


};

#endif
