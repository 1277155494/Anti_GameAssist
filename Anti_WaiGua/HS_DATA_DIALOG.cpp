// HS_DATA_DIALOG.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Anti_WaiGua.h"
#include "HS_DATA_DIALOG.h"
#include "afxdialogex.h"
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
using namespace std;
// HS_DATA_DIALOG �Ի���
#ifdef _DEBUG
#endif

IMPLEMENT_DYNAMIC(HS_DATA_DIALOG, CDialogEx)

HS_DATA_DIALOG::HS_DATA_DIALOG(CWnd* pParent /*=NULL*/)
	: CDialogEx(HS_DATA_DIALOG::IDD, pParent)
{

}

HS_DATA_DIALOG::~HS_DATA_DIALOG()
{

}
BOOL HS_DATA_DIALOG::OnInitDialog()
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
	
	//HS_DATA_DIALOG::OnBnClickedOk();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}
void HS_DATA_DIALOG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, mExeEditBrowse);
	DDX_Control(pDX, IDC_LIST2, mProListLeft);
	DDX_Control(pDX, IDC_LIST3, mProListRight);
}


BEGIN_MESSAGE_MAP(HS_DATA_DIALOG, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &HS_DATA_DIALOG::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE1, &HS_DATA_DIALOG::OnEnChangeMfceditbrowse1)
	ON_BN_CLICKED(IDC_BUTTON2, &HS_DATA_DIALOG::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &HS_DATA_DIALOG::OnBnClickedButton3)
END_MESSAGE_MAP()

