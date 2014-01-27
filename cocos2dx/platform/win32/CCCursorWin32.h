#ifndef __CCCURSORWIN32_H__
#define __CCCURSORWIN32_H__

#include "platform/CCCursor.h"
#include <Windows.h>

NS_CC_BEGIN

class CCImage;
class CCPoint;
class CCArray;

class CC_DLL CCCursorWin32 : public CCCursor
{
public:
    virtual ~CCCursorWin32();

    void set();

    CCCursorWin32(CCImage* iImage, const CCPoint& iHotspot);
//    CCCursorWin32() {}

    HCURSOR getHandle() { return m_hCursor; }

protected:
    HCURSOR m_hCursor;
};

class CC_DLL CCCursorManagerWin32 : public CCCursorManager
{
public:
    CCCursorManagerWin32() {}
    virtual ~CCCursorManagerWin32() {}

    virtual void showCurrentCursor();
    virtual void hideCurrentCursor();

    virtual CCCursor* createCursorFromImage(CCImage* iImage, const CCPoint& iHotspot);

    void updateCurrentCursor() { _updateCurrentCursor(); }


protected:

    virtual void _updateCurrentCursor();
};


NS_CC_END

#endif /* __CCCURSORWIN32_H__ */
