//
//  CCLoadableSpriteBatchNode.h
//
//  Created by Gérald Guyomard on 08/01/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __CCLoadableSpriteBatchNode__
#define __CCLoadableSpriteBatchNode__

#include "sprite_nodes/CCSpriteBatchNode.h"
#include "CCBReader/CCNodeLoader.h"
#include "CCBReader/CCNodeLoaderListener.h"

NS_CC_EXT_BEGIN
    class CCLoadableSpriteBatchNodeLoader;
NS_CC_EXT_END

NS_CC_BEGIN

    class CCLoadableSpriteBatchNodeLoader;
    
    class CCLoadableSpriteBatchNode : public CCSpriteBatchNode, public extension::CCNodeLoaderListener
    {
    public:
        static CCLoadableSpriteBatchNode* create();

        virtual void addChild(CCNode * child, int zOrder, int tag) override;        
        virtual void onNodeLoaded(CCNode * pNode, extension::CCNodeLoader * pNodeLoader) override;

        void setAtlasTexture(CCTexture2D* iTexture);
        
    private:
        friend class extension::CCLoadableSpriteBatchNodeLoader;
        CCLoadableSpriteBatchNode();
        
        bool fLoading;
    };

NS_CC_END

NS_CC_EXT_BEGIN

    class CCLoadableSpriteBatchNodeLoader : public extension::CCNodeLoader
    {
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCLoadableSpriteBatchNodeLoader, loader);
        
        virtual CCNode* createCCNode(cocos2d::CCNode * pParent, cocos2d::extension::CCBReader * pCCBReader) override;

    };

NS_CC_EXT_END


#endif /* defined(__CCLoadableSpriteBatchNode__) */
