// PRO_TEXT_Dialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Anti_WaiGua.h"
#include "PRO_TEXT_Dialog.h"
#include "HS_DATA_DIALOG.h"
#include "afxdialogex.h"
#include "md5.h"
#include <string>
#include <iostream>
#include <TlHelp32.h>
#include <AtlConv.h>
#include <fstream>
#include <windows.h>
#include<thread>
using namespace std;
// PRO_TEXT_Dialog �Ի���
#define SZLen 0x400
IMPLEMENT_DYNAMIC(PRO_TEXT_Dialog, CDialogEx)
//��������
DWORD pid;
DWORD AddrFuncSZ[SZLEN];
BYTE AddrFunOrignal[SZLEN];
BYTE g_chINT3 = 0xCC;
CHAR szLogFilePath[SZLEN];
HS_DATA_DIALOG DataHs;
//��������
BOOL SetUserFunc(LPDEBUG_EVENT pde);
DWORD WINAPI DebugLoop(LPVOID pid);
BOOL OnExceptionDebugEvent(LPDEBUG_EVENT pde);

CREATE_PROCESS_DEBUG_INFO g_cpdi;
PRO_TEXT_Dialog::PRO_TEXT_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(PRO_TEXT_Dialog::IDD, pParent)
{

}

PRO_TEXT_Dialog::~PRO_TEXT_Dialog()
{
}

void PRO_TEXT_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, mExeEditBrowse);
	DDX_Control(pDX, IDC_MFCEDITBROWSE2, mExeEditBrowse_Ver);
	DDX_Control(pDX, IDC_LIST5, mProList);
}


BEGIN_MESSAGE_MAP(PRO_TEXT_Dialog, CDialogEx)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE1, &PRO_TEXT_Dialog::OnEnChangeMfceditbrowse1)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE2, &PRO_TEXT_Dialog::OnEnChangeMfceditbrowse2)
	ON_BN_CLICKED(IDC_BUTTON1, &PRO_TEXT_Dialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &PRO_TEXT_Dialog::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &PRO_TEXT_Dialog::OnBnClickedButton2)
END_MESSAGE_MAP()
BOOL PRO_TEXT_Dialog::OnInitDialog()
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
	AddProcessToList();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}
