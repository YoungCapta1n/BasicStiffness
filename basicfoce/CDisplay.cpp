// CDisplay.cpp: 实现文件
//

#include "stdafx.h"
#include "basicfoce.h"
#include "CDisplay.h"
#include "afxdialogex.h"


// CDisplay 对话框

IMPLEMENT_DYNAMIC(CDisplay, CDialogEx)

CDisplay::CDisplay(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIADISPLAY, pParent)
{

}

CDisplay::~CDisplay()
{
}

void CDisplay::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CHANNELVALUE, m_listchannelvalue);
	DDX_Control(pDX, IDC_PIC_STIFFNESS, m_picstiffness);
	DDX_Control(pDX, IDC_EDIT_STEP, m_lengthstep);
}


BEGIN_MESSAGE_MAP(CDisplay, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CHANNELVALUE, &CDisplay::OnClickListChannelvalue)
END_MESSAGE_MAP()


// CDisplay 消息处理程序
BOOL CDisplay::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitializePIC(m_picshowstiffness, IDC_PIC_STIFFNESS, _T("Load(kg)"));
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDisplay::OnClickListChannelvalue(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// get the click position and check
	POSITION pos = m_listchannelvalue.GetFirstSelectedItemPosition();
	selected_item = m_listchannelvalue.GetNextSelectedItem(pos);
	if (m_listchannelvalue.GetCheck(selected_item))
		m_listchannelvalue.SetCheck(selected_item, FALSE);
	else
		m_listchannelvalue.SetCheck(selected_item, TRUE);
	*pResult = 0;
}


void CDisplay::InitializeLIST(StiffnessData &t_stiffnessdata,
	CString t_current_path, CReportCtrl &_listchannelvalue) {

	// Initilize the channel list
	_listchannelvalue.DeleteAllItems();
	// m_list.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_DOUBLEBUFFER);
	_listchannelvalue.SetExtendedStyle(LVS_EX_DOUBLEBUFFER);
	// m_list.ModifyStyle(LVS_SINGLESEL, 0);
	_listchannelvalue.InsertColumn(0, _T("Channel"), LVCFMT_LEFT, 80);
	_listchannelvalue.InsertColumn(1, _T("Voltage(V)"), LVCFMT_LEFT, 90);
	_listchannelvalue.InsertColumn(2, _T("Load(kg)"), LVCFMT_LEFT, 100);
	_listchannelvalue.InsertColumn(3, _T("Min (kg)"), LVCFMT_LEFT, 90);
	_listchannelvalue.InsertColumn(4, _T("Max (kg)"), LVCFMT_LEFT, 90);
	_listchannelvalue.InsertColumn(5, _T("Avg (kg)"), LVCFMT_LEFT, 90);
	_listchannelvalue.InsertColumn(6, _T("Std (kg)"), LVCFMT_LEFT, 90);
	_listchannelvalue.InsertColumn(7, _T("Stiffness (kg/mm)"), LVCFMT_LEFT, 100);
	_listchannelvalue.InsertColumn(8, _T("Zero (kg/mm)"), LVCFMT_LEFT, 100);
	_listchannelvalue.SetGridLines(TRUE); // SHow grid lines
							   // m_list.SetCheckboxeStyle(RC_CHKBOX_NORMAL); // Enable checkboxes

	t_stiffnessdata.ReadConfigurationData(t_current_path);

	for (int it = 0; it != t_stiffnessdata.total_num_channel; ++it) {
		CString temp_channel;
		temp_channel.Format(_T("%2d"), it);
		temp_channel = _T("channel ") + temp_channel;
		_listchannelvalue.InsertItem(it, _T(""));
		_listchannelvalue.SetItemText(it, 0, temp_channel);

	}

	Addstiffness2List(t_stiffnessdata, _listchannelvalue);
}

