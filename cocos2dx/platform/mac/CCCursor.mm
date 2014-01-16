#include "CCCursor.h"
#include <AppKit/AppKit.h>

NS_CC_BEGIN

bool
CCCursor::isSupported()
{
    return true;
}

void
CCCursor::showCurrentCursor()
{
    [NSCursor unhide];
}

void
CCCursor::hideCurrentCursor()
{
    [NSCursor hide];
}


NS_CC_END

