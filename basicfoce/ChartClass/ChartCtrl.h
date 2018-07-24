/*
 *
 *	ChartCtrl.h
 *
 *	Written by Cédric Moonen (cedric_moonen@hotmail.com)
 *
 *
 *
 *	This code may be used for any non-commercial and commercial purposes in a compiled form.
 *	The code may be redistributed as long as it remains unmodified and providing that the 
 *	author name and this disclaimer remain intact. The sources can be modified WITH the author 
 *	consent only.
 *	
 *	This code is provided without any garanties. I cannot be held responsible for the damage or
 *	the loss of time it causes. Use it at your own risks
 *
 *	An e-mail to notify me that you are using this code is appreciated also.
 *
 *
 */


#pragma once

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ChartSerie.h"
#include "ChartAxis.h"
#include "ChartGrid.h"
#include "ChartStandardAxis.h"
#include <map>


class CChartStandardAxis;
class CChartLineSerie;

/////////////////////////////////////////////////////////////////////////////
// CChartCtrl window

//! The main chart control class.
/**

**/
class CChartCtrl : public CWnd
{

public:
	//! Retrieves de device context.
	/**
		This function is used for internal purposes only.
	**/
	CDC* GetDC();
	//! Retrieves the plotting rectangle.
	CRect GetPlottingRect()  const { return m_PlottingRect; }

	//! An enumeration of the different axis positions.
	enum EAxisPos
	{
		LeftAxis = 0,
		BottomAxis,
		RightAxis,
		TopAxis
	};

	//! Create and attach a standard axis to the control.
	/**
		@param axisPos
			The position of the axis. 
		@return The created standard axis.
	**/
	CChartStandardAxis* CreateStandardAxis(EAxisPos axisPos);
	
	void AttachCustomAxis(CChartAxis* pAxis, EAxisPos axisPos);


	CChartLineSerie* CreateLineSerie(bool bSecondaryHorizAxis=false, bool bSecondaryVertAxis=false);



	/**
		@param uSerieId
			The Id of the series to be removed.
	**/
	void RemoveSerie(unsigned uSerieId);
	//! Removes all the series from the chart
	void RemoveAllSeries();
	//! Returns the number of series in the chart
	size_t GetSeriesCount() const;

	CChartAxis* GetBottomAxis() const;
	CChartAxis* GetLeftAxis() const;
	CChartAxis* GetTopAxis() const;
	CChartAxis* GetRightAxis() const;
	//! Returns a specific axis attached to the control
	/**
		If the specified axis does not exist, NULL is returned.
		@param axisPos
			The axis position (left, bottom, right or top).
	**/
	CChartAxis* GetAxis(EAxisPos axisPos) const
	{
		return m_pAxes[axisPos];
	}

	//! Returns the type of the edge used as border.
	UINT GetEdgeType() const        { return EdgeType;    }
	//! Sets the edge type.
	/**
		@param NewEdge 
			The type of the edge. See the DrawEdge function in MSDN for 
			a list of the different types.
	**/
	void SetEdgeType(UINT NewEdge)  
	{ 
		EdgeType = NewEdge; 
		RefreshCtrl();
	}

	//! Returns the background color
	COLORREF GetBackColor() const			{ return m_BackColor;   }
	//! Sets the background color.
	void SetBackColor(COLORREF NewCol)		
	{ 
		m_BackColor = NewCol;  
		RefreshCtrl();
	}
	//! Returns the color of the plotting area's border 
	COLORREF GetBorderColor() const			{ return m_BorderColor;   }
	//! Sets the color of the plotting area's border 
	void SetBorderColor(COLORREF NewCol)	{ m_BorderColor = NewCol;	RefreshCtrl(); }
	//! Enables/disables the pan feature
	void SetPanEnabled(bool bEnabled)  { m_bPanEnabled = bEnabled;  }
	//! Returns true if the pan feature is enabled
	bool GetPanEnabled() const		   { return m_bPanEnabled;	    }
	//! Enables/disables the zoom feature
	//! Forces a refresh of the control.
	/**
		This function is used for internal purposes.
	**/
	void RefreshCtrl();
	//! Enables/disables the refresh of the control
	/**
		This function is used when several settings have to be changed at the same 
		time on the control. This way we can avoid refreshing the control when it 
		is not needed.
		@param bEnable
			false to disable the refresh and true to re-enable the refresh.
	**/
	void AttachCustomSerie(CChartSerie* pNewSeries, bool bSecondaryHorizAxis = false, bool bSecondaryVertAxis = false);
	void EnableRefresh(bool bEnable);
	//! Creates the control dynamically.
	/**
		@param pParentWnd 
			Parent window of the control
		@param rect 
			Position of the control
		@param nID
			ID of the control
		@param dwStyle
			Style of the control
	**/
	int Create(CWnd* pParentWnd, const RECT& rect, UINT nID, DWORD dwStyle=WS_VISIBLE);
	
