//
//  cocos2d-GeometryHelpers.h
//
//  Created by Gérald Guyomard on 20/02/13.
//
//

#ifndef __cocos2d_GeometryHelpers__
#define __cocos2d_GeometryHelpers__

#include "ccTypes.h"
#include "CCGeometry.h"

NS_CC_BEGIN

inline bool operator==(const ccColor3B& iC1, const ccColor3B& iC2)
{
    return (iC1.r == iC2.r) && (iC2.g == iC2.g) && (iC1.b == iC2.b);
}

inline bool operator!=(const ccColor3B& iC1, const ccColor3B& iC2)
{
    return !(iC1 == iC2);
}

inline bool operator==(const ccColor4B& iC1, const ccColor4B& iC2)
{
    return (iC1.r == iC2.r) && (iC1.g == iC2.g) && (iC1.b == iC2.b) && (iC1.a == iC2.a);
}

inline bool operator!=(const ccColor4B& iC1, const ccColor4B& iC2)
{
    return !(iC1 == iC2);
}

inline bool operator==(const CCSize& iS1, const CCSize& iS2)
{
    return (iS1.width == iS2.width) && (iS1.height == iS2.height);
}

inline bool operator!=(const CCSize& iS1, const CCSize& iS2)
{
    return !(iS1 == iS2);
}

NS_CC_END


#endif /* defined(__cocos2d_GeometryHelpers__) */