void CDisplay::InitializePIC(ChartDisplay_c &t_ChartDisplay,
	int ID_PicControl, CString str_Channel) {

	// Initialize the Picture Control
	GetDlgItem(ID_PicControl)->GetWindowRect(t_ChartDisplay.rect);
	GetDlgItem(ID_PicControl)->ShowWindow(SW_HIDE);
	ScreenToClient(t_ChartDisplay.rect);
	t_ChartDisplay.m_tab.Create(CMFCTabCtrl::STYLE_3D_ROUNDED,
		t_ChartDisplay.rect, this, 1,
		CMFCTabCtrl::LOCATION_TOP);
	t_ChartDisplay.m_tab.AutoSizeWindow(TRUE);

	t_ChartDisplay.m_HSChartCtrl.Create(&t_ChartDisplay.m_tab,
		t_ChartDisplay.rect, 2);
	t_ChartDisplay.m_HSChartCtrl.SetBackColor(RGB(80, 32, 121));

	// X axis on the left
	t_ChartDisplay.pAxisX =
		t_ChartDisplay.m_HSChartCtrl.CreateStandardAxis(CChartCtrl::BottomAxis);
	t_ChartDisplay.pAxisX->SetAxisColor(RGB(255, 255, 255));
	t_ChartDisplay.pAxisX->SetTextColor(RGB(255, 255, 255));
	t_ChartDisplay.pAxisX->SetAutomatic(false);
	t_ChartDisplay.pAxisX->SetMinMax(-5, 5);
	t_ChartDisplay.pAxisX->SetInverted(false);
	t_ChartDisplay.pAxisX->SetVisible(true);
	// Y axis on the bottom
	t_ChartDisplay.pAxisY =
		t_ChartDisplay.m_HSChartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);
	t_ChartDisplay.pAxisY->SetAutomatic(false);
	t_ChartDisplay.pAxisY->SetMinMax(-5, 5);
	t_ChartDisplay.pAxisY->SetTextColor(RGB(255, 255, 255));
	t_ChartDisplay.pAxisY->SetAxisColor(RGB(255, 255, 255));
	t_ChartDisplay.pAxisY->SetInverted(false);
	t_ChartDisplay.pAxisY->SetVisible(true);


	// Linear Fitting
	t_ChartDisplay.m_pLineSerie = t_ChartDisplay.m_HSChartCtrl.CreateLineSerie();
	t_ChartDisplay.m_pLineSerie->SetColor(RGB(255, 255, 255));

	UpdateData(FALSE);
	t_ChartDisplay.m_tab.AddTab(&t_ChartDisplay.m_HSChartCtrl, str_Channel);
	t_ChartDisplay.m_tab.SetActiveTab(0);
	t_ChartDisplay.m_tab.ShowWindow(SW_SHOWNORMAL);
}

void CDisplay::ShowFittingResults(size_t t_num_point, CString t_channel_name,
	const std::vector<double> &t_linevector_x, const std::vector<double> &t_linevector_y,
	const std::vector<double> &t_pointvector_x, const std::vector<double> &t_pointvector_y,
	double t_display_min, double t_display_max) {

	m_picshowstiffness.m_tab.SetTabLabel(0, t_channel_name);
	ChangeNumPoints(m_picshowstiffness, t_num_point);
	InitializePointVector(m_picshowstiffness);
	Copy2ChartDisplay(m_picshowstiffness, t_linevector_x, t_linevector_y,
		t_pointvector_x, t_pointvector_y, t_display_min,
		t_display_max);
	MakeArrary(m_picshowstiffness);
	ClearArrayAndDisplay_Multi(m_picshowstiffness);
	ResetAxisRange(m_picshowstiffness);
}

void CDisplay::ChangeNumPoints(ChartDisplay_c &t_ChartDisplay,
	size_t t_num_point) {
	t_ChartDisplay.num_points = t_num_point;
}

void CDisplay::InitializePointVector(ChartDisplay_c &t_ChartDisplay) {
	// Points of Original Data
	t_ChartDisplay.v_pXYSerie.clear();
	for (size_t it = 0; it != t_ChartDisplay.num_points; ++it) {
		t_ChartDisplay.v_pXYSerie.push_back(
			t_ChartDisplay.m_HSChartCtrl.CreateLineSerie());
		t_ChartDisplay.v_pXYSerie.back()->SetColor(RGB(255, 255, 0));
	}
}

void CDisplay::ClearArrayAndDisplay_Multi(ChartDisplay_c &t_ChartDisplay) {
	// Line
	t_ChartDisplay.m_pLineSerie->ClearSerie();
	t_ChartDisplay.m_pLineSerie->AddPoints(
		&t_ChartDisplay.linedisplay_x_vector[0],
		&t_ChartDisplay.linedisplay_y_vector[0], t_ChartDisplay.m_c_arrayLength);

	// Points
	int t_index_map = 0;
	for (auto it = t_ChartDisplay.v_pXYSerie.begin();
		it != t_ChartDisplay.v_pXYSerie.end(); ++it) {
		(*it)->ClearSerie();
		(*it)->AddPoints(&t_ChartDisplay.map_vector_x[t_index_map].front(),
			&t_ChartDisplay.map_vector_y[t_index_map].front(),
			t_ChartDisplay.m_c_arrayLength);
		++t_index_map;
	}
}

