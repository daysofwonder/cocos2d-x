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


NSCursor*
CCCursorManagerMac::getCurrentNativeCursor()
{
    CCCursorMac* cursor = static_cast<CCCursorMac*>(getCurrentCursor());
    return (cursor != NULL) ? cursor->nativeCursor() : nil;
}

void
CCCursorManagerMac::_updateCurrentCursor()
{
    EAGLView* view = [EAGLView sharedEGLView];
    [view.window invalidateCursorRectsForView:view];
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

