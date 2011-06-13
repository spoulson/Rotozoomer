#include "stdafx.h"
#include "Rotozoomer3.h"
#include "Rotozoomer3View.h"
#include "MainFrm.h"
#include <math.h>
#include <algorithm>

IMPLEMENT_DYNCREATE(CRotozoomer3View, CView)

BEGIN_MESSAGE_MAP(CRotozoomer3View, CView)
   ON_WM_ERASEBKGND()
   ON_WM_DESTROY()
   ON_WM_TIMER()
   ON_WM_SIZE()
END_MESSAGE_MAP()

CRotozoomer3View::CRotozoomer3View() :
   m_RenderEnable(false),
   m_clientRect(0, 0, 0, 0),
   m_FrameCounter(0),
   m_FPSLastFrameCounter(0),
   m_FPSLastTickCount(::GetTickCount()) {

   m_d3d9.pContext = NULL;
   m_d3d9.pDevice = NULL;
   m_d3d9.pRZImage = NULL;
   m_d3d9.pRZSurface = NULL;
   m_anim.angle = 0;
   m_anim.zoom = 1;
   m_anim.referencePoint = FPoint(0, 0);
   m_anim.pivotPoint = FPoint(0, 0);
   m_anim.counter = 0;
   m_calc.horizdx = 0;
   m_calc.horizdy = 0;
   m_calc.vertdx = 0;
   m_calc.vertdy = 0;
   m_calc.startPoint = FPoint(0, 0);
}

CRotozoomer3View::~CRotozoomer3View() {
}

void CRotozoomer3View::InitD3D(int cx, int cy) {
   HRESULT hr;

   m_RenderEnable = false;

   // Initialize context only once
   if (m_d3d9.pContext == NULL) {
      // Get Direct 3D context
      m_d3d9.pContext = Direct3DCreate9(D3D_SDK_VERSION);
      ASSERT(m_d3d9.pContext);

      // Get adapter display info
      if (FAILED(hr = m_d3d9.pContext->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_d3d9.dm)))
         DXTrace(__FILE__, __LINE__, hr, _T("GetAdapterDisplayMode()"), TRUE);

      // Check that device can be created
      if (FAILED(hr = m_d3d9.pContext->CheckDeviceFormat(D3DADAPTER_DEFAULT,
         D3DDEVTYPE_HAL, m_d3d9.dm.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16)))
         DXTrace(__FILE__, __LINE__, hr, _T("CheckDeviceFormat()"), TRUE);

      // Get device capabilities
      if (FAILED(hr = m_d3d9.pContext->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, &m_d3d9.caps)))
         DXTrace(__FILE__, __LINE__, hr, _T("GetDeviceCaps()"), TRUE);
   }

   // Create/reset device
   DWORD dwBehaviorFlags = m_d3d9.caps.VertexProcessingCaps ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING;
   D3DPRESENT_PARAMETERS d3dpp;
   ZeroMemory(&d3dpp, sizeof(d3dpp));
   d3dpp.BackBufferCount = 0;
   d3dpp.BackBufferWidth = cx;
   d3dpp.BackBufferHeight = cy;
   d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
   d3dpp.Windowed = TRUE;
   d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

   if (m_d3d9.pDevice) {
      if (FAILED(hr = m_d3d9.pDevice->Reset(&d3dpp)))
         DXTrace(__FILE__, __LINE__, hr, _T("Reset()"), TRUE);
   }
   else {
      if (FAILED(hr = m_d3d9.pContext->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetSafeHwnd(),
      dwBehaviorFlags, &d3dpp, &m_d3d9.pDevice)))
      DXTrace(__FILE__, __LINE__, hr, _T("CreateDevice()"), TRUE);
   }
   ASSERT(m_d3d9.pDevice);

   // Create offscreen buffer in system memory the same size as the viewport
   if (m_d3d9.pRZSurface) {
      m_d3d9.pRZSurface->Release();
      m_d3d9.pRZSurface = NULL;
   }
   if (FAILED(hr = m_d3d9.pDevice->CreateOffscreenPlainSurface(cx, cy,
      D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &m_d3d9.pRZSurface, NULL)))
      DXTrace(__FILE__, __LINE__, hr, _T("CreateOffscreenPlainSurface()"), TRUE);
   ASSERT(m_d3d9.pRZSurface);

   m_RenderEnable = m_d3d9.pRZImage && m_d3d9.pRZSurface;
}

