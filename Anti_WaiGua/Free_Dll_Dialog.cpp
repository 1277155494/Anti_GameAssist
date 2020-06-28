// Free_Dll_Dialog.cpp : ʵ���ļ�
#include "stdafx.h"
#include "Anti_WaiGua.h"
#include "Free_Dll_Dialog.h"
#include "afxdialogex.h"
#include "PRO_TEXT_Dialog.h"
#include <string>
#include <iostream>
#include <cstdio>
#include <TlHelp32.h>
#include<list>
#include <AtlConv.h>
#include <io.h>
#include "HS_DATA_DIALOG.h"
using namespace std;

// Free_Dll_Dialog �Ի���
list<CString> files;

struct HSInfo{
	DWORD StartAddr;
	DWORD EndAddr;
}AllHs[MaxLen];
struct _FuncHeader
{
	BYTE p1, p2, p3, p4;

}FuncHeader, *pFuncHeader;
IMPLEMENT_DYNAMIC(Free_Dll_Dialog, CDialogEx)
BOOL Free_Dll_Dialog::OnInitDialog()
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
	AddProcessToList(mProList);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}
Free_Dll_Dialog::Free_Dll_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Free_Dll_Dialog::IDD, pParent)
{

}

Free_Dll_Dialog::~Free_Dll_Dialog()
{

}

void Free_Dll_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, mProList);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, mProPath);
}


BEGIN_MESSAGE_MAP(Free_Dll_Dialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &Free_Dll_Dialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &Free_Dll_Dialog::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &Free_Dll_Dialog::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON5, &Free_Dll_Dialog::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, &Free_Dll_Dialog::OnBnClickedButton4)
END_MESSAGE_MAP()
//���б�����ӽ�����Ϣ

void Free_Dll_Dialog::AddProcessToList(CListBox &mProList1)
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
			int count = mProList1.AddString(szBuf);
			//����������ֵ
			mProList1.SetItemData(count, pe32.th32ProcessID);
		}
		CloseHandle(hProcess);
	} while (Process32Next(hProcessSnap, &pe32));
	CloseHandle(hProcessSnap);
	return;
}

void Free_Dll_Dialog::GetFileFromDir(CString csDirPath)
{
	//��¼·��
	CString tDirPath = csDirPath;
	CString tDirPath1 = csDirPath;
	//ƥ��dll�ļ��ַ��� 
	tDirPath += "\\*.dll";
	//������Ŀ¼������dll�ļ�
	char* p = Datahs.CStringToCharSz(tDirPath);
	//printf("upper mian :%s\n", p);
	//ƥ������dll�ļ�
	_finddata_t fileInfo;
	long handle = _findfirst(p, &fileInfo);
	if (handle == -1L)
	{
		return ;
	}
	int i=0;
	//��ŵĶ�������
	do
	{
		char* tmpChar = Datahs.CStringToCharSz(fileInfo.name);
		tmpChar = SZCharSwapToSmall(tmpChar);
		files.push_back(CString(tmpChar));
		files.unique();
	} while (_findnext(handle, &fileInfo) == 0);
	_findclose(handle);
	//ѭ�������ļ����µ����ļ���
	_finddata_t FileInfo;
	csDirPath +=  "\\*"; 
	p = Datahs.CStringToCharSz(csDirPath);
	long Handle = _findfirst(p, &FileInfo);
	if (Handle == -1L)
	{
		cerr << "can not match the folder path" << endl;
		exit(-1);
	}
	do{
		//�ж��Ƿ�����Ŀ¼
		if (FileInfo.attrib & _A_SUBDIR)
		{
			//���˵�������Ŀ¼��.����һ��Ŀ¼��..
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
			{
				CString newPath = tDirPath1 + "\\" + FileInfo.name;
				GetFileFromDir(newPath);
			}
		}
	} while (_findnext(Handle, &FileInfo) == 0);
	_findclose(Handle);
	
	return ;
}
//ˢ�½���
void Free_Dll_Dialog::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	mProList.ResetContent();
	AddProcessToList(mProList);
	return;
}

