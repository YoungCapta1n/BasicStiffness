#include "stdafx.h"
#include "DataAcquisition.h"

float DataAcquisition::m_Rate = 100.0f;			//Scan Rate

DataAcquisition::DataAcquisition(int t_count_chan)
	:UpdateRate(100),
	count_chan(t_count_chan),
	m_bStartStop(FALSE)
{}

DataAcquisition::~DataAcquisition()
{
	// Prevent exceptions from leaving destructors

	//try
	//{
	//	m_pSys->Close();
	//	m_pSys = NULL;
	//}
	//catch (_com_error &e) {
	//	dump_daqcom_error(e);
	//}

}

void DataAcquisition::InitilizeDaqCom(CComboBox &t_Devices)
{
	dcTRY	//Start up DaqCOM

		::CoInitialize(NULL);

	//Create DaqCOM objects.
	IDaqSystemPtr	pSys(__uuidof(DaqSystem));

	m_pSys = pSys;
	m_pAcq = m_pSys->Add();
	m_pAcq->DataStore->AutoSizeBuffers = FALSE;
	m_pAcq->DataStore->BufferSizeInScans = 100000;
	m_pAcq->DataStore->IgnoreDataStoreOverruns = TRUE;

	USES_CONVERSION;
	m_pSysDevs = m_pAcq->AvailableDevices;
	m_pConfig = m_pAcq->Config;


	long numDevicesAvail = m_pAcq->AvailableDevices->Count;

	for (long i = 1; i <= numDevicesAvail; i++)
	{
		IAvailableDevicePtr pDevAvail = m_pAcq->AvailableDevices->Item[i];

		DeviceType devType = pDevAvail->GetDeviceType();
		CString szDevName = OLE2T(pDevAvail->Name);

		//m_Devices.AddString(szDevName);
		t_Devices.InsertString(i - 1, szDevName);
		t_Devices.SetItemData(i - 1, devType);
		pDevAvail = NULL;	//optional clean up of smart pointer
	}
	t_Devices.SetCurSel(0); //select the first device


	dcCATCH
		t_Devices.SetCurSel(0); //select the first device

	//End DCWizard additions in this section //////////////////////////////////////


}


DataAcquisition &DataAcquisition::SelectDAQDevice(CComboBox &t_Devices)
{
	dcTRY
		//Create the device based on the position in the list.
		int ComboIndex = t_Devices.GetCurSel();
	m_pDev = m_pSysDevs->CreateFromIndex(ComboIndex + 1);
	switch (m_pDev->GetDeviceType())
	{
	case dtPDaq3000:
	case dtPDaq3001:
	case dtPDaq3005:
		break;
	default:
		MessageBox(NULL, _T("This example is for the PDaq3K series only!"),
			_T("Select Device Error"), MB_OK);
		return *this;
	}
	m_Daq = m_pDev;
	m_pDev->Open();
	m_pAIs = m_pDev->AnalogInputs;

	//GUI management stuff.
	t_Devices.EnableWindow(false);
	return *this;
	dcCATCH
		return *this;
}

DataAcquisition &DataAcquisition::SetupDAQCollection()
{

	dcTRY
		//All the major interfaces have been made global.
		//Check out the Class view for the interface member names.

		IAvailableBaseChannelsPtr m_pAvailableChannels = m_pDev->AvailableBaseChannels;
	IBaseChannelPtr pBaseChannel;

	//Re-use analog input pointer to another direct analog input to the collection of analog inputs. 
	for (long i = 0; i != count_chan; i++)
	{

		CString ChannelName;
		ChannelName.Format(_T("%d"), i);
		ChannelName = _T("Channel ") + ChannelName;
		//refresh for next item
		pBaseChannel = m_pAvailableChannels->GetItem(1);
		//the generic interface to add channels. This avoids problems between hardware types.
		m_pAI = m_pAIs->Add(aitDirect, pBaseChannel->BaseChannel, (DeviceModulePosition)0);
		//Change name of analog input channel.
		m_pAI->Channels->GetItem(1)->Name = (_bstr_t)ChannelName;
		//Add channel to the scan list. 
		m_pAI->Channels->GetItem(1)->AddToScanList();

	}


	//////Configure some setpoints
	//m_SetPt = m_Daq->SetPoints->Add();
	//m_SetPt->InputChannel = m_pAIs->GetItem(1)->Channels->GetItem(1);
	//m_SetPt->Criteria = spcInsideLimits;
	//m_SetPt->EvaluateHighWord = FALSE;
	//m_SetPt->PutLimitB(1.2f);
	//m_SetPt->PutLimitA(2.2f);//LimitA must be greater than LimitB
	//m_SetPt->OutputChannel = spoP2PortC;
	//m_SetPt->OutputMask1 = 1;
	//m_SetPt->OutputValue1 = 1;
	//m_SetPt->OutputMask2 = 1;
	//m_SetPt->OutputValue2 = 0;


	//m_Stat = m_pDev->DigitalIOs->Add(diotRegisters, dbaVirtual, dmpPosition0);
	//m_Stat->AddToScanList();

	////can be used to monitor setpoint outputs
	//m_DIO = m_pDev->DigitalIOs->Add(diotDirectP2, dbaP2Address0, dmpPosition0);
	//m_DIO->Ports->GetItem(2)->PutConfiguration(pcInput);
	//m_DIO->Ports->GetItem(2)->AddToScanList();
	////clear setpoint output port
	//m_DIO->Ports->GetItem(3)->Write(0);


	//Set Start/Stop condition to manual.
	m_pAcq->Starts->GetItemByType((StartType)sttManual)->UseAsAcqStart();
	m_pAcq->Stops->GetItemByType((StopType)sptManual)->UseAsAcqStop();

	//if the data store is not instructed to ignore buffer over-runs then
	//the acquisition will stop when the data store reaches its internal buffer
	//limit.
	m_pAcq->DataStore->AutoSizeBuffers = FALSE;
	m_pAcq->DataStore->BufferSizeInScans = 100000;
	m_pAcq->DataStore->IgnoreDataStoreOverruns = TRUE;

	//tell data store to return the newest data
	m_pAcq->DataStore->PeekNewest = true;

	//setup some averaging
	m_Daq->PutOverSampleMultiplier(osmX32);

	//Set the rate a which to collect the above scans.
	m_pConfig->ScanRate = m_pConfig->MaxScanRate;
	m_Rate = m_pConfig->MaxScanRate;


	//Arm the acquistion
	m_pAcq->Arm();
	//Start data collection
	m_pAcq->Start();

	return *this;
	dcCATCH
		return *this;

}



