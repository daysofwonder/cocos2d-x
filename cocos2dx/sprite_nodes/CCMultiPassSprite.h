//
//  CCMultiPassSprite.h
//
//  Created by Gérald Guyomard on 12/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCMultiPassSprite__
#define __CCMultiPassSprite__

#include <cocos2d.h>
#include "DOWFoundation/DOWFoundation.h"

NS_CC_BEGIN

    class CCMultiPassSprite : public CCSprite
    {
    public:
        DOW_DECLARE_HIERARCHY(CCMultiPassSprite, CCSprite);
        
        static CCMultiPassSprite* create();
        
        uint32_t nbRenderingPasses() const { return fNbRenderingPasses; }
        void setNbRenderingPasses(uint32_t iNbRenderingPasses) { fNbRenderingPasses = iNbRenderingPasses; }
        
        virtual void draw() override;
        
    protected:
        CCMultiPassSprite();

        uint32_t fNbRenderingPasses;
    };

NS_CC_END

#endif /* defined(__CCMultiPassSprite__) */