void CDisplay::Copy2ChartDisplay(ChartDisplay_c &t_ChartDisplay,
	const std::vector<double> &t_linevector_x,
	const std::vector<double> &t_linevector_y,
	const std::vector<double> &t_pointvector_x,
	const std::vector<double> &t_pointvector_y,
	double t_display_min,
	double t_display_max) {
	t_ChartDisplay.linedisplay_x_vector = t_linevector_x;
	t_ChartDisplay.linedisplay_y_vector = t_linevector_y;
	t_ChartDisplay.pointdisplay_x_vector = t_pointvector_x;
	t_ChartDisplay.pointdisplay_y_vector = t_pointvector_y;
	t_ChartDisplay.display_max = t_display_max;
	t_ChartDisplay.display_min = t_display_min;
}

void CDisplay::MakeArrary(ChartDisplay_c &t_ChartDisplay) {
	double Radius =
		0.008 * (t_ChartDisplay.display_max - t_ChartDisplay.display_min);

	for (int it = 0; it != t_ChartDisplay.pointdisplay_x_vector.size(); ++it) {
		// Initialize the map
		t_ChartDisplay.map_vector_x.insert(
			std::make_pair(it, std::vector<double>{}));
		t_ChartDisplay.map_vector_y.insert(
			std::make_pair(it, std::vector<double>{}));

		// Center point
		double CenterX = t_ChartDisplay.pointdisplay_x_vector[it];
		double CenterY = t_ChartDisplay.pointdisplay_y_vector[it];

		for (size_t i = 0; i != t_ChartDisplay.m_c_arrayLength; ++i) {
			// Make a circle
			double Temp_x =
				CenterX +
				Radius * std::cos(2 * 3.1415926 * i / t_ChartDisplay.m_c_arrayLength);
			double Temp_y =
				CenterY +
				Radius * std::sin(2 * 3.1415926 * i / t_ChartDisplay.m_c_arrayLength);
			t_ChartDisplay.map_vector_x[it].push_back(Temp_x);
			t_ChartDisplay.map_vector_y[it].push_back(Temp_y);
		}
	}
}

void CDisplay::ResetAxisRange(ChartDisplay_c &t_ChartDisplay) {
	t_ChartDisplay.pAxisX->SetMinMax(t_ChartDisplay.display_min,
		t_ChartDisplay.display_max);
	t_ChartDisplay.pAxisY->SetMinMax(t_ChartDisplay.display_min,
		t_ChartDisplay.display_max);
}

void CDisplay::Addstiffness2List(const StiffnessData &t_stiffnessdata,
	CReportCtrl &t_stifnesslist) {

	for (int it = 0; it != t_stiffnessdata.total_num_channel; ++it) {
		CString temp_slope;
		CString temp_intercept;
		temp_slope.Format(_T("%2.5f"), t_stiffnessdata.stiffness_slope[it]);
		temp_intercept.Format(_T("%2.5f"),
			t_stiffnessdata.stiffness_intercept[it]);
		t_stifnesslist.SetItemText(it, 7, temp_slope);
		t_stifnesslist.SetItemText(it, 8, temp_intercept);

		// update the list color according to
		// {empty--> purple; acquisition-->red; calculation-->green}
		switch (t_stiffnessdata.stiffness_status[it]) {
		case 0:
			t_stifnesslist.SetItemBkColor(it, 0, RGB(52, 168, 83));
			break;
		case 1:
			t_stifnesslist.SetItemBkColor(it, 0, RGB(234, 67, 53));
			break;
		case 2:
			t_stifnesslist.SetItemBkColor(it, 0, RGB(212, 153, 211));
			break;
		default:
			break;
		}
	}
}

void UpdateListusingRawData(CDisplay &t_cdisplay,
	const CalibrationData &t_cablirationdata)
{
	// Update Listcontrol
	CString str4Display;
	CString str4Display_Force;
	computerealtimeLoad(t_cablirationdata, realtimeVoltage, realtimeLoad);
	std::vector<double>::const_iterator it_begin = realtimeVoltage.begin();
	for (std::vector<double>::const_iterator it = it_begin; it != realtimeVoltage.end();
		++it) {
		int index = it - it_begin;
		str4Display.Format(_T("%2.4f"), *it);
		str4Display_Force.Format(_T("%2.4f"), realtimeLoad[index]);
		int t_index = t_cablirationdata.v_selected_channel[index];
		t_cdisplay.m_listchannelvalue.SetItemText(t_index, 1, str4Display);
		t_cdisplay.m_listchannelvalue.SetItemText(t_index, 2, str4Display_Force);
	}

}


