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

#include "objects.h"

#include <algorithm>
#include "CfgReader.h"

using namespace std;

// CObject class -------------------------------------------------------------

// default constructor
CObject::CObject()
{
    // do nothing
		score = 0;
		killed = false;
}

// copy constructor
CObject::CObject(const CObject& obj)
{
    rect = obj.rect;
    label = obj.label;
		score = obj.score;
		killed = obj.killed;
}

// full constructor
CObject::CObject(const CvRect& r, const string &s)
{
    rect = r;
    label = s;

		// lest something breaks
		score = 0;
		killed = false;
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

	double overlps = overlap(obj);
	double size = rect.width * rect.height + obj.rect.width * obj.rect.height;

	return overlps / (size - overlps);

}

// assignment operator
CObject& CObject::operator=(const CObject& obj)
{
    rect = obj.rect;
    label = obj.label;
		score = obj.score;
		killed = obj.killed;
    
    return *this;
}

void CObject::copyOverwrite(const std::vector<CObject>& src, std::vector<CObject>& dest){


	dest.clear();

	for(int i=0;i<src.size();++i){

		if(!src[i].killed)
			dest.push_back(src[i]);

	}
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
				if(!src[j].killed && src[i].percentOverlap(src[j]) < overlapThreshold){
					src[j].killed = true;
				}
			}
		}
	}

	vector<CObject> tmp;

	copyOverwrite(src, tmp);
	copyOverwrite(tmp, src);


}














