## ---------------------------------------------------------------------------
## CS221 Computer Vision Project, 2006
## Copyright (c) 2006, Stanford University
##
## Makefile
##
## LINUX INSTALLATION NOTES:
##  1. Install ffmpeg (if not already installed)
##      ./configure --prefix=<home> --enable-shared && make && make install
##  2. Install opencv (if not already installed)
##      ./configure --prefix=<home> CPPFLAGS="-I<home>/include" \
##          LDFLAGS="-L<home>/lib"&& make && make install
##  3. Make CS221 project
##      make
## ---------------------------------------------------------------------------

include Make.defaults

PROGS = test train evaluate
HDR = objects.h classifier.h replay.h utils.h CXMLParser.h features.h classer.h CfgReader.h boostTree.h randomTree.h
SRC = objects.cpp classifier.cpp replay.cpp utils.cpp CXMLParser.cpp features.cpp classer.cpp CfgReader.cpp boostTree.cpp randomTree.cpp

#MORE_LIBS=-ltiff -lpthread -lpng -lm
#GTK_LIBS=$(shell pkg-config gtk+ --libs)
#GT_LIBS=$(shell pkg-config gthread-2.0 --libs)
CV_LIBS=-lcxcore -lcv -lhighgui -lcvaux -lml
LIBS=$(MORE_LIBS) $(GT_LIBS) $(GTK_LIBS) $(CV_LIBS)



DEFS = -O2
CC = g++
#OPENCV_LIB_DIR = /afs/ir.stanford.edu/class/cs221/lib64/

CCFLAGS = -Wall $(DEFS) -I$(OPENCV_INC_DIR)
LDFLAGS = -L$(OPENCV_LIB_DIR)

SUFFIX = $(firstword $(suffix $(SRC)))
OBJ = $(SRC:$(SUFFIX)=.o)

# rule for making everything
all: $(OBJ) $(PROGS)

# specify how to build binaries
test: test.o $(OBJ)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(OBJ) test.o -o $@ $(LIBS) 

train: train.o $(OBJ)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(OBJ) train.o -o $@ $(LIBS) 

evaluate: evaluate.o $(OBJ)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(OBJ) evaluate.o -o $@ $(LIBS) 

# define compilation rule for .o files
%.o : %.cpp
	$(CC) $(CCFLAGS) -c $< -o $@

# clean
.PHONY : clean
clean:
	rm -f *.o $(PROGS) core

