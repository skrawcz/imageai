Eric Liaw liaw@stanford.edu 005536922
Stefan Krawczyk stefank@stanford.edu
Filip Gruvsatd Gruvstad@stanford.edu 005570639

to train:
./train -c <treefile.xml>

to run:
./test -c <treefile.xml> -o <outputfile.xml> /afs/ir/class/cs221/vision/data/easy.avi

to evaluate:
./evaluate -m <outputfile.xml> -g /afs/ir/class/cs221/vision/data/easy.xml

Some optimizations we did to improve the F1 Score:
1) If Information gain is <=10 we default the node into a leaf to prevent overfitting.
2) We only output the best mug per frame. <will need to adjust this when we have more
than 1 mug per frame>

Known Issues for easy.avi at point of milestone:
1) Identifies the clock as a mug
2) Can't find the yellow mug