void CRotozoomer3View::Render(void) {
   if (!m_RenderEnable) return;

   HRESULT hr;

   // Viewport size
   int scx = m_clientRect.Width();
   int scy = m_clientRect.Height();

   if (FAILED(hr = m_d3d9.pDevice->BeginScene()))
      DXTrace(__FILE__, __LINE__, hr, _T("BeginScene()"), TRUE);
   else {

      // Draw directly to surface
      D3DLOCKED_RECT lockedRect = { 0 };
      if (FAILED(hr = m_d3d9.pRZSurface->LockRect(&lockedRect, &m_clientRect, 0)))
         DXTrace(__FILE__, __LINE__, hr, _T("LockRect()"), TRUE);
      else {
         int tcx = m_d3d9.imageDesc.Width;
         int tcy = m_d3d9.imageDesc.Height;
         int osbPitchPixels = lockedRect.Pitch / sizeof(DWORD);
         int imgPitchPixels = m_d3d9.lockedImageRect.Pitch / sizeof(DWORD);
         DWORD *pOsbBits = reinterpret_cast<DWORD *>(lockedRect.pBits);
         DWORD *pImgBits = reinterpret_cast<DWORD *>(m_d3d9.lockedImageRect.pBits);
         FPoint t(m_calc.startPoint);

         for (int sy = 0; sy < scy; sy++) {
            FPoint t2(t);
            DWORD *pOsbBitsx = pOsbBits;

            for (int sx = 0; sx < scx; sx++) {
               int px = (int)t2.x;
               int py = (int)t2.y;
   
               // Draw pixel
               DWORD readOffset = px + (py * imgPitchPixels);
               *pOsbBitsx = pImgBits[readOffset];
   
               t2.x += m_calc.horizdx;
               if (t2.x >= tcx) t2.x -= tcx;
               t2.y += m_calc.horizdy;
               if (t2.y >= tcy) t2.y -= tcy;
               pOsbBitsx++;
            }
   
            t.x += m_calc.vertdx;
            if (t.x >= tcx) t.x -= tcx;
            t.y += m_calc.vertdy;
            if (t.y >= tcy) t.y -= tcy;
            pOsbBits += osbPitchPixels;
         }

         if (FAILED(hr = m_d3d9.pRZSurface->UnlockRect()))
            DXTrace(__FILE__, __LINE__, hr, _T("UnlockRect()"), TRUE);
      }

      // Copy offscreen buffer to screen
      LPDIRECT3DSURFACE9 pScreenSurface;
      if (FAILED(hr = m_d3d9.pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pScreenSurface)))
         DXTrace(__FILE__, __LINE__, hr, _T("GetBackBuffer()"), TRUE);
      if (FAILED(hr = m_d3d9.pDevice->UpdateSurface(m_d3d9.pRZSurface, NULL, pScreenSurface, NULL)))
         DXTrace(__FILE__, __LINE__, hr, _T("UpdateSurface()"), TRUE);
      pScreenSurface->Release();

      // Done drawing
      if (FAILED(hr = m_d3d9.pDevice->EndScene()))
         DXTrace(__FILE__, __LINE__, hr, _T("EndScene()"), TRUE);
   }

   if (FAILED(m_d3d9.pDevice->Present(NULL, NULL, NULL, NULL)))
      DXTrace(__FILE__, __LINE__, hr, _T("Present()"), TRUE);

   m_FrameCounter++;
}

void CRotozoomer3View::CleanupD3D(void) {
   m_RenderEnable = false;

   if (m_d3d9.pRZImage) {
      m_d3d9.pRZImage->UnlockRect(0);
      m_d3d9.pRZImage->Release();
      m_d3d9.pRZImage = NULL;
   }
   if (m_d3d9.pRZSurface) {
      m_d3d9.pRZSurface->Release();
      m_d3d9.pRZSurface = NULL;
   }
   if (m_d3d9.pDevice) {
      m_d3d9.pDevice->Release();
      m_d3d9.pDevice = NULL;
   }
   if (m_d3d9.pContext) {
      m_d3d9.pContext->Release();
      m_d3d9.pContext = NULL;
   }
}

