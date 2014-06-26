//
//  StdScrollBar.h
//
//  Created by Gérald Guyomard on 07/08/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __StdScrollBar__
#define __StdScrollBar__

#include "GUI/CCScrollView/CCScrollView.h"
#include "DOWFoundation/DOWFoundation.h"

NS_CC_EXT_BEGIN

    class StdScrollBar : public CCScrollBar
    {
    public:
        DOW_DECLARE_HIERARCHY(StdScrollBar, CCScrollBar);
        
        static StdScrollBar* create(CCScrollView* iOwner, bool iIsHorizontal);
        virtual ~StdScrollBar();
        
        virtual void updatePositionAndSize() override;
        
    private:
        StdScrollBar(CCScrollView* iOwner, bool iIsHorizontal);
        
        bool init();
        
        virtual CCNode* thumb() override;
        
        virtual void setThumbPos(float iPos) override;
        virtual void setThumbSize(float iSize) override;
        
        class Impl;
        CCObjectPtr<Impl> fImpl;
    };
    
    class StdScrollBarFactory : public CCScrollBarFactory
    {
    public:
        static CCObjectPtr<StdScrollBarFactory> create();
        
        virtual CCScrollBar* createHorizontalScrollBar(CCScrollView* iScrollView) override;
        virtual CCScrollBar* createVerticalScrollBar(CCScrollView* iScrollView) override;
        
    private:
        StdScrollBarFactory()
        {}
    };

NS_CC_EXT_END

#endif /* defined(__StdScrollBar__) */
