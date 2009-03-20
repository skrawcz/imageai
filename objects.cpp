/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    objects.cpp
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
** DESCRIPTION:
**  See objects.h
**
*****************************************************************************/

#include <cassert>
#include <string>
#include "objects.h"

#include <algorithm>
#include "CfgReader.h"
#include <math.h>

using namespace std;

// CObject class -------------------------------------------------------------

// default constructor
CObject::CObject()
{
    // do nothing
		score = 0;
		killed = false;
		useful = false;
		type = 5;
}

// copy constructor
CObject::CObject(const CObject& obj)
{
    rect = obj.rect;
    label = obj.label;
		score = obj.score;
		killed = obj.killed;
		useful = obj.useful;
		type = obj.type;
}

// full constructor
CObject::CObject(const CvRect& r, const string &s)
{
    rect = r;
    label = s;

		// lest something breaks
		score = 0;
		killed = false;
		useful = false;
		type = 5;
}

// destructor
CObject::~CObject()
{
    // do nothing
}

// writeAsXML
// Writes the object out as XML formatted text.
void CObject::writeAsXML(ostream& os)
{
    os  << "<object x=\"" << rect.x << "\""
        << " y=\"" << rect.y << "\""
        << " w=\"" << rect.width << "\""
        << " h=\"" << rect.height << "\""
        << " label=\"" << label.c_str() << "\""
        << " />" << endl;
}

// draw
// Draws the object with label on an IplImage
void CObject::draw(IplImage *image, CvScalar color, CvFont *font)
{
    assert(image != NULL);
    
    // draw the bounding box
    cvRectangle(image, cvPoint(rect.x, rect.y),
        cvPoint(rect.x + rect.width, rect.y + rect.height),
        color, 2);
    
    // draw the label text
    if (font != NULL) {
        cvPutText(image, label.c_str(), cvPoint(rect.x + 3, rect.y - 3),
            font, color);
    }
}

// intersect
// Computes the intersection between any two objects.
CvRect CObject::intersect(const CObject& obj) const
{
    CvRect intersection;
    
    // find overlapping region
    intersection.x = (rect.x < obj.rect.x) ? obj.rect.x : rect.x;
    intersection.y = (rect.y < obj.rect.y) ? obj.rect.y : rect.y;
    intersection.width = (rect.x + rect.width < obj.rect.x + obj.rect.width) ?
        rect.x + rect.width : obj.rect.x + obj.rect.width;
    intersection.width -= intersection.x;
    intersection.height = (rect.y + rect.height < obj.rect.y + obj.rect.height) ?
        rect.y + rect.height : obj.rect.y + obj.rect.height;
    intersection.height -= intersection.y;    
    
    // check for non-overlapping regions
    if ((intersection.width <= 0) || (intersection.height <= 0)) {
        intersection = cvRect(0, 0, 0, 0);
    }
    
    return intersection;
}

// overlap
// Computes the number of pixels that overlap between two objects
// of the same class.
int CObject::overlap(const CObject& obj) const
{
    CvRect overlappingRegion;
    
    // if the labels aren't the same then, by definition, there
    // can be no overlap
    if (obj.label != label) {
        return 0;
    }
    
    overlappingRegion = this->intersect(obj);

    // compute size of overlapping region
    return (overlappingRegion.width * overlappingRegion.height);
}

// percentage overlap
double CObject::percentOverlap(const CObject& obj) const {

 	CvRect overlappingRegion = this->intersect(obj);

	double minArea = std::min( rect.width * rect.height, obj.rect.width * obj.rect.height);

	return (double)(overlappingRegion.width * overlappingRegion.height) / minArea;

}

// assignment operator
CObject& CObject::operator=(const CObject& obj)
{
    rect = obj.rect;
    label = obj.label;
		score = obj.score;
		killed = obj.killed;
		useful = obj.useful;
		type = obj.type;
    
    return *this;
}

void CObject::copyOverwrite(const std::vector<CObject>& src, std::vector<CObject>& dest, int limit){

	int counter = 0;
	dest.clear();

	for(int i=0;i<src.size();++i){

		if(!src[i].killed){
			dest.push_back(src[i]);
			++counter;
		}
		if(counter == limit)
			break;

	}
}

