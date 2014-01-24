#include "platform/win32/CCCursorWin32.h"
#include "CCGeometry.h"
#include "CCImage.h"
#include "platform/CCCursor.h"

NS_CC_BEGIN

void CCCursorManagerWin32::showCurrentCursor() {
    ShowCursor(true);
}

void
CCCursorManagerWin32::hideCurrentCursor() {
    ShowCursor(false);
}

CCCursor* CCCursorManagerWin32::createCursorFromImage(CCImage* iImage, const CCPoint& iHotspot) {
    m_currentCursor = new CCCursorWin32(iImage, iHotspot);
    return m_currentCursor;
}

void CCCursorManagerWin32::_updateCurrentCursor() {
    SetCursor(m_currentCursor->getHandle());
}



void CCCursorWin32::GetMaskBitmaps(CCImage* iImage,
    HBITMAP &hMask,
    HBITMAP &hBitmap)
{
    HDC hDC = ::GetDC(NULL);
    HDC hMaskDC = ::CreateCompatibleDC(hDC);
    HDC hBitmapDC = ::CreateCompatibleDC(hDC);


    hMask = ::CreateCompatibleBitmap(hDC, iImage->getWidth(), iImage->getHeight());
    hBitmap = ::CreateCompatibleBitmap(hDC, iImage->getWidth(), iImage->getHeight());

    //Select the bitmaps to DC
    HBITMAP hOldAndMaskBitmap = (HBITMAP)::SelectObject(hMaskDC, hMask);
    HBITMAP hOldXorMaskBitmap = (HBITMAP)::SelectObject(hBitmapDC, hBitmap);

    //Scan each pixel of the souce bitmap and create the masks
    unsigned char * pixels = iImage->getData();

    for (int y = 0; y<iImage->getHeight(); ++y)
    {
        for (int x = 0; x<iImage->getWidth(); ++x)
        {
            int i = (x + y*iImage->getWidth())*4;
            int r = pixels[i];
            int g = pixels[i + 1];
            int b = pixels[i + 2];
            int a = pixels[i + 3];

            ::SetPixel(hMaskDC, x, y, RGB(0xFF - a, 0xFF - a, 0xFF - a));
            ::SetPixel(hBitmapDC, x, y, RGB(r, g, b)); // | (((DWORD)(0xFF - a)) << 24));
        }
    }

    ::SelectObject(hMaskDC, hOldAndMaskBitmap);
    ::SelectObject(hBitmapDC, hOldXorMaskBitmap);

    ::DeleteDC(hBitmapDC);
    ::DeleteDC(hMaskDC);

    ::ReleaseDC(NULL, hDC);
}

CCCursorWin32::CCCursorWin32(CCImage* iImage, const CCPoint& iHotspot) {
    HBITMAP hMask;
    HBITMAP hBitmap;

    GetMaskBitmaps(iImage, hMask, hBitmap);

    ICONINFO ii;
    ii.fIcon = FALSE;  // Change fIcon to TRUE to create an icon instead of a cursor
    ii.xHotspot = (DWORD)iHotspot.x;
    ii.yHotspot = (DWORD)iHotspot.y;
    ii.hbmMask = hMask;
    ii.hbmColor = hBitmap;

    // Create the alpha cursor with the alpha DIB section.
    m_hCursor = CreateIconIndirect(&ii);

    DeleteObject(hMask);
    DeleteObject(hBitmap);
}

// Inspired by http://support.microsoft.com/kb/318876/en-us?fr=1
//CCCursorWin32::CCCursorWin32(CCImage* iImage, const CCPoint& iHotspot) {
//    HDC hDC = ::GetDC(NULL);
//
//    BITMAPV5HEADER bi;
//
//    ZeroMemory(&bi, sizeof(BITMAPV5HEADER));
//    bi.bV5Size = sizeof(BITMAPV5HEADER);
//    bi.bV5Width = iImage->getWidth();
//    bi.bV5Height = iImage->getHeight();
//    bi.bV5Planes = 1;
//    bi.bV5BitCount = 32;
//    bi.bV5Compression = BI_BITFIELDS;
//    // The following mask specification specifies a supported 32 BPP
//    // alpha format for Windows XP.
//    bi.bV5RedMask = 0x00FF0000;
//    bi.bV5GreenMask = 0x0000FF00;
//    bi.bV5BlueMask = 0x000000FF;
//    bi.bV5AlphaMask = 0xFF000000;
//
//    void *lpBits;
//
//    // Create the DIB section with an alpha channel.
//    HBITMAP hBitmap = CreateDIBSection(hDC, (BITMAPINFO *)&bi, DIB_RGB_COLORS,
//        (void **)&lpBits, NULL, (DWORD)0);
//
//
//    // Create an empty mask bitmap.
//    HBITMAP hMonoBitmap = CreateBitmap(iImage->getWidth(), iImage->getHeight(), 1, 1, NULL);
//
//    // Set the alpha values for each pixel in the cursor so that
//    // the complete cursor is semi-transparent.
//    DWORD *lpdwPixel;
//    lpdwPixel = (DWORD *)lpBits;
//    for (int x = 0; x<iImage->getWidth(); x++)
//    for (int y = 0; y<iImage->getHeight(); y++)
//    {
//        int i = (x + y*iImage->getWidth()) * 4;
//
//        *lpdwPixel = 0x7F7F7F7F;
//        //*lpdwPixel |= iImage->getData()[i + 2];
//        //*lpdwPixel |= (iImage->getData()[i + 1]) << 8;
//        //*lpdwPixel |= (iImage->getData()[i]) << 16;
//
//        // Set the alpha bits 
//        //*lpdwPixel |= (iImage->getData()[i + 3]) << 24;
//
//        lpdwPixel++;
//    }
//
//    // Create the alpha cursor with the alpha DIB section.
//    ICONINFO ii;
//    ii.fIcon = FALSE;  // Change fIcon to TRUE to create an icon instead of a cursor
//    ii.xHotspot = (DWORD)iHotspot.x;
//    ii.yHotspot = (DWORD)iHotspot.y;
//    ii.hbmMask = hMonoBitmap;
//    ii.hbmColor = hBitmap;
//
//    HCURSOR hAlphaCursor = CreateIconIndirect(&ii);
//
//    DeleteObject(hBitmap);
//    DeleteObject(hMonoBitmap);
//    ::ReleaseDC(NULL, hDC);
//
//    m_hCursor = CreateIconIndirect(&ii);
//}
//

CCCursorWin32::~CCCursorWin32() {
    DestroyCursor(m_hCursor);
}

NS_CC_END
