// ToolAndOpenMore.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "Anti_WaiGua.h"
#include "ToolAndOpenMore.h"
#include "afxdialogex.h"
#include "Free_Dll_Dialog.h"
#include "PRO_TEXT_Dialog.h"
#include <TlHelp32.h>
#include "HS_DATA_DIALOG.h"
#include<cstdio>
#include<windows.h>
#include <list>
#include <vector>
#include <windows.h>
#include <iostream>
using namespace std;
// ToolAndOpenMore �Ի���
BYTE g_chINT32 = 0xCC;
BYTE g_Orignal;
LPVOID g_pCreateMutex = NULL;
CREATE_PROCESS_DEBUG_INFO g_cpdi2;
int NameNum=0;
string Name[SZLEN];
int IsOpenMore;

typedef struct AllWindowsTitle
{
	DWORD pid;
	vector<HWND> *HWNDPid;
}EnumHWndsArg, *LPEnumHWndsArg;
//��������
void OnStartDebug(LPDEBUG_EVENT pde);
DWORD OnDealException(LPDEBUG_EVENT pde);
DWORD WINAPI DebugLoop2(LPVOID pid);
BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam);
IMPLEMENT_DYNAMIC(ToolAndOpenMore, CDialogEx)
ToolAndOpenMore::ToolAndOpenMore(CWnd* pParent /*=NULL*/)
	: CDialogEx(ToolAndOpenMore::IDD, pParent)
{

}

BOOL ToolAndOpenMore::OnInitDialog()
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
	FreeDll.AddProcessToList(mProList);
	//ֻ�ܴ�exe�ļ�
	mExeFilePath.EnableFileBrowseButton(NULL, _T("Exe Files (*.exe)|*.exe|All Files (*.*)|*.*||"));
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}
ToolAndOpenMore::~ToolAndOpenMore()
{

}

void ToolAndOpenMore::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, mExeFilePath);
	DDX_Control(pDX, IDC_LIST1, mProList);
}


