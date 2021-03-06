#pragma once
#include "ChartClass\ChartCtrl.h"
#include "ChartClass\ChartLineSerie.h"
#include "ChartClass\ChartAxis.h"
#include "ChartClass\ChartAxisLabel.h"
#include "afxwin.h"
#include <math.h>
#include <vector>
#include "CalibrationData.h"

class ChartDisplay_c
{
public:
	ChartDisplay_c()
		:pAxisX(nullptr),
		pAxisY(nullptr),
		pAxisX_top(nullptr),
		pAxisY_right(nullptr),
		m_c_arrayLength(101), //the size of fitting vector to be displayed
		num_points(5)			//the number of points to be displayed
	{}

	~ChartDisplay_c()
	{
		v_pXYSerie.clear();
		pointdisplay_x_vector.clear();
		pointdisplay_y_vector.clear();
		linedisplay_x_vector.clear();
		linedisplay_y_vector.clear();
		map_vector_y.clear();
		map_vector_x.clear();
	}

public:

	CChartLineSerie * m_pLineSerie;
	std::vector<CChartXYSerie*> v_pXYSerie;
	CChartCtrl m_HSChartCtrl;
	CMFCTabCtrl m_tab;
	CRect rect;
	CChartAxis *pAxisX;
	CChartAxis *pAxisY;
	CChartAxis *pAxisX_top;
	CChartAxis *pAxisY_right;

	double display_min;
	double display_max;
	std::vector<double> pointdisplay_x_vector;
	std::vector<double> pointdisplay_y_vector;
	std::vector<double> linedisplay_x_vector;
	std::vector<double> linedisplay_y_vector;

	std::map<int, std::vector<double>> map_vector_y;
	std::map<int, std::vector<double>> map_vector_x;

	size_t m_c_arrayLength;
	size_t num_points;
};




// CalibrationDiag 对话框
class CalibrationDiag : public CDialogEx
{
	DECLARE_DYNAMIC(CalibrationDiag)

public:
	CalibrationDiag(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CalibrationDiag();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIA_CALIBRATION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CReportCtrl m_calibrationlist;
	CStatic m_picFit;
	CEdit m_inputvalue;
	CEdit m_averagevalue;

	// List control function
	void InitializeReportlist(CalibrationData &t_calibrationdata,
		CString t_current_path, CReportCtrl &_calibrationlist);

	// Picture control Function
	void InitializePIC(ChartDisplay_c &t_ChartDisplay, int ID_PicControl, CString str_Channel);
	void UpdateList(const CalibrationData &t_calibrationdata, CReportCtrl &t_calibrationlist);
	void RecordMap(CalibrationData &t_calibrationdata,
		std::vector<int> &t_sample_channel);
	void ChangeNumPoints(ChartDisplay_c &t_ChartDisplay, size_t t_num_point);
	void InitializePointVector(ChartDisplay_c &t_ChartDisplay);
	void ClearArrayAndDisplay_Multi(ChartDisplay_c &t_ChartDisplay);
	void Copy2ChartDisplay(ChartDisplay_c &t_ChartDisplay,const std::vector<double> &t_linevector_x,
		const std::vector<double> &t_linevector_y,const std::vector<double> &t_pointvector_x,
		const std::vector<double> &t_pointvector_y, double t_display_min, double t_display_max);
	void MakeArrary(ChartDisplay_c &t_ChartDisplay);
	void ResetAxisRange(ChartDisplay_c &t_ChartDisplay);
	void ShowFittingResults(size_t t_num_point, CString t_channel_name,
		const std::vector<double> &t_linevector_x, const std::vector<double> &t_linevector_y, 
		const std::vector<double> &t_pointvector_x, const std::vector<double> &t_pointvector_y, 
		double t_display_min, double t_display_max);

	ChartDisplay_c ChartDisplay;
	int selected_item;

	virtual BOOL OnInitDialog();
	afx_msg void OnClickListCalibration(NMHDR *pNMHDR, LRESULT *pResult);
};

// Non-member function//
void call_calibration_acquisition(CalibrationDiag &t_tabcali, CalibrationData &t_calidata,
	CString t_input_value, double t_average);
void call_calibration_calculate(CalibrationDiag &t_tabcali, CalibrationData &t_calidata,
	CString t_current_path);
void call_show_calibration(CalibrationDiag &t_tabcali, CalibrationData &t_calidata);
void call_calibration_clear(CalibrationDiag &t_tabcali, CalibrationData &t_calidata,
	CString t_current_path);