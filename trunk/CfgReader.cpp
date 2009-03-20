/**
 *This class reads in our configuration file. I.e. it parses it and stores
 *the values for later retrieval by our program.
 *
 *
 *
 */


#include "CfgReader.h"

std::map<std::string, std::string> CfgReader::values;
bool CfgReader::cfgRead = false;

// This reads the configuration file and stores the variables
void CfgReader::readConfig(const std::string &fileName){


	std::ifstream myfile (fileName.c_str());
	bool lika = false;
	

	if (myfile.is_open())
	{
		//std::cout<<"opened file "<<fileName<<std::endl;
		std::string line, value, variable;

		while (getline(myfile,line))
		{
			if(line.length() > 0){

				value = variable = "";

				std::string::iterator it = line.begin();

				if((*it) != '['){

					while(it != line.end()){

						if((*it) == '='){
							lika = true;

						}else{
							
							if(lika){

								value += (*it);

							}else{

								variable += (*it);

							}
						}

						it++;

					}
				}

				if(lika){
					values[variable] = value;

				}
				
				lika = false;
			}

		}
    myfile.close();

	}else std::cout << "Unable to open file" << std::endl;

	cfgRead = true;


};

//this sets the cfg
void CfgReader::setUpCfg(const char *dir){

	std::string main(dir);


	readConfig(main);



}

//this is hard coded so when test is run it knows where to look
void CfgReader::readMainConfig(){

	readConfig("configs/basic.cfg");

}

//this gets a string value corresponding to the string passed
std::string CfgReader::getValue(const std::string &variable){


	if(!cfgRead)
		readMainConfig();

	//std::values
	return values[variable];

}

//this gets a double
double CfgReader::getDouble(const std::string &variable){


	if(!cfgRead)
		readMainConfig();


	return strToDouble(values[variable]);

}

//this gets an int
int CfgReader::getInt(const std::string &variable){


	if(!cfgRead)
		readMainConfig();

	return strToInt(values[variable]);

}

//this turns a string into an int
int	 CfgReader::strToInt(const std::string &in){

	return atoi(in.c_str());

}
//this turns a string into a double
double	CfgReader::strToDouble(const std::string &in){

	return atof(in.c_str());

}
