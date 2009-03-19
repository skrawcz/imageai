# file written by Stefan Krawczyk
# randomly pics a number of lines from a language model
import sys

if len(sys.argv) < 7:
    print("Error not enough arguments:\n")
    sys.exit([-1])

#for arg in sys.argv:
#    print arg


txtString = "\
[ A config file. Note I have been lazy so lines have to be empty, contain a propper statement a=b; (no spaces) or a comment [ ... ] ]\n\
\n\
\n\
[ amount of frames to skip] \n\
frameJump=5; \n\
\n\
[the threshold upon which images of objects are considered] \n\
isObjectThreshold=0.0; \n\
\n\
[ the allowed overlap between two objects ] \n\
\n\
boxOverlapThreshold=0.4; \n\
\n\
\n\
\n\
[type of classification] \n\
classifierType=%s; \n\
\n\
[if one half of training set is used to look at quality of results] \n\
createGraph=1; [is a boolean, use 1 for true, 0 for false] \n\
graphNames=boostResults; \n\
graphTrainTestRatio=0.7; [percentage of examples used for training if createGraph = true ]\n\
[the subset defines the proportion of images to use. 1= all images 10= one tenth etc ]\n\
useSubset=1; \n\
 \n\
[boost options]\n\
boostType=%s;   [CvBoost::DISCRETE = 0 CvBoost::REAL = 1  CvBoost::LOGIT = 2 CvBoost::GENTLE = 3 ] \n\
boostWeakCount=%s; \n\
boostWeightTrimRrate=%s; [from 0 to 1, inclusive] \n\
boostMaxDepth=%s; \n\
boostUseSurrogates=%s;  [is a boolean, use 1 for true, 0 for false] \n\
\n\
[this is where you set the feature type] \n\
[HAAR= 0, HAARTILT = 1, HAAR_HAARTILT = 2,HOGN = 3, HOGUN = 4, HOGRI = 5] \n\
[HAAR_HOG = 6, HCORNER = 7,	HAAR_HCORNER = 8, HOG_HCORNER = 8] \n\
[HAAR_HOG_HCORNER = 9] \n\
featureType=%s \n" % \
(sys.argv[1],sys.argv[2],sys.argv[3],sys.argv[4],sys.argv[5],sys.argv[6],sys.argv[7])

fname = "configs/basic.cfg"
f = open(fname,'w')

#print("writing to file %s \n" %fname)
f.write(txtString)
f.close()
