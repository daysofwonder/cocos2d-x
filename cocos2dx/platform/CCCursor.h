#ifndef __CCCURSOR_H__
#define __CCCURSOR_H__

#include "CCPlatformMacros.h"
#include "cocoa/CCObject.h"

NS_CC_BEGIN

class CCImage;
class CCPoint;
class CCArray;

class CC_DLL CCCursor : public CCObject
{
public:
    virtual ~CCCursor() {}
    
    void set();
    
protected:
    
    CCCursor() {}
};

class CC_DLL CCCursorManager
{
public:
    virtual ~CCCursorManager();
    
    static CCCursorManager* sharedCursorManager() { return s_Instance; }
    static void setSharedCursorManager(CCCursorManager* iManager);
    
    virtual void showCurrentCursor() = 0;
    virtual void hideCurrentCursor() = 0;
    
    virtual CCCursor* getCurrentCursor() = 0;
    virtual void setCurrentCursor(CCCursor* iCursor) = 0;

    void push(CCCursor* iCursor);
    void pop();
    
    CCCursor* createCursorFromImageFile(const std::string& iImageName, const CCPoint& iHotspot);
    
    virtual CCCursor* createCursorFromImage(CCImage* iImage, const CCPoint& iHotspot) = 0;
    
protected:
    CCCursorManager();
    
private:
    static CCCursorManager* s_Instance;
    
    CCArray* m_CursorStack;
};


NS_CC_END

#endif /* __CCCURSOR_H__ */
