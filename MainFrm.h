#pragma once

#include "Rotozoomer3View.h"

class CMainFrame : public CFrameWndEx {
protected:
   CMainFrame();
   virtual ~CMainFrame();
   DECLARE_DYNCREATE(CMainFrame)

public:
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext &dc) const;
#endif

   void SetStatusFPS(double fps);

protected:
   CMFCRibbonBar m_wndRibbonBar;
   CMFCRibbonApplicationButton m_MainButton;
   CMFCToolBarImages m_PanelImages;
   CMFCRibbonStatusBar m_wndStatusBar;
   CMFCRibbonStatusBarPane *m_pStatusPane2;
   double m_peakFPS;

   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnApplicationLook(UINT id);
   afx_msg void OnUpdateApplicationLook(CCmdUI *pCmdUI);
   DECLARE_MESSAGE_MAP()
};
