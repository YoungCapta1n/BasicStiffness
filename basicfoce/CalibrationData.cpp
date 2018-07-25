#include "stdafx.h"
#include "CalibrationData.h"


CalibrationData::CalibrationData(int t_num_channel)
    : sample_num(100), total_num_channel(t_num_channel),
      calibration_slope(t_num_channel, 1.0),
      calibration_intercept(t_num_channel, 0.0),
      desired_load(t_num_channel, 0.0), calibration_status(t_num_channel, 2) {

  v_motor_index.reserve(total_num_channel);
  for (int i = 0; i != total_num_channel; ++i) {
    v_motor_index.push_back(i);
    calibration_dict.insert(std::make_pair(i, std::vector<double>{}));
  }
}

CalibrationData::~CalibrationData() {}

void CalibrationData::WriteConfigurationData(CString Filename) const {
  std::ofstream fout(Filename);
  if (fout) {
    for (int it = 0; it != total_num_channel; ++it)
      fout << it << " " << v_motor_index[it] << " " << calibration_slope[it]
           << " " << calibration_intercept[it] << " " << desired_load[it] << " "
           << calibration_status[it] << std::endl;
  }
  fout.close();
}

void CalibrationData::ReadConfigurationData(CString Filename) {
  // Read slope and intercept
  Filename += _T("\\calibration.txt");
  std::ifstream infile(Filename, std::ios::in);
  std::string delim(" ");
  if (infile) {
    calibration_slope.clear();
    calibration_intercept.clear();
    desired_load.clear();
    v_motor_index.clear();
    calibration_status.clear();
    for (std::string line; getline(infile, line, '\n');) {
      std::vector<std::string> results;
      line = line + " ";
      split(line, delim, results);
      v_motor_index.push_back(std::stoi(results[1])); // convert string to int
      calibration_slope.push_back(
          std::stod(results[2])); // convert string to double
      calibration_intercept.push_back(std::stod(results[3]));
      desired_load.push_back(std::stod(results[4]));
      calibration_status.push_back(std::stoi(results.back()));
    }
  } else
    WriteConfigurationData(Filename);

  infile.close();
}

void CalibrationData::split(const std::string &src, const std::string &delim,
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

void CalibrationData::Add2Map(int t_index_channel, double t_input_value,
                              double t_average) {
  // Add the measured and user-input values to the key-value map
  std::map<int, std::vector<double>>::iterator it =
      calibration_dict.find(t_index_channel);
  if (it != calibration_dict.end()) {
    it->second.push_back(t_input_value);
    it->second.push_back(t_average);
  }
  // setup calibration status
  calibration_status[t_index_channel] = 1;
}

void CalibrationData::LLSCalculate(int t_index_channel) {
  // Fitting using Linear Least Square
  std::map<int, std::vector<double>>::const_iterator it =
      calibration_dict.find(t_index_channel);
  std::vector<double> t_result;
  LinearFitV2V(it->second, t_result);
  calibration_slope[t_index_channel] = t_result.front();
  calibration_intercept[t_index_channel] = t_result.back();
  // setup calibration status
  calibration_status[t_index_channel] = 0;
}

void CalibrationData::ClearOne(int t_index_channel) {
  // Clear one key-value of map, to re-fit
  std::map<int, std::vector<double>>::iterator it =
      calibration_dict.find(t_index_channel);
  it->second.clear();
  // setup calibration status
  calibration_status[t_index_channel] = 2;
  // reset calibrated zero and slope
  calibration_intercept[t_index_channel] = 0;
  calibration_slope[t_index_channel] = 1;
}

void CalibrationData::Convert4Display(int t_index_channel, int &t_num_point,
                                      std::vector<double> &t_linevector_x,
                                      std::vector<double> &t_linevector_y,
                                      std::vector<double> &t_pointvector_x,
                                      std::vector<double> &t_pointvector_y,
                                      double &display_min,
                                      double &display_max) {
  std::map<int, std::vector<double>>::const_iterator it =
      calibration_dict.find(t_index_channel);
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
    t_linevector_y.push_back(temp_x * calibration_slope[t_index_channel] +
                             calibration_intercept[t_index_channel]);
  }
}