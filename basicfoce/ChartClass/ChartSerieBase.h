/*
 *
 *	ChartSerieBase.h
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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChartSerie.h"
#include "ChartAxis.h"
#include "ChartPointsArray.h"
#include <map>
#include "ChartString.h"
#include "PointsOrdering.h"

class CChartLegend;


//! Base class for all series of the control
/**
	This class inherits from CChartSeries and introduces the concept of 
	points through the template parameter. 

	This class is much more than a simple base class. It already store
	all the data points and provide utility functions to manipulate them.
**/
template <class T>
class CChartSerieBase : public CChartSerie
{
	friend CChartCtrl;
	friend CChartLegend;

public:
	//! Adds a single data point to the series.
	void AddPoint(const T& newPoint);
	//! Adds an array of points to the series.
	/**
		Points which were already present in the series are kept.
		@param pPoints
			Array of the new points
		@param Count
			Size of the array
	**/
	void AddPoints(T* pPoints, unsigned Count);
	//! Retrieves the point at the specified index.
	/**
		@param index
			The index of the point to retrieve
	**/
	const T& GetPoint(unsigned index) const;
	//! Sets an array of points to the series.
	/**
		Points which were already present in the series are discarded.
		@param pPoints
			Array of the new points
		@param Count
			Size of the array
	**/
	void SetPoints(T* pPoints, unsigned Count);
	//! Removes a certain amount of points from the begining of the series.
	void RemovePointsFromBegin(unsigned Count);
	//! Removes a certain amount of points from the end of the series.
	void RemovePointsFromEnd(unsigned Count);
	//! Removes all points from the series.
	void ClearSerie();

	//! Returns the number of points in the series.
	unsigned GetPointsCount() const  { return m_vPoints.GetPointsCount(); }

	//! Retrieves the minimum and maxium Y values of the series.
	/**
		@param Min
			Minimum value will be stored in this parameter
		@param Max
			Maximum value will be stored in this parameter
		@return
			false if the series doesn't contain data or is invisible
	**/
	bool GetSerieYMinMax(double& Min, double& Max)  const;
	//! Retrieves the minimum and maxium X values of the series.
	/**
		@param Min
			Minimum value will be stored in this parameter
		@param Max
			Maximum value will be stored in this parameter
		@return
			false if the series doesn't contain data or is invisible
	**/
	bool GetSerieXMinMax(double& Min, double& Max)  const;
	//! Retrieves the minimum and maxium screen X values of the series.
	/**
		@param Min
			Minimum value will be stored in this parameter
		@param Max
			Maximum value will be stored in this parameter
		@return
			false if the series doesn't contain data or is invisible
	**/
	bool GetSerieXScreenMinMax(double& Min, double& Max)  const;
	//! Retrieves the minimum and maxium screen Y values of the series.
	/**
		@param Min
			Minimum value will be stored in this parameter
		@param Max
			Maximum value will be stored in this parameter
		@return
			false if the series doesn't contain data or is invisible
	**/
	bool GetSerieYScreenMinMax(double& Min, double& Max)  const;

//////////////////////////////////////////////////////////////////////////
	//! Constructor
	CChartSerieBase(CChartCtrl* pParent);
	//! Destructor
	virtual ~CChartSerieBase();

	//! Specifies how the points should be ordered in the series.
	/**
		This specifies if the points should be ordered on their X values,
		on their Y values or not ordered (kept in order they are added to 
		the control). Ordering can improve performances a lot but makes it
		impossible to draw some specific curves (for instance, drawing an 
		ellipse is only possible if no ordering is set).
	**/
	virtual void SetSeriesOrdering(PointsOrdering newOrdering);

	//! Retrieves the screen point of a specific data point
	/**
		@param uPointIndex
			The index of the point for which to retrieve the screen point
		@return the screen point
	**/
	CPoint GetPointScreenCoord(unsigned uPointIndex);

protected:
	//! Returns the first and last visible points of the series.
	/**
		This function only works if ordering is enabled.
		@param uFirst
			The index of the first visible point is stored in this argument
		@param uLast
			The index of the last visible point is stored in this argument
		@return false if the series has no ordering or no data points.
	**/
	bool GetVisiblePoints(unsigned& uFirst, unsigned& uLast) const;

	//! The helper class containing all the data points.
	CChartPointsArray<T> m_vPoints;
	//! Index of the last point drawn
	unsigned m_uLastDrawnPoint;		
};

#include "ChartSerieBase.inl"