//���
//��ť���ļ��У����ҷ���·����
void Free_Dll_Dialog::OnBnClickedButton2()
{
	CHAR szFolderPath[MAX_PATH] = { 0 };
	CString strFolderPath = TEXT("");
	BROWSEINFO  sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("��ѡ�������洢·��");
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_DONTGOBELOWDOMAIN;
	sInfo.lpfn = NULL;
	files.push_back("winspool");
	// ��ʾ�ļ���ѡ��Ի���  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);

	if (lpidlBrowse != NULL)
	{
		// ȡ���ļ�����  
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			strFolderPath = CString(szFolderPath);
			//��������dll�ļ�
			GetFileFromDir(strFolderPath);
			
			//MessageBox(szFolderPath);
		}
	}
	if (lpidlBrowse != NULL)
	{
		::CoTaskMemFree(lpidlBrowse);
	}
	return;
}
//�Ѵ�д��ĸת����Сд��ĸ
char* Free_Dll_Dialog::SZCharSwapToSmall(char* str)
{
	char* deal = (char*)malloc(0x100);
	memset(deal,0,0x100);  
	int i = 0;
	for ( i = 0; i < strlen(str)-4; i++)
	{
		if (0x41 <= str[i] && str[i] <= 0x5A)
		{
			deal[i] =str[i]+ 0x20;
			continue;
		}
		deal[i] = str[i];
	}
	return deal;
}
//�ַ���ת�ַ�����
char* Free_Dll_Dialog::StringToChar(string str)
{
	char* tmp = (char*)malloc(100);
	memset(tmp,0,100);
	for (int i = 0; i < str.size(); i++)
	{
		tmp[i] = str.at(i);
	}
	return tmp;
}