void CRotozoomer3View::Animate(void) {
   // Swing angle on sine wave.
   // Swing zoom on two overlapped sines.
   const int angleDelay = 1000;
   const int zoomDelay1 = 400;
   const double zoomFactor1 = 1;
   const int zoomDelay2 = 800;
   const double zoomFactor2 = 0.25;
   m_anim.angle = fmod((m_anim.counter % angleDelay) * 2 * PI / angleDelay, 2 * PI);
   m_anim.zoom = sin((m_anim.counter % zoomDelay1) * 2 * PI / zoomDelay1) * zoomFactor1 + zoomFactor1 +
      sin((m_anim.counter % zoomDelay2) * 2 * PI / zoomDelay2) * zoomFactor2 + zoomFactor2;
   
   // Fix pivot point in middle of viewport.
   m_anim.pivotPoint.x = m_clientRect.Width() / 2;
   m_anim.pivotPoint.y = m_clientRect.Height() / 2;

   // Pan image around in a circle.
   const int referencePointDelay = 2000;
   const int referencePointRadiusX = 10 * m_d3d9.imageDesc.Width;
   const int referencePointRadiusY = 10 * m_d3d9.imageDesc.Height;
   m_anim.referencePoint.x = fmod(
      sin((m_anim.counter % referencePointDelay) * 2 * PI / referencePointDelay) * referencePointRadiusX + referencePointRadiusX,
      m_d3d9.imageDesc.Width);
   m_anim.referencePoint.y = fmod(
      cos((m_anim.counter % referencePointDelay) * 2 * PI / referencePointDelay) * referencePointRadiusY + referencePointRadiusY,
      m_d3d9.imageDesc.Height);
   
   SetCoordinates(m_anim.referencePoint, m_anim.zoom, m_anim.pivotPoint, m_anim.angle);

   m_anim.counter++;
}

LPDIRECT3DTEXTURE9 CRotozoomer3View::GetImage(void) const {
   return m_d3d9.pRZImage;
}

void CRotozoomer3View::SetImage(UINT resId) {
   HRESULT hr;
   HINSTANCE hInstance = AfxGetInstanceHandle();

   if (m_d3d9.pRZImage) {
      m_RenderEnable = false;
      m_d3d9.pRZImage->Release();
      m_d3d9.pRZImage = NULL;
   }

   D3DXCreateTextureFromResourceEx(m_d3d9.pDevice, hInstance, MAKEINTRESOURCE(resId), 0, 0, 1,
      0, D3DFMT_FROM_FILE, D3DPOOL_SYSTEMMEM,
      D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, &m_d3d9.imageDesc, NULL, &m_d3d9.pRZImage);
   ASSERT(m_d3d9.pRZImage);

   // Lock image for reading pixels
   if (FAILED(hr = m_d3d9.pRZImage->LockRect(0, &m_d3d9.lockedImageRect, NULL, D3DLOCK_READONLY)))
      DXTrace(__FILE__, __LINE__, hr, _T("LockRect() on image"), TRUE);

   m_RenderEnable = m_d3d9.pRZImage && m_d3d9.pRZSurface;
}

// Set mapping by rectangular texture coordinates
void CRotozoomer3View::SetCoordinates(int viewcx, int viewcy, FPoint upperLeft, FPoint upperRight, FPoint lowerRight) {
   m_calc.horizdx = (upperRight.x - upperLeft.x) / viewcx;
   m_calc.horizdy = (upperRight.y - upperLeft.y) / viewcy;
   m_calc.vertdx = (lowerRight.x - upperRight.x) / viewcx;
   m_calc.vertdy = (lowerRight.y - upperRight.y) / viewcy;
   m_calc.startPoint.x = upperLeft.x;
   m_calc.startPoint.y = upperLeft.y;
}

