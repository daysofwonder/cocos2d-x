#include "CCNativeViewImplLinux.h"
#include "CCDirector.h"
#include "CCScheduler.h"
#include "platform/linux/SW2EGLView.h"
#include "DOWFoundation/NotificationCenter.h"
#include <boost/log/trivial.hpp>

// to finish move with SW2EGLView...
using namespace swapp;

NS_CC_BEGIN

cocos2d::CCNode* nodeWithKeyboardFocus()
{
    return nullptr;
}

NS_CC_END

NS_CC_EXT_BEGIN

    class CCNativeViewImplLinux::Updater : public cocos2d::CCObject {
    public:
        Updater(CCNativeViewImplLinux* iImpl);
        virtual ~Updater();

        virtual void update(float iTime) override;

        void listen();
        void stopListening();

        void terminate();

    private:
        CCNativeViewImplLinux* fImpl;
    };

    CCNativeViewImplLinux::Updater::Updater(CCNativeViewImplLinux* iImpl)
        : fImpl(iImpl)
    {}

    CCNativeViewImplLinux::Updater::~Updater()
    {
        stopListening();
    }

    void
    CCNativeViewImplLinux::Updater::update(float iTime)
    {
        if (fImpl != nullptr) {
            fImpl->_updateNativeUIView();
        }
    }

    void
    CCNativeViewImplLinux::Updater::listen()
    {
        cocos2d::CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this, -10, false);
    }

    void
    CCNativeViewImplLinux::Updater::stopListening()
    {
        cocos2d::CCDirector::sharedDirector()->getScheduler()->unscheduleUpdateForTarget(this);
    }

    void
    CCNativeViewImplLinux::Updater::terminate()
    {
        stopListening();
        fImpl = nullptr;
    }


    CCNativeViewImplLinux::CCNativeViewImplLinux()
    {
        fContainee=nullptr;
        fIsDiscarded = false;
        fOwner = nullptr;
        fIsVisible = false;
    }

    CCNativeViewImplLinux::~CCNativeViewImplLinux()
    {
    	//assert(!fIsVisible);
    	DOW::NotificationCenter::defaultNotificationCenter().removeObserver(this);
        assert(!fContainee);
        if (fUpdater != nullptr) {
            fUpdater->terminate();
        }
    }

    bool CCNativeViewImplLinux::init(cocos2d::CCNode* iNode)
    {
        fOwner = iNode;
    	assert(fOwner);

        DOW::NotificationCenter& center = DOW::NotificationCenter::defaultNotificationCenter();

        //BOOST_LOG_TRIVIAL(info) << "CCNativeViewImplLinux::init this " << this << std::endl;

        center.addObserver(this, cocos2d::kShouldDiscardNonDescendantNativeNodesNotifID, [this](void* iObserver, const DOW::Notification& iNotification)
                           {
            				//BOOST_LOG_TRIVIAL(info) << "CCNativeViewImplLinux::init lambda this " << this << std::endl;

                               if (!fIsDiscarded)
                               {
                            	   cocos2d::CCNode* node = (cocos2d::CCNode*) iNotification.object();
                                   if (!node->isAncestor(fOwner))
                                   {
                                	   fIsDiscarded = true;
                                	   _updateNativeUIView();
                                   }
                               }
                           });

        center.addObserver(this, cocos2d::kShouldRestoreNonDescendantNativeNodesNotifID, [this](void* iObserver, const DOW::Notification& iNotification)
                           {
                               if (fIsDiscarded)
                               {
                            	   cocos2d::CCNode* node = (cocos2d::CCNode*) iNotification.object();
                                   if (!node->isAncestor(fOwner))
                                   {
                                       fIsDiscarded = false;
                                       _updateNativeUIView();
                                   }
                               }
                           });

        return true;
    }

    cocos2d::CCNode* CCNativeViewImplLinux::owner() const
    {
        return fOwner;
    }

    void CCNativeViewImplLinux::onEnter()
    {
        if (fUpdater == nullptr) {
            fUpdater << new Updater(this);
            fUpdater->listen();
        }
    }

    void CCNativeViewImplLinux::onExit(void)
    {
        if (fUpdater != nullptr) {
            fUpdater->stopListening();
            fUpdater = nullptr;
        }
    }



    void CCNativeViewImplLinux::attachView(GtkWidget *iCountainee)
    {
        assert(!fContainee);
        assert(iCountainee);

        updateViewRect();
        fContainee=iCountainee;

		SW2EGLView::sharedOpenGLView()->attachFloating(GTK_WIDGET(fContainee),fViewRect);
    }

    void CCNativeViewImplLinux::detachView()
    {
        assert(fContainee);
        assert(GTK_IS_WIDGET(fContainee));
		SW2EGLView::sharedOpenGLView()->detachFloating(fContainee);
        fContainee = nullptr;
    }

    bool CCNativeViewImplLinux::isAttached() const
    {
    	return fContainee;
    }

    cocos2d::CCRect
	CCNativeViewImplLinux::_getContentArea()
	{
        return fOwner->boundingBox();
	}

    void
    CCNativeViewImplLinux::updateViewRect()
    {
        if (!fOwner) {
            return;
        }
        cocos2d::CCNode* parent = fOwner->getParent();
        //cocos2d::CCRect r = fOwner->boundingBox();
        cocos2d::CCRect r = _getContentArea();
        r = CCRectApplyAffineTransform(r, parent->nodeToWorldTransform());

        cocos2d::CCEGLViewProtocol* eglView = cocos2d::CCEGLView::sharedOpenGLView();
        const float scaleX = eglView->getScaleX();
        const float scaleY = eglView->getScaleY();

        // flip Y
        const float kHeight = SW2EGLView::sharedOpenGLView()->getHeight()/scaleY;
        const float flipY = kHeight - (r.origin.y + r.size.height);
        r.origin.y = flipY;


        r.origin.x *= scaleX;
        r.origin.y *= scaleY;
        r.size.width *= scaleX;
        r.size.height *= scaleY;

        fViewRect = r;
    }

	void CCNativeViewImplLinux::_onShow()
	{

	}

	void CCNativeViewImplLinux::_onHide()
	{

	}

    void CCNativeViewImplLinux::_updateNativeUIView()
    {
        updateViewRect();

        bool visible = true;
        if (!fOwner->isRunning()) {
        	visible=false;
        }
        if (fIsDiscarded) {
        	visible = false;
        } else {
			cocos2d::CCNode* node = fOwner;
			while (node != nullptr) {
				if (!node->isVisible()) {
					visible=false;
					break;
				}
				node = node->getParent();
			}
        }

		if (fContainee && visible) { //happens, so do not crash
			SW2EGLView::sharedOpenGLView()->moveFloating(fContainee,fViewRect);
		}

        if (fIsVisible != visible) {
        	fIsVisible = visible;
        	if (visible) {
        		SW2EGLView::sharedOpenGLView()->postToGTK([this](){
            		_onShow();
            	});
        	} else {
        		SW2EGLView::sharedOpenGLView()->postToGTK([this](){
            		_onHide();
            	});
        	}
        }

    }

NS_CC_EXT_END

