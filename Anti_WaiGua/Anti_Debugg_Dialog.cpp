// Anti_Debugg_Dialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Anti_WaiGua.h"
#include "Anti_Debugg_Dialog.h"
#include "afxdialogex.h"
#include "Anti_WaiGua.h"
#include "ToolAndOpenMore.h"
#include "afxdialogex.h"
#include "Free_Dll_Dialog.h"
#include "PRO_TEXT_Dialog.h"
#include <TlHelp32.h>
#include "HS_DATA_DIALOG.h"
#include <fstream>
#include <iostream>  
using namespace std;
unsigned char ShellCode[118] = { 0x68, 0xb8, 0x10, 0x40, 0x00, 0x64, 0xFF, 0x35, 0x00, 0x00, 0x00, 0x00, 0x64, 0x89, 0x25, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x33, 0xC0, 0xC7, 0x00, 0x01, 0x00, 0x00, 0x00, 0x90
, 0x00, 0x00, 0x6A, 0x00, 0x6A, 0x00, 0x68, 0x20, 0x30, 0x40, 0x00, 0x6A, 0x00, 0xE8, 0xC2, 0xFF, 0xFF, 0xFF, 0x64, 0x8F, 0x05, 0x00, 0x00, 0x00, 0x00, 0x83, 0xC4, 0x04, 0x6A, 0x00, 0xE8, 0xB7
, 0xFF, 0xFF, 0xFF, 0xC3, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x8B, 0x74, 0x24, 0x0C
, 0x64, 0xA1, 0x30, 0x00, 0x00, 0x00, 0x80, 0x78, 0x02, 0x01, 0x75, 0x0C, 0xC7, 0x86, 0xB8, 0x00
, 0x00, 0x00, 0x8E, 0x10, 0x40, 0x00, 0xEB, 0x0A, 0xC7, 0x86, 0xB8, 0x00, 0x00, 0x00, 0x20, 0x10
, 0x40, 0x00, 0x33, 0xC0, 0xC3, 0 };
unsigned char TLSCode[0x32] = { 0x83, 0x7C, 0x24, 0x08, 0x01, 0x75, 0x28, 0x64, 0xA1, 0x30, 0x00, 0x00, 0x00, 0x80, 0x78, 0x02
, 0x00, 0x74, 0x1C, 0x6A, 0x00, 0x68, 0x70, 0xC2, 0x40, 0x00, 0x68, 0x80, 0xC2, 0x40, 0x00, 0x6A
, 0x00, 0xFF, 0x15, 0xE8, 0x80, 0x40, 0x00, 0x6A, 0x01, 0xFF, 0x15, 0x28, 0x80, 0x40, 0x00, 0xC2
, 0x0C, 0x00 };

IMPLEMENT_DYNAMIC(Anti_Debugg_Dialog, CDialogEx)

Anti_Debugg_Dialog::Anti_Debugg_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Anti_Debugg_Dialog::IDD, pParent)
{

}

Anti_Debugg_Dialog::~Anti_Debugg_Dialog()
{
}
BOOL Anti_Debugg_Dialog::OnInitDialog()
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
	//ֻ�ܴ�exe�ļ�
	mExeEditBrowse.EnableFileBrowseButton(NULL, _T("Exe Files (*.exe)|*.exe|All Files (*.*)|*.*||"));
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}
void Anti_Debugg_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, mExeEditBrowse);
}


BEGIN_MESSAGE_MAP(Anti_Debugg_Dialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &Anti_Debugg_Dialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON6, &Anti_Debugg_Dialog::OnBnClickedButton6)
END_MESSAGE_MAP()


