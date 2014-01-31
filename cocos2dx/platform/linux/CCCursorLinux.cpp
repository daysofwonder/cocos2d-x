#include "platform/linux/CCCursorLinux.h"
#include "CCGeometry.h"
#include "CCImage.h"
#include "platform/CCCursor.h"

NS_CC_BEGIN

void CCCursorManagerLinux::showCurrentCursor() {
}

void
CCCursorManagerLinux::hideCurrentCursor() {
}

CCCursor* CCCursorManagerLinux::createCursorFromImage(CCImage* iImage, const CCPoint& iHotspot) {
    return nullptr;
}

void CCCursorManagerLinux::_updateCurrentCursor() {
}




CCCursorLinux::CCCursorLinux(CCImage* iImage, const CCPoint& iHotspot) {
}


CCCursorLinux::~CCCursorLinux() {
}

NS_CC_END
