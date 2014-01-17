#include "platform/CCCursor.h"
#include "platform/CCImage.h"
#include "cocoa/CCArray.h"

NS_CC_BEGIN

void
CCCursor::set()
{
    CCCursorManager::sharedCursorManager()->setCurrentCursor(this);
}

CCCursorManager* CCCursorManager::s_Instance = NULL;

void
CCCursorManager::setSharedCursorManager(CCCursorManager* iManager)
{
    if (s_Instance != iManager)
    {
        CC_SAFE_DELETE(s_Instance);
        s_Instance = iManager;
    }
}

CCCursor*
CCCursorManager::createCursorFromImageFile(const std::string& iImageName, const CCPoint& iHotspot)
{
    CCCursor* cursor = NULL;
    CCImage* img = new CCImage;
    if (img->initWithImageFile(iImageName.c_str()))
    {
        cursor = createCursorFromImage(img, iHotspot);
    }
    
    img->release();
    
    return cursor;
}

CCCursorManager::CCCursorManager()
: m_CursorStack(NULL)
{
    
}

CCCursorManager::~CCCursorManager()
{
    CC_SAFE_RELEASE(m_CursorStack);
}

void
CCCursorManager::push(CCCursor* iCursor)
{
    CCCursor* current = getCurrentCursor();
    if (current != NULL)
    {
        if (m_CursorStack == NULL)
        {
            m_CursorStack = new CCArray;
        }
        
        m_CursorStack->addObject(current);
    }
    
    setCurrentCursor(iCursor);
}

void
CCCursorManager::pop()
{
    if ((m_CursorStack != NULL) && (m_CursorStack->count() != 0))
    {
        CCCursor* restoredCursor = (CCCursor*) m_CursorStack->lastObject();
        
        restoredCursor->retain();
        m_CursorStack->removeLastObject();
        if (m_CursorStack->count() == 0)
        {
            CC_SAFE_RELEASE_NULL(m_CursorStack);
        }
        
        setCurrentCursor(restoredCursor);
        restoredCursor->release();
    }
    else
    {
        setCurrentCursor(NULL);
    }
}

NS_CC_END

