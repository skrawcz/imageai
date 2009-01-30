#ifndef DECISIONTREE_H
#define DECISIONTREE_H


#include <iostream>
#include <fstream>
#include <vector>

class DecisionTree{


public:

DecisionTree();
DecisionTree(std::ifstream &in);
~DecisionTree();
void print(std::ofstream &out);


private:


std::vector<DecisionTree*> children;



};


#endif
