#include "stdafx.h"
#include "Rotozoomer3.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
   ON_WM_CREATE()
   ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
   ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
END_MESSAGE_MAP()

CMainFrame::CMainFrame() :
   m_peakFPS(0) {
   theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_WINDOWS_7);
}

CMainFrame::~CMainFrame() {
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
   if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
      return -1;

   // set the visual manager and style based on persisted value
   OnApplicationLook(theApp.m_nAppLook);

   m_wndRibbonBar.Create(this);
   m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

   if (!m_wndStatusBar.Create(this)) {
      TRACE0("Failed to create status bar\n");
      return -1;      // fail to create
   }

   CMFCRibbonStatusBarPane *statusEl1 = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, NULL, TRUE);
   m_wndStatusBar.AddElement(statusEl1, _T(""));

   m_pStatusPane2 = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, NULL, TRUE);
   m_pStatusPane2->SetAlmostLargeText(_T("XXXXXXXXXXXXXXXXXXXX"));
   m_wndStatusBar.AddExtendedElement(m_pStatusPane2, _T(""));

   // enable Visual Studio 2005 style docking window behavior
   CDockingManager::SetDockingMode(DT_SMART);
   // enable Visual Studio 2005 style docking window auto-hide behavior
   EnableAutoHidePanes(CBRS_ALIGN_ANY);

   return 0;
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const {
   CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext &dc) const {
   CFrameWndEx::Dump(dc);
}
#endif //_DEBUG

void CMainFrame::OnApplicationLook(UINT id) {
   CWaitCursor wait;

   theApp.m_nAppLook = id;

   switch (theApp.m_nAppLook) {
   case ID_VIEW_APPLOOK_WIN_2000:
      CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
      m_wndRibbonBar.SetWindows7Look(FALSE);
      break;

   case ID_VIEW_APPLOOK_OFF_XP:
      CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
      m_wndRibbonBar.SetWindows7Look(FALSE);
      break;

   case ID_VIEW_APPLOOK_WIN_XP:
      CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
      CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
      m_wndRibbonBar.SetWindows7Look(FALSE);
      break;

   case ID_VIEW_APPLOOK_OFF_2003:
      CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
      CDockingManager::SetDockingMode(DT_SMART);
      m_wndRibbonBar.SetWindows7Look(FALSE);
      break;

   case ID_VIEW_APPLOOK_VS_2005:
      CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
      CDockingManager::SetDockingMode(DT_SMART);
      m_wndRibbonBar.SetWindows7Look(FALSE);
      break;

   case ID_VIEW_APPLOOK_VS_2008:
      CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
      CDockingManager::SetDockingMode(DT_SMART);
      m_wndRibbonBar.SetWindows7Look(FALSE);
      break;

   case ID_VIEW_APPLOOK_WINDOWS_7:
      CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
      CDockingManager::SetDockingMode(DT_SMART);
      m_wndRibbonBar.SetWindows7Look(TRUE);
      break;

   default:
      switch (theApp.m_nAppLook) {
      case ID_VIEW_APPLOOK_OFF_2007_BLUE:
         CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
         break;

      case ID_VIEW_APPLOOK_OFF_2007_BLACK:
         CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
         break;

      case ID_VIEW_APPLOOK_OFF_2007_SILVER:
         CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
         break;

      case ID_VIEW_APPLOOK_OFF_2007_AQUA:
         CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
         break;
      }

      CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
      CDockingManager::SetDockingMode(DT_SMART);
      m_wndRibbonBar.SetWindows7Look(FALSE);
   }

   RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

   theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI *pCmdUI) {
   pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::SetStatusFPS(double fps) {
   // Update status bar text with current and peak FPS values
   if (fps > m_peakFPS) m_peakFPS = fps;

   CString fpsStr;
   fpsStr.Format(_T("FPS: %0.2f, peak %0.2f"), fps, m_peakFPS);

   m_pStatusPane2->SetText(fpsStr);
   m_wndStatusBar.Invalidate();
}