	//! Helper function to convert a date to a double value
	static double DateToValue(const COleDateTime& Date);
	//! Helper function to convert a double value to a date
	static COleDateTime ValueToDate(double Value);

	//! Print the chart
	/**
		@param strTitle
			The title of the print document.
		@param pPrntDialog
			A pointer to a CPrintDialog. 
			If NULL is passed, the default print dialog will be displayed.
	**/
    virtual void Print(const TChartString& strTitle, CPrintDialog* pPrntDialog = NULL);

	//! Default constructor
    CChartCtrl();	
	//! Default destructor
    virtual ~CChartCtrl();

	//! Tell the control to set the current series to the first series.
	/**
		This function is used with the GetNextSerie to iterate over all 
		series in the control.
	**/
	void GoToFirstSerie();
	//! Returns the next series in the control.
	/**
		This function is used with the GoToFirstSerie to iterate over all 
		series in the control. First call GoToFirstSerie and then call this 
		function until it returns NULL to iterate over all series.
		Warning: calling this function without calling GoToFirstSerie before
		might lead to unpredicted results. The same if you add or remove
		series between the call to GetFirstSerie and the call to GetNextSerie.
		@return the next series or NULL if we already are at the last series.
	**/
	CChartSerie* GetNextSerie();

	//! Refreshes all the axes which are automatic for the screen.
	void RefreshScreenAutoAxes();

	// Generated message map functions
protected:
	//{{AFX_MSG(CChartCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
    virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnPrint(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);

	// This function can be called to draw the chart
	// on the screen or for printing.
	virtual void DrawChart(CDC* pDC, CRect ChartRect);
	virtual void DrawBackground(CDC* pDC, CRect ChartRect);

private:
	//! Register the window class used for this control
	bool RegisterWindowClass();


	//! Specifies if the refresh is currently enabled or not.
	int m_iEnableRefresh ;
	//! Specifies if there is a pending refresh. 
	/**
		If true, once EnableRefresh(true) is called, a refresh of the control
		will be	done.
	**/
	bool m_bPendingRefresh;
	//! Memory bitmap on which the chart background is drawn (axis, grid, title, ...)
	CDC m_BackgroundDC;	
	//! Specifies if the memory bitmap has already been created.
	bool m_bMemDCCreated;

	//! The background color (if no gradient used)
	COLORREF m_BackColor;	
	//! The border color
	COLORREF m_BorderColor;
	//! The type of edge
	UINT EdgeType;		

	//! Zone in wich the series will be plotted
	CRect m_PlottingRect;	

	typedef std::map<unsigned, CChartSerie*> TSeriesMap;
	//! Map containing all the series added to the chart.
	TSeriesMap m_mapSeries;	
	//! The four axis of the control.
	CChartAxis* m_pAxes[4];

	//! Specifies if the mouse panning is enabled
	bool m_bPanEnabled;
	//! Specifies if the right mouse button is currently pressed
	bool m_bRMouseDown;		
	//! The point on which the panning started
	CPoint m_PanAnchor;

	//! Specifies if the left mouse button is currently pressed
	bool  m_bLMouseDown;
	//! Specifies if the toolbars have already been created
	bool m_bToolBarCreated;

	//! The font used for printing
    CFont  m_PrinterFont;  
	//! Page size in chartctrl units.
    CSize m_LogicalPageSize;     
	//! Page size in device units.
    CSize m_PaperSize;    

	typedef TSeriesMap::const_iterator TSeriesMapIter;
	//! The iterator of the current series
	TSeriesMapIter m_currentSeries;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

