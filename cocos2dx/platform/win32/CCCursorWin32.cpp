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






// Inspired by http://support.microsoft.com/kb/318876/en-us?fr=1
CCCursorWin32::CCCursorWin32(CCImage* iImage, const CCPoint& iHotspot) {
    HDC hDC = ::GetDC(NULL);

    BITMAPV5HEADER bi;

    ZeroMemory(&bi, sizeof(BITMAPV5HEADER));
    bi.bV5Size = sizeof(BITMAPV5HEADER);
    bi.bV5Width = iImage->getWidth();
    bi.bV5Height = -iImage->getHeight();
    bi.bV5Planes = 1;
    bi.bV5BitCount = 32;
    bi.bV5Compression = BI_BITFIELDS;
    // The following mask specification specifies a supported 32 BPP
    // alpha format for Windows XP.
    bi.bV5RedMask = 0x00FF0000;
    bi.bV5GreenMask = 0x0000FF00;
    bi.bV5BlueMask = 0x000000FF;
    bi.bV5AlphaMask = 0xFF000000;

    void *lpBits;

    // Create the DIB section with an alpha channel.
    HBITMAP hBitmap = CreateDIBSection(hDC, (BITMAPINFO *)&bi, DIB_RGB_COLORS,
        (void **)&lpBits, NULL, (DWORD)0);


    // Create an empty mask bitmap.
    HBITMAP hMonoBitmap = CreateBitmap(iImage->getWidth(), iImage->getHeight(), 1, 1, NULL);

    CopyMemory(lpBits, iImage->getData(), iImage->getDataLen()*4);


    // Create the alpha cursor with the alpha DIB section.
    ICONINFO ii;
    ii.fIcon = FALSE;  // Change fIcon to TRUE to create an icon instead of a cursor
    ii.xHotspot = (DWORD)iHotspot.x;
    ii.yHotspot = (DWORD)iHotspot.y;
    ii.hbmMask = hMonoBitmap;
    ii.hbmColor = hBitmap;

    m_hCursor = CreateIconIndirect(&ii);

    DeleteObject(hBitmap);
    DeleteObject(hMonoBitmap);
}


CCCursorWin32::~CCCursorWin32() {
    DestroyCursor(m_hCursor);
}

NS_CC_END
