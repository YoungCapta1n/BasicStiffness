
#ifndef _CALIBRATIONDATA_H_
#define _CALIBRATIONDATA_H_

#include "BoostLib.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <algorithm>

class CalibrationData {
public:
	std::vector<double> calibration_slope;
	std::vector<double> calibration_intercept;
	std::vector<int> v_selected_channel;
	// Record calibration status and selected or not
	// 0--> calibration is done and then selected
	// 1--> doing acquisition but calibration is not finished
	// 2--> empty caliration data
	std::vector<int> calibration_status;
	std::map<int, std::vector<double>> calibration_dict;
	int sample_num; // the number of sample points for calibration
	int total_num_channel;

public:
	CalibrationData(int t_num_channel = 16);
	~CalibrationData();
	void ReadConfigurationData(CString Filename);
	void WriteConfigurationData(CString Filename) const;
	void split(const std::string &src, const std::string &delim,
		std::vector<std::string> &dest);
	void Add2Map(int t_index_channel, double t_input_value, double t_average);
	void LLSCalculate(int t_index_channel);
	void ClearOne(int t_index_channel);
	void Convert4Display(int t_index_channel, int &t_num_point,
		std::vector<double> &t_linevector_x,
		std::vector<double> &t_linevector_y,
		std::vector<double> &t_pointvector_x,
		std::vector<double> &t_pointvector_y,
		double &display_min, double &display_max);

private:
};

#endif //_CALIBRATIONDATA_H_