BEGIN_MESSAGE_MAP(ToolAndOpenMore, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON6, &ToolAndOpenMore::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON1, &ToolAndOpenMore::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &ToolAndOpenMore::OnBnClickedButton2)
END_MESSAGE_MAP()
BOOL ToolAndOpenMore::TraverseAllPro(DWORD pid)
{
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	//���̿��� ������ȡ��������
	HANDLE hProSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (hProSnap == INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}
	//ѭ������
	for (Process32First(hProSnap, &pe); Process32Next(hProSnap, &pe);)
	{
		if (pe.th32ProcessID == pid)
			continue;
		//������߹���
		if (FindOpenMoreByCode(pe.th32ProcessID, 1) == 2)
			continue;
		else if (FindOpenMoreByCode(pe.th32ProcessID, 1)==0)
		{
			return FALSE;
		}
	}
	//�ص����
	CloseHandle(hProSnap);
	return TRUE;
}
//ʹ���������ҵ�Duokai
DWORD ToolAndOpenMore::FindOpenMoreByCode(DWORD pid,int n)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	//��ȡ���̻���
	//�ѽ����ڴ���ص��ַ�������
	unsigned char ProMem[MaxLenSz];
	FreeDll.GetProMemToChar(pid, ProMem);
	if (ProMem[0]==0xFF)
	{
		return 2;
	}
	//��¼code�εĲ��ִ���
	//PE�ļ�ͷ
	pDosHeader = (PIMAGE_DOS_HEADER)ProMem;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew+0x4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	DWORD EP = pOptionHeader->AddressOfEntryPoint;
	//�����
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader+pPEHeader->SizeOfOptionalHeader);
	//�洢������
	int i = 0;
	int j = EP;
	for (; i < MaxLenSz && i < pSectionHeader->SizeOfRawData && n == 0; i++, j++)
	{
		TextCode[i] = *((PBYTE)ProMem + j);
	}
	//�����������̷����Ƿ�����ͬ������
	i = 0;
    j = EP;
	for (; i < MaxLenSz && i < pSectionHeader->SizeOfRawData && n; i++, j++)
	{
		if (TextCode[i] != *((PBYTE)ProMem + j))
		{
			return 1;
		}
	}
	return 0;
}
//ʹ�������ҵ�Duokai
BOOL ToolAndOpenMore::FindOpenMoreByName(DWORD pid)
{
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	HANDLE hPro = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	string Name;
	//��ȡpid��Ӧ�Ľ�����
	for (Process32First(hPro, &pe); Process32Next(hPro, &pe);)
	{
		if (pe.th32ProcessID == pid)
		{
			Name = pe.szExeFile;
			break;
		}
	}
	//�Ƚ�����������
	for (Process32First(hPro, &pe); Process32Next(hPro, &pe);)
	{
		if (pe.th32ProcessID != pid)
		{
			if (!strcmp(Name.c_str(), pe.szExeFile))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
//�ص��������巽ʽ
BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam)
{
	LPEnumHWndsArg Win = (LPEnumHWndsArg)lParam;
	DWORD Pid;
	GetWindowThreadProcessId(hwnd,&Pid);
	if (Pid == Win->pid)
	{
		Win->HWNDPid->push_back(hwnd);
	}
	return TRUE;
}
void ToolAndOpenMore::GetHWndsByProcessID(DWORD processID, std::vector<HWND> &vecHWnds)
{
	EnumHWndsArg wi;
	wi.pid = processID;
	wi.HWNDPid = &vecHWnds;
	EnumWindows(lpEnumFunc, (LPARAM)&wi);
}
//ʹ�ô������ҵ�Duokai
BOOL ToolAndOpenMore::FindOpenMoreByWinName(DWORD pid)
{
	vector<HWND> vecHWnds;
	GetHWndsByProcessID(pid,vecHWnds);
	for (int i = 0; i<2; i++) //vecHWnds.size()
	{
		char a[SZLEN];
		::GetWindowTextA((HWND)vecHWnds[i], a, SZLEN);
		CWnd * m_pWnd = FindWindow(NULL, a);
		DWORD Pid;
		GetWindowThreadProcessId(m_pWnd->m_hWnd, &Pid);
		if (Pid!=pid)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void OnStartDebug(LPDEBUG_EVENT pde)
{
	//��ȡCreateMutex��ַ
	g_pCreateMutex = GetProcAddress(GetModuleHandleA("kernel32.dll"),"CreateMutex");
	//��ͷ�������޸�
	memcpy(&g_cpdi2,&pde->u.CreateProcessInfo,sizeof(CREATE_PROCESS_DEBUG_INFO));
	ReadProcessMemory(g_cpdi2.hProcess,g_pCreateMutex,&g_Orignal,sizeof(BYTE),NULL);
	WriteProcessMemory(g_cpdi2.hProcess,g_pCreateMutex,&g_chINT32,sizeof(BYTE),NULL);
	return;
}
DWORD OnDealException(LPDEBUG_EVENT pde)
{
	CONTEXT ctx;
	PEXCEPTION_RECORD per = &pde->u.Exception.ExceptionRecord;
	DWORD EspContent;
	//�ж��Ƿ��Ǻ��������쳣
	if (per->ExceptionAddress == g_pCreateMutex && pde->dwDebugEventCode == EXCEPTION_BREAKPOINT)
	{
		//���HOOK �ָ�ԭֵ
		WriteProcessMemory(&pde->dwProcessId, g_pCreateMutex,&g_Orignal,sizeof(BYTE),NULL);
		//��ȡ�߳�������
		ctx.ContextFlags = CONTEXT_CONTROL;
		GetThreadContext(g_cpdi2.hThread,&ctx);
		//�õ�ESP+8��ֵ
		ReadProcessMemory(g_cpdi2.hProcess, LPVOID(ctx.Esp + 0x8), &EspContent, sizeof(DWORD), NULL);
		//ȡ����ַ����ȡ�ַ���
		Name[NameNum++] = (char*)(EspContent);
		//�ж������ظ�������
		list<string> list1;
		for (int i = 0; i < NameNum; i++)
		{
			list1.push_back(Name[i]);
		}
		int len1 = list1.size();
		list1.unique();
		int len2 = list1.size();
		//�ж��Ƿ����ظ�Ԫ��
		if (len1 != len2)
		{
			//::MessageBox(0,"��������࿪","��ܰ��ʾ",0);
			IsOpenMore = 1;
			return -1;
		}
		//��EIPΪ��ǰ��ַ
		ctx.Eip = (DWORD)g_pCreateMutex;
		//�����߳�������
		SetThreadContext(g_cpdi2.hThread, &ctx);
		//������ʽ
		ContinueDebugEvent(pde->dwProcessId, pde->dwThreadId, DBG_CONTINUE);
		//�޸�ͷ��
		Sleep(0);
		WriteProcessMemory(g_cpdi2.hProcess, g_pCreateMutex, &g_chINT32, sizeof(BYTE), NULL);
		return 1;
	}
	return 0;
}
//�࿪���
void ToolAndOpenMore::OnBnClickedButton6()
{
	IsOpenMore = 0;
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	DWORD index = mProList.GetCurSel();
	if (index == -1)
	{
		::MessageBoxA(0, "��ѡ�����", "��ܰ��ʾ", 1);
		return;
	}
	//��ȡ����pid
	DWORD dwPid = mProList.GetItemData(index);
	if (!FindOpenMoreByCode(dwPid, 0) && !TraverseAllPro(dwPid))
	{
		::MessageBox(0, "��������࿪", "��ܰ��ʾ", 0);
		return ;
	}
	//���˫��
	if (FindOpenMoreByWinName(dwPid) || FindOpenMoreByName(dwPid))
	{
		::MessageBox(0, "��������࿪", "��ܰ��ʾ", 0);
		return;
	}
	else
	{
		//��ʽ����
		if (!DebugActiveProcess(dwPid))
		{
			printf("DebugActiveProcess(%d) failed!!!\n"
				"Error Code = %d\n", dwPid, GetLastError());
		}
		//�����߳�ִ�е�ʽ
		HANDLE hThread1 = CreateThread(NULL, 0, DebugLoop2, NULL, 0, NULL);
		CloseHandle(hThread1);
	}
	if (!IsOpenMore)
	{
		::MessageBox(0,"û�м����࿪","��ܰ��ʾ",0);
	}
	return;
}
//��ʽ����
DWORD WINAPI DebugLoop2(LPVOID pid)
{
	DEBUG_EVENT de;
	DWORD dwContinueStatus;
	while (WaitForDebugEvent(&de, -1))
	{
		dwContinueStatus = DBG_CONTINUE;
		if (de.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT)
		{
			OnStartDebug(&de);
		}
		else if (de.dwDebugEventCode == EXCEPTION_DEBUG_EVENT)
		{
			if (OnDealException(&de) == 1)
				continue;
			else if (OnDealException(&de) == -1)
				return 0;
		}
		else if (de.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
		{
			return 0;
		}
		ContinueDebugEvent(de.dwProcessId, de.dwThreadId, dwContinueStatus);

	}
	return 1;
}
//��ȡ���������뺯�� PEFileToMemory
void ToolAndOpenMore::GetToolCode(BYTE* ToolCode, CString strPath)
{
    PeBuffer = (PVOID)DataHs.PEFileToMemory(DataHs.CStringToCharSz(strPath));
	//��λ�������
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL, tpSectionHeader;
	if (PeBuffer == NULL)
	{
		printf("(GetToolCode)PE bufferΪ��\n");
		return ;
	}
	if (*((PWORD)(PeBuffer)) != IMAGE_DOS_SIGNATURE)
	{
		printf("(GetToolCode)������Ч��EXE�ļ�\n");
		return ;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)PeBuffer;

	if (*((PDWORD)((DWORD)pDosHeader + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE)
	{
		printf("(GetToolCode)������Ч��PE�ļ�\n");
		return ;
	}
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)PeBuffer + pDosHeader->e_lfanew + 0x4);
	DWORD SectionNum = pPEHeader->NumberOfSections;
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	DWORD EP = pOptionHeader->AddressOfEntryPoint;
	RVAToolEp = EP;
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	tpSectionHeader = pSectionHeader;
	int j = 0;
	for (int i = tpSectionHeader->PointerToRawData; i < tpSectionHeader->PointerToRawData + tpSectionHeader->SizeOfRawData && j<SZLEN; i++)
	{
		if (DataHs.FoaToRva(PeBuffer, i) < EP)
			continue;
		ToolCode[j++] = *((BYTE*)((DWORD)PeBuffer + i));
	}
	return;
}
DWORD ToolAndOpenMore::GetProCodeToSZ(DWORD pid)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);//��ȡ���̾��
	PBYTE ExeProBase = FreeDll.GetExeBase(pid);
	if (ExeProBase == NULL)
	{
		return 0;
	}
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ExeProBase;
	PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 0x4);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);

	DWORD SectionAligment = pOptionHeader->SectionAlignment;
	int i;
	for (i = 0; i < MaxLenSz; i++)
	{
		ReadProcessMemory(hProcess, (ExeProBase + i), &ProMem[i], sizeof(char), NULL);
	}
	return 1;
}
//�жϹ����Ƿ������У��Ƿ��Ǳ������̵�PPID
BOOL ToolAndOpenMore::ToolIsRunning(DWORD pid,BYTE* ToolCode)
{
	//�������н��̣���ȡPID
	int len = mProList.GetCount();
	CString con;
	for (int i = 0; i < len; i++)
	{
		mProList.GetText(i, con);
		memset(ProMem, 0, MaxLenSz);
		if (!GetProCodeToSZ(mProList.GetItemData(i)))
			continue;
		//PE�ļ�ͷ
		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ProMem;
		PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 0x4);
		PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
		DWORD EP = pOptionHeader->AddressOfEntryPoint;
		//�ж�EP�ǲ��Ǳ�������
		if (EP> 0x2000)
			EP = RVAToolEp;
		PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);;
		PIMAGE_SECTION_HEADER tpSectionHeader = pSectionHeader;
		int ii = 0;
		for (int j = EP; ii < SZLEN && j < tpSectionHeader->VirtualAddress + tpSectionHeader->SizeOfRawData; j++, ii++)
		{
			BYTE con = *((BYTE*)((DWORD)pDosHeader+j));
			if (ToolCode[ii] != con)
			{
				break;
			}
		}
		//�ҵ���ʽ����
		if (ii == SZLEN)
		{
			if (isToolSon(mProList.GetItemData(i), pid))
				return TRUE;
		}
	}
	return FALSE;
}
BOOL ToolAndOpenMore::isToolSon(DWORD father, DWORD son)
{
	HANDLE hSnapPro = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32 pe;
	for (Process32First(hSnapPro, &pe); Process32Next(hSnapPro, &pe);)
	{
		if (pe.th32ProcessID == son && pe.th32ParentProcessID == father)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//���߼��
void ToolAndOpenMore::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString ProPath;
	mExeFilePath.GetWindowText(ProPath);
	if (ProPath.GetLength() == 0)
	{
		::MessageBox(0, "��ѡ���ʽ����", "��ܰ��ʾ", 0);
		return;
	}
	DWORD index = mProList.GetCurSel();
	if (index == -1)
	{
		::MessageBoxA(0, "��ѡ��Ҫ�����Ľ���", "��ܰ��ʾ", 0);
		return;
	}
	BYTE TextCode[SZLEN];
	memset(TextCode,0,SZLEN);
	GetToolCode(TextCode, ProPath);
	DWORD pid = mProList.GetItemData(index);
	//��������ID
	if (ToolIsRunning(pid, TextCode))
	{
		::MessageBoxA(0, "��⵽��ʽ�������ڵ�ʽ��������", "��ܰ��ʾ", 0);
	}
	else
	{
		::MessageBoxA(0, "û�м�⵽��ʽ�������ڵ�ʽ��������", "��ܰ��ʾ", 0);
	}
	return;
}
//ˢ�½���
void ToolAndOpenMore::OnBnClickedButton2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	mProList.ResetContent();
	FreeDll.AddProcessToList(mProList);
	return;
}
