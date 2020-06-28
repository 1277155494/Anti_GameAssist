
// Anti_WaiGuaDlg.h : ͷ�ļ�
//

#pragma once
#include "TabSheet.h"
#include "HS_DATA_DIALOG.h"
#include "PRO_TEXT_Dialog.h"
#include "Free_Dll_Dialog.h"
#include "Anti_Debugg_Dialog.h"
#include "ToolAndOpenMore.h"

// CAnti_WaiGuaDlg �Ի���
class CAnti_WaiGuaDlg : public CDialogEx
{
// ����
public:
	CAnti_WaiGuaDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ANTI_WAIGUA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabSheet m_tab;
	HS_DATA_DIALOG HS_DATA;
	PRO_TEXT_Dialog Pro_Code;
	Free_Dll_Dialog Free_Dll;
	Anti_Debugg_Dialog AntiDebug;
	ToolAndOpenMore TAOM;
};
