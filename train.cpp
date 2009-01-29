/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    train.cpp
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
** DESCRIPTION:
**  This file contains the main executable code for training classifiers. It
**  simply loads a list of image filenames (and classes) and passes them to
**  the classifier train() method. Given the directory structure:
**      data/mug/<filestem>.jpg
**      data/stapler/<filestem>.jpg
**      data/other/<filestem>.jpg
**
**  executing with
**      % ./train -c parameters.txt -v data
**
**  will load training files for classes "mug", "stapler", and "other", and
**  save parameters in file "parameters.txt".
**
** CS221 TO DO:
**  You are free to modify anything in this file.
** 
*****************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>

#include <sys/types.h>
#include <dirent.h>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "utils.h"
#include "objects.h"
#include "classifier.h"

using namespace std;

/* Main *********************************************************************/

void usage() {
    cerr << "./train [<options>] <directory>" << endl << endl;
    cerr << "OPTIONS:" << endl;
    cerr << "    -c <filename>  :: configuration file for saving state" << endl;
    cerr << "    -h             :: show this message" << endl;
    cerr << "    -v             :: verbose" << endl;
    cerr << endl;
}

int main(int argc, char *argv[])
{
  char *configurationFile;
  bool bVerbose;
  char **args;
    
  CClassifier classifier;

  // set defaults
  configurationFile = NULL;   // set using -c <filename>
  bVerbose = false;           // turn on with -v

  // check arguments
  args = argv + 1;
	while (argc-- > 2) {
		if (!strcmp(*args, "-c")) {
			argc--; args++;
			if (configurationFile != NULL) {
				usage();
				return -1;
			}
			configurationFile = *args;

		} else if (!strcmp(*args, "-h")) {
			usage();
			return 0;

		} else if (!strcmp(*args, "-v")) {
			bVerbose = !bVerbose;

		} else {
			cerr << "ERROR: unrecognized option " << *args << endl;
			return -1;
		}

		args++;
	}

  if (argc != 1) {

		*args = NULL;
		//usage();
		//exit(-1);
  }

  // load the training file list
  TTrainingFileList fileList;
  fileList = getTrainingFiles(*args, ".jpg");

  // now train the classifier
  if (!classifier.train(fileList)) {
		cerr << "ERROR: could not train classifier" << endl;
		exit(-1);
  }

  // save classifier configuration
	if (configurationFile != NULL) {
		if (!classifier.saveState(configurationFile)) {
			cerr << "ERROR: could not save classifier configuration" << endl;
			exit(-1);
		}
  }

	return 0;
}
