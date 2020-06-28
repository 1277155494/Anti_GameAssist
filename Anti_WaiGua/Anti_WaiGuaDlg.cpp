
// Anti_WaiGuaDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Anti_WaiGua.h"
#include "Anti_WaiGuaDlg.h"
#include "afxdialogex.h"
#include "HS_DATA_DIALOG.h"
#include "PRO_TEXT_Dialog.h"
#include <Windows.h>
#include<sstream>
#include<iostream>
#include<string>
#include<cstring>
#include<stack>
#include<cstdlib>
#include<cstdio>
#include <io.h>  
#include <fcntl.h> 
#include "Free_Dll_Dialog.h"
#include "Anti_Debugg_Dialog.h"
#include "ToolAndOpenMore.h"
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAnti_WaiGuaDlg �Ի���



CAnti_WaiGuaDlg::CAnti_WaiGuaDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAnti_WaiGuaDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAnti_WaiGuaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}

BEGIN_MESSAGE_MAP(CAnti_WaiGuaDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CAnti_WaiGuaDlg ��Ϣ�������

BOOL CAnti_WaiGuaDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//��ӿؼ�̨��Դ����ʽʹ��
	/*
	::AllocConsole();
	FILE *fp;
	freopen_s(&fp, "CONOUT$", "w+t", stdout);//����д����������VS2013�汾�Ĵ��룬��VS��Ϊ���ڵİ汾��
	*/

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������ 
	m_tab.AddPage(TEXT("call�����ݱ���"), &HS_DATA, IDD_DIALOG_HS_DATA);  
	m_tab.AddPage(TEXT("������"), &Pro_Code, IDD_DIALOG_Pro_Code); 
	m_tab.AddPage(TEXT("HOOK���"), &Free_Dll, IDD_DIALOG_FreeDll);
	m_tab.AddPage(TEXT("��ʽ���߼��Ͷ࿪���"), &TAOM, IDD_DIALOG_ToolAndOpenMore);
	m_tab.AddPage(TEXT("��ӷ�����"), &AntiDebug, IDD_DIALOG_AntiDebugging);

	m_tab.Show();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CAnti_WaiGuaDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAnti_WaiGuaDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAnti_WaiGuaDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

