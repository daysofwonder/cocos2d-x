#ifndef __CCCURSORWIN32_H__
#define __CCCURSORWIN32_H__

#include "platform/CCCursor.h"

NS_CC_BEGIN

class CCImage;
class CCPoint;
class CCArray;

class CCCursorLinux : public CCCursor
{
public:
    virtual ~CCCursorLinux();

    void set();

    CCCursorLinux(CCImage* iImage, const CCPoint& iHotspot);
//    CCCursorWin32() {}


};

class CCCursorManagerLinux : public CCCursorManager
{
public:
    CCCursorManagerLinux() {}
    virtual ~CCCursorManagerLinux() {}

    virtual void showCurrentCursor();
    virtual void hideCurrentCursor();

    virtual CCCursor* createCursorFromImage(CCImage* iImage, const CCPoint& iHotspot);

    void updateCurrentCursor() { _updateCurrentCursor(); }


protected:

    virtual void _updateCurrentCursor();
};


NS_CC_END

#endif /* __CCCURSORWIN32_H__ */
