//
//  NumberRasterizer.h
//
//  Created by Gérald Guyomard on 26/02/13.
//  Copyright (c) 2013 Days of Wonder. All rights reserved.
//
//

#ifndef __NumberRasterizer__
#define __NumberRasterizer__

#include "base_nodes/CCNode.h"
#include "cocoa/CCObjectPtr.h"
#include <string>

NS_CC_BEGIN

    class NumberRasterizer
    {
    public:
        NumberRasterizer(const std::string& iAtlasPListName);
        
        void prefetchResources();
        CCObjectPtr<CCNode> rasterize(int32_t iScore);
        CCObjectPtr<CCNode> rasterize(int32_t iScore, CCNode* iParent, float iCoveragePercent = 1.f);
        
    private:
        std::string fAtlasPListName;
    };

NS_CC_END

#endif /* defined(__NumberRasterizer__) */
