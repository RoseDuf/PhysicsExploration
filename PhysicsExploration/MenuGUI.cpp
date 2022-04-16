/*
Adding MFC windows module to existing console application
reference : https://www.codeproject.com/Tips/5296682/A-Tip-to-Get-MFC-Support-for-Console-Application-i
*/
#include<afxwin.h>
#include "MenuGUI.h"

int MenuGUI::_loadMenu()
{
	::MessageBox(NULL, _T("There would Simulation List here!"), _T("Physics Simulations"), MB_OK);
	return 0;
}
