#!/bin/bash
echo "Setting script to run"

CLASSIFIER="boost"
<<<<<<< .mine
BOOSTTYPE="3"
BOOSTWEAKCOUNT="150"   #"50 100 150"
BOOSTTRIMWEIGHT="0.95"
BOOSTMAXDEPTH="5"
BOOSTUSESURROGATES="0"
#FEATURETYPE="0 1 2 3 4 5 6 7 8 9 10"
FEATURETYPE="9"
=======
BOOSTTYPE="1"
BOOSTWEAKCOUNT="50 100 150"
BOOSTTRIMWEIGHT="0.95"
BOOSTMAXDEPTH="5 10 15 20"
BOOSTUSESURROGATES="0"
FEATURETYPE="9"
#FEATURETYPE="0"
>>>>>>> .r109
COUNTER=0
<<<<<<< .mine
MACHINENAME="myth4"
=======
MACHINENAME="myth23"
>>>>>>> .r109
for classifier in $CLASSIFIER
do
		for boosttype in $BOOSTTYPE
		do
				for boostweakcount in $BOOSTWEAKCOUNT
				do
						for boosttrimweight in $BOOSTTRIMWEIGHT
						do
								for boostmaxdepth in $BOOSTMAXDEPTH
								do
										for boostusesurrogates in $BOOSTUSESURROGATES
										do
												for feature in $FEATURETYPE
												do
										 				echo "python createConfig.py $classifier $boosttype $boostweakcount $boosttrimweight $boostmaxdepth $boostusesurrogates $feature"
														python createConfig.py $classifier $boosttype $boostweakcount $boosttrimweight $boostmaxdepth $boostusesurrogates $feature
														mv -f configs/basic.cfg configs/basic.$feature.$classifier.cfg

														sleep 5
														echo "nohup ./train -c $classifier.$feature.trees config/basic.$feature.$classifier.cfg >>	$MACHINENAME.$feature.txt"
														nohup ./train -c $classifier.$feature.tree configs/basic.$feature.$classifier.cfg >>	$MACHINENAME.$feature.txt
														#nohup echo "monkey" >> $MACHINENAME.$feature.txt
														let COUNTER=COUNTER+1
														rm -f configs/basic.$feature.$classifier.cfg
												done
										done
								done
						done
				done
		done
done
#
#		nohup ./digit $Type $OneKImg $OneKLabel $Test1KImg $Test1KLabel $DEPTH $Q1DIR/onek.$DEPTH.txt >> nohupSingle.out
#		echo "nohup ./digit singletree $OneKImg $OneKLabel $Test1KImg $Test1Klabel $DEPTH $Q1DIR/onek.$DEPTH.txt"
#    let DEPTH=DEPTH+2 
#done
#echo "Copying Results to $Q1DIR/one"
#mv -f results.singletree.* $Q1DIR/one
#let DEPTH=4
#while [  $DEPTH -lt $DEPTHMAX ]; do
#		nohup ./digit singletree $TenKImg $TenKLabel $Test1KImg $Test1KLabel $DEPTH $Q1DIR/tenk.$DEPTH.txt >> nohupSingle10.out
#		echo "nohup ./digit singletree $TenKImg $TenKLabel $Test1KImg $Test1KLabel $DEPTH $Q1DIR/tenk.$DEPTH.txt"
#    let DEPTH=DEPTH+2 
#done
#echo "Copying Results to $Q1DIR/ten"
#mv -f results.singletree.* $Q1DIR/ten
