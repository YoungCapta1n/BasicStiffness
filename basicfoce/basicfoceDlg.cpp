
// basicfoceDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "basicfoce.h"
#include "basicfoceDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDT_TIMER 200

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CbasicfoceDlg 对话框

CbasicfoceDlg::CbasicfoceDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BASICFOCE_DIALOG, pParent), total_num_channel(16), 
	_data_acquisition(total_num_channel), calibration_data(total_num_channel), 
	stiffness_data(total_num_channel),
	time_inteval(10), deformation_index(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CbasicfoceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBODEVICE, m_Devices);
	DDX_Control(pDX, IDC_BUT_SELECTDEVICE, m_selectdevice);
	DDX_Control(pDX, IDC_EDIT_COLLECTIONRATE, m_collectrate);
	DDX_Control(pDX, IDC_BUT_SETUPANDSTART, m_startcollection);
	DDX_Control(pDX, IDC_BUT_PAUSE, m_puase);
	DDX_Control(pDX, IDC_BUT_STOPCOLLECTION, m_stopcollection);
	DDX_Control(pDX, IDC_LIST_LOGINFO, m_outputinformation);
	DDX_Control(pDX, IDC_MAINTAB, m_maintab);
	DDX_Control(pDX, IDC_BUT_COLLECTSHORT, m_collectshort);
	DDX_Control(pDX, IDC_BUT_CALCULATE, m_calculate);
	DDX_Control(pDX, IDC_BUT_CLEARCALI, m_clearcalibration);
	DDX_Control(pDX, IDC_BUT_FINISHCALI, m_finishcalibration);
	DDX_Control(pDX, IDC_EDIT_PROJECTNAME, m_projectname);
	DDX_Control(pDX, IDC_BUT_CLEARSTIFFNESS, m_clearstiffness);
	DDX_Control(pDX, IDC_BUT_COMPUTESTIFFNESS, m_computestiffness);
}

BEGIN_MESSAGE_MAP(CbasicfoceDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_MAINTAB, &CbasicfoceDlg::OnTcnSelchangeMaintab)
	ON_BN_CLICKED(IDC_BUT_SELECTDEVICE, &CbasicfoceDlg::OnBnClickedButSelectdevice)
	ON_BN_CLICKED(IDC_BUT_SETUPANDSTART, &CbasicfoceDlg::OnBnClickedButSetupandstart)
	ON_BN_CLICKED(IDC_BUT_PAUSE, &CbasicfoceDlg::OnBnClickedButPause)
	ON_BN_CLICKED(IDC_BUT_STOPCOLLECTION, &CbasicfoceDlg::OnBnClickedButStopcollection)
	ON_BN_CLICKED(IDC_BUT_COLLECTSHORT, &CbasicfoceDlg::OnBnClickedButCollectshort)
	ON_BN_CLICKED(IDC_BUT_CALCULATE, &CbasicfoceDlg::OnBnClickedButCalculate)
	ON_BN_CLICKED(IDC_BUT_CLEARCALI, &CbasicfoceDlg::OnBnClickedButClearcali)
	ON_BN_CLICKED(IDC_BUT_FINISHCALI, &CbasicfoceDlg::OnBnClickedButFinishcali)
	ON_BN_CLICKED(IDC_BUT_COMPUTESTIFFNESS, &CbasicfoceDlg::OnBnClickedButComputestiffness)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CbasicfoceDlg 消息处理程序

BOOL CbasicfoceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// Tab control
	m_maintab.InsertItem(0, _T("Calibration"));
	m_maintab.InsertItem(1, _T("Real-time"));
	m_tabcalibration.Create(IDD_DIA_CALIBRATION, GetDlgItem(IDC_MAINTAB));
	m_tabDisplay.Create(IDD_DIADISPLAY, GetDlgItem(IDC_MAINTAB));
	m_maintab.SetCurSel(0);

	// Current path
	GetCurrentEXEPath();

	// Initialize the calibration list
	((CalibrationDiag *)GetParent())
		->InitializeReportlist(calibration_data, current_path,
			m_tabcalibration.m_calibrationlist);
	((CDisplay *)GetParent())
		->InitializeLIST(stiffness_data, current_path,
			m_tabDisplay.m_listchannelvalue);
	// Data Acquisition
	_data_acquisition.InitilizeDaqCom(m_Devices);


	// Gui
	CString TempRate;
	TempRate.Format(_T("%d"), _data_acquisition.get_UpdateRate());
	m_collectrate.SetWindowText(TempRate);
	m_projectname.SetWindowText(_T("LoadTest"));
	ListboxMessage(_T("Please specify the Project Name and Collection Frequency!"));
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CbasicfoceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CbasicfoceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);



	}
	else
	{
		CRect rs;
		m_maintab.GetClientRect(&rs);
		//调整Tab大小
		rs.top += 21;
		rs.bottom -= 1;
		rs.left += 1;
		rs.right -= 1;

		//移动并显示
		m_tabcalibration.MoveWindow(&rs);
		m_tabDisplay.MoveWindow(&rs);

		m_tabcalibration.ShowWindow(true);
		m_tabDisplay.ShowWindow(false);

		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CbasicfoceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CbasicfoceDlg::OnTcnSelchangeMaintab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int CurSel = m_maintab.GetCurSel();
	m_tabcalibration.ShowWindow(CurSel == 0);
	m_tabDisplay.ShowWindow(CurSel == 1);

	*pResult = 0;

}


