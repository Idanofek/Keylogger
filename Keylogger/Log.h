#pragma once
#include <iostream>
#include <string>
#include <json/json.h>
#include <fstream>

using namespace std;
class Log {
	
	public:
		
		Log(string logfile_location) 
		{
			_logfile_name = logfile_location; 
			_out = std::ofstream(logfile_location, ios::app);
			cout << "---------> Opening log.txt for writing" << endl;
		}

		void write(string data) { _out << data << endl; }
		void write(char data) {	_out << data << endl; }
		void write_time();

		~Log()
		{
			_out.close();
			cout << "---------> Saving " << _logfile_name << endl;
		}

	private:

		string _logfile_name;
		std::ofstream _out;  // Log file handler

};