//
//  RenderTexture.cpp
//
//  Created by Gérald Guyomard on 15/01/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "RenderTexture.h"
#include "support/ccUtils.h"
#include "support/TransformUtils.h"

NS_CC_BEGIN

    RenderTexture * RenderTexture::create(int w, int h, CCTexture2DPixelFormat eFormat)
    {
        RenderTexture *pRet = new RenderTexture();
        
        if(pRet && pRet->initWithWidthAndHeight(w, h, eFormat))
        {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
        
    RenderTexture * RenderTexture::create(int w ,int h, CCTexture2DPixelFormat eFormat, GLuint uDepthStencilFormat)
    {
        RenderTexture *pRet = new RenderTexture();
        
        if(pRet && pRet->initWithWidthAndHeight(w, h, eFormat, uDepthStencilFormat))
        {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
        
    RenderTexture * RenderTexture::create(int w, int h)
    {
        RenderTexture *pRet = new RenderTexture();
        
        if(pRet && pRet->initWithWidthAndHeight(w, h, kCCTexture2DPixelFormat_RGBA8888, 0))
        {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
    
    RenderTexture::RenderTexture()
    {}
    
    RenderTexture::GfxContext*
    RenderTexture::beginLocal(bool iFlipY)
    {
        ccBlendFunc tBlendFunc = {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };
        //ccBlendFunc tBlendFunc = {GL_SRC_ALPHA, GL_ZERO };
        m_pSprite->setBlendFunc(tBlendFunc);

        const CCSize& texSize = m_pTexture->getContentSizeInPixels();
        
        RenderTexture::GfxContext* ctx = new RenderTexture::GfxContext;
        glGetFloatv(GL_VIEWPORT, ctx->fSavedViewport);
        
        // Adjust the orthographic projection and viewport
        glViewport(0, 0, (GLsizei)texSize.width, (GLsizei)texSize.height);
        
        kmGLMatrixMode(KM_GL_PROJECTION);
        kmGLPushMatrix();
        
        kmMat4 orthoMatrix;
        kmMat4OrthographicProjection(&orthoMatrix, 0,  texSize.width,
                                     0, texSize.height, -1,1 );
        kmGLLoadMatrix(&orthoMatrix);
        
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_nOldFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_uFBO);

        kmGLMatrixMode(KM_GL_MODELVIEW);
        kmGLPushMatrix();
        kmGLLoadIdentity();
        
        const float kScaleFactor = CC_CONTENT_SCALE_FACTOR();
        kmGLScalef(kScaleFactor, kScaleFactor, 1);

        if (iFlipY)
        {
            kmGLScalef(1, -1, 1);
            kmGLTranslatef(0, -m_pTexture->getContentSize().height, 0);
        }
        
        return ctx;
    }
    
    RenderTexture::GfxContext*
    RenderTexture::beginLocalWithClear(float r, float g, float b, float a, bool iFlipY)
    {
        GfxContext* ctx = beginLocal(iFlipY);
        
        // save clear color
        GLfloat    clearColor[4];
        glGetFloatv(GL_COLOR_CLEAR_VALUE,clearColor);
        
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // restore clear color
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

        return ctx;
    }
    
    void
    RenderTexture::endLocal(GfxContext* iContext)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_nOldFBO);
        
        kmGLMatrixMode(KM_GL_PROJECTION);
        kmGLPopMatrix();
        
        // restore viewport
        glViewport(iContext->fSavedViewport[0], iContext->fSavedViewport[1], iContext->fSavedViewport[2], iContext->fSavedViewport[3]);
        
        kmGLMatrixMode(KM_GL_MODELVIEW);
        kmGLPopMatrix();
        
        CCDirector *director = CCDirector::sharedDirector();
        director->setProjection(director->getProjection());

        delete iContext;
    }
    
    void
    RenderTexture::renderNode(CCNode* iNode, const ccColor4B& iClearColor)
    {
        const ccColor4F clearColor = ccc4FFromccc4B(iClearColor);
        
        auto* gfxContext = beginLocalWithClear(clearColor.r, clearColor.g, clearColor.b, clearColor.a, true /*flip Y*/);
        {
            const auto inverseTransform = iNode->parentToNodeTransform();
            kmMat4 inverseT;
            CGAffineToGL(&inverseTransform, inverseT.mat);
            
            kmGLMultMatrix(&inverseT);
            
            iNode->visit();
        }
        
        endLocal(gfxContext);
    }

NS_CC_END

