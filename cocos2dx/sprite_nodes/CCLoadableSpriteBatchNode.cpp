//
//  CCLoadableSpriteBatchNode.cpp
//
//  Created by Gérald Guyomard on 08/01/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCLoadableSpriteBatchNode.h"

NS_CC_BEGIN

using namespace extension;

    CCLoadableSpriteBatchNode*
    CCLoadableSpriteBatchNode::create()
    {
        CCLoadableSpriteBatchNode* pRet = new CCLoadableSpriteBatchNode();
        if (pRet && pRet->init())
        {
            pRet->autorelease();
        }
        else
        {
            CC_SAFE_DELETE(pRet);
        }
        return pRet;
    }

    CCLoadableSpriteBatchNode::CCLoadableSpriteBatchNode()
    : fLoading(false)
    {}
    
    void
    CCLoadableSpriteBatchNode::onNodeLoaded(CCNode * pNode, CCNodeLoader * pNodeLoader)
    {
        // Deduce the texture from one child's texture
        CC_ASSERT(pNode == this);
        
        CCTexture2D* texture = nullptr;
        
        CCObject* child;
        CCARRAY_FOREACH(getChildren(), child)
        {
            CCSprite* sprite = dynamic_cast<CCSprite*>(child);
            if (sprite != NULL)
            {
                texture = sprite->getTexture();
                if (texture != NULL)
                {
                    break;
                }
            }
        }
        
        if (texture != NULL)
        {
            setAtlasTexture(texture);
        }
        
        fLoading = false;
        CCARRAY_FOREACH(getChildren(), child)
        {
            CCSprite* sprite = dynamic_cast<CCSprite*>(child);
            if (sprite != NULL)
            {
                appendChild(sprite);
            }
        }        
    }
    
    void
    CCLoadableSpriteBatchNode::addChild(CCNode * child, int zOrder, int tag)
    {
        if (fLoading)
        {
            // Bypass asserts and appending of children in atlas
            CCNode::addChild(child, zOrder, tag);
        }
        else
        {
            CCSpriteBatchNode::addChild(child, zOrder, tag);
        }
    }
        
    void
    CCLoadableSpriteBatchNode::setAtlasTexture(CCTexture2D* iTexture)
    {
        CC_SAFE_RELEASE(m_pobTextureAtlas);
        
        m_pobTextureAtlas = new CCTextureAtlas();
        m_pobTextureAtlas->initWithTexture(iTexture, kDefaultSpriteBatchCapacity);
    }

NS_CC_END

NS_CC_EXT_BEGIN

    CCNode*
    CCLoadableSpriteBatchNodeLoader::createCCNode(cocos2d::CCNode * pParent, cocos2d::extension::CCBReader * pCCBReader)
    {
        CCLoadableSpriteBatchNode* node = CCLoadableSpriteBatchNode::create();
        node->fLoading = true;
        
        return node;
    }

NS_CC_EXT_END
