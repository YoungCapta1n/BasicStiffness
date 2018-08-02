#pragma once

#include "BoostLib.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <algorithm>
#include "database.h"

const int MAXNUMSTEP=5;

class StiffnessData {
public:
	std::vector<double> stiffness_slope;
	std::vector<double> stiffness_intercept;
	std::vector<int> v_selected_channel;
	// Record calibration status and selected or not
	// 0--> calibration is done and then selected
	// 1--> doing acquisition but calibration is not finished
	// 2--> empty caliration data
	std::vector<int> stiffness_status;
	// Record steplength of deformation and associated Load
	// stepandmeanvalue_dict[:][index]
	// index-->even : steplength
	// index-->odd :  load
	std::map<int, std::vector<double>> stepandmeanvalue_dict;
	std::vector<double> max_onetable;
	std::vector<double> min_onetable;
	std::vector<double> avg_onetable;
	std::vector<double> std_onetable;
	int sample_num; // the number of sample points for calibration
	int total_num_channel;


public:
	explicit StiffnessData(int t_num_channel = 16);
	~StiffnessData();
	void ReadConfigurationData(CString Filename);
	void WriteConfigurationData(const CString &Filename) ;
	void split(const std::string &src, const std::string &delim,
		std::vector<std::string> &dest);
	void Add2Map(int t_index_channel, double t_input_value, double t_average_load, int index);
	void LLSCalculate(int t_index_channel);
	void ClearOne(int t_index_channel);
	void Convert4Display(int t_index_channel, int &t_num_point,
		std::vector<double> &t_linevector_x,
		std::vector<double> &t_linevector_y,
		std::vector<double> &t_pointvector_x,
		std::vector<double> &t_pointvector_y,
		double &display_min, double &display_max);
	void computestatistics(database &_database);

private:
};