//�ж�����dllע��
BOOL Free_Dll_Dialog::IsInjectDll(DWORD dwPid)
{
	int flag=0;
	MODULEENTRY32 moduleEntry;
	HANDLE handle = NULL;
	LPTHREAD_START_ROUTINE pThreadPro;
	handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
	//��0����ڴ�����
	ZeroMemory(&moduleEntry, sizeof(MODULEENTRY32));
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	//�ҵ������е�WeChatWin.dll��ַ
	if (Module32First(handle, &moduleEntry))
	{
		do
		{
			//ģ����
			string szMod = moduleEntry.szModule;
			char* ModName = StringToChar(szMod);
			//�ж��Ƿ�Ϊ����ģ��
			if (ModName[strlen(ModName) - 1] == 'e'&& ModName[strlen(ModName) - 2] == 'x' &&ModName[strlen(ModName) - 3] == 'e' && ModName[strlen(ModName) - 4] == '.')
			{
				continue;
			}
			ModName = SZCharSwapToSmall(ModName);
			CString tszMod = CString(ModName);
			std::list<CString>::iterator iter;
			iter = std::find(files.begin(), files.end(), tszMod);
			//Ѱ��ͬ��dll
			if (iter != files.end())
				continue;
			else
			{
				//ж��dll
				pThreadPro = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("kernel32.dll"),"FreeLibrary");
				HANDLE hPro = OpenProcess(PROCESS_ALL_ACCESS,false,dwPid);
				HANDLE HThread = CreateRemoteThread(hPro, NULL, 0, pThreadPro, moduleEntry.modBaseAddr,0,NULL);
				WaitForSingleObject(HThread,INFINITE);
				CloseHandle(hPro);
				CloseHandle(HThread);
				CloseHandle(handle);
				flag = 1;
			}
		} while (Module32Next(handle, &moduleEntry));
	}
	CloseHandle(handle);
	if (flag)
		return FALSE;
	return TRUE;
}
//���dllע��
void Free_Dll_Dialog::OnBnClickedButton3()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	DWORD index = mProList.GetCurSel();
	if (index == -1)
	{
		::MessageBoxA(0,"��ѡ�����","��ܰ��ʾ",1);
		return;
	}
	//��ȡ����pid
	DWORD dwPid = mProList.GetItemData(index);
	CString strPath("C:\\Windows\\SysWOW64");
	//����ϵͳdll
	GetFileFromDir(strPath);
	//find list����ע���dll
	if (!IsInjectDll(dwPid))
	{
		::MessageBoxA(0, "��DLLע��", "��ܰ��ʾ", 0);
	}
	else
	{
		::MessageBoxA(0, "��DLLע��", "��ܰ��ʾ", 0);
	}
	return;
}
PBYTE Free_Dll_Dialog::GetExeBase(DWORD pid)
{
	MODULEENTRY32 me = { 0 };
	me.dwSize = sizeof(MODULEENTRY32);
	//��ȡ����ȫ��ģ�����
	HANDLE hMod = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid);
	if (hMod == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	//��ȡEXE����
	if (::Module32First(hMod, &me))
	{
		do
		{
			string szMod = me.szModule;
			char* ModName = StringToChar(szMod);
			//�ж��Ƿ�Ϊ����ģ��
			if (ModName[strlen(ModName) - 1] == 'e'&& ModName[strlen(ModName) - 2] == 'x' &&ModName[strlen(ModName) - 3] == 'e' && ModName[strlen(ModName) - 4] == '.')
			{
				return me.modBaseAddr;
			}
		} while (::Module32Next(hMod, &me));                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            

	}
	//�ص����
	CloseHandle(hMod);
	return NULL;
}
void Free_Dll_Dialog::GetFirstHsAndEndHs(DWORD pid, unsigned char ProMem[])
{
	//�õ�PEͷ����ַ
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ProMem;
	PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 0x4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pFileHeader + 0x14);
	//��λ����ѡͷ��
	ImageBase = pOptionalHeader->ImageBase;
	DWORD EP = pOptionalHeader->AddressOfEntryPoint;
	//����ν���λ��
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionalHeader + pFileHeader->SizeOfOptionalHeader);
	//��ʼ��
	memset(AllHs, -1, SZLEN);
	//��OEP��ʼ���������Һ�������¼ ;
	int i = 0;
	for (DWORD startAddr = EP, i = 0; startAddr <Datahs.FoaToRva(ProMem,pSectionHeader->PointerToRawData) + pSectionHeader->SizeOfRawData; startAddr++)
	{
		//��ȡ����ͷ����Ϣ
		pFuncHeader = (_FuncHeader*)((DWORD)ProMem+startAddr);
		//�ж��Ƿ�Ϊ����ͷ��
		if (pFuncHeader->p1 == 0x55 && pFuncHeader->p2 == 0x89 && pFuncHeader->p3 == 0xE5 || pFuncHeader->p1 == 0x53 && pFuncHeader->p2 == 0x56 && pFuncHeader->p3 == 0x57 && pFuncHeader->p4 == 0x55 || pFuncHeader->p1 == 0x55 && pFuncHeader->p2 == 0x8B && pFuncHeader->p3 == 0xEC)
		{
			int j;
			for (j = 0; j<0x500; j++)
			{
				PBYTE Con = (PBYTE)((DWORD)ProMem + startAddr + j);
				if (*Con == 0xC3)
				{
					AllHs[i].StartAddr = startAddr;
					AllHs[i].EndAddr = startAddr + j;
					i++;
					break;
				}
			}
		}
	}
	return;
}
//���InLine HOOK
void Free_Dll_Dialog::OnBnClickedButton5()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString ProPath;
	mProPath.GetWindowText(ProPath);
	if (ProPath.GetLength() == 0)
	{
		::MessageBox(0, "��ѡ��PE�ļ�", "��ܰ��ʾ", 0);
		return;
	}
	DWORD index = mProList.GetCurSel();
	if (index == -1)
	{
	::MessageBoxA(0, "��ѡ�����", "��ܰ��ʾ", 0);
	return;
	}

	CString str;
	mProList.GetText(mProList.GetCurSel(), str);
	int ans1 = ProText.IsNameEqual(ProPath, str, ' ');
	if (!ans1)
	{
		::MessageBoxA(0, "û��ѡ����ȷ�Ľ���", "��ܰ��ʾ", 0);
		return;
	}
	//��ȡ����pid
	DWORD dwPid = mProList.GetItemData(index);
	//�ѽ����ڴ���ص������ڴ���
	unsigned char ProMem[0x10000];
	memset(ProMem, 0, 0x10000);
	GetProMemToChar(dwPid, ProMem);
	//��ʼ���ṹ��
	GetFirstHsAndEndHs(dwPid, ProMem);
	//�������д���
	for (int i = 0;; i++)
	{
		if (AllHs[i].EndAddr = -1 || AllHs[i].StartAddr == -1)
			break;
		for (DWORD addr = AllHs[i].StartAddr; addr < AllHs[i].EndAddr; addr++)
		{
			PBYTE con = (PBYTE)((DWORD)ProMem + addr);
			//�ж��Ƿ�Ϊ����תָ��
			if (0x70 <= *con&&*con <= 0x7F)
			{
				PBYTE OpCode = (PBYTE)((DWORD)ProMem + addr+1);
				if (*OpCode + addr + 2>AllHs[i].EndAddr )
				{
					::MessageBox(0,"��������InLine Hook","��ܰ��ʾ",0);
					return ;
				}
			}
			//�ж��Ƿ�Ϊ����תָ��
			else if (0x0F80 <= *con&&*con <= 0x0F8F || *con == 0xE9)
			{
				PDWORD OpCode = (PDWORD)((DWORD)ProMem + addr + 1);
				if (*OpCode + addr + 5>AllHs[i].EndAddr)
				{
					::MessageBox(0, "��������InLine Hook", "��ܰ��ʾ", 0);
					return;
				}
			}

		}
	}
	::MessageBox(0, "��������InLine Hook", "��ܰ��ʾ", 0);
	return;
}
BOOL Free_Dll_Dialog::GetAllIatFromPE(CString ProPath)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory_ImportHeader = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pImportHeader = NULL;
	//��ȡָ��PE��ָ�� 
	char* Path = (char*)Datahs.CStringToCharSz(ProPath);
	PVOID PeBuffer = (PVOID)Datahs.PEFileToMemory(Path);
	if (PeBuffer == NULL)
	{
		printf("(GetAllIatFromPE)PE bufferΪ��\n");
		return FALSE;
	}
	if (*((PWORD)(PeBuffer)) != IMAGE_DOS_SIGNATURE)
	{
		printf("(GetAllIatFromPE)������Ч��EXE�ļ�\n");
		return FALSE;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)PeBuffer;

	if (*((PDWORD)((DWORD)pDosHeader + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE)
	{
		printf("(GetAllIatFromPE)������Ч��PE�ļ�\n");
		return FALSE;
	}
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 0x4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); 
	//ʹ�ýṹ��ָ�붨λ
	pDataDirectory = (PIMAGE_DATA_DIRECTORY)pOptionHeader->DataDirectory;
	pDataDirectory_ImportHeader = &pDataDirectory[1];
	if (!pDataDirectory_ImportHeader->VirtualAddress)
	{
		printf("This program has no import table.\n");
		return FALSE;
	}
	DWORD Foa_pImportHeader = Datahs.RvaToFoa(PeBuffer, pDataDirectory_ImportHeader->VirtualAddress);
   //Get Import Table
	pImportHeader = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)PeBuffer + Foa_pImportHeader);
	//����IAT����ȡdll�ļ���
	int i = 0;
	for (; pImportHeader->Name !=0; pImportHeader++)
	{
		//��ȡdllģ����
		DWORD Foa_DllName = Datahs.RvaToFoa(PeBuffer, pImportHeader->Name);
		PDWORD Foa_pDllName = (PDWORD)((DWORD)PeBuffer + Foa_DllName);
		DWORD Foa_OrginalFirstThunkAddr = Datahs.RvaToFoa(PeBuffer, pImportHeader->OriginalFirstThunk);
		PDWORD Foa_pOrginalFirstThunkAddr = (PDWORD)((DWORD)PeBuffer + Foa_OrginalFirstThunkAddr);
		PIMAGE_THUNK_DATA pOriginalFirstThunk = (PIMAGE_THUNK_DATA)((DWORD)PeBuffer + Foa_OrginalFirstThunkAddr);
		char* szLibName = (char*)Foa_pDllName;
		char* szLibNameToSmall = SZCharSwapToSmall(szLibName);
		printf("szLibName:%s\n", szLibName);
		int num = 0;
		if (!strcmp(szLibNameToSmall, "kernel32")) 
		{
			//����������ַ�����
			while (*(PDWORD)pOriginalFirstThunk)
			{
				DWORD value = *((PDWORD)pOriginalFirstThunk);
				//�ж�����Ż��Ǻ�����
				int judge = (value & IMAGE_ORDINAL_FLAG32)>>31;
				if (judge)
				{
					continue;
				}
				//ֵ��Ϊ��ţ���Ϊ��������ַ
				else
				{
					//ͨ��PIMAGE_IMPORT_BY_NAME��ȡ���̺�����
					DWORD Foa_ImportByName = Datahs.RvaToFoa(PeBuffer, value);
					PIMAGE_IMPORT_BY_NAME  pImportName = (PIMAGE_IMPORT_BY_NAME)((DWORD)PeBuffer+Foa_ImportByName);
					AllIAT[i] = (DWORD)GetProcAddress(GetModuleHandle(szLibName), pImportName->Name);

				}
				if (i == MaxLen)
				{
					return TRUE;
				}
				i++;
				pOriginalFirstThunk++;
			}
		}
	}
	return TRUE;
}
void Free_Dll_Dialog::GetProMemToChar(DWORD pid, unsigned char ans[])
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);//��ȡ���̾��
	PBYTE ExeProBase = GetExeBase(pid);
	if (ExeProBase == NULL)
	{
		memset(ans, 0xFF, 0x10000);
		return;
	}
	//��ȡ�����ڴ��С
	DWORD NTPos;
	ReadProcessMemory(hProcess, (PVOID)((DWORD)ExeProBase+0x3C), &NTPos, sizeof(DWORD), NULL); 
	DWORD SizeOfImagePos = (DWORD)ExeProBase + NTPos + 0x4 + 0x14 + 0x38;
	DWORD SizeOfImage;

	ReadProcessMemory(hProcess, (PVOID)SizeOfImagePos, &SizeOfImage, sizeof(DWORD), NULL); 
	if (SizeOfImage > 0x10000)
	{
		memset(ans, 0xFF, 0x10000);
		return;
	}
	//��ans��ֵ��ֵ
	for (int i = 0; i < SizeOfImage; i++)
	{
		ReadProcessMemory(hProcess, (ExeProBase + i), &ans[i], sizeof(char), NULL);
	}
	return;
}
//�������IAT HOOK
void Free_Dll_Dialog::OnBnClickedButton4()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString ProPath;
	mProPath.GetWindowText(ProPath);
	if (ProPath.GetLength() == 0)
	{
		::MessageBox(0, "��ѡ��PE�ļ�", "��ܰ��ʾ", 0);
		return;
	}
	DWORD index = mProList.GetCurSel();
	if (index == -1)
	{
		::MessageBoxA(0, "��ѡ�����", "��ܰ��ʾ", 0);
		return;
	}
	CString str;
	mProList.GetText(mProList.GetCurSel(), str);
	int ans1 = ProText.IsNameEqual(ProPath, str, ' ');
	if (!ans1)
	{
		::MessageBoxA(0, "û��ѡ����ȷ�Ľ���", "��ܰ��ʾ", 0);
		return;
	}
	//��ȡ����pid
	DWORD dwPid = mProList.GetItemData(index);
	memset(AllIAT, -1, MaxLen);
	//��¼PE�е�IAT
	if (!GetAllIatFromPE(ProPath))
	{
		printf("��ȡIAT��ʧ��\n");
		return ;
	}
	//��ȡ�����ڴ���Ϣ
	unsigned char ProMem[0x10000];
	memset(ProMem, 0, 0x10000);
	GetProMemToChar(dwPid,ProMem);
	//�ж��ļ��Ƿ����
	PBYTE ExeProBase = (PBYTE)ProMem;
	if (*ExeProBase == 0xFF)
	{
		::MessageBox(0,"�ļ������޷���� IAT  HOOK","��ܰ��ʾ",0);
		return;
	}
	//��ȡͷ����Ϣ
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ProMem;
	PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader+pDosHeader->e_lfanew+0x4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pFileHeader + 0x14);
	PIMAGE_DATA_DIRECTORY pDataDirectory = (PIMAGE_DATA_DIRECTORY)pOptionalHeader->DataDirectory;
	PIMAGE_DATA_DIRECTORY pDataDirectory_ImportHeader = &pDataDirectory[1];
	//�����ͷ
	DWORD ImportRva = pDataDirectory_ImportHeader->VirtualAddress;
	PIMAGE_IMPORT_DESCRIPTOR pImportHeader = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)ProMem + ImportRva);
	//������̬IAT���Ƚ�
	int i = 0;
	cout << "RVA   pImportHeader->FirstThunk\n";
	for (; pImportHeader->Name != 0; pImportHeader++)
	{
		//��ȡdllģ����
		PDWORD RVA_pDllName = (PDWORD)((DWORD)ProMem + pImportHeader->Name);
		PIMAGE_THUNK_DATA pFirstThunk = (PIMAGE_THUNK_DATA)((DWORD)ProMem + pImportHeader->FirstThunk);

		char* szLibName = (char*)RVA_pDllName;
		char* szLibNameToSmall = SZCharSwapToSmall(szLibName);
		int i = 0;
		if (!strcmp(szLibNameToSmall, "kernel32"))
		{
			//����������ַ�����
			while (*(PDWORD)pFirstThunk)
			{
				//ֵ��Ϊ��ţ���Ϊ��������ַ
				DWORD value = *(PDWORD)pFirstThunk;
				//�ж�����Ż��Ǻ�����
				int judge = (value & IMAGE_ORDINAL_FLAG32) >> 31;
				if (judge)
				{
					continue;
				}
				printf("value: %x  AllIAT[i]:%x i:%x\n", value, AllIAT[i],i);
				if (value != AllIAT[i])
				{
					::MessageBoxA(0, "����IAT HOOK", "��ܰ��ʾ", 0);
					return;
				}
				i++;
				pFirstThunk++;
			}
		}
	}
	::MessageBox(0, "û�м���IAT HOOK", "��ܰ��ʾ", 0);
	return;
}