// Set mapping by texture point, angle and zoom
// referencePoint - Texture coordinate that maps to the upper-left of the screen, when pivotAngle is 0
// pivotPoint - Screen coordinate of pivot point of rotation
void CRotozoomer3View::SetCoordinates(FPoint referencePoint, double zoom, FPoint pivotPoint, double angleRadians) {
   m_calc.horizdx = fmod(cos(angleRadians) / zoom, m_d3d9.imageDesc.Width);
   if (m_calc.horizdx < 0) m_calc.horizdx += m_d3d9.imageDesc.Width;
   m_calc.horizdy = fmod(sin(angleRadians) / zoom, m_d3d9.imageDesc.Height);
   if (m_calc.horizdy < 0) m_calc.horizdy += m_d3d9.imageDesc.Height;

   double rightAngle = angleRadians + PI / 2;
   m_calc.vertdx = fmod(cos(rightAngle) / zoom, m_d3d9.imageDesc.Width);
   if (m_calc.vertdx < 0) m_calc.vertdx += m_d3d9.imageDesc.Width;
   m_calc.vertdy = fmod(sin(rightAngle) / zoom, m_d3d9.imageDesc.Height);
   if (m_calc.vertdy < 0) m_calc.vertdy += m_d3d9.imageDesc.Height;
   
   m_calc.startPoint.x = fmod(referencePoint.x - pivotPoint.x * m_calc.horizdx - pivotPoint.y * m_calc.vertdx, m_d3d9.imageDesc.Width);
   if (m_calc.startPoint.x < 0) m_calc.startPoint.x += m_d3d9.imageDesc.Width;
   m_calc.startPoint.y = fmod(referencePoint.y - pivotPoint.y * m_calc.vertdy - pivotPoint.x * m_calc.horizdy, m_d3d9.imageDesc.Height);
   if (m_calc.startPoint.y < 0) m_calc.startPoint.y += m_d3d9.imageDesc.Height;
}

BOOL CRotozoomer3View::OnIdle(LONG lCount) {
   Render();
   return TRUE;
}

void CRotozoomer3View::OnDraw(CDC *pDC) {
}

BOOL CRotozoomer3View::OnEraseBkgnd(CDC *pDC) {
   // Do nothing if rendering is active
   if (m_RenderEnable) return TRUE;
   
   return __super::OnEraseBkgnd(pDC);
}

void CRotozoomer3View::PreSubclassWindow() {
   __super::PreSubclassWindow();

   InitD3D(1, 1); // Start with 1x1 viewport until OnSize() resizes it; need a device to load an image
   SetImage(IDR_PIC1);
   m_anim.referencePoint.x = m_d3d9.imageDesc.Width / 2;
   m_anim.referencePoint.y = m_d3d9.imageDesc.Height / 2;

   // Add this class' idle handler for CWinApp::OnIdle()
   theApp.IdleHandlers.push_back(this);
}

void CRotozoomer3View::OnDestroy() {
   __super::OnDestroy();

   // Remove from idle handlers
   theApp.IdleHandlers.erase(
      find(theApp.IdleHandlers.begin(), theApp.IdleHandlers.end(), this));

   KillTimer(TIMER_FPS);
   KillTimer(TIMER_ANIMATE);
   CleanupD3D();
}

void CRotozoomer3View::OnTimer(UINT_PTR nIDEvent) {
   switch (nIDEvent) {
   case TIMER_FPS:
      {
         // Calculate frames per second
         int frames = m_FrameCounter - m_FPSLastFrameCounter;
         int ticks = ::GetTickCount() - m_FPSLastTickCount;

         double fps = frames * 1000.0 / ticks;

         m_FPSLastFrameCounter += frames;
         m_FPSLastTickCount += ticks;

         // Pass FPS value to main frame status bar
         static_cast<CMainFrame *>(GetParentFrame())->SetStatusFPS(fps);
         return;
      }
   case TIMER_ANIMATE:
      Animate();
      return;
   }

   __super::OnTimer(nIDEvent);
}

void CRotozoomer3View::OnSize(UINT nType, int cx, int cy) {
   __super::OnSize(nType, cx, cy);

   if ((nType == SIZE_MAXIMIZED || nType == SIZE_RESTORED) && cx != 00 && cy != 0) {
      // Make sure Direct 3D is fully initialized
      GetClientRect(&m_clientRect);
      InitD3D(m_clientRect.Width(), m_clientRect.Height());

      // Start animation timer
      SetTimer(TIMER_ANIMATE, 25, NULL);
      SetTimer(TIMER_FPS, 250, NULL);
   }
   else if (nType == SIZE_MINIMIZED) {
      // Clean up when minimized to reduce overhead when not in use
      KillTimer(TIMER_ANIMATE);
      KillTimer(TIMER_FPS);
      CleanupD3D();
   }
}
