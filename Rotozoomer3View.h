#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "dxerr.lib")

#ifdef _DEBUG
#pragma comment (lib, "d3dx9d.lib")
#else
#pragma comment (lib, "d3dx9.lib")
#endif

class CRotozoomer3View : public CView, public IIdleHandler {
   enum { TIMER_FPS, TIMER_ANIMATE };

protected:
   CRotozoomer3View();
   virtual ~CRotozoomer3View();
   DECLARE_DYNCREATE(CRotozoomer3View)

private:
   bool m_RenderEnable;
   CRect m_clientRect;
   int m_FrameCounter;
   int m_FPSLastFrameCounter;
   DWORD m_FPSLastTickCount;

   // Direct3D resources
   struct {
      LPDIRECT3D9 pContext;
      D3DCAPS9 caps;
      LPDIRECT3DDEVICE9 pDevice;
      D3DDISPLAYMODE dm;
      LPDIRECT3DTEXTURE9 pRZImage;
      D3DXIMAGE_INFO imageDesc;
      D3DLOCKED_RECT lockedImageRect;
      LPDIRECT3DSURFACE9 pRZSurface;
   } m_d3d9;

   // View settings
   struct {
      double angle;
      double zoom;
      FPoint pivotPoint;
      FPoint referencePoint;
      int counter;
   } m_anim;

   // Calculated view settings
   struct {
      double horizdx;
      double horizdy;
      double vertdx;
      double vertdy;
      FPoint startPoint;
   } m_calc;

   void InitD3D(int cx, int cy);
   void Render(void);
   void CleanupD3D(void);
   void Animate(void);

public:
   LPDIRECT3DTEXTURE9 GetImage(void) const;
   void SetImage(UINT resId);
   void SetCoordinates(int viewcx, int viewcy, FPoint upperLeft, FPoint upperRight, FPoint lowerRight);
   void SetCoordinates(FPoint referencePoint, double zoom, FPoint pivotPoint, double angleRadians);
   BOOL OnIdle(LONG lCount);

protected:
   virtual void OnDraw(CDC *pDC);
   virtual void PreSubclassWindow();

   afx_msg BOOL OnEraseBkgnd(CDC *pDC);
   afx_msg void OnDestroy();
   afx_msg void OnTimer(UINT_PTR nIDEvent);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   DECLARE_MESSAGE_MAP()
};
