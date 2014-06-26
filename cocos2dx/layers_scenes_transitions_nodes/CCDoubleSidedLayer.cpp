//
//  CCDoubleSideLayer.cpp
//
//  Created by Gérald Guyomard on 24/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCDoubleSidedLayer.h"
#include <algorithm>
#undef min
#undef mac

NS_CC_BEGIN
    
    CCDoubleSidedLayer*
    CCDoubleSidedLayer::create()
    {
        CCDoubleSidedLayer* pRet = new CCDoubleSidedLayer();
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
    
    CCDoubleSidedLayer::CCDoubleSidedLayer()
    {}
    
    CCDoubleSidedLayer::~CCDoubleSidedLayer()
    {
        
    }
    
    
void
CCDoubleSidedLayer::setFrontNode(CCNode* iFrontNode)
{
    if (fFrontNode != iFrontNode)
    {
        if (fFrontNode != nullptr)
        {
            fFrontNode->removeFromParentAndCleanup(true);
        }
        
        fFrontNode = iFrontNode;
        if (fFrontNode != nullptr)
        {
			const float sX = fFrontNode->getScaleX();
			if (sX < 0)
			{
				fFrontNode->setScaleX(-sX);
			}

            addChildSafe(fFrontNode);
        }
        
        _layoutFrontAndBackNodes();
    }
}
    
    class CCDoubleSidedLayer::LoadData
    {
    public:
        int fFrontChildTag;
        bool fFrontChildTagSet = false;
        
        int fBackChildTag;
        bool fBackChildTagSet = false;
    };
    
    void
    CCDoubleSidedLayer::setFrontNodeWithChild(int iChildTag)
    {
        setFrontNode(getChildByTag(iChildTag));
    }
    
    void
    CCDoubleSidedLayer::setBackNodeWithChild(int iChildTag)
    {
        setBackNode(getChildByTag(iChildTag));
    }
    
    void
    CCDoubleSidedLayer::setBackNode(CCNode* iBackNode)
    {
        if (fBackNode != iBackNode)
        {
            if (fBackNode != nullptr)
            {
                fBackNode->removeFromParentAndCleanup(true);
            }
            
            fBackNode = iBackNode;
            if (fBackNode != nullptr)
            {
				const float sX = fBackNode->getScaleX();
				if (sX > 0)
				{
					fBackNode->setScaleX(-sX);
				}
                
                addChildSafe(fBackNode);
            }
            
            _layoutFrontAndBackNodes();
        }    
    }
    
    void
    CCDoubleSidedLayer::_setFrontNodeChildTag(int iTag)
    {
        if (fLoadData == nullptr)
        {
            fLoadData.reset(new LoadData);
        }
        
        fLoadData->fFrontChildTagSet = true;
        fLoadData->fFrontChildTag = iTag;
    }
    
    void
    CCDoubleSidedLayer::_setBackNodeChildTag(int iTag)
    {
        if (fLoadData == nullptr)
        {
            fLoadData.reset(new LoadData);
        }
        
        fLoadData->fBackChildTagSet = true;
        fLoadData->fBackChildTag = iTag;
    }

	void
	CCDoubleSidedLayer::swapFrontAndBackNode()
	{
		auto front = fBackNode;
		auto back = fFrontNode;

		fBackNode = nullptr;
		fFrontNode = nullptr;

		setFrontNode(front);
		setBackNode(back);
	}

void
CCDoubleSidedLayer::_layoutFrontAndBackNodes()
{
    // Content Size is the max of the 2 nodes
    CCSize size;
    
    if (fFrontNode != nullptr)
    {
        const CCSize& frontSize = fFrontNode->getContentSize();
        size.width = std::max(size.width, frontSize.width);
        size.height = std::max(size.height, frontSize.height);
    }

    if (fBackNode != nullptr)
    {
        const CCSize& backSize = fBackNode->getContentSize();
        size.width = std::max(size.width, backSize.width);
        size.height = std::max(size.height, backSize.height);
    }
    
    // Center the 2 nodes
    const CCPoint pos(size.width / 2, size.height / 2);
    
    if (fFrontNode != nullptr)
    {
        fFrontNode->ignoreAnchorPointForPosition(false);
        fFrontNode->setAnchorPoint(CCPoint(0.5f, 0.5f));
        fFrontNode->setPosition(pos);
    }

    if (fBackNode != nullptr)
    {
        fBackNode->ignoreAnchorPointForPosition(false);
        fBackNode->setAnchorPoint(CCPoint(0.5f, 0.5f));
        fBackNode->setPosition(pos);
    }
    
    setContentSize(size);
}
    
void
CCDoubleSidedLayer::_visitChild(CCNode* iChild)
{
    if (iChild == fFrontNode)
    {
        const GLboolean isEnabled = glIsEnabled(GL_CULL_FACE);
        glEnable(GL_CULL_FACE);

        glCullFace(GL_BACK);
        iChild->visit();
        
        if (!isEnabled)
        {
            glDisable(GL_CULL_FACE);
        }
    }
    else if (iChild == fBackNode)
    {
        const GLboolean isEnabled = glIsEnabled(GL_CULL_FACE);
        glEnable(GL_CULL_FACE);
        
        // Because scaleX has been flipped
        // the winding of vertices have changed
        // and we must allso cull back faces
        //glCullFace(GL_FRONT);
        glCullFace(GL_BACK);
        
        iChild->visit();
        
        if (!isEnabled)
        {
            glDisable(GL_CULL_FACE);
        }        
    }
    else
    {
        iChild->visit();
    }
}
    
    
CCActionInterval*
CCDoubleSidedLayer::flipAnimation(float iDuration, float iInitialRotationInDegrees)
{
    return CCOrbitCamera::create(iDuration, 1 /*radius*/, 0 /*deltaRadius*/, iInitialRotationInDegrees /*angleZ*/, 180 /*deltaAngleZ*/, 0 /*angleX*/, 0 /*deltaAngleX*/);
}

void
CCDoubleSidedLayer::flip()
{
    CCCamera *camera = getCamera();
    camera->setEyeXYZ(0, 0, 0);
}

	void
	CCDoubleSidedLayer::restoreOrientation()
	{
		// Simply remove the camera
		CC_SAFE_RELEASE_NULL(m_pCamera);
	}

    void
    CCDoubleSidedLayer::onNodeLoaded(CCNode * pNode, extension::CCNodeLoader * pNodeLoader)
    {
        SuperClass::onNodeLoaded(pNode, pNodeLoader);
        
        assert(pNode == this);
        
        if (fLoadData != nullptr)
        {
            if (fLoadData->fFrontChildTagSet)
            {
                setFrontNodeWithChild(fLoadData->fFrontChildTag);
            }

            if (fLoadData->fBackChildTagSet)
            {
                setBackNodeWithChild(fLoadData->fBackChildTag);
            }
            
            fLoadData.reset();
        }
    }

NS_CC_END

NS_CC_EXT_BEGIN

    void
    CCDoubleSidedLayerLoader::onHandlePropTypeInteger(CCNode * pNode, CCNode * pParent, const char* pPropertyName, int pInteger, CCBReader * pCCBReader)
    {
        CCDoubleSidedLayer* layer = static_cast<CCDoubleSidedLayer*>(pNode);
        
        if (strcmp(pPropertyName, "frontNodeTag") == 0)
        {
            layer->_setFrontNodeChildTag(pInteger);
        }
        else if (strcmp(pPropertyName, "backNodeTag") == 0)
        {
            layer->_setBackNodeChildTag(pInteger);
        }
        else
        {
            SuperClass::onHandlePropTypeInteger(pNode, pParent, pPropertyName, pInteger, pCCBReader);
        }
    }

NS_CC_EXT_END
