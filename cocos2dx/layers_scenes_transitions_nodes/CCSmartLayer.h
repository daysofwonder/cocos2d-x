//
//  CCSmartLayer.h
//
// Created by Gérald Guyomard on 10/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCSmartLayer__
#define __CCSmartLayer__

#include "layers_scenes_transitions_nodes/CCLayer.h"
#include "touch_dispatcher/HitTestProtocol.h"
#include <map>
#include <memory>
#include "cocoa/CCObjectPtr.h"
#include "CCBReader/CCNodeLoaderListener.h"

namespace cocos2d
{
    class CCCursor;
    class CCTooltip;

    class CCSmartLayer : public CCLayerRGBA, public HitTestProtocol, public extension::CCNodeLoaderListener
    {
    public:
        DOW_DECLARE_HIERARCHY(CCSmartLayer, CCLayerRGBA);

        static CCSmartLayer* create();
        virtual ~CCSmartLayer();

        // From CCNode
        virtual void registerWithTouchDispatcher(void) override;
        virtual void visit() override;

        virtual void setVisible(bool visible) override;

        virtual void onEnter(void) override;
        virtual void onEnterTransitionDidFinish() override;
        virtual void onExit() override;
        virtual void onExitTransitionDidStart() override;

        // From CCTouchDelegate;
        virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) override;
        virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent) override;
        virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) override;
        virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent) override;

        // From HitTestProtocol
        virtual bool isEnabled() override;
        virtual void setIsEnabled(bool iEnabled) override;

        // From CCRGBAProtocol

        virtual void setColor(const ccColor3B& color) override;
        virtual const ccColor3B& getColor(void) override;

        virtual GLubyte getOpacity(void) override;
        virtual void setOpacity(GLubyte opacity) override;

        virtual void setOpacityModifyRGB(bool bValue) override;
        virtual bool isOpacityModifyRGB(void) override;

        // From CCNodeLoaderListener
        virtual void onNodeLoaded(cocos2d::CCNode* pNode, extension::CCNodeLoader* pNodeLoader) override;

        // Rollover system
        virtual bool canHandleMouseOver() override;
        virtual bool isMouseOver(const CCPoint& iWorldMouseLocation) override;
        virtual void onMouseEnter(const CCPoint& iWorldMouseLocation) override;
        virtual void onMouseOver(const CCPoint& iWorldMouseLocation) override;
        virtual void onMouseExit(const CCPoint& iWorldMouseLocation) override;

        void setTooltipText(const std::string& iText);
        void setTooltipLocalizedTextKey(const std::string& iKey);

        // New methods
        void setSaveOpacitiesAfterLoad(bool iSave) { fSaveOpacitiesAfterLoad = iSave; }

        void saveOpacities();
        bool isUsingRenderTexture() const { return fUseRenderTexture != 0; }
        void useRenderTexture(bool iUse);
        void invalidateRenderTexture();

        bool isSelfDiscardedInRenderTexture() const { return fDiscardSelfInRenderTexture; }
        void setDiscardSelfInRenderTexture(bool iEnable) { fDiscardSelfInRenderTexture = iEnable; }

        bool isClippedToBounds() const { return fClipToBounds; }
        void setClipToBounds(bool iClipToBounds);

        bool shouldNonDescendantNativeNodes() const { return fShouldDiscardNonDescendantNativeNodes; }
        void setShouldDiscardNonDescendantNativeNodes(bool iEnable) { fShouldDiscardNonDescendantNativeNodes = iEnable; }

        enum TouchSequence
        {
            Began,
            Moved,
            Ended,
            Cancelled
        };
        void setSingleTouchMethod(const std::function<bool (CCSmartLayer* iLayer, TouchSequence iTouchSequence, CCTouch *pTouch, CCEvent *pEvent)>& iMethod);

        virtual bool isTouchInside(const CCPoint& iLocation) override;

        virtual bool isClickThrough() override { return fIsClickThrough; }
        void setIsClickThrough(bool iEnable) { fIsClickThrough = iEnable; }
        
    protected:
        CCSmartLayer();

        virtual void _visitChild(CCNode* iChild);

        virtual CCCursor* _cursor() { return nullptr; }

        void    _saveOpacities(CCNode* iRoot);
        void    _modifyOpacity(CCNode* iRoot, GLubyte iGlobalOpacity);
        void    _registerUpdatingRenderTexture();
        void    _updateRenderTexture();
        void    _visitStandard();

        void    _discardNonDescendantNatives();
        void    _restoreNonDescendantNatives();

        void _resetCustomCursor();

        ccColor3B   fColor;
        GLubyte     fOpacity;

        union
        {
            struct
            {
                uint32_t    fDisabled : 1;
                uint32_t    fIsClickThrough: 1;
                uint32_t    fUseRenderTexture : 1;
                uint32_t    fClipToBounds : 1;
                uint32_t    fDiscardSelfInRenderTexture : 1;
                uint32_t    fSaveOpacitiesAfterLoad : 1;

                uint32_t    fShouldDiscardNonDescendantNativeNodes : 1;
                uint32_t    fIsCurrentlyDiscardingNonDescendantNativeNodes : 1;

                uint32_t    fHasChangedMouseCursorOnMouseOver : 1;
            };

            uint32_t fPackedFlags;
        };

        typedef std::map<CCNode*, GLubyte> TDescendantToOpacity;
        std::unique_ptr<TDescendantToOpacity> fDescendantToOpacity;

        class RenderTextureManager;
        CCObjectPtr<CCSprite> fRenderTexture;
        CCPoint               fRenderOffset;

        std::function<bool (CCSmartLayer* iLayer, TouchSequence iTouchSequence, CCTouch *pTouch, CCEvent *pEvent)> fSingleTouchMethod;

        CCObjectPtr<CCTooltip> fTooltip;
    };

} // namespace cocos2d


#endif /* defined(__CCSmartLayer__) */
