#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include "features.h"
#include <math.h>

Features::Features(){
  readHaars();
}

Features::~Features(){
  
}

// creates set of haar values for an image.
void Features::getHaarFeatures(const IplImage *im, HaarOutput *haary){

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