bool DataAcquisition::StartDAQCollection()
{
	dcTRY
		//UINT UpdateRate = 100; //increase this number to slow down the display rate
		//Arm the acquistion
		m_pAcq->Arm();
	//Start data collection
	m_pAcq->Start();

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	return m_bStartStop = true;
	dcCATCH
		return m_bStartStop = false;

}


void DataAcquisition::StopDAQCollection()
{
	dcTRY
		m_pAcq->Disarm();
	m_bStartStop = FALSE;
	dcCATCH

}

void DataAcquisition::CloseDAQCollection()
{
	try
	{
		m_pSys->Close();
		m_pSys = NULL;
	}
	catch (_com_error &e) {
		dump_daqcom_error(e);
	}

}

DataAcquisition &DataAcquisition::SetupDisplay(CReportCtrl &m_List)
{
	dcTRY
		//Setup list and display channel names

		USES_CONVERSION;	//define unicode conversion macros (ie OLE2A, A2OLE,...)
		//used in BSTR conversions.

		//Configure scan list display.
	m_pScanList = m_pConfig->GetScanList();

	if (m_pScanList) {
		IScannablePtr pChannel;
		//get number of channels to display
		long count = m_pScanList->Count;
		//Add channels to the display list
		for (int i = 0; i < count; i++) {
			pChannel = m_pScanList->GetItem(i + 1);//one based collections!
			m_List.InsertItem(i, pChannel->Name);
		}
		pChannel = NULL; //clean up pointer
	}

	return *this;
	dcCATCH
		return *this;
}


void DataAcquisition::ScanAndGatherData(std::vector<double> &ChannelValue)
{
	//if (m_pAcq->DataStore->AvailableScans == 0) return; //no data yet
	dcTRY	//Get the data
		long ScansRet;		//holds the number of scans returned from data store

	float* rgElems;		//pointer to safe array out parameter
	CString str4Display;

	// allocate the memory for the descriptor and the array data - scans * # of channel 
	SAFEARRAY *psa = SafeArrayCreateVector(VT_R4, 0,
		m_pConfig->ScanCount * m_pConfig->ScanList->Count);

	count_chan = m_pAcq->Config->ScanList->Count;
	//request one scan
	ScansRet = m_pAcq->DataStore->PeekData(&psa, 1);
	if (psa && ScansRet) {
		//Lock it down!
		if (SUCCEEDED(SafeArrayAccessData(psa, (void**)&rgElems)))
		{
			for (int index = 0; index < count_chan; index++)
				ChannelValue.push_back((double)rgElems[index]);
			//Unlock it
			SafeArrayUnaccessData(psa);
			//Destroy it because DaqCOM will re-allocate
			SafeArrayDestroy(psa);
		}
	}

	dcCATCH

}

void DataAcquisition::ScanAndGatherData(std::vector<double>
	&t_ChannelValue, const std::vector<int> &t_index)
{

	//if (m_pAcq->DataStore->AvailableScans == 0) return; //no data yet
	dcTRY	//Get the data
		long ScansRet;		//holds the number of scans returned from data store

	float* rgElems;		//pointer to safe array out parameter
	CString str4Display;

	// allocate the memory for the descriptor and the array data - scans * # of channel 
	SAFEARRAY *psa = SafeArrayCreateVector(VT_R4, 0,
		m_pConfig->ScanCount * m_pConfig->ScanList->Count);

	count_chan = m_pAcq->Config->ScanList->Count;
	//request one scan
	ScansRet = m_pAcq->DataStore->PeekData(&psa, 1);
	if (psa && ScansRet) {
		//Lock it down!
		if (SUCCEEDED(SafeArrayAccessData(psa, (void**)&rgElems)))
		{

			t_ChannelValue.reserve(t_index.size());
			for (std::vector<int>::const_iterator it = t_index.begin();
				it != t_index.end(); ++it)
				t_ChannelValue.push_back((double)rgElems[*it]);
			//Unlock it
			SafeArrayUnaccessData(psa);
			//Destroy it because DaqCOM will re-allocate
			SafeArrayDestroy(psa);
		}
	}

	dcCATCH
}

