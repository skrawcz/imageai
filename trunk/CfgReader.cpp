#include "CfgReader.h"

std::map<std::string, std::string> CfgReader::values;
bool CfgReader::cfgRead = false;

void CfgReader::readConfig(const std::string &fileName){


	std::ifstream myfile (fileName.c_str());
	bool lika = false;
	

	if (myfile.is_open())
	{
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

void CfgReader::readMainConfig(){


	std::ifstream myfile ("configs/mainConfig.cfg");

	if (myfile.is_open()){

		std::string line;
		getline(myfile,line);

		std::cout << line << std::endl;

		readConfig(line);
	}else
		std::cout << "unable to open main cfg" << std::endl;

	myfile.close();

}

std::string CfgReader::getValue(const std::string &variable){


	if(!cfgRead)
		readMainConfig();

	// if one feels like looking at the content of the map
	//std::map<std::string, std::string>::iterator it = values.begin();

	//while(it != values.end()){
	//	std::cout << it->first.size() << std::endl;
	//	++it;
	//}


	return values[variable];

}

int	 CfgReader::strToInt(const std::string &in){

	return atoi(in.c_str());

}

double	CfgReader::strToDouble(const std::string &in){

	return atof(in.c_str());

}