void CObject::boostScores(std::vector<CObject>& newVec, std::vector<CObject>& oldVec, float dx, float dy){

	vector<CObject> tmp;

	// limit size
	copyOverwrite(newVec, tmp, 40);


	CObject egon;
	std::sort(tmp.begin(), tmp.end(), egon);


	//for(unsigned i=0;i<tmp.size();++i)
	//	std::cout << tmp[i].score << " ";

	//std::cout << std::endl;

	// do same type boosting so that two boxes of same type get a boost.
	for(unsigned i=0;i<tmp.size();++i){

		float tmpSize = tmp[i].rect.width*tmp[i].rect.height;

		//tmp[i].score *= log(tmpSize);

		for(unsigned j=i;j<tmp.size();++j){
			
			
			// if close in size
			if(tmp[i].type == tmp[j].type && fabs(tmpSize - tmp[j].rect.width*tmp[j].rect.height) < 0.1*tmpSize){
				
				// if high level of overlap
				if(tmp[i].percentOverlap(tmp[j]) > .9){
					//std::cout << "simple boost" << std::endl;
					tmp[i].score *= 1.1;
			
				}
			}
		}
	}



	CvRect newRect, oldRect;

	// boost on similarity with previous frame
	for(unsigned i=0;i<tmp.size();++i){

		newRect = tmp[i].rect;

		for(unsigned j=0;j<oldVec.size();++j){

			
			oldRect = oldVec[j].rect;

			float tmpSize = newRect.width*newRect.height;
			
			// if close in size
			if(tmp[i].type == oldVec[j].type && fabs(tmpSize - oldRect.width * oldRect.height) < 0.1*tmpSize){
				
				// if high level of overlap
				if(tmp[i].percentOverlap(oldVec[j]) > .9){

					if((newRect.x - oldRect.x > 0) == (dx > 0)){
						if((newRect.y - oldRect.y > 0) == (dy > 0)){
							//std::cout << "uber boost" << std::endl;
							tmp[i].score = (tmp[i].score + oldVec[j].score)*.5*1.3;
						}
					}			
				}
			}
		}
	}



	//for(unsigned i=0;i<tmp.size();++i)
	//	std::cout << tmp[i].score << " ";

	//std::cout << std::endl;

	copyOverwrite(tmp, newVec);

}


void CObject::filterOverlap(std::vector<CObject>& src){

	CObject egon;
	std::sort(src.begin(), src.end(), egon);

	std::vector<CObject> out;

	double overlapThreshold = CfgReader::getDouble("boxOverlapThreshold");
	//std::cout << overlapThreshold << std::endl;


	for(int i=0;i<src.size();++i){

		if(!src[i].killed){
			for(int j=i+1;j<src.size();++j){
				//std::cout << src[i].percentOverlap(src[j]) << std::endl;
				if(!src[j].killed && src[i].percentOverlap(src[j]) > overlapThreshold){
					src[j].killed = true;
				}
			}
		}
	}

	vector<CObject> tmp;

	copyOverwrite(src, tmp);
	copyOverwrite(tmp, src, 3);


}



void CObject::stefansOverlap(std::vector<CObject>& src,int num){
	CObject egon;
	std::vector<CObject> mug;
	std::vector<CObject> stapler;
	std::vector<CObject> scissors;
	std::vector<CObject> keyboard;
	std::vector<CObject> clock;

	string objects[]={"mug","scissors","stapler","clock","keyboard"};
	//making vectors of just the one type
	for(unsigned i=0;i<src.size();++i){
		if(objects[0].compare(src[i].label)== 0 ){
			mug.push_back(src[i]);
		}else if(objects[1].compare(src[i].label)== 0 ){
			scissors.push_back(src[i]);
		} else if(objects[2].compare(src[i].label)== 0 ){
			stapler.push_back(src[i]);
		}else if(objects[3].compare(src[i].label)== 0 ){
			clock.push_back(src[i]);
		}else if(objects[4].compare(src[i].label)== 0 ){
			keyboard.push_back(src[i]);
		}else{
			std::cout<<"error couldnt find type of object"<<std::endl;
		}
	}

	//	std::cout<<"staring sort "<<mug.size()<<std::endl;
	//sorting them based on ?
	std::sort(mug.begin(), mug.end(), egon);
	std::sort(stapler.begin(), stapler.end(), egon);
	std::sort(keyboard.begin(), keyboard.end(), egon);
	std::sort(clock.begin(), clock.end(), egon);
	std::sort(scissors.begin(), scissors.end(), egon);
	//std::cout<<"finished sort"<<std::endl;
	src.clear();
	//	std::cout<<"cleared out old vect"<<std::endl;
	for(unsigned i=0; i<num && i < mug.size();++i){
			src.push_back(mug[i]);
	}
				//	std::cout<<"done mug"<<std::endl;
	for(unsigned i=0; i<num && i < stapler.size() ;++i){
			src.push_back(stapler[i]);
	}
				//	std::cout<<"done stapler"<<std::endl;
	for(unsigned i=0; i<num && i < scissors.size();++i){
			src.push_back(scissors[i]);
	}
				//	std::cout<<"done scissors"<<std::endl;
	for(unsigned i=0; i<num && i<  keyboard.size();++i){
			src.push_back(keyboard[i]);
	}
				//	std::cout<<"done keyboard"<<std::endl;
				//	num=100;
	for(unsigned i=0; i<num && i < clock.size();++i){
			src.push_back(clock[i]);
	}
				//std::cout<<"done clock"<<std::endl;
				//	std::cout<<"returning stuff"<<std::endl;
}












