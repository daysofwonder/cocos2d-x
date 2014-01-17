#include "platform/CCCursor.h"
#include "platform/CCImage.h"

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

NS_CC_END

