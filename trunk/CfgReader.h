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
	static  void					setUpCfg(const char *dir);

private:

	static void readMainConfig();
	static void readConfig(const std::string &fileName);

	// store stuff
	static std::map<std::string, std::string> values;

	// make sure to only read cfg once.
	static bool cfgRead;


};

#endif // CfgReader_H
