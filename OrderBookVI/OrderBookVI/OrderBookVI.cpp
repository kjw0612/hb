
// OrderBookVI.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "OrderBookVI.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "OrderBookVIDoc.h"
#include "OrderBookVIView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COrderBookVIApp

BEGIN_MESSAGE_MAP(COrderBookVIApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &COrderBookVIApp::OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// COrderBookVIApp ����

COrderBookVIApp::COrderBookVIApp()
{
	m_bHiColorIcons = TRUE;

	// TODO: �Ʒ� ���� ���α׷� ID ���ڿ��� ���� ID ���ڿ��� �ٲٽʽÿ�(����).
	// ���ڿ��� ���� ����: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("OrderBookVI.AppID.NoVersion"));

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

// ������ COrderBookVIApp ��ü�Դϴ�.

COrderBookVIApp theApp;


// COrderBookVIApp �ʱ�ȭ

BOOL COrderBookVIApp::InitInstance()
{
	
	CWinAppEx::InitInstance();

	EnableTaskbarInteraction();

	// RichEdit ��Ʈ���� ����Ϸ���  AfxInitRichEdit2()�� �־�� �մϴ�.	
	// AfxInitRichEdit2();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.


	InitContextMenuManager();
	InitShellManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ���� ���α׷��� ���� ���ø��� ����մϴ�. ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_OrderBookVITYPE,
		RUNTIME_CLASS(COrderBookVIDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(COrderBookVIView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// �� MDI ������ â�� ����ϴ�.
	CWinAppEx::CleanState();

	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// ����ٿ� ������ ����� ����ġ�մϴ�.
	// ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// �� â�� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int COrderBookVIApp::ExitInstance()
{
	//TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.
	return CWinAppEx::ExitInstance();
}

// COrderBookVIApp �޽��� ó����


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void COrderBookVIApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// COrderBookVIApp ����� ���� �ε�/���� �޼���

void COrderBookVIApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void COrderBookVIApp::LoadCustomState()
{
}

void COrderBookVIApp::SaveCustomState()
{
}

// COrderBookVIApp �޽��� ó����





void COrderBookVIApp::GetDocumentList(CObList * pDocList)
{
   ASSERT(pDocList->IsEmpty());

   POSITION pos = GetFirstDocTemplatePosition();

   while (pos)
   {
      CDocTemplate* pTemplate = (CDocTemplate*)GetNextDocTemplate(pos);
      POSITION pos2 = pTemplate->GetFirstDocPosition();
      while (pos2)
      {
         CDocument * pDocument;
         if ((pDocument=pTemplate->GetNextDoc(pos2)) != NULL)
            pDocList->AddHead(pDocument);
      }
   }
}
