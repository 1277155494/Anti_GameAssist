
// Anti_WaiGua.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAnti_WaiGuaApp: 
// �йش����ʵ�֣������ Anti_WaiGua.cpp
//

class CAnti_WaiGuaApp : public CWinApp
{
public:
	CAnti_WaiGuaApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAnti_WaiGuaApp theApp;