// HS_DATA_DIALOG ��Ϣ�������
LPVOID HS_DATA_DIALOG::PEFileToMemory(LPSTR lpszFile)
{
	FILE *pFile = NULL;
	LPVOID pFileBuffer = NULL;
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
	pFileBuffer = malloc(FileSize);

	if (!pFileBuffer)
	{
		::MessageBoxA(0, "����ռ�ʧ��!", "", 1);
		fclose(pFile);
		return NULL;
	}
	//���ļ����ݶ�ȡ��������
	size_t n = fread(pFileBuffer, FileSize, 1, pFile);
	if (!n)
	{
		::MessageBoxA(0, "��ȡ����ʧ��!", "", 1);
		free(pFileBuffer);
		fclose(pFile);
		return NULL;
	}
	//�ر��ļ�
	fclose(pFile);
	return pFileBuffer;
}
// FOAת��ΪRVA
DWORD HS_DATA_DIALOG::FoaToRva(PVOID PeBuffer, DWORD Foa)
{
	//�����Ҫ�ı���
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	DWORD SectionNum = 0;


	if (PeBuffer == NULL)
	{
		printf("(FoaToRva)PE bufferΪ��\n");
		return 0;
	}
	if (*((PWORD)(PeBuffer)) != IMAGE_DOS_SIGNATURE)
	{
		printf("(FoaToRva)������Ч��EXE�ļ�\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)PeBuffer;

	if (*((PDWORD)((DWORD)pDosHeader + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE)
	{
		printf("(FoaToRva)������Ч��PE�ļ�\n");
		return 0;
	}
	//PE��׼ͷ����
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 0x4);
	SectionNum = pPEHeader->NumberOfSections;
	//PE��ѡͷ���� PIMAGE_OPTIONAL_HEADER32 pOptionHeader
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); 
	//��ȡͷ�ڱ�ṹ
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	//����ͷ�ڱ�ṹ���жϽڱ�
	PIMAGE_SECTION_HEADER tpSectionHeader = pSectionHeader;

	if (Foa <= pOptionHeader->SizeOfHeaders)  
		return (DWORD)Foa;
	else
	for (int i = 0; i<SectionNum; i++)
	{
		if (Foa >= tpSectionHeader->PointerToRawData&&Foa <= tpSectionHeader->PointerToRawData + tpSectionHeader->SizeOfRawData)
		{
			return Foa - tpSectionHeader->PointerToRawData + tpSectionHeader->VirtualAddress;
		}
		tpSectionHeader++;
	}
	return 0;
}
// RVAת��ΪFOA
DWORD HS_DATA_DIALOG::RvaToFoa(PVOID PeBuffer, DWORD Rva)
{
	//�����Ҫ�ı���
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	DWORD SectionNum = 0;

	if (PeBuffer == NULL)
	{
		printf("(RvaToFoa)PE bufferΪ��\n");
		return 0;
	}

	if (*((PWORD)PeBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("(RvaToFoa)������Ч��EXE�ļ�\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)PeBuffer;

	if (*((PWORD)((DWORD)pDosHeader + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE)
	{
		printf("(RvaToFoa)������Ч��PE�ļ�\n");
		return 0;
	}
	//PE�ļ�ͷ����
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 0x4);
	SectionNum = pPEHeader->NumberOfSections;
	//PE��ѡͷ���� PIMAGE_OPTIONAL_HEADER32 pOptionHeader
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); 
	//��ȡͷ�ڱ�ṹ
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	//����ͷ�ڱ�ṹ���жϽڱ�
	PIMAGE_SECTION_HEADER tpSectionHeader = pSectionHeader;

	if (Rva <= pOptionHeader->SizeOfHeaders)  
		return (DWORD)Rva;
	else
	for (int i = 0; i<SectionNum; i++)
	{
		if (Rva >= tpSectionHeader->VirtualAddress&&Rva <= tpSectionHeader->VirtualAddress + tpSectionHeader->Misc.VirtualSize)
		{
			return Rva - tpSectionHeader->VirtualAddress + tpSectionHeader->PointerToRawData;
		}
		tpSectionHeader++;
	}
	return 0;
}
BOOL HS_DATA_DIALOG::TraverseFuncAndData(LPVOID pFileBuffer)
{
	// ��ʼ��PEͷ���ṹ��
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL,tpSectionHeader=NULL;
	// ��ʼ��IMAGE_BUFFERָ��(temparay)
	LPVOID pTempImagebuffer = NULL;

	if (!pFileBuffer)
	{
		::MessageBoxA(0, "(TraverseFuncAndData)��ȡ���ڴ��pfilebuffer��Ч", "", 2);
		return FALSE;
	}
	// �ж��Ƿ��ǿ�ִ���ļ�
	if (*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)  
	{
		::MessageBoxA(0, "(TraverseFuncAndData)����MZ��־������exe�ļ���", "", 2);
		return FALSE;
	}
	//ǿ�ƽṹ������ת��pDosHeader
	pDosHeader = PIMAGE_DOS_HEADER(pFileBuffer);
	//�ж��Ƿ�ΪPE��־
	if (*((PDWORD)((DWORD)pFileBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE) 
	{			
		::MessageBoxA(0, "(TraverseFuncAndData)������Ч��PE��־��", "", 2);
		return FALSE;
	}
	// ǿ�ƽṹ������ת��
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	pPEFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEFileHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEFileHeader->SizeOfOptionalHeader);
	tpSectionHeader = pSectionHeader;
	//�������ʼ��ַ
	TextStartAddress = pSectionHeader->PointerToRawData;
	TextEndAddress = pSectionHeader->PointerToRawData + pSectionHeader->SizeOfRawData;
	//����OEP
	OEP = pOptionHeader->AddressOfEntryPoint + pOptionHeader->ImageBase;
	ImageBase = pOptionHeader->ImageBase;
	int Funi = 0;
	int Datai = 0;
	//Ѱ�Һ���λ��
	for (DWORD i = TextStartAddress; i < TextEndAddress-2; i++)
	{
		//::MessageBoxA(0,"����ѭ��","",1);
		//��ȡ����ͷ����Ϣ
		pFuncHeader = (_FuncHeader*)((DWORD)pFileBuffer + i);
		//�ж��Ƿ�Ϊ����ͷ��
		if (pFuncHeader->p1 == 0x55 && pFuncHeader->p2 == 0x89 && pFuncHeader->p3 == 0xE5 || pFuncHeader->p1 == 0x53 && pFuncHeader->p2 == 0x56 && pFuncHeader->p3 == 0x57 && pFuncHeader->p4 == 0x55 || pFuncHeader->p1 == 0x55 && pFuncHeader->p2 == 0x8B && pFuncHeader->p3 == 0xEC)
		{
			//��FOA��ַת����VA
			DWORD FuncAddr = FoaToRva(pFileBuffer, i) + ImageBase;
			if (FuncAddr>OEP)
			{
				//ModFunAddr[Funi++] = FuncAddr;
				CString str;
				str.Format("Func:%x",FuncAddr);
				//���б��������
				int count=mProListLeft.AddString(str);
				//����FOAֵ
				mProListLeft.SetItemData(count, i);
			}
			//::MessageBoxA(0, "mProListLeft.AddString", "", 1);
		}
		
	}
	DWORD DataAddr;
	DWORD DataEnd;
	//�ҵ����ݶε���ʼ�ͽ���λ��
	for (int i = 0; i < pPEFileHeader->NumberOfSections; i++, tpSectionHeader++)
	{
		int num = 0;
		char* name01 = ".data";
		char name[9] = { 0 };
		memcpy(name, tpSectionHeader->Name, 8);
		if (!strcmp(name, name01))
		{
			//��ʼ��ַ
			DataAddr = FoaToRva(pFileBuffer, tpSectionHeader->PointerToRawData) + ImageBase;
			//������ַ
			DataEnd = FoaToRva(pFileBuffer, tpSectionHeader->PointerToRawData + tpSectionHeader->SizeOfRawData) + ImageBase;
		}
	}
	//��������Σ��ҵ�movָ��
	for (DWORD i = TextStartAddress; i < TextEndAddress - 2; i++)
	{
		BYTE* IsMov = (BYTE*)((DWORD)pFileBuffer + i);
		DWORD* MovValue = (DWORD*)((DWORD)pFileBuffer + i + 1);
		//0xB8��0xBFΪmovָ���Ӳ����
		if (0xB8 <= *IsMov&&*IsMov<0xC0)
		{
			if (DataAddr <= *MovValue&&*MovValue<DataEnd)
			{
				BYTE* DataLast = (BYTE*)((DWORD)pFileBuffer + i + 1);
				CString str;
				str.Format("Data:%x", *MovValue);
				//���б��������
				int count = mProListLeft.AddString(str);
				//����������ֵ
				mProListLeft.SetItemData(count, i+1);

			}
		}
	}
	return true;
}
//��ѡ�е�����ӵ��ұߵ��б���
void HS_DATA_DIALOG::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//��ʾ���б�(����ʾϵͳ��API)�У���ʾ����,������ӹ���
	CString str;
	int index = mProListLeft.GetCurSel();
	if (index == -1)
	{
		::MessageBoxA(0,"��ѡ��Ҫ�����ĺ���������","��ܰ��ʾ",0);
		return;
	}
	//���û���ظ�ѡ
	if (IsSelect[index] == -1)
	{
		mProListLeft.GetText(index, str);
		int count = mProListRight.AddString(str);
		DWORD value=mProListLeft.GetItemData(index);
		//����������ֵ
		mProListRight.SetItemData(count, value);
		IsSelect[index] = 1;
	}
	//���������ѡ
	else
	{
		::MessageBoxA(0, "�����������ұ��б���", "��ܰ��ʾ", 0);
	}
	return ;
}

//��EXE�ļ��еĺ�����������ʾ������б���
void HS_DATA_DIALOG::OnEnChangeMfceditbrowse1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//�õ�����·��
	mExeEditBrowse.GetWindowText(ExePath);
	CStringA tmp = ExePath;
	char* pp = tmp.GetBuffer();
	//�����б�����
	mProListLeft.ResetContent();
	mProListRight.ResetContent();
	//��exe�ļ����ص��ڴ���
	pPeBuffer = PEFileToMemory(pp);
	//������б�����Ӻ�����������Ϣ
	TraverseFuncAndData(pPeBuffer);
	memset(IsSelect,-1,SZLEN);
	return;
}
DWORD HS_DATA_DIALOG::ReverseData(DWORD value)
{
	DWORD ans = 0;
	DWORD unit[4] = { 0x1000000, 0x10000, 0x100,0x1};
	DWORD Mod = 0x100;
	for (int i = 0; i < 4; i++)
	{
		byte b = value%Mod;
		ans += unit[i] * b;
		value /= Mod;
	}
	return ans;
}
CHAR* HS_DATA_DIALOG::DWORDToString(DWORD value)
{
	DWORD tvalue = value;
	char ans[10];
	char* tans = (char*)malloc(10);
	memset(ans, '\0', 10);
	//�����±������ֵ��ͬ
	char H[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	int pos = 0;
	while (tvalue % 0x10)
	{
		BYTE end = tvalue % 0x10;
		for (int j = 0; j<16; j++)
		{
			if (end == j)
			{
				ans[pos] = H[j];
				pos++;
			}
		}
		tvalue /= 0x10;
	}
	int i = 0;
	for (int j = strlen(ans) - 1; j>-1; j--, i++)
	{
		tans[i] = ans[j];
	}
	tans[i] = '\0';
	return tans;
}
DWORD HS_DATA_DIALOG::CharToDword(char* value)
{
	DWORD ans = 0;
	//�����±������ֵ��ͬ
	char H[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	char h[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	int len = strlen(value);
	int pos = 1;
	for (int i = len - 1; i >= 0; i--)
	{
		for (int j = 0; j<16; j++)
		{
			if (h[j] == value[i] || H[j] == value[i])
			{
				ans += j*pos;
				pos *= 0x10;
			}
		}
	}
	return ans;
}
CHAR* HS_DATA_DIALOG::CStringToCharSz(CString value)
{
	char* p = (char*)malloc(0x100);
	memset(p,0,0x100);
	memcpy(p, value, value.GetLength());
	return p;
}
//�޸ĺ�����ַ�����ݵ�ַ
void HS_DATA_DIALOG::OnBnClickedButton2()
{
	if (!mProListRight.GetCount())
	{
		::MessageBoxA(0, "��ѡ��Ҫ�����ĺ���������", "��Ϣ",2);
	}
	//��ȡ�����еĺ��������ݵ�ַ�����������ֿ���
	CString Liststring;
	int Funi = 0, Datai = 0;
	for (int i = 0; i < mProListRight.GetCount(); i++)
	{
		mProListRight.GetText(i, Liststring);
		int len = Liststring.GetLength();
		if (Liststring.GetAt(0) == 'F')
		{
			//��ȡ������ַ
			CString Fun = Liststring.Right(len - 5);
			//CStringA tmp=
			char* ValueSz = CStringToCharSz(Fun);
			DWORD RealValue = CharToDword(ValueSz);			
			for (int j = TextStartAddress; j < TextEndAddress; j++)
			{
				BYTE* Diss = (BYTE*)((DWORD)pPeBuffer + j);
				if (*Diss == 0xE8)
				{
					//call��һ�л������ַ
					DWORD NextDiss = FoaToRva(pPeBuffer, j + 5) + ImageBase;
					DWORD* CodeContext = (DWORD*)((DWORD)pPeBuffer+j+1);
					BYTE* FunLastData = (BYTE*)((DWORD)pPeBuffer + j + 1);
					//call ��ӵ�ַ
					DWORD CallValue = RealValue - NextDiss;
					if (*CodeContext == CallValue)
					{
						//::MessageBoxA(0, "�޸�", "�ұ��б�ֵ", 2);
						AllFuncData[lenFun].FunAddr = FoaToRva(pPeBuffer, j + 1) + ImageBase;
						AllFuncData[lenFun++].RealData = *FunLastData;
						*FunLastData = rand() % 0x100;
					}
				}
			}
		}
		else if (Liststring.GetAt(0) == 'D')
		{
			//��ȡ���ݵ�ַ
			CString Data = Liststring.Right(len - 5);
			char* ValueSz = CStringToCharSz(Data);
			//::MessageBoxA(0, ValueSz, "Data ValueSz", 2);
			DWORD DataFoa = mProListRight.GetItemData(i);
			BYTE* LastByte = (BYTE*)((DWORD)pPeBuffer + DataFoa);
			//���޸Ľṹ�帳ֵ
			AllModData[lenData].ModAddr = FoaToRva(pPeBuffer, DataFoa) + ImageBase;
			AllModData[lenData++].RealData = *LastByte;
			*LastByte = rand() % 0x100;
		}
	}
	CStringA tmp;
	tmp = ExePath;
	char *Path = tmp.GetBuffer();
	int i = 0;
	string FuPath = "_Pro.exe";
	for (i = 0; i<strlen(Path)-4; i++)
	{
		tmpPath[i] = Path[i];
	}
	//�����ϱ�����
	for (int j = 0; j < 8; j++)
	{
		tmpPath[j + i] = FuPath.at(j);
	}
	//д�����
	if (!MemoryToFile(pPeBuffer, FileSize, tmpPath))
	{
		printf("MemoryToFile is fail\n");
	}
	AfxMessageBox(_T("PE�ļ������ɹ�"), MB_OKCANCEL | MB_ICONQUESTION, 0);
	return ;
}
BOOL HS_DATA_DIALOG::MemoryToFile(PVOID pPEbuffer, DWORD filesize, LPSTR filePath)
{
	FILE* fp;
	fp = fopen(filePath, "wb");
	if (!fp)
	{
		::MessageBoxA(0, "(MemoryToFile) �޷��� EXE �ļ�!", "", 1);
		return 0;
	}
	else
	{
		fwrite(pPEbuffer, filesize, 1, fp);
	}
	fclose(fp);
	return 1;
}

//�����ѱ������
void HS_DATA_DIALOG::OnBnClickedButton3()
{
	STARTUPINFO si = {};
	//�������̵���Ϣ�ṹ��
	PROCESS_INFORMATION pi = {};
	//�������̣�������
	CreateProcess(tmpPath, 0, 0, 0, FALSE, CREATE_SUSPENDED,0, 0, &si, &pi);
	DWORD dwWrite = 0;
	for (int i = 0; i<lenFun; i++)
	{
		if (!::WriteProcessMemory(pi.hProcess, (LPVOID)AllFuncData[i].FunAddr, &AllFuncData[i].RealData, 1, &dwWrite))
		{
			printf("Func WriteProcessMemory is fail\n");
		}
		else
			printf("Func WriteProcessMemory is success\n");
	}
	//�ָ�����
	for (int i = 0; i<lenData; i++)
	{
		if (!::WriteProcessMemory(pi.hProcess, (LPVOID)AllModData[i].ModAddr, &AllModData[i].RealData, 1, &dwWrite))
		{
			printf("Data WriteProcessMemory is fail\n");
		}
		else
			printf("Data WriteProcessMemory is success\n");
	}
	//���н���
	ResumeThread(pi.hThread);
	return ;
}
