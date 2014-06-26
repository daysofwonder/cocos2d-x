//
//  CCNativeSmartLayer.cpp
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCNativeSmartLayer.h"
#include "CCNativeViewImpl.h"

NS_CC_BEGIN

    CCNativeSmartLayer::CCNativeSmartLayer()
    {}
    
    CCNativeSmartLayer::~CCNativeSmartLayer()
    {}
    
    bool
    CCNativeSmartLayer::init()
    {
        fImpl = _createImpl();
        if (!fImpl->init(this))
        {
            return false;
        }
        
        if (!SuperClass::init())
        {
            return false;
        }
        
        return true;
    }
    
    CCNativeViewImpl*
    CCNativeSmartLayer::_impl() const
    {
        return fImpl.get();
    }
    
    void
    CCNativeSmartLayer::onEnter()
    {
        SuperClass::onEnter();
        
        fImpl->onEnter();
    }
    
    void
    CCNativeSmartLayer::onEnterTransitionDidFinish()
    {
        SuperClass::onEnterTransitionDidFinish();
        
        fImpl->onEnterTransitionDidFinish();
    }
    
    void
    CCNativeSmartLayer::onExit(void)
    {
        SuperClass::onExit();
        
        fImpl->onExit();
    }
    
    void
    CCNativeSmartLayer::onExitTransitionDidStart()
    {
        SuperClass::onExitTransitionDidStart();
        
        fImpl->onExitTransitionDidStart();
    }    

NS_CC_END