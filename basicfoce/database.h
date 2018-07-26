#pragma once
#include "stdafx.h"
#include "CppSQLite3.h"
#include <ctime>
#include <iostream>
#include <sstream>

using namespace std;

class database {
public:
	explicit database() : tablename(""),
		time4table(std::time(nullptr)), selected_channel(16, 0) {}
	~database() {}

	void opendatabase(const CString &_currentpath, const CString &_projectname) {
		try {
			CString _savepath = _currentpath + _T("\\") + _projectname + _T(".db");
			const char *savepath = (LPCSTR)_savepath;
			_db.open(savepath);
		}
		catch (CppSQLite3Exception& e)
		{
			cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		}

	}

	void createonetable(const std::vector<int> &_selected_channel) {
		try {
			// get current unix time and save to std::string
			generatetablename();
			set_selectedchannel(_selected_channel);
			size_t num_channel = selected_channel.size();
			// convert to const char *
			const char *table_start = "CREATE TABLE S";
			const char *table_name = tablename.c_str();
			const char *table_middle = " (DATETIME    TEXT       NOT NULL";
			char *channel_char = (char *)malloc(3 + num_channel * 15 + strlen(table_start) +
				strlen(table_name) + strlen(table_middle));
			strcpy(channel_char, table_start);
			strcat(channel_char, table_name);
			strcat(channel_char, table_middle);
			for (size_t i = 0; i != num_channel; ++i) {
				char buffer[3];
				sprintf(buffer, "%d", selected_channel[i]);
				strcat(channel_char, ",Chan");
				strcat(channel_char, buffer);
				strcat(channel_char, " DOUBLE");
			}
			strcat(channel_char, ");");
			_db.execDML(channel_char);
			free(channel_char);
		}
		catch (CppSQLite3Exception& e)
		{
			cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		}
	}

	void loop_updateonetable_t() {
		std::thread t1(&database::loop_updateonetable, this);
		t1.detach();
	}

private:
	std::string tablename;
	CppSQLite3DB _db;
	std::time_t time4table;
	std::vector<int> selected_channel;

	void loop_updateonetable() {
		while (1)
		{
			updateonetable();
			std::this_thread::sleep_for(std::chrono::microseconds(20));
		}
	}

	void generatetablename() {
		// update time
		time4table = std::time(nullptr);
		std::stringstream ss;
		ss << time4table;
		tablename = ss.str();
	}

	void set_selectedchannel(const std::vector<int> &_selected_channel) {
		selected_channel = _selected_channel;
	}

	// convert a float array to char array
	void convert_doublep_char(const std::vector<double> &t_motiondata, char *t_str) {
		// t_str should be initialized
		for (size_t i = 0; i != t_motiondata.size(); ++i) {
			char buffer[64];
			snprintf(buffer, sizeof buffer, "%lf", t_motiondata[i]);
			strcat(t_str, ", ");
			strcat(t_str, buffer);
		}
	}

	void updateonetable() {

		size_t num_channel = realtimeVoltage.size();
		// convert to const char *
		const char *update_start = "INSERT INTO S";
		const char *update_name = tablename.c_str();
		const char *update_middle = " VALUES (julianday('now')";
		char *channel_char = (char *)malloc(3 + num_channel * 64 + strlen(update_start) +
			strlen(update_name) + strlen(update_middle));
		strcpy(channel_char, update_start);
		strcat(channel_char, update_name);
		strcat(channel_char, update_middle);
		convert_doublep_char(realtimeVoltage, channel_char);
		strcat(channel_char, ");");
		_db.execDML(channel_char);
		free(channel_char);
	}
};