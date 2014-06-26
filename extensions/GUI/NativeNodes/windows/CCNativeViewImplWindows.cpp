//

#include "GUI/NativeNodes/windows/CCNativeViewImplWindows.h"
#include "DOWFoundation/NotificationCenter.h"
#include "CCEGLView.h"
#include "layers_scenes_transitions_nodes/CCScene.h"

#include <algorithm>
#include <boost/log/trivial.hpp>

using namespace DOW;

namespace
{

	struct WinAndNode{
		HWND window;
		cocos2d::CCNode* node;
	};
	static std::vector<WinAndNode> gHWinToNode;

	static void removeFromMap(cocos2d::CCNode* iNode) {
		gHWinToNode.erase(std::remove_if(gHWinToNode.begin(), gHWinToNode.end(), [iNode](const WinAndNode& pair){
			return pair.node == iNode;
		}), gHWinToNode.end());
	}

	static void addToMap(HWND window, cocos2d::CCNode* node) {
		removeFromMap(node);

		CHAR className[255];
		GetClassName(window, className, 255);

		WinAndNode element;
		element.window = window;
		element.node = node;
		gHWinToNode.push_back(element);
	}


}

NS_CC_EXT_BEGIN

    class CCNativeViewImplWindows::Updater : public CCObject
    {
    public:
		Updater(CCNativeViewImplWindows* iImpl);
        virtual ~Updater();
        
        virtual void update(float iTime) override;
        
        void listen();
        void stopListening();
        
        void terminate();
        
    private:
		CCNativeViewImplWindows* fImpl;
    };
    
	CCNativeViewImplWindows::Updater::Updater(CCNativeViewImplWindows* iImpl)
    : fImpl(iImpl)
    {}
    
	CCNativeViewImplWindows::Updater::~Updater()
    {
        stopListening();
    }
    
    void
		CCNativeViewImplWindows::Updater::update(float iTime)
    {
        if (fImpl != nullptr)
        {
            fImpl->_updateNativeUIView();            
        }
    }

    void
		CCNativeViewImplWindows::Updater::listen()
    {
        CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this, -10, false);
    }
    
    void
		CCNativeViewImplWindows::Updater::stopListening()
    {
        CCDirector::sharedDirector()->getScheduler()->unscheduleUpdateForTarget(this);
    }

    void
		CCNativeViewImplWindows::Updater::terminate()
    {
        stopListening();
        fImpl = nullptr;
    }

	CCNativeViewImplWindows::CCNativeViewImplWindows()
    :  fIsDiscarded(false)
    {
		fZoomPercent = 100;
	}
    
	CCNativeViewImplWindows::~CCNativeViewImplWindows()
    {
        NotificationCenter::defaultNotificationCenter().removeObserver(this);        
        if (fUpdater != nullptr)
        {
            fUpdater->terminate();
        }
    }
    
	static CCNativeViewImplWindows::Accelerator gAccelerator;

	void CCNativeViewImplWindows::setAccelerator(Accelerator iAccelerator) {
		gAccelerator = iAccelerator;
	}

	bool CCNativeViewImplWindows::processAccelerator(LPMSG lpmsg)
	{
		if (gAccelerator) {
			return gAccelerator(lpmsg);
		}
		return false;
	}

    bool CCNativeViewImplWindows::init(CCNode* iNode)
    {
        fOwner = iNode;

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
    
	CCRect CCNativeViewImplWindows::getContentArea()
	{
		CCRect result;
		result.origin.x = 0;
		result.origin.y = 0;
		result.size.width = owner()->getContentSize().width;
		result.size.height = owner()->getContentSize().height;
		return result;
	}

	bool CCNativeViewImplWindows::isVisible() const
	{
		return true;
	}

    void CCNativeViewImplWindows::_updateNativeUIView()
    {
		if (!fAxWindow.IsWindow()) {
			return;
		}
		if (fOwner->isRunning() && !fIsDiscarded && isVisible()) {
			CCNode* node = fOwner;
			while (node != nullptr) {
				if (!node->isVisible()) {
					fAxWindow.ShowWindow(SW_HIDE);
					BOOL success = fAxWindow.SetWindowPos(NULL, &fBoundsRect, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW);
					assert(success);
					return;
				}
				node = node->getParent();
			}
			

			CWindow ownerWindow = fAxWindow.GetWindow(GW_OWNER);
			const bool isPopup = ownerWindow != nullptr; // i.e. not child of openGLView

			CCEGLView* openGLView = CCEGLView::sharedOpenGLView();

			CCRect localRect = getContentArea();
			CCRect worldRect = CCRectApplyAffineTransform(localRect, owner()->nodeToWorldTransform());
			const float screenHeight = openGLView->getFrameSize().height;
			const CCRect& viewPortRect = openGLView->getViewPortRect();
			const float scaleX = openGLView->getScaleX();
			const float scaleY = openGLView->getScaleY();

			CCRect screenRect;
			screenRect.size.width = worldRect.size.width*scaleX;
			screenRect.size.height = worldRect.size.height*scaleY;
			screenRect.origin.x = viewPortRect.origin.x + scaleX*worldRect.origin.x;
			screenRect.origin.y = viewPortRect.origin.y + scaleY*worldRect.origin.y;

			fBoundsRect.top = screenHeight - screenRect.origin.y - screenRect.size.height;
			fBoundsRect.left = screenRect.origin.x;
			fBoundsRect.bottom = screenHeight - screenRect.origin.y;
			fBoundsRect.right = screenRect.origin.x + screenRect.size.width;

			RECT oldBounds;
			BOOL isOK = fAxWindow.GetWindowRect(&oldBounds);
			assert(isOK);
			HWND openGLWin = openGLView->getHWnd();

			if (isPopup) {
				// also offset by position of openGLView
				int offset = MapWindowPoints(openGLWin, HWND_DESKTOP, reinterpret_cast<LPPOINT>(&fBoundsRect), 2);
				//assert(offset!=0);
			}
			else {
				int offset = MapWindowPoints(HWND_DESKTOP, openGLWin, reinterpret_cast<LPPOINT>(&oldBounds), 2);
				//assert(offset != 0);
			}

			CComPtr<IWebBrowser2> pWebBrowser2;
			HRESULT hr1 = uiView().QueryControl(&pWebBrowser2);
			if (hr1 == S_OK) {
				if (pWebBrowser2) {
					const int32_t scale = screenRect.size.height * 100 / localRect.size.height; // or worldRect.size.height depending on OLECMDID_OPTICAL_ZOOM semantics that change between IE versions !

					if (scale != fZoomPercent) {
						CComVariant vZoom = scale;
						HRESULT hr2 = pWebBrowser2->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &vZoom, NULL);
						BOOST_LOG_TRIVIAL(debug) << "web zoom hresult: " << hr2 << std::endl;
						fZoomPercent = scale;
					}
				}
			}


			const LONG style = fAxWindow.GetWindowLong(GWL_STYLE);
			if (!(style & WS_VISIBLE)) {
				BOOL success = fAxWindow.SetWindowPos(HWND_TOP, &fBoundsRect, SWP_NOZORDER | SWP_SHOWWINDOW);
				assert(success);
			}
			else {
				if (!EqualRect(&oldBounds,&fBoundsRect)) {
					BOOL success = fAxWindow.SetWindowPos(HWND_TOP, &fBoundsRect, SWP_NOZORDER | SWP_NOACTIVATE);
					assert(success);
				}
			}
		} else {
			const LONG style = fAxWindow.GetWindowLong(GWL_STYLE);
			if (style & WS_VISIBLE) {
				BOOL success = fAxWindow.SetWindowPos(HWND_TOP, &fBoundsRect, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);
				assert(success);
			}
		}
    }
    
    void
	CCNativeViewImplWindows::_onEnter()
    {
        //[[EAGLView sharedEGLView] addSubview:fUIView];
        
        _updateNativeUIView();
        
        if (fUpdater == nullptr)
        {
            fUpdater << new Updater(this);
            fUpdater->listen();
        }
    }
    
    void
	CCNativeViewImplWindows::_onExit()
    {
        if (fUpdater != nullptr)
        {
            fUpdater->stopListening();
            fUpdater = nullptr;
        }
		if (fAxWindow.IsWindow()) {
			fAxWindow.DestroyWindow();
		}
    }

    void
		CCNativeViewImplWindows::onEnter()
	{
		if (!fAxWindow.IsWindow()) { 
			_createUIView(); //lazy init
			assert(fAxWindow.IsWindow());
		}
		addToMap(fAxWindow.m_hWnd, fOwner);

        _onEnter();
    }
    
    void
		CCNativeViewImplWindows::onEnterTransitionDidFinish()
    {
        _onEnter();        
    }
    
    void
		CCNativeViewImplWindows::onExit(void)
    {
        _onExit();
    }
    
    void
		CCNativeViewImplWindows::onExitTransitionDidStart()
    {
        _onExit();
		if (fAxWindow.IsWindow()) {
			fAxWindow.DestroyWindow();
		}
    }

NS_CC_EXT_END

NS_CC_BEGIN

cocos2d::CCNode* nodeWithKeyboardFocus()
{
	HWND focussedWindow = GetFocus();
	if (focussedWindow) {
		CHAR className[255];
		GetClassName(focussedWindow, className, 255);
		HWND cocosWindow = CCEGLView::sharedOpenGLView()->getHWnd();
		if (focussedWindow == cocosWindow) {
			return CCDirector::sharedDirector()->getRunningScene();
		}
		HWND parent = GetParent(focussedWindow);
		CHAR parentName[255];
		GetClassName(parent, parentName, 255);
		for (const WinAndNode& pair : gHWinToNode) {
			if ((pair.window == focussedWindow) || (pair.window == parent)) {
				return pair.node;
			}
		}
	}
	return NULL;
}

NS_CC_END

