#pragma once
#include <json/json.h>
#include <string>
#include <fstream>
#include <iostream>

#define DEFAULT_CONFIG_FILE "configuration.conf"

using namespace std;
class Configuration
{
	private:
		int _srvport;
		string _srvip;
		string _lgfl_name;

	public:
		Configuration(string conf = DEFAULT_CONFIG_FILE);
		~Configuration();
		int get_srvport() const;
		string get_srvip() const;
		string get_lgfl_name() const;
};