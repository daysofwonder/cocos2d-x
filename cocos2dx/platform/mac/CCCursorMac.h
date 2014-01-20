#ifndef __CCCURSORMAC_H__
#define __CCCURSORMAC_H__

#include "platform/CCCursor.h"
#include <AppKit/AppKit.h>

NS_CC_BEGIN

class CCCursorMac;

class CCCursorManagerMac : public CCCursorManager
{
public:
    virtual void showCurrentCursor();
    virtual void hideCurrentCursor();
    
    virtual CCCursor* createCursorFromImage(CCImage* iImage, const CCPoint& iHotspot);
    
    NSCursor* getCurrentNativeCursor();
    
private:
    virtual void _updateCurrentCursor();
};


NS_CC_END

#endif /* __CCCURSORMAC_H__ */