//��������οհ״���
DWORD Anti_Debugg_Dialog::FindEmptyCode()
{
	//��exe�ļ����ؽ��ڴ�
	//��ȡPE�ļ���صı���
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 0x4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	ImageBase = pOptionHeader->ImageBase;
	AddrEP = pOptionHeader->AddressOfEntryPoint;
	SectionNum = pPEHeader->NumberOfSections;
	RVAFileSize = pOptionHeader->SizeOfImage;
	//��λ������νṹ��
	PIMAGE_SECTION_HEADER tpSectionHeader = pSectionHeader;
	//��������Σ��ҵ��հ״�������
	int i, j;
	for (i = tpSectionHeader->PointerToRawData; i < tpSectionHeader->SizeOfRawData + tpSectionHeader->PointerToRawData; i++)
	{
		for (j = 0; j<118; j++)
		{
			if (*((PBYTE)((DWORD)pFileBuffer + i+j)) != 0x0)
			{
				break;
			}
		}
		if (j == 118)
		{
			FoaEP = i;
			StartTextShellCode = DataHs.FoaToRva(pFileBuffer, FoaEP) + ImageBase;
			AfterEP = DataHs.FoaToRva(pFileBuffer, FoaEP);
			return 1;
		}
	}
	//����ڴ����û���ҵ������ʵĿ����ڴ�
	FoaEP = FileSize;
	StartTextShellCode = DataHs.FoaToRva(pFileBuffer, FoaEP) + ImageBase;
	AfterEP = DataHs.FoaToRva(pFileBuffer, FoaEP);
	SectionNum = pPEHeader->NumberOfSections;
	//��λ�����νṹ��
	tpSectionHeader = pSectionHeader;
	for (int i = 0; i < SectionNum; i++)
	{
		tpSectionHeader++;
	}
	tpSectionHeader--;
	tpSectionHeader->Characteristics = 0xE0000060;
	tpSectionHeader->SizeOfRawData += pOptionHeader->FileAlignment;
	return 0;
}