void computerealtimeLoad(const CalibrationData &t_cablirationdata,
	const std::vector<double> &t_rawdata, std::vector<double> &_loaddata) {
	std::vector<double>::const_iterator it_begin = t_rawdata.begin();
	for (std::vector<double>::const_iterator it = it_begin; it != t_rawdata.end();
		++it) {
		int index = it - it_begin;
		int t_index = t_cablirationdata.v_selected_channel[index];
		_loaddata[index] =
			t_rawdata[index] * t_cablirationdata.calibration_slope[t_index] +
			t_cablirationdata.calibration_intercept[t_index];
	}
}

double showstatisticsdata(CDisplay &t_cdisplay,
	StiffnessData &t_stiffnessdata, database &_database)
{
	// Update Listcontrol
	CString str4Display_min;
	CString str4Display_max;
	CString str4Display_avg;
	CString str4Display_std;
	std::vector<int> selected_channel = _database.getselected_channel();
	size_t length = _database.getnumselectedchannel();
	t_stiffnessdata.computestatistics(_database);

	std::vector<double>::const_iterator it_begin = realtimeVoltage.begin();
	double selected_item_average = 0.0;
	for (size_t it = 0; it != length; ++it) {
		str4Display_min.Format(_T("%2.4f"), t_stiffnessdata.min_onetable[it]);
		str4Display_max.Format(_T("%2.4f"), t_stiffnessdata.max_onetable[it]);
		str4Display_avg.Format(_T("%2.4f"), t_stiffnessdata.avg_onetable[it]);
		str4Display_std.Format(_T("%2.4f"), t_stiffnessdata.std_onetable[it]);
		int t_index = selected_channel[it];
		if (t_index == t_cdisplay.selected_item)
			selected_item_average = t_stiffnessdata.avg_onetable[it];
		t_cdisplay.m_listchannelvalue.SetItemText(t_index, 3, str4Display_min);
		t_cdisplay.m_listchannelvalue.SetItemText(t_index, 4, str4Display_max);
		t_cdisplay.m_listchannelvalue.SetItemText(t_index, 5, str4Display_avg);
		t_cdisplay.m_listchannelvalue.SetItemText(t_index, 6, str4Display_std);
	}
	return selected_item_average;
}

void call_stiffness_acquisition(CDisplay &t_cdisplay, StiffnessData &t_stiffnessdata,
	database &_database, CString t_input_value, int deformation_index)
{
	double t_average = showstatisticsdata(t_cdisplay, t_stiffnessdata, _database);
	t_stiffnessdata.Add2Map(t_cdisplay.selected_item, 
		deformation_index*atof(t_input_value), t_average, deformation_index);

	t_cdisplay.Addstiffness2List(t_stiffnessdata, t_cdisplay.m_listchannelvalue);
}

void call_stiffness_calculate(CDisplay &t_cdisplay, StiffnessData &t_stiffnessdata,
	CString t_current_path) {

	int t_index_of_channel = t_cdisplay.selected_item;
	t_stiffnessdata.LLSCalculate(t_index_of_channel);

	CString Filename = t_current_path + _T("\\stiffness.txt");
	t_stiffnessdata.WriteConfigurationData(Filename);

	call_show_stiffness(t_cdisplay, t_stiffnessdata);

	t_cdisplay.Addstiffness2List(t_stiffnessdata, t_cdisplay.m_listchannelvalue);

}

void call_show_stiffness(CDisplay &t_cdisplay, StiffnessData &t_stiffnessdata) {

	int t_index_of_channel = t_cdisplay.selected_item;
	std::vector<double> linevector_x;
	std::vector<double> linevector_y;
	std::vector<double> pointvector_x;
	std::vector<double> pointvector_y;
	double display_min;
	double display_max;
	int t_num_points;
	t_stiffnessdata.Convert4Display(t_index_of_channel, t_num_points, linevector_x,
		linevector_y, pointvector_x, pointvector_y,
		display_min, display_max);

	CString t_channel_name;
	t_channel_name.Format(_T("%d"), t_index_of_channel);
	t_channel_name = _T("Load(kg): channel ") + t_channel_name;
	t_cdisplay.ShowFittingResults(t_num_points, t_channel_name, linevector_x,
		linevector_y, pointvector_x, pointvector_y, display_min, display_max);

}

void call_stiffness_clear(CalibrationDiag &t_tabcali, StiffnessData &t_calidata,
	CString t_current_path) {


}

