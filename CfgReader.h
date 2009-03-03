#ifndef CFGREADER_H
#define CFGREADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector> 
#include <string> 


class CfgReader {

public:
	
	static	int						strToInt(const std::string &in);
	static	double				strToDouble(const std::string &in);

	static 	std::string 	getValue(const std::string &variable);

private:


	static void readConfig(const std::string &fileName);

	static std::map<std::string, std::string> values;
	static bool cfgRead;


};

#endif // CfgReader_H
