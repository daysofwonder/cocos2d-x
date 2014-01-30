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
    
    CCCursor* getCurrentCursor() { return m_CurrentCursor; }
    void setCurrentCursor(CCCursor* iCursor);

    void push(CCCursor* iCursor);
    void pop();
    
    CCCursor* createCursorFromImageFile(const std::string& iImageName, const CCPoint& iHotspot);
    
    virtual CCCursor* createCursorFromImage(CCImage* iImage, const CCPoint& iHotspot) = 0;
    
protected:
    CCCursorManager();
    
    friend class CCCursorUpdater;
    virtual void _updateCurrentCursor() = 0;
    
private:
    static CCCursorManager* s_Instance;
    
    CCCursor* m_CurrentCursor;
    CCArray* m_CursorStack;
};

// This is a helper class that can be used
// by any platform that need to refresh regularly the cursor...
class CC_DLL CCCursorUpdater : public CCObject
{
public:
    static CCCursorUpdater* create();
    
    void terminate();
    
private:
    CCCursorUpdater();
    void _init();
    
    void _update(float);
    
};


NS_CC_END

#endif /* __CCCURSOR_H__ */
