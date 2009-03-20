#ifndef CFGREADER_H
#define CFGREADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector> 
#include <string> 

/*
 * This reads in a confing file and stores the contents in and array for
 * later retrieval by the programs.
 *
 *
 */
class CfgReader {

public:
	//static method calls so it can be called from anywhere
	static	int						strToInt(const std::string &in);
	static	double				strToDouble(const std::string &in);


	static 	std::string 	getValue(const std::string &variable);
	static 	double 				getDouble(const std::string &variable);
	static 	int 					getInt(const std::string &variable);

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