//�������ݶοհ�����
DWORD Anti_Debugg_Dialog::FindEmptyDataAndAddData()
{
	char s[] = "Debugging";
	//��λ������νṹ��
	PIMAGE_SECTION_HEADER tpSectionHeader = pSectionHeader;
	int i = 0;
	for (i = 0; i < SectionNum; i++, tpSectionHeader++)
	{
		char name[9] = { 0 };
		memset(name,0,9);
		memcpy(name, tpSectionHeader->Name, 9);
		char t[] = ".data";
		if (!strcmp(t, name))
		{
			break;
		}
	}
	if (i == SectionNum)
	{
		return 0;
	}
	int j = 0;
	PBYTE Con;
	for (i = tpSectionHeader->PointerToRawData; i < tpSectionHeader->SizeOfRawData + tpSectionHeader->PointerToRawData; i++)
	{
		for (j = 0; j < 10; j++)
		{
			if (*((PBYTE)((DWORD)pFileBuffer+j+i)) != 0x0)
				break;
		}
		if (j == 10)
		{
			//Debugging���ַ����������ڴ���
			Con = (PBYTE)((DWORD)pFileBuffer + i);
			memcpy(Con, s, 10);
			AfterRVAData = DataHs.FoaToRva(pFileBuffer, i) + ImageBase;
			printf("AfterRVAData: %x DataFOA: %x\n", AfterRVAData, i);
			return 1;
		}
	}
	return 0;
}
//�޸�shellcode����
void Anti_Debugg_Dialog::ModefyAllShellCode()
{
	//�޸�AddressOfEntryPoint
	pOptionHeader->AddressOfEntryPoint = AfterEP;

	DWORD MessageBoxAddr = (DWORD)GetProcAddress(GetModuleHandle("USER32.dll"), "MessageBoxA");
	DWORD ExitProcessAddr = (DWORD)GetProcAddress(GetModuleHandle("KERNEL32.dll"), "ExitProcess");
	//MessageBox��ַ-StartAddr-0x32,,off:0x2E
	MessageBoxAddr = MessageBoxAddr - StartTextShellCode - 0x32;
	*((DWORD*)(ShellCode + 0x2E)) = MessageBoxAddr; 
	//ExitProcess��ַ-StartAddr-0x43,off:0x3F
	ExitProcessAddr = ExitProcessAddr - StartTextShellCode - 0x43;
	*((DWORD*)(ShellCode + 0x3F)) = ExitProcessAddr; 
	//�޸�SEH������StartAddr + 0x4C, off:0x1
	*((DWORD*)(ShellCode + 0x1)) = StartTextShellCode + 0x4C;

	//�޸����ݵ�ַ  (���ݵ�ַ,off:0x27)
	*((DWORD*)(ShellCode + 0x27)) = AfterRVAData; 

	//SEH������εط� StartAddr+0x22,off:0x62
	*((DWORD*)(ShellCode + 0x62)) = StartTextShellCode + 0x22; 

	//�޸�ԭOEP��ַ ԭOEP,off:0x6E
	*((DWORD*)(ShellCode + 0x6E)) = AddrEP + ImageBase; 
	//��Shellcodeд�뵽�ڴ���
	memcpy(((PBYTE)pFileBuffer+FoaEP),ShellCode,118);
	return;
}
//���ڴ�д�뵽������
DWORD Anti_Debugg_Dialog::WriterToPEFileAndDisk(char* name)
{
	char SzExePath[0x100];
	memset(SzExePath,0,0x100);
	GetModuleFileName(NULL, SzExePath, 0x100);
	int Pos=Pro_Text.GerGangPos(SzExePath);
	int i, j;
	for (i = Pos+1, j = 0; j < strlen(name); i++, j++)
	{
		SzExePath[i] = name[j];
	}
	for (j = 0; j < 0x20; j++,i++)
		SzExePath[i] = 0;
	//���ļ�
	FILE* fp = fopen(SzExePath, "wb");
	//�жϣ�д��
	if (fp != NULL)
	{
		fwrite(pFileBuffer, FileSize + FileSizeAdd, 1, fp);
	}
	else
	{
		printf("MemoryToFile �ļ���ʧ��\n");
		return 0;
	}
	fclose(fp);
	return 1;
}
LPVOID Anti_Debugg_Dialog::AntiPEFileToMemory(LPSTR lpszFile, int IsAdd)
{
	FILE *pFile = NULL;
	LPVOID pFileBuffer1= NULL;
	pFile = fopen(lpszFile, "rb");
	if (!pFile)
	{
		::MessageBoxA(0, "�޷��� EXE �ļ�!", "", 1);
		return 0;
	}
	//��ȡ�ļ���С
	fseek(pFile, 0, SEEK_END);
	FileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	//���仺����
	pFileBuffer1 = malloc(FileSize);
	if (!pFileBuffer1)
	{
		::MessageBoxA(0, "����ռ�ʧ��!", "", 1);
		fclose(pFile);
		return NULL;
	}
	//���ļ����ݶ�ȡ��������
	size_t n = fread(pFileBuffer1, FileSize, 1, pFile);
	if (IsAdd && n)
	{
		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer1;
		PIMAGE_OPTIONAL_HEADER32 pOPTIONALHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pFileBuffer1 + pDosHeader->e_lfanew + 0x4 + IMAGE_SIZEOF_FILE_HEADER);
		FileSizeAdd = pOPTIONALHeader->FileAlignment;
		pFileBuffer = malloc(FileSize + FileSizeAdd);
		memset(pFileBuffer, 0, FileSize + FileSizeAdd);
		free(pFileBuffer1);
		if (!pFileBuffer)
		{
			::MessageBoxA(0, "����ռ�ʧ��!", "", 0);
			fclose(pFile);
			return NULL;
		}
		pFile = fopen(lpszFile, "rb");
		if (!pFile)
		{
			::MessageBoxA(0, "�޷��� EXE �ļ�!", "", 1);
			return 0;
		}
		n = fread(pFileBuffer, FileSize, 1, pFile);
	}
	if (!n)
	{
		::MessageBoxA(0, "��ȡ����ʧ��!", "", 0);
		free(pFileBuffer);
		fclose(pFile);
		return NULL;
	}
	//�ر��ļ�
	fclose(pFile);
	return pFileBuffer;
}
//���SEH������
void Anti_Debugg_Dialog::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString ProPath;
	mExeEditBrowse.GetWindowText(ProPath);
	char* path = DataHs.CStringToCharSz(ProPath);
	pFileBuffer = AntiPEFileToMemory(path, 1);
	FindEmptyCode();
	if (!FindEmptyDataAndAddData())
	{
		AfxMessageBox("û���ҵ��հ����������޷���ӷ�����");
		return;
	}
	ModefyAllShellCode();
	if (WriterToPEFileAndDisk("seh_add.exe"))
	{
		AfxMessageBox("�ɹ����SEH�����ԣ�������Ϊseh_add.exe");
		return;
	}
	return;
}

