#ifndef _CCB_CCNODELOADERLISTENER_H_
#define _CCB_CCNODELOADERLISTENER_H_

#include "platform/CCPlatformMacros.h"
#include "cocoa/CCGeometry.h"
#include "ExtensionMacros.h"

NS_CC_BEGIN
class CCNode;
NS_CC_END

NS_CC_EXT_BEGIN

class CCNodeLoader;

enum CCBPositionType
{
    kCCBPositionTypeRelativeBottomLeft,
    kCCBPositionTypeRelativeTopLeft,
    kCCBPositionTypeRelativeTopRight,
    kCCBPositionTypeRelativeBottomRight,
    kCCBPositionTypePercent,
    kCCBPositionTypeMultiplyResolution,
};

enum CCBSizeType
{
    kCCBSizeTypeAbsolute,
    kCCBSizeTypePercent,
    kCCBSizeTypeRelativeContainer,
    kCCBSizeTypeHorizontalPercent,
    kCCBSizeTypeVerticalPercent,
    kCCBSizeTypeMultiplyResolution,
};

class CCNodeLoaderListener {
    public:
        virtual ~CCNodeLoaderListener() {};

        virtual void onNodeLoaded(CCNode * pNode, CCNodeLoader * pNodeLoader) {}
    
        virtual void onChildPositionParsed(CCNode * pNode, CCNode* pChild, const CCPoint& pos, CCBPositionType ntype, const CCSize& containerSize, const char* pPropertyName) {}
        virtual void onChildSizeParsed(CCNode * pNode, CCNode* pChild, const CCSize& size, CCBSizeType ntype, const CCSize& containerSize) {}
};

NS_CC_EXT_END

#endif
