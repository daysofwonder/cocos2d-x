#include "platform/CCCursor.h"
#include "platform/CCImage.h"
#include "cocoa/CCArray.h"
#include "CCDirector.h"
#include "CCScheduler.h"

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

void
CCCursorManager::setCurrentCursor(CCCursor *iCursor)
{
    if (iCursor != m_CurrentCursor)
    {
        CC_SAFE_RELEASE(m_CurrentCursor);
        m_CurrentCursor = iCursor;
        CC_SAFE_RETAIN(m_CurrentCursor);
        
        _updateCurrentCursor();
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
: m_CursorStack(NULL), m_CurrentCursor(NULL)
{
    
}

CCCursorManager::~CCCursorManager()
{
    CC_SAFE_RELEASE(m_CurrentCursor);
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

CCCursorUpdater*
CCCursorUpdater::create()
{
    CCCursorUpdater* updater = new CCCursorUpdater;
    updater->_init();
    updater->autorelease();
    
    return updater;
}

void
CCCursorUpdater::_init()
{
    const float kRefreshInterval = 1;
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector((SEL_SCHEDULE) &CCCursorUpdater::_update, this, kRefreshInterval, false);
}

void
CCCursorUpdater::terminate()
{
    CCDirector::sharedDirector()->getScheduler()->unscheduleAllForTarget(this);
}

CCCursorUpdater::CCCursorUpdater()
{}

void
CCCursorUpdater::_update(float)
{
    CCCursorManager* manager = CCCursorManager::sharedCursorManager();
    CC_ASSERT(manager != NULL);
    if (manager != NULL)
    {
        manager->_updateCurrentCursor();
    }
}


NS_CC_END

