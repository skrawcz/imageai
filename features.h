#ifndef FEATURES_H
#define FEATURES_H

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "utils.h"
#include "objects.h"
#include <iostream>

#define HAARAMOUNT 57

class Features{

public:
  Features();
  ~Features();

  enum HaarType{
    hH, hV, hD, hTL, hTR, hBL, hBR
  };

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

private:
  std::vector<HaarFeature> haars; 

  HaarFeature strToHaar(const std::string &in);
  void readHaars();
  //void applyHaar(const IplImage *im, HaarOutput *haary);


};

#endif
