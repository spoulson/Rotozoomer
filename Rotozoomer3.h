#pragma once

#ifndef __AFXWIN_H__
   #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"
#include "FPoint.h"
#include <vector>

#define PI (3.14159265)

// Idle time handler interface
class IIdleHandler {
public:
   virtual BOOL OnIdle(LONG lCount) = 0;
};

class CRotozoomer3App : public CWinAppEx {
protected:
   int ExitInstance();

public:
   CRotozoomer3App();

   UINT m_nAppLook;
   BOOL m_bHiColorIcons;
   std::vector<IIdleHandler *> IdleHandlers;

   virtual BOOL InitInstance();
   virtual void PreLoadState();
   virtual void LoadCustomState();
   virtual void SaveCustomState();
   BOOL OnIdle(LONG lCount);

   afx_msg void OnAppAbout();
   DECLARE_MESSAGE_MAP()
};

extern CRotozoomer3App theApp;
