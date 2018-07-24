#pragma once

#ifndef AFX_DCWIZDLG_H__4E85C325_2795_4F37_B314_F321401DD0C0__INCLUDED_
//#if !defined(AFX_DCWIZDLG_H__4E85C325_2795_4F37_B314_F321401DD0C0__INCLUDED_)
#define AFX_DCWIZDLG_H__4E85C325_2795_4F37_B314_F321401DD0C0__INCLUDED_

#include "DAQClass\DCHelpers.h"
#include "ReportCtrl.h"
#include "afxcmn.h"
#include "afxwin.h"
#include <chrono>
#include <thread>
#include <vector>

class DataAcquisition {

public:
	DataAcquisition(int t_count_chan = 16);
	~DataAcquisition();
	void InitilizeDaqCom(CComboBox &t_Devices);
	DataAcquisition &SelectDAQDevice(CComboBox &t_Devices);
	DataAcquisition &SetupDAQCollection();
	DataAcquisition &SetupDisplay(CReportCtrl &m_List);
	bool StartDAQCollection();
	void StopDAQCollection();
	void CloseDAQCollection();
	void ScanAndGatherData(std::vector<double> &ChannelValue);
	void ScanAndGatherData(std::vector<double> &t_ChannelValue,
		const std::vector<int> &t_index);
	static float get_rate() { return m_Rate; }
	int get_UpdateRate() const { return UpdateRate; }
	DataAcquisition &set_UpdateRate(int t) {
		UpdateRate = t;
		return *this;
	}
	DataAcquisition &set_bStartStop(BOOL t) {
		m_bStartStop = t;
		return *this;
	}

private:
	static float m_Rate; // Scan Rate
	int UpdateRate;      // Update Rate
	long count_chan;     // number of channels
	BOOL m_bStartStop;   // Start or Stop

	//**********DAQCom *****************//
	IDaqSystemPtr m_pSys;
	IAvailableDevicesPtr m_pSysDevs;
	IDevicePtr m_pDev;
	IDaq3xxxPtr m_Daq;
	ISetPointPtr m_SetPt;
	IDigitalIOPtr m_Stat, m_DIO;
	IAcqPtr m_pAcq;
	IConfigPtr m_pConfig;
	IScanListPtr m_pScanList;
	IAnalogInputPtr m_pAI;
	IAnalogInputsPtr m_pAIs;
};

#endif // !defined(AFX_DCWIZDLG_H__4E85C325_2795_4F37_B314_F321401DD0C0__INCLUDED_)