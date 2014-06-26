//
//  CCDoubleSidedLayer.h
//
//  Created by Gérald Guyomard on 24/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCDoubleSidedLayer__
#define __CCDoubleSidedLayer__

#include "layers_scenes_transitions_nodes/CCSmartLayer.h"
#include "cocoa/CCObjectPtr.h"
#include "CCBReader/CCSmartLayerLoader.h"
#include <memory>

NS_CC_EXT_BEGIN
class CCDoubleSidedLayerLoader;
NS_CC_EXT_END

NS_CC_BEGIN

    class CCDoubleSidedLayer : public CCSmartLayer
    {
    public:
        DOW_DECLARE_HIERARCHY(CCDoubleSidedLayer, CCSmartLayer);
        
        static CCDoubleSidedLayer* create();
        
        CCDoubleSidedLayer();
        virtual ~CCDoubleSidedLayer();
        
        virtual void onNodeLoaded(CCNode * pNode, extension::CCNodeLoader * pNodeLoader) override;
        
        CCNode* frontNode() const { return fFrontNode; }
        void setFrontNode(CCNode* iFrontNode);
        void setFrontNodeWithChild(int iChildTag);
        
        CCNode* backNode() const { return fBackNode; }
        void setBackNode(CCNode* iBackNode);
        void setBackNodeWithChild(int iChildTag);
        
		void swapFrontAndBackNode();

        // Sweet helpers for rotation
        static CCActionInterval* flipAnimation(float iDuration, float iInitialRotationInDegrees = 0);
        void flip();
        
		void restoreOrientation();

    private:
        friend class extension::CCDoubleSidedLayerLoader;
        
        void _setFrontNodeChildTag(int iTag);
        void _setBackNodeChildTag(int iTag);
        
        void _layoutFrontAndBackNodes();
        virtual void _visitChild(CCNode* iChild) override;
        
        CCObjectPtr<CCNode> fFrontNode;
        CCObjectPtr<CCNode> fBackNode;
        
        class LoadData;
        std::unique_ptr<LoadData> fLoadData;
    };

NS_CC_END

NS_CC_EXT_BEGIN

    class CCDoubleSidedLayerLoader : public CCSmartLayerLoader
    {
    public:
        DOW_DECLARE_HIERARCHY(CCDoubleSidedLayerLoader, CCSmartLayerLoader);
        
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCDoubleSidedLayerLoader, loader);
        
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCDoubleSidedLayer);
        
        virtual void onHandlePropTypeInteger(CCNode * pNode, CCNode * pParent, const char* pPropertyName, int pInteger, CCBReader * pCCBReader) override;
    };

NS_CC_EXT_END

#endif /* defined(__SmallWorld2__CCDoubleSidedLayer__) */
