//	DCHelpers.cpp
#include "stdafx.h"
#include "DCHelpers.h"


//	This function removes the COM error and displays it in a Mesage Box.
void dump_daqcom_error(_com_error &e)
{
	USES_CONVERSION;

	CComBSTR bstrDescription;
	CString szSysErr, szComErr, szMsg;

   // load the application specific system error string
	szSysErr = _T("DaqCOM System Error");

	// check if an Automation IErrorInfo object exists in the current thread
	IErrorInfo* pei = NULL;
	HRESULT hr = GetErrorInfo(0, &pei);

	// try to find system-provided IErrorInfo message
	if(pei && hr==S_OK) 
	{
		pei->GetDescription(&bstrDescription.m_str);
		pei->Release();

		if(bstrDescription.Length() > 0)
      {
         // use the system error message
         szComErr = OLE2T(bstrDescription);
		}
		else 	//if no error in description available in system, look in the _com_error object
		{
			// first try the description string in _com_error
         bstrDescription.m_str = e.Description();

			if(bstrDescription.Length() > 0)
         {
            szComErr = OLE2T(bstrDescription);
         }
         else
			{
				// if no Description exists, use ErrorMessage
            szComErr = e.ErrorMessage();
			}
		}
	}
	else 	//if no error in description available in system, look in the _com_error object
	{
		// first try the description string in _com_error
      bstrDescription.m_str = e.Description();

		if(bstrDescription.Length() > 0)
      {
         szComErr = OLE2T(bstrDescription);
      }
      else
		{
			// if no Description exists, use ErrorMessage
         szComErr = e.ErrorMessage();
		}
	}

	szMsg.Format(_T("Error %d: %s"), e.WCode(), szComErr);
   MessageBox(NULL, szMsg, szSysErr, MB_OK|MB_ICONSTOP);

	SetLastError(0);
	return;
}

bool DoEvents()
{
	MSG msg;
	if (!::GetMessage(&msg, NULL, NULL, NULL))
	{
		return FALSE;
	}

	// process this message
	
	if (msg.message != WM_KICKIDLE )
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return TRUE;
}
