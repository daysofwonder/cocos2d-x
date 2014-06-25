//
//  CCOverallScaleProtocol.h
//
//  Created by Gérald Guyomard
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCOverallScaleProtocol_h
#define __CCOverallScaleProtocol_h

#include "cocos2d.h"

NS_CC_BEGIN


class CCOverallScaleProtocol
{
public:

    virtual float overallScale() const = 0;

protected:
    ~CCOverallScaleProtocol() {}
};

NS_CC_END


#endif // __CCOverallScaleProtocol_h