void CbasicfoceDlg::OnBnClickedButSelectdevice()
{
	// Select the device for data acquisition
	//_data_acquisition.SelectDAQDevice(m_Devices)
	//	.SetupDAQCollection()
	//	.SetupDisplay(m_tabDisplay.m_listchannelvalue);
	_data_acquisition.SelectDAQDevice(m_Devices)
		.SetupDAQCollection();
	_data_acquisition.set_UpdateRate(ConvertEditToDouble(m_collectrate));
	CString Str_ScanFreq;
	CString Str_SampleFreq;
	Str_ScanFreq.Format(_T("The Scan Freq (Hz):  %.1f\n"),
		_data_acquisition.get_rate());
	Str_SampleFreq.Format(_T("The Sample Freq (Hz):  %d\n"),
		_data_acquisition.get_UpdateRate());
	ListboxMessage(Str_ScanFreq);
	ListboxMessage(Str_SampleFreq);
	m_collectrate.EnableWindow(FALSE);

	_data_acquisition.StopDAQCollection();
	m_projectname.GetWindowText(ProjectName);
	// Button operation
	m_selectdevice.EnableWindow(false);
	m_projectname.EnableWindow(false);
	m_collectshort.EnableWindow(true);
	m_maintab.EnableWindow(true);
	m_finishcalibration.EnableWindow(true);
	m_clearcalibration.EnableWindow(true);
	// output messsage
	ListboxMessage(_T("Acquisition Device is selected, Ready for calibration and collection!"));
	
}


void CbasicfoceDlg::OnBnClickedButSetupandstart()
{
	// UINT UpdateRate = 100; increase this number to slow down the display rate

	_data_acquisition.StartDAQCollection();
	_database.createonetable(selected_channel);
		// On every timer hit call OnTimer()
	SetTimer(IDT_TIMER, _data_acquisition.get_UpdateRate(), NULL);

	// GUI Management.
	m_startcollection.EnableWindow(false);
	m_stopcollection.EnableWindow(true);
	m_puase.EnableWindow(true);
	m_tabDisplay.ShowWindow(true);
	m_tabcalibration.ShowWindow(false);
	m_tabDisplay.m_lengthstep.EnableWindow(false);

	// output messsage
	ListboxMessage(_T("Real-time collection started!"));

}


void CbasicfoceDlg::OnBnClickedButPause()
{
	KillTimer(IDT_TIMER);
	_data_acquisition.StopDAQCollection();

	// get the step length
	CString c_input_step;
	m_tabDisplay.m_lengthstep.GetWindowText(c_input_step);

	call_stiffness_acquisition(m_tabDisplay, stiffness_data, _database, c_input_step, deformation_index);

	++deformation_index;  // increase the count for deformation index
	// winsocket_server.ShutDownSocket();
	m_startcollection.EnableWindow(true);
	m_stopcollection.EnableWindow(false);
	m_puase.EnableWindow(false);

	// output messsage
	ListboxMessage(_T("Real-time collection suspended!"));
}


void CbasicfoceDlg::OnBnClickedButStopcollection()
{
	KillTimer(IDT_TIMER);
	_data_acquisition.StopDAQCollection();

	// winsocket_server.ShutDownSocket();
	m_startcollection.EnableWindow(true);
	m_stopcollection.EnableWindow(false);
	m_puase.EnableWindow(false);

	// output messsage
	ListboxMessage(_T("Real-time collection stop!"));
}


