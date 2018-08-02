#include "stdafx.h"
#include "StiffnessData.h"


StiffnessData::StiffnessData(int t_num_channel)
	: sample_num(100), total_num_channel(t_num_channel),
	stiffness_slope(t_num_channel, 1.0),
	stiffness_intercept(t_num_channel, 0.0),
	stiffness_status(t_num_channel, 2)
{
	for (int i = 0; i != total_num_channel; ++i) {
		stepandmeanvalue_dict.insert(std::make_pair(i, std::vector<double>(2*MAXNUMSTEP, 0)));
	}
}

StiffnessData::~StiffnessData() {}

void StiffnessData::WriteConfigurationData(const CString &Filename) {
	std::ofstream fout(Filename);
	if (fout) {
		for (int it = 0; it != total_num_channel; ++it)
		{
			fout << it << " ";
			for (int index_step = 0; index_step != 2*MAXNUMSTEP; ++index_step)
			{
				fout << stepandmeanvalue_dict[it][index_step] << " ";
			}
			fout << stiffness_slope[it] << " " << stiffness_intercept[it] << " "
				<< stiffness_status[it] << std::endl;
		}
	}
	fout.close();
}

void StiffnessData::ReadConfigurationData(CString Filename) {
	// Read slope and intercept
	Filename += _T("\\stiffness.txt");
	std::ifstream infile(Filename, std::ios::in);
	std::string delim(" ");
	if (infile) {
		stiffness_slope.clear();
		stiffness_intercept.clear();
		stiffness_status.clear();
		int it = 0;
		for (std::string line; getline(infile, line, '\n');) {
			std::vector<std::string> results;
			line = line + " ";
			split(line, delim, results);
			for (int index_step = 0; index_step != MAXNUMSTEP; ++index_step)
			{
				stepandmeanvalue_dict[it][2 * index_step] = std::stod(results[2 * index_step + 1]);
				stepandmeanvalue_dict[it][2 * index_step + 1] = std::stod(results[2 * index_step + 2]);
			}
			stiffness_slope.push_back(
				std::stod(results[2 * MAXNUMSTEP + 1])); // convert string to double
			stiffness_intercept.push_back(std::stod(results[2 * MAXNUMSTEP + 2]));
			stiffness_status.push_back(std::stoi(results.back()));
			++it;
		}
	}
	else
		WriteConfigurationData(Filename);

	infile.close();
}

void StiffnessData::split(const std::string &src, const std::string &delim,
	std::vector<std::string> &dest) {
	// Split a string using some delim
	std::string str = src;
	std::string::size_type start = 0, index;
	std::string substr;

	index = str.find_first_of(delim, start); //
	while (index != std::string::npos) {
		substr = str.substr(start, index - start);
		dest.push_back(substr);
		start = str.find_first_not_of(delim, index); //
		if (start == std::string::npos)
			return;

		index = str.find_first_of(delim, start);
	}
}

void StiffnessData::computestatistics(database &_database) {
	size_t selected_length = _database.getnumselectedchannel();
	max_onetable.resize(selected_length);
	min_onetable.resize(selected_length);
	avg_onetable.resize(selected_length);
	std_onetable.resize(selected_length);
	_database.computestatistics(max_onetable, min_onetable, avg_onetable, std_onetable);
}


void StiffnessData::Add2Map(int t_index_channel, double t_input_value,
	double t_average_load, int index) {
	// Add the measured and user-input values to the key-value map
	// argument t_input_value will be an arithmetic sequence
	// argument index indicates where to add the deformation and mean load
	stepandmeanvalue_dict[t_index_channel][2 * index] = t_input_value;
	stepandmeanvalue_dict[t_index_channel][2 * index + 1] = t_average_load;
	
	// setup calibration status
	stiffness_status[t_index_channel] = 1;
}

void StiffnessData::LLSCalculate(int t_index_channel) {
	// Fitting using Linear Least Square
	std::map<int, std::vector<double>>::const_iterator it =
		stepandmeanvalue_dict.find(t_index_channel);
	std::vector<double> t_result;
	LinearFitV2V(it->second, t_result);
	stiffness_slope[t_index_channel] = t_result.front();
	stiffness_intercept[t_index_channel] = t_result.back();
	// setup calibration status
	stiffness_status[t_index_channel] = 0;
}

void StiffnessData::ClearOne(int t_index_channel) {
	// Clear one key-value of map, to re-fit
	std::map<int, std::vector<double>>::iterator it =
		stepandmeanvalue_dict.find(t_index_channel);
	it->second.clear();
	// setup calibration status
	stiffness_status[t_index_channel] = 2;
	// reset calibrated zero and slope
	stiffness_intercept[t_index_channel] = 0;
	stiffness_slope[t_index_channel] = 1;
}

void StiffnessData::Convert4Display(int t_index_channel, int &t_num_point,
	std::vector<double> &t_linevector_x,
	std::vector<double> &t_linevector_y,
	std::vector<double> &t_pointvector_x,
	std::vector<double> &t_pointvector_y,
	double &display_min,
	double &display_max) {
	std::map<int, std::vector<double>>::const_iterator it =
		stepandmeanvalue_dict.find(t_index_channel);
	double t_min = *std::min_element(it->second.begin(), it->second.end());
	double t_max = *std::max_element(it->second.begin(), it->second.end());
	display_min = t_min - 0.05 * (t_max - t_min);
	display_max = t_max + 0.05 * (t_max - t_min);
	if (it->second.size() % 2 == 0) {
		t_num_point = it->second.size() / 2; // the number of points
		for (int i = 0; i != t_num_point; ++i) {
			t_pointvector_x.push_back(it->second[2 * i + 1]);
			t_pointvector_y.push_back(it->second[2 * i]);
		}
	}
	double t_minx =
		*std::min_element(t_pointvector_x.begin(), t_pointvector_x.end());
	double t_maxx =
		*std::max_element(t_pointvector_x.begin(), t_pointvector_x.end());

	for (unsigned i = 0; i != 101; ++i) {
		double temp_x = t_minx + i * (t_maxx - t_minx) / 100;
		t_linevector_x.push_back(temp_x);
		t_linevector_y.push_back(temp_x * stiffness_slope[t_index_channel] +
			stiffness_intercept[t_index_channel]);
	}
}