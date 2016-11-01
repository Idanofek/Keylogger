#include "Configuration.h"
#include <typeinfo>

using namespace std;
Configuration::Configuration(string conf)
{
	ifstream ifs(conf, ios::in | ios::binary | ios::ate);

	ifstream::pos_type fileSize = ifs.tellg();
	ifs.seekg(0, ios::beg);

	vector<char> bytes(fileSize);
	ifs.read(&bytes[0], fileSize);

	string data(&bytes[0], fileSize);
	Json::Value root;
	Json::Reader reader;

	bool prs = reader.parse(data, root);
	if (!prs)
	{
		std::cout << "Failed to parse!" << reader.getFormattedErrorMessages();
	}

	_lgfl_name = root["logfile-configuration"]["filepath"].toStyledString();
	_srvport = atoi((root["server-configuration"]["port"].asString()).c_str());
	_srvip = root["server-configuration"]["ip-address"].toStyledString();

}

string Configuration::get_lgfl_name() const { return _lgfl_name; }
string Configuration::get_srvip() const { return _srvip; }
int Configuration::get_srvport() const { return _srvport; }
