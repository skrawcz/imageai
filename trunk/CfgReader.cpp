#include "CfgReader.h"

std::map<std::string, std::string> CfgReader::values;
bool CfgReader::cfgRead = false;

std::map<std::string, std::string> CfgReader::powerUps;
bool CfgReader::powerUpsRead = false;

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

	}else std::cout << "Unable to open file";

	cfgRead = true;


};

std::string CfgReader::getValue(const std::string &variable){


	if(!cfgRead)
		readConfig("config.cfg");


	return values[variable];

}

Eggit::Vector3f CfgReader::strToVector(std::string in){

	std::string::iterator it = in.begin();
	std::string num = "";
	int i = 0;
	Eggit::Vector3f out;

	out.Set(0.0f,0.0f,0.0f);

	while(it != in.end()){

		if((*it) != ','){
			num += (*it);
		}else{
			float value = (float)atof(num.c_str());
			if(i == 0) out.x = value;
			if(i == 1) out.y = value;
			num = "";
			i++;
		}
		
		it++;
			

	}

	float value = (float)atof(num.c_str());
	out.z = value;

	return out;

}

int	 CfgReader::strToInt(std::string in){

	return atoi(in.c_str());

}

double	CfgReader::strToDouble(std::string in){

	return atof(in.c_str());

}

void CfgReader::readPowerUps(const std::string &fileName){

	std::ifstream myfile (fileName.c_str());

	

	if (myfile.is_open())
	{
		std::string line, value, variable;

		value = variable = "";


		while (getline(myfile,line))
		{
			if(line.length() > 0 && line.at(0) != '['){

				if(line.at(0) ==  '<'){
					if(variable.size() != 0){

						powerUps[variable] = value;

					}

					variable = line.substr(1,line.size() - 2);

					value = "";
				}else{
					if(value.size() != 0)
						value.append("\n");
					value.append(line);

				}

			}
		}

	powerUps[variable] = value;

    myfile.close();

	}else std::cout << "Unable to open file";

	powerUpsRead = true;



}

std::vector<std::string> CfgReader::getPowerUps(){

	// we need normal cfg to know where powerups are placed
	if(!cfgRead)
		readConfig("config.cfg");

	if(!powerUpsRead){
		// find real path to powerups cfg
		std::string path = getValue("PowerUpsPath");
		path.append("powerups.cfg");

		readPowerUps(path);
	}

	std::map<std::string,std::string>::iterator it = powerUps.begin();

	std::vector<std::string> out;

	while(it != powerUps.end()){

		out.push_back(it->first);

		++it;

	}

	return out;


}

std::string CfgReader::getPowerUp(const std::string &variable){


	// we need normal cfg to know where powerups are placed
	if(!cfgRead)
		readConfig("config.cfg");

	if(!powerUpsRead){
		// find real path to powerups cfg
		std::string path = getValue("PowerUpsPath");
		path.append("powerups.cfg");

		readPowerUps(path);
	}


	return powerUps[variable];	



}