#ifndef CFGREADER_H
#define CFGREADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector> 
#include <string> 

#include "../math3d.h"


class CfgReader {

public:
	


	static	Eggit::Vector3f			 strToVector(std::string in);
	static	int						 strToInt(std::string in);
	static	double					 strToDouble(std::string in);

	static std::string getValue(const std::string &variable);

	static std::vector<std::string> getPowerUps();

	static std::string getPowerUp(const std::string &variable);


private:


	static void readConfig(const std::string &fileName);
	static void readPowerUps(const std::string &fileName);

	static std::map<std::string, std::string> values;
	static bool cfgRead;

	static std::map<std::string, std::string> powerUps;
	static bool powerUpsRead;


};

#endif // CfgReader_H