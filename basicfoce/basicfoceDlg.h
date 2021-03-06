
// basicfoceDlg.h: 头文件
//

#pragma once
#include "CalibrationDiag.h"
#include "CDisplay.h"
#include "ChartClass\ChartAxis.h"
#include "ChartClass\ChartCtrl.h"
#include "ChartClass\ChartLineSerie.h"
#include "ReportCtrl.h"
#include "DataAcquisition.h"
#include "database.h"
#include "StiffnessData.h"

// CbasicfoceDlg 对话框
class CbasicfoceDlg : public CDialogEx
{
	// 构造
public:
	CbasicfoceDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BASICFOCE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_Devices;
	CButton m_selectdevice;
	CEdit m_collectrate;
	CButton m_startcollection;
	CButton m_puase;
	CButton m_stopcollection;
	CListBox m_outputinformation;
	CTabCtrl m_maintab;
	CButton m_collectshort;
	CButton m_calculate;
	CButton m_clearcalibration;
	CButton m_finishcalibration;
	CEdit m_projectname;
	CButton m_computestiffness;
	CButton m_clearstiffness;
	afx_msg void OnBnClickedButComputestiffness();
	afx_msg void OnTcnSelchangeMaintab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButSelectdevice();
	afx_msg void OnBnClickedButSetupandstart();
	afx_msg void OnBnClickedButPause();
	afx_msg void OnBnClickedButStopcollection();
	afx_msg void OnBnClickedButCollectshort();
	afx_msg void OnBnClickedButCalculate();
	afx_msg void OnBnClickedButClearcali();
	afx_msg void OnBnClickedButFinishcali();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL DestroyWindow();


private:

	CalibrationDiag m_tabcalibration;
	CDisplay m_tabDisplay;
	CString current_path;
	CString ProjectName;
	const int total_num_channel;
	DataAcquisition _data_acquisition;
	CalibrationData calibration_data;
	StiffnessData stiffness_data;
	database _database;
	const unsigned int time_inteval;
	int deformation_index;

	void Func4onTimer(); // inline function to speed up
	int ConvertEditToDouble(CEdit &m_edit);
	void ListboxMessage(CString str);
	void GetCurrentEXEPath();
	void IndependentThread4Socket();
	void ThreadCaller4Socket();
	void IndependentThread4Calibration(int t_index_channel, double &t_average);



};
