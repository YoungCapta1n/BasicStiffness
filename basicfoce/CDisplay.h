#pragma once

#include "CalibrationData.h"
#include "database.h"
#include "CalibrationDiag.h"
#include "StiffnessData.h"
// CDisplay 对话框


class CDisplay : public CDialogEx
{
	DECLARE_DYNAMIC(CDisplay)

public:
	CDisplay(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDisplay();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIADISPLAY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:

	void InitializeLIST(StiffnessData &t_stiffnessdata,
		CString t_current_path, CReportCtrl &_listchannelvalue);
	void InitializePIC(ChartDisplay_c &t_ChartDisplay,
		int ID_PicControl, CString str_Channel);
	void Addstiffness2List(const StiffnessData &_stiffnessdata, CReportCtrl &t_stiffnesslist);
	void RecordMap(StiffnessData &_stiffnessdata,
		std::vector<int> &t_sample_channel);
	void ChangeNumPoints(ChartDisplay_c &t_ChartDisplay, size_t t_num_point);
	void InitializePointVector(ChartDisplay_c &t_ChartDisplay);
	void ClearArrayAndDisplay_Multi(ChartDisplay_c &t_ChartDisplay);
	void Copy2ChartDisplay(ChartDisplay_c &t_ChartDisplay, const std::vector<double> &t_linevector_x,
		const std::vector<double> &t_linevector_y, const std::vector<double> &t_pointvector_x,
		const std::vector<double> &t_pointvector_y, double t_display_min, double t_display_max);
	void MakeArrary(ChartDisplay_c &t_ChartDisplay);
	void ResetAxisRange(ChartDisplay_c &t_ChartDisplay);
	void ShowFittingResults(size_t t_num_point, CString t_channel_name,
		const std::vector<double> &t_linevector_x, const std::vector<double> &t_linevector_y, 
		const std::vector<double> &t_pointvector_x, const std::vector<double> &t_pointvector_y, 
		double t_display_min, double t_display_max);

	CReportCtrl m_listchannelvalue;
	ChartDisplay_c m_picshowstiffness;
	CStatic m_picstiffness;

	CEdit m_lengthstep;
	int selected_item;
	std::vector<double> max_onetable;
	std::vector<double> min_onetable;
	std::vector<double> avg_onetable;
	std::vector<double> std_onetable;

	afx_msg void OnClickListChannelvalue(NMHDR *pNMHDR, LRESULT *pResult);
};


// Non-member function//
void UpdateListusingRawData(CDisplay &t_cdisplay,
	const CalibrationData &t_cablirationdata);

void computerealtimeLoad(const CalibrationData &t_cablirationdata,
	const std::vector<double> &t_rawdata, std::vector<double> &_loaddata);

double showstatisticsdata(CDisplay &t_cdisplay, 
	StiffnessData &t_stiffnessdata, database &_database);

void call_stiffness_acquisition(CDisplay &t_cdisplay, StiffnessData &t_stiffnessdata, 
	database &_database, CString t_input_value, int deformation_index);
void call_stiffness_calculate(CDisplay &t_cdisplay, StiffnessData &t_stiffnessdata,
	CString t_current_path);
void call_show_stiffness(CDisplay &t_cdisplay, StiffnessData &t_stiffnessdata);

void call_stiffness_clear(CalibrationDiag &t_tabcali, StiffnessData &t_calidata,
	CString t_current_path);