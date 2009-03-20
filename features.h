#ifndef FEATURES_H
#define FEATURES_H

#include "cv.h"

#include <iostream>
#include <vector>

#define TYPECOUNT 5

//this class creates all our features
//it creates them depending on the feature type 
// selected in the config file.

class Features{

public:
  Features();
  ~Features();

	// add more types ! :) 
	enum FeatureType{
		HAAR, HAARTILT, HAAR_HAARTILT,HOGN, HOGUN, HOGRI, HAAR_HOG, HCORNER, 
		HAAR_HCORNER, HOG_HCORNER, HAAR_HOG_HCORNER, ERROR
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

	
	CvMat *getFeaturesSubset(CvMat *inData, Features::FeatureType subFeat);
	void getFeatures(const IplImage *im, const IplImage *imTilt, CvMat *data, int item, const	IplImage *realImg);
	
	int  amountOfFeatures(FeatureType feat = ERROR, bool *numFeatsOutPad = false);
	bool amountOfFeaturesRounded();

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
	int numOfFeatures;
	bool needToPadFeature;

	void getHaarFeatures(const IplImage *im, CvMat *data, int item, int
	startIndex);
	void getHOGFeatures(const IplImage *im, CvMat *data, int item, int
											startIndex, bool RI, bool normalize);

	void getNormHOGFeatures(const IplImage *im, CvMat *data, int item, int
													startIndex);
	void getUnNormHOGFeatures(const IplImage *im, CvMat *data, int item, int
													startIndex);

	void getRINormHOGFeatures(const IplImage *im, CvMat *data, int item, int
														startIndex);
	float* makeHistogram(std::vector<float> vect,std::vector<float>
														mags,bool normalize);
	
	void getHarrisCornerCount(const IplImage *im, CvMat *data, int item, int
	startIndex);

};

#endif
