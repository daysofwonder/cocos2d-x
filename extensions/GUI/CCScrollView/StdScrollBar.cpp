//
//  StdScrollBar.cpp
//
//  Created by Gérald Guyomard
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "GUI/CCScrollView/StdScrollBar.h"
#include "NodeControllers/NodeController.h"

NS_CC_EXT_BEGIN

    class StdScrollBar::Impl : public NodeController
    {
    public:
        DOW_DECLARE_HIERARCHY(Impl, NodeController);
        static CCObjectPtr<Impl> create(bool iHorizontal);
       
        CCNode* background() const { return fBackground; }
        CCNode* thumb() const { return fThumb; }
        
    private:
        virtual void _fillResolvers(Resolvers& iResolvers) override;
        virtual void _didLoad() override;
        
        CCObjectPtr<CCNode> fBackground;
        CCObjectPtr<CCNode> fThumb;
    };
    
    
    CCObjectPtr<StdScrollBar::Impl>
    StdScrollBar::Impl::create(bool iHorizontal)
    {
        CCObjectPtr<Impl> impl;
        impl << new Impl;
        
        impl->_load(iHorizontal ? "HorizontalScrollBar" : "VerticalScrollBar");
        
        return impl;
    }
    
    void
    StdScrollBar::Impl::_fillResolvers(Resolvers& iResolvers)
    {
        SuperClass::_fillResolvers(iResolvers);
        
        auto& assigner = iResolvers.memberVariableAssigner();
        
        assigner.addMember("background", fBackground);
        assigner.addMember("thumb", fThumb);
    }
    
    void
    StdScrollBar::Impl::_didLoad()
    {
        SuperClass::_didLoad();
        
        assert(fBackground != nullptr);
        assert(fThumb != nullptr);
    }
    
    StdScrollBar*
    StdScrollBar::create(CCScrollView* iOwner, bool iIsHorizontal)
    {
        auto* bar = new StdScrollBar(iOwner, iIsHorizontal);
        if (!bar->init())
        {
            bar->release();
            return nullptr;
        }
        
        bar->autorelease();
        return bar;
    }
    
    StdScrollBar::StdScrollBar(CCScrollView* iOwner, bool iIsHorizontal)
    : SuperClass(iOwner, iIsHorizontal)
    {}
    
    StdScrollBar::~StdScrollBar()
    {
        
    }
    
    bool
    StdScrollBar::init()
    {
        if (SuperClass::init())
        {
            fImpl = Impl::create(isHorizontal());
            if (fImpl == nullptr)
            {
                return false;
            }
            
            auto* root = fImpl->rootNode();
            root->ignoreAnchorPointForPosition(true);
            addChild(root);
            
            setContentSize(root->getContentSize());
            
            return true;
        }
        else
        {
            return false;
        }
    }

    void
    StdScrollBar::updatePositionAndSize()
    {
        // Make sure to stretch the entire scrollbar
        CCSize viewSize = owner()->getViewSize();
        CCSize mySize = getContentSize();
        CCSize bgSize = fImpl->background()->getContentSize();
        
        if (isHorizontal())
        {
            mySize.width = viewSize.width;
            bgSize.height = viewSize.width; // Always modify height
        }
        else
        {
            mySize.height = viewSize.height;
            bgSize.height = viewSize.height; // Always modify height
        }
        
        setContentSize(mySize);
        
        fImpl->background()->setContentSize(bgSize);
        
        SuperClass::updatePositionAndSize();
    }

    void
    StdScrollBar::setThumbPos(float iPos)
    {
        auto* t = thumb();
        
        t->ignoreAnchorPointForPosition(true);
        
        CCPoint pos = t->getPosition();
        
        if (isHorizontal())
        {
            pos.x = iPos;
        }
        else
        {
            pos.y = iPos;
        }
        
        t->setPosition(pos);
    }
    
    void
    StdScrollBar::setThumbSize(float iSize)
    {
        CCNode* t = thumb();
        CCSize size = t->getContentSize();
        size.height = iSize;
        
        t->setContentSize(size);
    }
    
    CCNode*
    StdScrollBar::thumb()
    {
        return fImpl->thumb();
    }

    CCObjectPtr<StdScrollBarFactory>
    StdScrollBarFactory::create()
    {
        CCObjectPtr<StdScrollBarFactory> factory;
        factory << new StdScrollBarFactory;
        
        return factory;
    }
    
    CCScrollBar*
    StdScrollBarFactory::createHorizontalScrollBar(CCScrollView* iScrollView)
    {
        return StdScrollBar::create(iScrollView, true);
    }
    
    CCScrollBar*
    StdScrollBarFactory::createVerticalScrollBar(CCScrollView* iScrollView)
    {
        return StdScrollBar::create(iScrollView, false);
    }

NS_CC_EXT_END