void Anti_Debugg_Dialog::TLSModefyMem(char* path)
{
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory_TLS = NULL;
	PIMAGE_TLS_DIRECTORY32 pTLS = NULL;
	AntiPEFileToMemory(path, 1);
	//��ȡĿ¼��ṹ���޸�ֵ
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + pDosHeader->e_lfanew + 0x4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	ImageBase = pOptionHeader->ImageBase;
	SectionNum = pPEHeader->NumberOfSections;
	//��λ�����νṹ��
	PIMAGE_SECTION_HEADER tpSectionHeader = pSectionHeader;
	for (int i = 0; i < SectionNum; i++)
	{
		tpSectionHeader++;
	}
	tpSectionHeader--;
	tpSectionHeader->Characteristics = 0xE0000060;
	tpSectionHeader->SizeOfRawData += pOptionHeader->FileAlignment;
	//�ҵ�TLSĿ¼
	pDataDirectory = (PIMAGE_DATA_DIRECTORY)pOptionHeader->DataDirectory;
	pDataDirectory_TLS = (PIMAGE_DATA_DIRECTORY)&pDataDirectory[9];
	pDataDirectory_TLS->Size = 0x18;
	DWORD FileBeforeSize = FileSize;
	pDataDirectory_TLS->VirtualAddress = DataHs.FoaToRva(pFileBuffer, FileBeforeSize);
	//���ļ�ĩβ���TLS�ṹ��Ϣ
	DWORD StartAddr = pDataDirectory_TLS->VirtualAddress + ImageBase;
	pTLS = (PIMAGE_TLS_DIRECTORY32)((DWORD)pFileBuffer + FileBeforeSize);
	pTLS->StartAddressOfRawData = StartAddr + 0x18;
	pTLS->EndAddressOfRawData = pTLS->StartAddressOfRawData + 0x4;
	pTLS->AddressOfIndex = pTLS->EndAddressOfRawData + 0x4;
	pTLS->AddressOfCallBacks = pTLS->AddressOfIndex + 0x4;
	pTLS->SizeOfZeroFill = 0x0;
	pTLS->Characteristics = 0x0;
	//��������������ĵ�ַ���޸�shellcode
	DWORD MessageBoxAddr = (DWORD)GetProcAddress(GetModuleHandle("USER32.dll"), "MessageBoxA");
	DWORD ExitProcessAddr = (DWORD)GetProcAddress(GetModuleHandle("KERNEL32.dll"), "ExitProcess");
	//MessageBoxA��ַ-startaddr-0x27,off:0x23
	MessageBoxAddr = MessageBoxAddr - StartAddr - 0x27;
	*((DWORD*)(TLSCode + 0x23)) = MessageBoxAddr;
	//ExitProcess��ַ-startaddr-0x2F,off:0x2B
	ExitProcessAddr = ExitProcessAddr - StartAddr - 0x2F;
	*((DWORD*)(TLSCode + 0x2B)) = ExitProcessAddr;
	//�ַ���1��startaddr+0x70��off��0x16
	*((DWORD*)(TLSCode + 0x16)) = StartAddr+0x70;
	//�ַ���2��startaddr+0x80,off:0x1B
	*((DWORD*)(TLSCode + 0x1B)) = StartAddr + 0x80;
	//callback��ַ
	*((DWORD*)((PBYTE)pTLS+ 0x24)) = StartAddr + 0x30;
	//����shellcode
	memcpy(((PBYTE)pTLS + 0x30), TLSCode, 0x32);
	//�����ַ���1
	char* c1 = "TLS CALLBACK";
	memcpy(((PBYTE)pTLS + 0x70), c1, strlen(c1));
	//�����ַ���2
	char* c2 = "debugging";
	memcpy(((PBYTE)pTLS + 0x80), c2, strlen(c2));
	return;
}
//���TLS������
void Anti_Debugg_Dialog::OnBnClickedButton6()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString ProPath;
	mExeEditBrowse.GetWindowText(ProPath);
	char* path = DataHs.CStringToCharSz(ProPath);
	TLSModefyMem(path);
	if (WriterToPEFileAndDisk("Tls_add.exe"))
	{
		AfxMessageBox("�ɹ����TLS�����ԣ�������ΪTls_add.exe");
		return;
	}
	return;
}
