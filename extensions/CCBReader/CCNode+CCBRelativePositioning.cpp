#include "CCNode+CCBRelativePositioning.h"
#include "CCBReader/CCBReader.h"

using namespace cocos2d;

NS_CC_EXT_BEGIN

CCPoint getAbsolutePosition(const CCPoint &pt, int nType, const CCSize &containerSize, const char *pPropName)
{
    CCPoint absPt = ccp(0,0);
    if (nType == kCCBPositionTypeRelativeBottomLeft)
    {
        absPt = pt;
    }
    else if (nType == kCCBPositionTypeRelativeTopLeft)
    {
        absPt.x = pt.x;
        absPt.y = containerSize.height - pt.y;
    }
    else if (nType == kCCBPositionTypeRelativeTopRight)
    {
        absPt.x = containerSize.width - pt.x;
        absPt.y = containerSize.height - pt.y;
    }
    else if (nType == kCCBPositionTypeRelativeBottomRight)
    {
        absPt.x = containerSize.width - pt.x;
        absPt.y = pt.y;
    }
    else if (nType == kCCBPositionTypePercent)
    {
        absPt.x = (int)(containerSize.width * pt.x / 100.0f);
        absPt.y = (int)(containerSize.height * pt.y / 100.0f);
    }
    else if (nType == kCCBPositionTypeMultiplyResolution)
    {
        float resolutionScale = CCBReader::getResolutionScale();
        
        absPt.x = pt.x * resolutionScale;
        absPt.y = pt.y * resolutionScale;
    }
    
    return absPt;
}

void setRelativeScale(CCNode *pNode, float fScaleX, float fScaleY, int nType, const char* pPropName)
{
    CCAssert(pNode, "pNode should not be null");
    
    if (nType == kCCBScaleTypeMultiplyResolution)
    {
        float resolutionScale = CCBReader::getResolutionScale();
        
        fScaleX *= resolutionScale;
        fScaleY *= resolutionScale;
    }
    
    pNode->setScaleX(fScaleX);
    pNode->setScaleY(fScaleY);
}

CCSize getAbsoluteSize(const CCSize& size, int ntype, const CCSize& containerSize)
{
    float width = size.width;
    float height = size.height;
    
    switch (ntype)
    {
        case kCCBSizeTypeAbsolute:
        {
            /* Nothing. */
            break;
        }
        case kCCBSizeTypeRelativeContainer:
        {
            width = containerSize.width - width;
            height = containerSize.height - height;
            break;
        }
        case kCCBSizeTypePercent:
        {
            width = (int)(containerSize.width * width / 100.0f);
            height = (int)(containerSize.height * height / 100.0f);
            break;
        }
        case kCCBSizeTypeHorizontalPercent:
        {
            width = (int)(containerSize.width * width / 100.0f);
            break;
        }
        case kCCBSizeTypeVerticalPercent:
        {
            height = (int)(containerSize.height * height / 100.0f);
            break;
        }
        case kCCBSizeTypeMultiplyResolution:
        {
            float resolutionScale = CCBReader::getResolutionScale();
            
            width *= resolutionScale;
            height *= resolutionScale;
            break;
        }
        default:
        {
            CCLog("Unknown CCB type.");
        }
            break;
    }
    
    return CCSize(width, height);
}

NS_CC_EXT_END
