#include "platform/mac/CCCursorMac.h"
#include "platform/CCImage.h"
#include "cocoa/CCGeometry.h"
#include "EAGLView.h"

NS_CC_BEGIN

class CCCursorMac : public CCCursor
{
public:
    CCCursorMac();
    virtual ~CCCursorMac();
    
    NSCursor* nativeCursor() const { return m_NativeCursor; }
    
    bool init(CCImage* iImage, const CCPoint& iHotspot);
    
private:
    
    NSCursor* m_NativeCursor;
};

CCCursorMac::CCCursorMac()
: m_NativeCursor(nil)
{}

CCCursorMac::~CCCursorMac()
{
    [m_NativeCursor release];
}

bool
CCCursorMac::init(CCImage* iImage, const CCPoint& iHotspot)
{
    uint8_t* bitmap = iImage->getData();
    
    CGColorSpaceRef colorSpace=CGColorSpaceCreateDeviceRGB();
    
    CGContextRef bitmapContext = CGBitmapContextCreate(bitmap, iImage->getWidth(), iImage->getHeight(), 8, 4 * iImage->getWidth(), colorSpace, iImage->hasAlpha() ? kCGImageAlphaPremultipliedLast : kCGImageAlphaNoneSkipLast);
    CFRelease(colorSpace);
    
    CGImageRef cgImage = CGBitmapContextCreateImage(bitmapContext);
    CGContextRelease(bitmapContext);
    
    NSImage* img = [[NSImage alloc] initWithCGImage:cgImage size:NSZeroSize];
    CGImageRelease(cgImage);
    
    NSPoint hs = NSMakePoint(iHotspot.x, iHotspot.y);
    m_NativeCursor = [[NSCursor alloc] initWithImage:img hotSpot:hs];
    [img release];
    
    if (m_NativeCursor == nil)
    {
        return false;
    }
    
    return true;
}

CCCursorManagerMac::CCCursorManagerMac()
: m_CurrentCursor(NULL)
{
    
}

CCCursorManagerMac::~CCCursorManagerMac()
{
    CC_SAFE_RELEASE(m_CurrentCursor);
}

void
CCCursorManagerMac::showCurrentCursor()
{
    [NSCursor unhide];
}

void
CCCursorManagerMac::hideCurrentCursor()
{
    [NSCursor hide];
}

CCCursor*
CCCursorManagerMac::getCurrentCursor()
{
    return m_CurrentCursor;
}

NSCursor*
CCCursorManagerMac::getCurrentNativeCursor()
{
    return (m_CurrentCursor != NULL) ? m_CurrentCursor->nativeCursor() : nil;
}

void
CCCursorManagerMac::setCurrentCursor(CCCursor* iCursor)
{
    if (iCursor != m_CurrentCursor)
    {
        CC_SAFE_RELEASE(m_CurrentCursor);
        CC_ASSERT((iCursor == NULL) || (dynamic_cast<CCCursorMac*>(iCursor) != NULL));
        m_CurrentCursor = (CCCursorMac*) iCursor;
        CC_SAFE_RETAIN(m_CurrentCursor);
        
        EAGLView* view = [EAGLView sharedEGLView];
        [view.window invalidateCursorRectsForView:view];
    }
}

CCCursor*
CCCursorManagerMac::createCursorFromImage(CCImage* iImage, const CCPoint& iHotspot)
{
    CCCursorMac* cursor = new CCCursorMac;
    if (!cursor->init(iImage, iHotspot))
    {
        cursor->release();
        return NULL;
    }
    
    cursor->autorelease();
    
    return cursor;
}

NS_CC_END

