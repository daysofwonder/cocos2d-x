#ifndef __CCCURSORWIN32_H__
#define __CCCURSORWIN32_H__

#include "platform/CCCursor.h"
#include <gdk/gdkcursor.h>
#include <gtk/gtk.h>

NS_CC_BEGIN

class CCImage;
class CCPoint;
class CCArray;

class CCCursorLinux : public CCCursor
{
public:
    virtual ~CCCursorLinux();

    CCCursorLinux(CCImage* iImage, const CCPoint& iHotspot);
    GdkCursor* getCursor() { return cursor; }

protected:
    GdkCursor* cursor;
};

class CCCursorManagerLinux : public CCCursorManager
{
public:
    CCCursorManagerLinux(GdkWindow* w) : window(w)
    {}
    virtual ~CCCursorManagerLinux() {}

    virtual void showCurrentCursor();
    virtual void hideCurrentCursor();

    virtual CCCursor* createCursorFromImage(CCImage* iImage, const CCPoint& iHotspot);

    CCCursor* createCursorFromImageFile(const std::string& iImageName, const CCPoint& iHotspot);

    void updateCurrentCursor() { _updateCurrentCursor(); }


protected:
    GdkWindow* window = nullptr;
    virtual void _updateCurrentCursor();
};


NS_CC_END

#endif /* __CCCURSORWIN32_H__ */