void CbasicfoceDlg::OnBnClickedButCollectshort()
{
	CString c_input_value;
	m_tabcalibration.m_inputvalue.GetWindowText(c_input_value);
	double t_average = 0.0;
	std::thread tt_thread(&CbasicfoceDlg::IndependentThread4Calibration,
		this, m_tabcalibration.selected_item,
		std::ref(t_average));
	tt_thread.join();

	call_calibration_acquisition(m_tabcalibration, calibration_data,
		c_input_value, t_average);

	// GUI Management.
	m_calculate.EnableWindow(true);

	// output messsage
	ListboxMessage(_T("[Calibration] one point is collected!"));
}


void CbasicfoceDlg::OnBnClickedButCalculate()
{
	call_calibration_calculate(m_tabcalibration, calibration_data, current_path);
	// output messsage
	ListboxMessage(_T("[Calibration] one channel is calibrated!"));
}


void CbasicfoceDlg::OnBnClickedButClearcali()
{
	call_calibration_clear(m_tabcalibration, calibration_data, current_path);
	// output messsage
	ListboxMessage(_T("[Calibration] one channel is clear!"));
}


void CbasicfoceDlg::OnBnClickedButFinishcali()
{
	m_tabcalibration.RecordMap(calibration_data, selected_channel);

	// save configuration to file
	CString Filename = current_path + _T("\\calibration.txt");
	calibration_data.WriteConfigurationData(Filename);
	// Collect and Scan once
	_data_acquisition.StartDAQCollection();
	realtimeVoltage.resize(selected_channel.size());
	realtimeLoad.resize(selected_channel.size());
	_data_acquisition.ScanAndGatherData(realtimeVoltage, selected_channel);

	//
	_database.opendatabase(current_path, ProjectName);
	

	_data_acquisition.StopDAQCollection();


	// Button operation
	m_collectshort.EnableWindow(false);
	m_finishcalibration.EnableWindow(false);
	m_clearcalibration.EnableWindow(false);
	m_calculate.EnableWindow(false);
	m_startcollection.EnableWindow(true);
	m_puase.EnableWindow(true);
	m_stopcollection.EnableWindow(true);
	m_computestiffness.EnableWindow(true);
	m_tabcalibration.ShowWindow(false);
	m_tabDisplay.ShowWindow(true);

	// output messsage
	ListboxMessage(_T("Calibration is finished, ready for real-time collection!"));
}

BOOL CbasicfoceDlg::DestroyWindow()
{
	_data_acquisition.CloseDAQCollection();
	return CDialogEx::DestroyWindow();
}

void CbasicfoceDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (!IDT_TIMER)
		return; // Not us...
	Func4onTimer();

	CDialogEx::OnTimer(nIDEvent);
}

inline void CbasicfoceDlg::Func4onTimer() {
	// Collect data
	_data_acquisition.ScanAndGatherData(realtimeVoltage, selected_channel);
	// Update Listcontrol and compute real-time load
	UpdateListusingRawData(m_tabDisplay, calibration_data);
	// save to database
	_database.updateonetable();
	// Socket Communication
	// to call the socket 1/10 of the frequency of data acquisition
	/*++t_socket_count;
	if (t_socket_count == time_inteval) {
		SendMessage2PiTest(smart_winsocket_server.get(), socket_message, Matsaving);
		// SendMessage2Pi(winsocket_server, socket_message, Matsaving);
		t_socket_count = 0;
	}
	*/



}



void CbasicfoceDlg::GetCurrentEXEPath() {
	CString path;
	GetModuleFileName(NULL, path.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	path.ReleaseBuffer();
	int pos = path.ReverseFind('\\');
	current_path = path.Left(pos);
}

int CbasicfoceDlg::ConvertEditToDouble(CEdit &m_edit) {
	CString Temp;
	m_edit.GetWindowText(Temp);
	return _ttoi(Temp);
}

void CbasicfoceDlg::ListboxMessage(CString str) {
	int index = m_outputinformation.GetCount();
	m_outputinformation.InsertString(index, str);
}

void CbasicfoceDlg::IndependentThread4Calibration(int t_index_channel,
	double &t_average) {
	t_average = 0;
	_data_acquisition.StartDAQCollection();
	for (unsigned it = 0; it != calibration_data.sample_num; ++it) {
		std::vector<double> channelvalue;
		_data_acquisition.ScanAndGatherData(channelvalue);
		t_average += channelvalue[t_index_channel];
		// std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	_data_acquisition.StopDAQCollection();
	t_average /= calibration_data.sample_num;
}






void CbasicfoceDlg::OnBnClickedButComputestiffness()
{
	deformation_index = 0;  // reset the count for deformation
	call_stiffness_calculate(m_tabDisplay, stiffness_data, current_path);
	// output messsage
	ListboxMessage(_T("[Stiffness] Stiffness of one channel is computed!"));
}
