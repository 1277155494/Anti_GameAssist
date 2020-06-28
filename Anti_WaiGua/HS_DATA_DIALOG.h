#pragma once
#include "afxeditbrowsectrl.h"
#include "afxwin.h"
#include <Windows.h>
#define SZLEN 0x400
// HS_DATA_DIALOG �Ի���

class HS_DATA_DIALOG : public CDialogEx
{
	DECLARE_DYNAMIC(HS_DATA_DIALOG)

public:
	HS_DATA_DIALOG(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~HS_DATA_DIALOG();
	virtual BOOL OnInitDialog();


	// �Ի�������
	enum { IDD = IDD_DIALOG_HS_DATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CMFCEditBrowseCtrl mExeEditBrowse;
	afx_msg void OnEnChangeMfceditbrowse1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg BOOL TraverseFuncAndData(LPVOID pPeBuffer);
	afx_msg CHAR* DWORDToString(DWORD value);
	afx_msg DWORD FoaToRva(PVOID PeBuffer, DWORD Foa);
	afx_msg DWORD RvaToFoa(PVOID PeBuffer, DWORD Rva);
	afx_msg DWORD ReverseData(DWORD data);
	afx_msg BOOL MemoryToFile(PVOID pPEbuffer, DWORD filesize, LPSTR filePath);
	afx_msg DWORD CharToDword(CHAR* value);
	afx_msg CHAR* CStringToCharSz(CString value);
	//��ȡPE�ļ���Ϣ���ڴ���
	LPVOID PEFileToMemory(LPSTR lpszFile);
private:
	//���������б�ؼ�
	CListBox mProListLeft;
	CListBox mProListRight;
	//�������ĩ��ַ
	DWORD TextStartAddress;
	DWORD TextEndAddress;
	DWORD DataAddr;
	DWORD DataLen;
	DWORD OEP;
	LPVOID pPeBuffer;
	CString ModExePath;
	//���޸ĵ�call��ַ�����ݵ�ַ��ֵΪFoa
	DWORD ModFunAddr[1024];
	DWORD ModDataAddr[1024];
	//�޸�ǰ��ֵ
	BYTE ModFunAddrValue[1024];
	BYTE ModDataAddrValue[1024];
	//PE�ļ���С
	DWORD FileSize;
	CString ExePath;
	//�޸ĺ�EXE·��
	char tmpPath[1024];
	DWORD ImageBase;
    struct _FuncHeader
	{
		BYTE p1, p2, p3, p4;

	}FuncHeader, *pFuncHeader;
	//pFuncHeader* pFuncHead;
	struct ModData
	{
		DWORD ModAddr;
		BYTE RealData;
	}AllModData[SZLEN];
	struct FuncData
	{
		DWORD FunAddr;
		BYTE RealData;
	}AllFuncData[SZLEN];
	DWORD lenFun;
	DWORD lenData;
	int IsSelect[SZLEN];
};