BOOL PRO_TEXT_Dialog::GetImportFilePathAndUserFunc(char* path)
{
	char* DiskPath = (char*)malloc(SZLen);
	PVOID PeBuffer = DataHs.PEFileToMemory(path); 
	DiskPath = "d:\\text.bin";
	//��ʼ������ͷ������
	memset(AddrFuncSZ,-1,SZLEN);
	//�����Ҫ�ı���
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	DWORD SectionNum = 0;

	if (PeBuffer == NULL)
	{
		printf("(GetImportFilePath)PE bufferΪ��\n");
		return 0;
	}

	if (*((PWORD)PeBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("(GetImportFilePath)������Ч��EXE�ļ�\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)PeBuffer;

	if (*((PWORD)((DWORD)pDosHeader + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE)
	{
		printf("(GetImportFilePath)������Ч��PE�ļ�\n");
		return 0;
	}
	//PE�ļ�ͷ����
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 0x4);
	//PE��ѡͷ���� PIMAGE_OPTIONAL_HEADER32 pOptionHeader
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	//��ȡͷ�ڱ�ṹ
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	ImageBase = pOptionHeader->ImageBase;
	TextStartAddr = pSectionHeader->PointerToRawData;
	TextEndAddr = (pSectionHeader + 1)->PointerToRawData;
	//����OEP
	DWORD OEP = pOptionHeader->AddressOfEntryPoint + pOptionHeader->ImageBase;
	//��ȡPE�ļ��к����ĵ�ַ
	ImageBase = pOptionHeader->ImageBase;
	int Funi = 0;
	//Ѱ�Һ���λ��
	for (DWORD i = TextStartAddr; i < TextStartAddr + pSectionHeader->SizeOfRawData; i++)
	{
		//��ȡ����ͷ����Ϣ
		pFuncHeader = (_FuncHeader*)((DWORD)PeBuffer + i);
		//�ж��Ƿ�Ϊ����ͷ��
		if (pFuncHeader->p1 == 0x55 && pFuncHeader->p2 == 0x89 && pFuncHeader->p3 == 0xE5 || pFuncHeader->p1 == 0x53 && pFuncHeader->p2 == 0x56 && pFuncHeader->p3 == 0x57 && pFuncHeader->p4 == 0x55 || pFuncHeader->p1 == 0x55 && pFuncHeader->p2 == 0x8B && pFuncHeader->p3 == 0xEC)
		{
			//��FOA��ַת����VA
			DWORD FuncAddr = DataHs.FoaToRva(PeBuffer, i) + ImageBase;
			if (FuncAddr>OEP)
			{
				AddrFuncSZ[Funi++] = FuncAddr;
			}
		}

	}
	//�Ѵ������Ϣд�뵽�����ļ���
	std::ofstream outFile;
	
	//���ļ�
	outFile.open(DiskPath);
	for (int i = TextStartAddr; i < TextEndAddr; i++)
	{
		BYTE* context = (BYTE*)((DWORD)pPEHeader+i);
		//д������
		outFile << *context;
	}
	//��FOA��ַת����RVA
	TextStartAddr = DataHs.FoaToRva(PeBuffer, TextStartAddr);
	TextStartAddr = DataHs.FoaToRva(PeBuffer, TextEndAddr);
	//�ر��ļ�
	outFile.close();
	MD5 md5(ifstream(DiskPath, ios::binary));
	ProCheckV=md5.toString();
	if (remove(DiskPath) != 0)
		return FALSE;
	return TRUE;
}
// PRO_TEXT_Dialog ��Ϣ�������
void PRO_TEXT_Dialog::OnEnChangeMfceditbrowse1()
{
	//�õ�����·��
	mExeEditBrowse.GetWindowText(ExePath);
	CStringA tmp = ExePath;
	char* pp = tmp.GetBuffer();
	//��ȡ�����������е��ļ�·��
	MD5 md5(ifstream(pp, ios::binary));
	//���㾲̬У��ֵ
	CheckValue = md5.toString();
	//���㶯̬У��ֵ
	GetImportFilePathAndUserFunc(pp);
	return ;
}
int PRO_TEXT_Dialog::GerGangPos(char* pp)
{
	for (int i = strlen(pp) - 1; i > -1; i--)
	{
		if (pp[i] == '\\')
		{
			return i;
			break;
		}
	}
	return -1;
}
BOOL PRO_TEXT_Dialog::IsNameEqual(CString Path1, CString Path2, char pos)
{
	CStringA tmp = Path2;    
	char* pp = tmp.GetBuffer();
	//�жϽ����Ƿ����
	int IsEqual = 1;
	//�������
	int GangXb1 = Path1.ReverseFind('\\');
	int GangXb2 = Path2.ReverseFind(pos);
	//���ֵĳ��Ȳ���
	if (Path1.GetLength() - GangXb1 != Path2.GetLength() - GangXb2)
		IsEqual = 0;
	else
		for (int i = GangXb1 + 1, j = GangXb2 + 1; i < Path1.GetLength() && j < Path2.GetLength(); i++, j++)
		{
			if (Path1.GetAt(i) != Path2.GetAt(j))
			{
				IsEqual = 0;
				break;
			}
		}
	return IsEqual;
}
void PRO_TEXT_Dialog::OnEnChangeMfceditbrowse2()
{
	//��ȡ�ļ�·��
	mExeEditBrowse_Ver.GetWindowText(ExePath_Ver);
	int ans=IsNameEqual(ExePath_Ver, ExePath,'\\');

	//ѡ��Ľ�����������
	if (ans == 0)
	{
		::MessageBoxA(0, "ѡ����ļ�������", "��ܰ��ʾ", 0);
		return;
	}
	//ȷ���ļ��󣬼���У��ֵ���Ƚ��Ƿ����
	CStringA tmpCS = ExePath_Ver;
	char* pp2 = tmpCS.GetBuffer();
	MD5 md5(ifstream(pp2, ios::binary));
	string CheckValue1 = md5.toString();
	if (CheckValue != CheckValue1 )
	{
		::MessageBoxA(0, "�ļ�������ѱ��޸�", "��ܰ��ʾ", 0);
	}
	else
	{
		::MessageBoxA(0, "�ļ������û�б��޸�", "��ܰ��ʾ", 0);
	}
	return;
}
//���б�����ӽ�����Ϣ
void PRO_TEXT_Dialog::AddProcessToList()
{
	SYSTEM_INFO sysInfo;
	GetNativeSystemInfo(&sysInfo);

	DWORD dwPid = 0;
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return;

	Process32First(hProcessSnap, &pe32);
	do
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
		BOOL isWow64;
		if (IsWow64Process(hProcess, &isWow64))
		{
			TCHAR szBuf[1024] = { 0 };
			if (isWow64 || sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
			{
				wsprintf(szBuf, _T("%s %4d %s"), _T("(32λ)"), pe32.th32ProcessID, pe32.szExeFile);
			}
			else
			{
				wsprintf(szBuf, _T("%s %4d %s"), _T("(64λ)"), pe32.th32ProcessID, pe32.szExeFile);
			}
			//�����б��е�����  
			int count = mProList.AddString(szBuf);
			//����������ֵ
			mProList.SetItemData(count, pe32.th32ProcessID);
		}
		CloseHandle(hProcess);
	} while (Process32Next(hProcessSnap, &pe32));
	CloseHandle(hProcessSnap);
	return;
}
//ˢ�½���
void PRO_TEXT_Dialog::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	mProList.ResetContent();
	AddProcessToList();
}
//ִ��У��
void PRO_TEXT_Dialog::OnBnClickedButton3()
{
	char* DiskPath = "c:\\ProInfo.bin";
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (mProList.GetCurSel() == -1)
	{
		::MessageBoxA(0,"��ѡ�����","��ܰ��ʾ",0);
		return;
	}
	if (CheckValue.length() == 0 || ExePath.GetLength() == 0)
	{
		::MessageBoxA(0, "û��ѡ��У���ļ�", "��ܰ��ʾ", 0);
		return;
	}
	DWORD pid = mProList.GetItemData(mProList.GetCurSel());
	//�жϽ������Ƿ��PE�ļ�����ͬ
	CString str;
	mProList.GetText(mProList.GetCurSel(), str);
	int ans1=IsNameEqual(ExePath,str,' ');
	if (!ans1)
	{
		::MessageBoxA(0, "û��ѡ����ȷ�Ľ���", "��ܰ��ʾ", 0);
		return;
	}
	//�Ѵ������Ϣд�뵽�����ļ���
	std::ofstream outFile;
	//���ļ�
	outFile.open(DiskPath);
	HANDLE hProcess;
    BYTE tmp;
    DWORD dwNumberOfBytesRead;
	//pidΪĿ����̵�id
	hProcess = OpenProcess(PROCESS_VM_READ, false, pid); 
	//printf("TextStartAddr:%x   TextEndAddr:%x\n", TextStartAddr + ImageBase, TextEndAddr + ImageBase);
	//�����ڴ��еĴ����
	for (DWORD i = TextStartAddr + ImageBase; i < TextEndAddr + ImageBase; i++)
	{
		ReadProcessMemory(hProcess, (LPCVOID)i, &tmp, sizeof(BYTE), NULL);
		//д������
		outFile << tmp;
	}
	//�ر��ļ�
	outFile.close();
	MD5 md5;
	string ans = MD5(ifstream(DiskPath, ios::binary)).toString();
	if (ans == ProCheckV)
		::MessageBoxA(0,"�ļ��ڴ�δ�޸�","��ܰ��ʾ",0);
	else
	{
		::MessageBoxA(0, "�ļ��ڴ����޸�", "��ܰ��ʾ", 0);
	}
	if (!remove(DiskPath))
		printf("OnBnClickedButton3 �Ƴ�ʧ��\n");
	else
		printf("OnBnClickedButton3 �Ƴ��ɹ�\n");
	return ;
}

//������־�ļ�
void PRO_TEXT_Dialog::OnBnClickedButton2()
{
	//�ж�����ѡ��PE�ļ��ͽ���
	if (CheckValue.length() == 0 || ExePath.GetLength() == 0)
	{
		::MessageBoxA(0, "û��ѡ��У���ļ�", "��ܰ��ʾ", 0);
		return;
	}
	if (mProList.GetCurSel() == -1)
	{
		::MessageBoxA(0, "��ѡ�����", "��ܰ��ʾ", 0);
		return;
	}
	//�ж�ѡ��Ľ����Ƿ���ȷ
	CString str;
	mProList.GetText(mProList.GetCurSel(), str);
	int ans1 = IsNameEqual(ExePath, str, ' ');
	if (!ans1)
	{
		::MessageBoxA(0, "û��ѡ����ȷ�Ľ���", "��ܰ��ʾ", 0);
		return;
	}
	memset(szLogFilePath, '\0', SZLEN);
	GetModuleFileName(NULL, szLogFilePath, MAX_PATH);
	// ɾ���ļ�����ֻ���·���ִ�
	(_tcsrchr(szLogFilePath, _T('\\')))[1] = 0;
	char* p = "\log.txt";
	int i,j;
	//ƴ���ļ���
	for (i = strlen(szLogFilePath), j = 0; j < strlen(p); j++,i++)
	{
		szLogFilePath[i] = p[j];
	}
	szLogFilePath[i] = '\0';
	
    pid = mProList.GetItemData(mProList.GetCurSel());
	//�����̣߳��������
	HANDLE hThread1 = CreateThread(NULL, 0, DebugLoop, NULL, 0, NULL);

	CloseHandle(hThread1);
}

DWORD WINAPI DebugLoop(LPVOID a)
{
	DEBUG_EVENT de;
	DWORD dwContinueStatus;
	if (!DebugActiveProcess(pid))
	{
		printf("DebugActiveProcess(%d) failed!!!"
			"Error Code = %d\n", pid, GetLastError());
		return 0;
	}
	//�ȴ���ʽ�¼�
	while (WaitForDebugEvent(&de, INFINITE))
	{
		LPDEBUG_EVENT pde = &de;
		PEXCEPTION_RECORD per = &pde->u.Exception.ExceptionRecord;
		dwContinueStatus = DBG_CONTINUE;

		if (CREATE_PROCESS_DEBUG_EVENT == de.dwDebugEventCode)
		{
			SetUserFunc(&de);
		}
		else if (EXCEPTION_DEBUG_EVENT == de.dwDebugEventCode)
		{
			printf("EXCEPTION_DEBUG_EVENT \n");
			if (OnExceptionDebugEvent(&de))
				continue;
		}
		else if (EXIT_PROCESS_DEBUG_EVENT == de.dwDebugEventCode)
		{
			break;
		}
		ContinueDebugEvent(de.dwProcessId, de.dwThreadId, dwContinueStatus);
	}
	return 0;
}

BOOL SetUserFunc(LPDEBUG_EVENT pde)
{
	printf("SetUserFunc\n");
	memset(AddrFunOrignal,-1,SZLEN);
	memcpy(&g_cpdi, &pde->u.CreateProcessInfo, sizeof(CREATE_PROCESS_DEBUG_INFO));
	for (int i = 0; AddrFuncSZ[i] != -1; i++)
	{
		if (!::ReadProcessMemory(g_cpdi.hProcess, (LPVOID)AddrFuncSZ[i], &AddrFunOrignal[i], sizeof(BYTE), NULL))
			printf("ReadProcessMemory is fail\n");
		else
			printf("AddrFunOrignal[i] is %x\n", AddrFunOrignal[i]);
		if (!::WriteProcessMemory(g_cpdi.hProcess, (LPVOID)AddrFuncSZ[i], &g_chINT3, sizeof(BYTE), NULL))
			printf("WriteProcessMemory is fail\n");
	}
	return TRUE;
}
BOOL OnExceptionDebugEvent(LPDEBUG_EVENT pde)
{
	CONTEXT ctx;
	PBYTE lpBuffer = NULL;
	DWORD dwNumOfBytesToWrite, dwAddrOfBuffer;
	PEXCEPTION_RECORD per = &pde->u.Exception.ExceptionRecord;
	FILE* dst = fopen(szLogFilePath, "ab+");
	if (dst != NULL)
		::MessageBoxA(0, "��־�ļ�������", "��Ϣ", 1);
	// BreakPoint exception (INT 3)
	if (EXCEPTION_BREAKPOINT == per->ExceptionCode)
	{
		DWORD ExAddress=(DWORD)(per->ExceptionAddress);
		for (int i = 0; AddrFuncSZ[i] != -1 ; i++)
		{
			//�Ӹ����
			//printf("AddrFuncSZ[i]:%x (DWORD)(per->ExceptionAddress): %x\n", AddrFuncSZ[i], ExAddress);
			if (AddrFuncSZ[i] == ExAddress)
				{
					//д��ԭ������
					WriteProcessMemory(g_cpdi.hProcess,&AddrFuncSZ[i],&AddrFunOrignal[i],sizeof(BYTE),NULL);
					// ��ȡThread Context
					ctx.ContextFlags = CONTEXT_CONTROL;
					GetThreadContext(g_cpdi.hThread, &ctx);
					//��ȡ�߳���������Ϣ
					CString str;
					str.Format("ִ�к���Ϊ��sub_%x,����EAXΪ��%x��EBXΪ��%x��ECXΪ��%x��EDXΪ��%x��EIPΪ��%x��ESPΪ��%x��EBPΪ��%x��EDIΪ��%x��ESIΪ��%x\n", AddrFuncSZ[i],ctx.Eax,ctx.Ebx,ctx.Ecx,ctx.Edx,ctx.Eip,ctx.Esp,ctx.Ebp,ctx.Edi,ctx.Esi);
					CStringA tmp = str;
					char* context = tmp.GetBuffer();
					fwrite(context, sizeof(context[0]), strlen(context), dst);
					//��������������
					ctx.Eip = AddrFuncSZ[i];
					SetThreadContext(g_cpdi.hThread, &ctx);
					//������ʽ
					ContinueDebugEvent(pde->dwProcessId, pde->dwThreadId, DBG_CONTINUE);
					Sleep(0);
					//�������öϵ�
					WriteProcessMemory(g_cpdi.hProcess,&AddrFuncSZ[i],&g_chINT3,sizeof(BYTE),NULL);
					return TRUE;
				}
		}
	}
	return FALSE;
}
