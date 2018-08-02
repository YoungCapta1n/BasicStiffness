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
		time4table(std::time(nullptr)), selected_channel(16, 0), num_channel(16){}
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

	// thread for database save
	void loop_updateonetable_t() {
		std::thread t1(&database::loop_updateonetable, this);
		t1.detach();
	}
	void loop_updateonetable() {
		while (1)
		{
			updateonetable();
			std::this_thread::sleep_for(std::chrono::microseconds(20));
		}
	}

	void updateonetable() {

		// convert to const char *
		const char *update_start = "INSERT INTO S";
		const char *update_name = tablename.c_str();
		const char *update_middle = " VALUES (julianday('now')";
		char *channel_char = (char *)malloc(3 + num_channel * 64 + strlen(update_start) +
			strlen(update_name) + strlen(update_middle));
		strcpy(channel_char, update_start);
		strcat(channel_char, update_name);
		strcat(channel_char, update_middle);
		convert_doublep_char(realtimeLoad, channel_char);
		strcat(channel_char, ");");
		_db.execDML(channel_char);
		free(channel_char);
	}

	void computestatistics(std::vector<double> &_max_onetable,
						    std::vector<double> &_min_onetable,
							std::vector<double> &_avg_onetable,
		                    std::vector<double> &_std_onetable) {
		computemax(_max_onetable);
		computemin(_min_onetable);
		computeavg(_avg_onetable);
		computestd(_std_onetable);
	}

	size_t getnumselectedchannel() const{
		return num_channel;
	}
	std::vector<int> getselected_channel() const{
		return selected_channel;
	}
private:
	std::string tablename;
	CppSQLite3DB _db;
	std::time_t time4table;
	std::vector<int> selected_channel;
	size_t num_channel;

	void generatetablename() {
		// update time
		time4table = std::time(nullptr);
		std::stringstream ss;
		ss << time4table;
		tablename = ss.str();
	}

	void set_selectedchannel(const std::vector<int> &_selected_channel) {
		selected_channel = _selected_channel;
		num_channel = _selected_channel.size();
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
	// compute the max value of all channels of selected table
	void computemax(std::vector<double> &_max_onetable) {
		CppSQLite3Query _q;
		// max
		const char *table_start = "SELECT MAX(";
		char table_end[50];
		strcpy(table_end, ") FROM S");
		strcat(table_end, tablename.c_str());
		strcat(table_end, ";");
		for (size_t i = 0; i != num_channel; ++i) {
			char *max_char = (char *)malloc(20 + strlen(table_start) +
				strlen(table_end));
			char buffer[3];
			sprintf(buffer, "%d", selected_channel[i]);
			strcpy(max_char, table_start);
			strcat(max_char, "Chan");
			strcat(max_char, buffer);
			strcat(max_char, table_end);
			_q = _db.execQuery(max_char);
			_max_onetable[i] = atof(_q.fieldValue(0));
			free(max_char);
		}
	}
	// compute the min value of all channels of selected table
	void computemin(std::vector<double> &_min_onetable) {
		CppSQLite3Query _q;
		// max
		const char *table_start = "SELECT MIN(";
		char table_end[50];
		strcpy(table_end, ") FROM S");
		strcat(table_end, tablename.c_str());
		strcat(table_end, ";");
		for (size_t i = 0; i != num_channel; ++i) {
			char *min_char = (char *)malloc(20 + strlen(table_start) +
				strlen(table_end));
			char buffer[3];
			sprintf(buffer, "%d", selected_channel[i]);
			strcpy(min_char, table_start);
			strcat(min_char, "Chan");
			strcat(min_char, buffer);
			strcat(min_char, table_end);
			_q = _db.execQuery(min_char);
			_min_onetable[i] = atof(_q.fieldValue(0));
			free(min_char);
		}
	}
	// compute the mean value of all channels of selected table
	void computeavg(std::vector<double> &_avg_onetable) {
		CppSQLite3Query _q;
		// max
		const char *table_start = "SELECT AVG(";
		char table_end[50];
		strcpy(table_end, ") FROM S");
		strcat(table_end, tablename.c_str());
		strcat(table_end, ";");
		for (size_t i = 0; i != num_channel; ++i) {
			char *avg_char = (char *)malloc(20 + strlen(table_start) +
				strlen(table_end));
			char buffer[3];
			sprintf(buffer, "%d", selected_channel[i]);
			strcpy(avg_char, table_start);
			strcat(avg_char, "Chan");
			strcat(avg_char, buffer);
			strcat(avg_char, table_end);
			_q = _db.execQuery(avg_char);
			_avg_onetable[i] = atof(_q.fieldValue(0));
			free(avg_char);
		}
	}
	// compute the mean value of all channels of selected table
	void computestd(std::vector<double> &_std_onetable) {
		CppSQLite3Query _q;
		// std
		const char *table_start = "SELECT AVG((";
		char table_namec[40];
		strcpy(table_namec, "S");
		strcat(table_namec, tablename.c_str());
		char table_end[100];
		strcpy(table_end, ") AS a FROM ");
		strcat(table_end, table_namec);
		strcat(table_end, ") AS sub;");
		for (size_t i = 0; i != num_channel; ++i) {
			char *std_char = (char *)malloc(300);
			char buffer[3];
			sprintf(buffer, "%d", selected_channel[i]);
			strcpy(std_char, table_start);
			strcat(std_char, "Chan");
			strcat(std_char, buffer);
			strcat(std_char, "-sub.a)*(Chan");
			strcat(std_char, buffer);
			strcat(std_char, "-sub.a)) FROM ");
			strcat(std_char, table_namec);
			strcat(std_char, ", (SELECT AVG(Chan");
			strcat(std_char, buffer);
			strcat(std_char, table_end);
			_q = _db.execQuery(std_char);
			_std_onetable[i] = atof(_q.fieldValue(0));
			free(std_char);
		}
	}

};