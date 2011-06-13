#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Rotozoomer3.h"
#include "MainFrm.h"
#include "Rotozoomer3Doc.h"
#include "Rotozoomer3View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CRotozoomer3App, CWinAppEx)
   ON_COMMAND(ID_APP_ABOUT, &CRotozoomer3App::OnAppAbout)
END_MESSAGE_MAP()

CRotozoomer3App::CRotozoomer3App() :
   m_bHiColorIcons(TRUE) {

   // TODO: replace application ID string below with unique ID string; recommended
   // format for string is CompanyName.ProductName.SubProduct.VersionInformation
   SetAppID(_T("Rotozoomer3.AppID.NoVersion"));

   // TODO: add construction code here,
   // Place all significant initialization in InitInstance
}

// The one and only CRotozoomer3App object
CRotozoomer3App theApp;

BOOL CRotozoomer3App::InitInstance() {
   // InitCommonControlsEx() is required on Windows XP if an application
   // manifest specifies use of ComCtl32.dll version 6 or later to enable
   // visual styles.  Otherwise, any window creation will fail.
   INITCOMMONCONTROLSEX InitCtrls;
   InitCtrls.dwSize = sizeof(InitCtrls);
   // Set this to include all the common control classes you want to use
   // in your application.
   InitCtrls.dwICC = ICC_WIN95_CLASSES;
   InitCommonControlsEx(&InitCtrls);

   CWinAppEx::InitInstance();

   // Initialize OLE libraries
   if (!AfxOleInit()) {
      AfxMessageBox(IDP_OLE_INIT_FAILED);
      return FALSE;
   }

   EnableTaskbarInteraction(FALSE);

   // Standard initialization
   // If you are not using these features and wish to reduce the size
   // of your final executable, you should remove from the following
   // the specific initialization routines you do not need
   // Change the registry key under which our settings are stored
   // TODO: You should modify this string to be something appropriate
   // such as the name of your company or organization
   SetRegistryKey(_T("explodingcoder.com"));

   InitContextMenuManager();
   InitKeyboardManager();
   InitTooltipManager();
   CMFCToolTipInfo ttParams;
   ttParams.m_bVislManagerTheme = TRUE;
   theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
      RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

   // Register the application's document templates.  Document templates
   //  serve as the connection between documents, frame windows and views
   CSingleDocTemplate *pDocTemplate = new CSingleDocTemplate(
      IDR_MAINFRAME,
      RUNTIME_CLASS(CRotozoomer3Doc),
      RUNTIME_CLASS(CMainFrame),
      RUNTIME_CLASS(CRotozoomer3View));
   if (!pDocTemplate) return FALSE;
   AddDocTemplate(pDocTemplate);

   // Parse command line for standard shell commands, DDE, file open
   CCommandLineInfo cmdInfo;
   ParseCommandLine(cmdInfo);

   // Dispatch commands specified on the command line.  Will return FALSE if
   // app was launched with /RegServer, /Register, /Unregserver or /Unregister.
   if (!ProcessShellCommand(cmdInfo))
      return FALSE;

   // The one and only window has been initialized, so show and update it
   m_pMainWnd->ShowWindow(SW_SHOW);
   m_pMainWnd->UpdateWindow();

   // call DragAcceptFiles only if there's a suffix
   //  In an SDI app, this should occur after ProcessShellCommand
   return TRUE;
}

int CRotozoomer3App::ExitInstance() {
   //TODO: handle additional resources you may have added
   AfxOleTerm(FALSE);

   return CWinAppEx::ExitInstance();
}

class CAboutDlg : public CDialogEx {
public:
   CAboutDlg();

// Dialog Data
   enum { IDD = IDD_ABOUTBOX };

protected:
   virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support

// Implementation
protected:
   DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
   CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

void CRotozoomer3App::OnAppAbout() {
   CAboutDlg aboutDlg;
   aboutDlg.DoModal();
}

void CRotozoomer3App::PreLoadState() {
   BOOL bNameValid;
   CString strName;
   bNameValid = strName.LoadString(IDS_EDIT_MENU);
   ASSERT(bNameValid);
   GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CRotozoomer3App::LoadCustomState() {
}

void CRotozoomer3App::SaveCustomState() {
}

// Provide idle time processing to any number of handlers.
// Implement IIdleHandler and add its pointer to CRotozoomer3App::IdleHandlers vector.
BOOL CRotozoomer3App::OnIdle(LONG lCount) {
   if (__super::OnIdle(lCount)) return TRUE;

   for (std::vector<IIdleHandler *>::iterator iter = IdleHandlers.begin(); iter != IdleHandlers.end(); iter++) {
      IIdleHandler *pIdle = *iter;
      ASSERT(pIdle);
      if (pIdle->OnIdle(lCount)) return TRUE;
   }

   return FALSE;
}
