//
//  CCNativeViewImplAndroid.cpp
//
//  Created by Gérald Guyomard on 05/03/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "GUI/NativeNodes/android/CCNativeViewImplAndroid.h"
#include "DOWFoundation/NotificationCenter.h"
#include <cocos2d.h>

using namespace DOW;

static const std::string kNativeViewControllerJavaClassName = "com/daysofwonder/smallworld2/NativeViewController";

NS_CC_BEGIN

cocos2d::CCNode*
nodeWithKeyboardFocus()
{
    // Request nodeImplWithKeyboardFocus
    JNIMethodInfo::Ptr getNodeImplWithKeyboardFocus = JNIManager::instance().getStaticMethodInfo(kNativeViewControllerJavaClassName.c_str(), "getNodeImplWithKeyboardFocus", "()J");;
    
    assert(getNodeImplWithKeyboardFocus != nullptr);
    uint64_t owner = getNodeImplWithKeyboardFocus->callStaticLongMethod();
    cocos2d::extension::CCNativeViewImplAndroid* impl = reinterpret_cast<cocos2d::extension::CCNativeViewImplAndroid*>(owner);
    
    return (impl != nullptr) ? impl->owner() : nullptr;
}

NS_CC_END

NS_CC_EXT_BEGIN

    class CCNativeViewImplAndroid::Updater : public CCObject
    {
    public:
        Updater(CCNativeViewImplAndroid* iImpl);
        virtual ~Updater();
        
        virtual void update(float iTime) override;
        
        void listen();
        void stopListening();
        
        void terminate();
        
    private:
        CCNativeViewImplAndroid* fImpl;
    };
    
    CCNativeViewImplAndroid::Updater::Updater(CCNativeViewImplAndroid* iImpl)
    : fImpl(iImpl)
    {}
    
    CCNativeViewImplAndroid::Updater::~Updater()
    {
        stopListening();
    }
    
    void
    CCNativeViewImplAndroid::Updater::update(float iTime)
    {
        if (fImpl != nullptr)
        {
            fImpl->_updateNativeUIView();
        }
    }
    
    void
    CCNativeViewImplAndroid::Updater::listen()
    {
        CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this, -10, false);
    }
    
    void
    CCNativeViewImplAndroid::Updater::stopListening()
    {
        CCDirector::sharedDirector()->getScheduler()->unscheduleUpdateForTarget(this);
    }
    
    void
    CCNativeViewImplAndroid::Updater::terminate()
    {
        stopListening();
        fImpl = nullptr;
    }

    CCNativeViewImplAndroid::CCNativeViewImplAndroid()
    : fOwner(nullptr), fIsDiscarded(false)
    {}
    
    CCNativeViewImplAndroid::~CCNativeViewImplAndroid()
    {
        NotificationCenter::defaultNotificationCenter().removeObserver(this);
        
        _removeFromSuperview();
        fNativeViewController.reset();
        
        if (fUpdater != nullptr)
        {
            fUpdater->terminate();
        }
    }
    
    JavaObject::Ptr
    CCNativeViewImplAndroid::_createNativeViewController()
    {
    	const std::string name = _nativeViewControllerFullName();

    	auto& manager = JNIManager::instance();
        auto createMethod = manager.getStaticMethodInfo(name.c_str(), "newInstance", "(J)Lcom/daysofwonder/smallworld2/NativeViewController;");

        if (createMethod == nullptr)
        {
            return nullptr;
        }

        jlong owner = reinterpret_cast<jlong>(this);
        auto controller = createMethod->callStaticObjectMethod(1, owner);
        if (controller != nullptr)
        {
        	// create view
        	// public void createView(float iRotation, float iX, float iY, float iWidth, float iHeight)
        	auto method = manager.getMethodInfo(kNativeViewControllerJavaClassName.c_str(), "createView", "()V");
        	if (method != nullptr)
        	{
        		method->callVoidMethod(controller->javaObject());
        	}
        }

        return controller;
    }

    bool
    CCNativeViewImplAndroid::init(CCNode* iNode)
    {
        fOwner = iNode;
        
        fNativeViewController = _createNativeViewController();
        if (fNativeViewController == nullptr)
        {
            return false;
        }
        
        NotificationCenter& center = NotificationCenter::defaultNotificationCenter();
        
        center.addObserver(this, kShouldDiscardNonDescendantNativeNodesNotifID, [this](void* iObserver, const Notification& iNotification)
                           {
                               if (!fIsDiscarded)
                               {
                                   CCNode* node = (CCNode*) iNotification.object();
                                   if (!node->isAncestor(fOwner))
                                   {
                                       fIsDiscarded = true;
                                       _updateNativeUIView();
                                   }
                               }
                           });
        
        center.addObserver(this, kShouldRestoreNonDescendantNativeNodesNotifID, [this](void* iObserver, const Notification& iNotification)
                           {
                               if (fIsDiscarded)
                               {
                                   CCNode* node = (CCNode*) iNotification.object();
                                   if (!node->isAncestor(fOwner))
                                   {
                                       fIsDiscarded = false;
                                       _updateNativeUIView();
                                   }
                               }
                           });
        
        return true;    
    }

    bool
    CCNativeViewImplAndroid::_computeTransform(float& oRotation, CCRect& oBoundingBox) const
    {
    	bool isVisible = true;
    	oRotation = 0;

         CCNode* n = fOwner;
         while (n != nullptr)
         {
             if (isVisible && !n->isVisible())
             {
                 // not visible on screen
                 isVisible = false;
             }

             oRotation += n->getRotation();

             n = n->getParent();
         }

         oRotation = fmod(oRotation, 360.f);

         oBoundingBox = fOwner->boundingBox();

         CCNode* parent = fOwner->getParent();
         if (parent != nullptr)
         {
        	 oBoundingBox = CCRectApplyAffineTransform(oBoundingBox, parent->nodeToWorldTransform());
         }

    	 oBoundingBox.origin.x = roundf(oBoundingBox.origin.x);
    	 oBoundingBox.origin.y = roundf(oBoundingBox.origin.y);

    	 oBoundingBox.size.width = ceilf(oBoundingBox.size.width);
    	 oBoundingBox.size.height = ceilf(oBoundingBox.size.height);

    	 const float contentScaleFactor = CC_CONTENT_SCALE_FACTOR();

    	 oBoundingBox.origin.x *= contentScaleFactor;
    	 oBoundingBox.origin.y *= contentScaleFactor;
    	 oBoundingBox.size.width *= contentScaleFactor;
    	 oBoundingBox.size.height *= contentScaleFactor;

		 return isVisible;
    }

    void
    CCNativeViewImplAndroid::_updateNativeUIView()
    {
        if (fOwner->isRunning() && !fIsDiscarded)
        {
        	float rotation = 0;
        	CCRect r;
        	const bool isVisible = _computeTransform(rotation, r);

        	_setVisible(isVisible);

        	if (isVisible)
        	{
        		_setTransform(rotation, r);
        	}
        }
        else
        {
        	_setVisible(false);
        }
    }

    void
    CCNativeViewImplAndroid::_onEnter()
    {
        _addToSuperview();
        
        _updateNativeUIView();
        
        if (fUpdater == nullptr)
        {
            fUpdater << new Updater(this);
            fUpdater->listen();
        }
    }
    
    void
    CCNativeViewImplAndroid::_onExit()
    {
        if (fUpdater != nullptr)
        {
            fUpdater->stopListening();
            fUpdater = nullptr;
        }
        
        _removeFromSuperview();
    }
    
    void
    CCNativeViewImplAndroid::onEnter()
    {
        _onEnter();
    }
    
    void
    CCNativeViewImplAndroid::onEnterTransitionDidFinish()
    {
        _onEnter();
    }
    
    void
    CCNativeViewImplAndroid::onExit(void)
    {
        _onExit();
    }
    
    void
    CCNativeViewImplAndroid::onExitTransitionDidStart()
    {
        _onExit();
    }

    void
    CCNativeViewImplAndroid::_addToSuperview()
    {
      	auto controller = nativeViewController();
		assert(controller != nullptr);

		auto& manager = JNIManager::instance();

		auto method = manager.getMethodInfo(kNativeViewControllerJavaClassName, "addToSuperview", "(FFFFF)V");

		if (method != nullptr)
		{
    		CCRect bounds;
    		float rotation = 0;
    		_computeTransform(rotation, bounds);

			 method->callVoidMethod(controller, (jfloat) rotation, (jfloat) bounds.origin.x, (jfloat) bounds.origin.y, (jfloat) bounds.size.width, (jfloat) bounds.size.height);
		}
    }

    void
    CCNativeViewImplAndroid::_removeFromSuperview()
    {
      	auto controller = nativeViewController();
		if (controller != nullptr)
		{
			auto& manager = JNIManager::instance();

			auto method = manager.getMethodInfo(kNativeViewControllerJavaClassName, "removeFromSuperview", "()V");

			if (method != nullptr)
			{
				 method->callVoidMethod(controller);
			}
		}
    }

    void
    CCNativeViewImplAndroid::_setVisible(bool iVisible)
    {
      	auto controller = nativeViewController();
		assert(controller != nullptr);

		auto& manager = JNIManager::instance();

		auto method = manager.getMethodInfo(kNativeViewControllerJavaClassName, "setVisible", "(Z)V");

		if (method != nullptr)
		{
			 method->callVoidMethod(controller, (jboolean) iVisible);
		}
    }

    void
    CCNativeViewImplAndroid::_setTransform(float iRotation, const CCRect& iBoundingBox)
    {
      	auto controller = nativeViewController();
		assert(controller != nullptr);

		auto& manager = JNIManager::instance();

		auto method = manager.getMethodInfo(kNativeViewControllerJavaClassName, "setTransform", "(FFFFF)V");

		if (method != nullptr)
		{
			const jfloat x = iBoundingBox.origin.x;
			const jfloat y = iBoundingBox.origin.y;
			const jfloat width = iBoundingBox.size.width;
			const jfloat height = iBoundingBox.size.height;

			 method->callVoidMethod(controller, (jfloat) iRotation, x, y, width, height);
		}
    }
    
NS_CC_EXT_END


