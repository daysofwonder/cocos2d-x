//
//  CCMultiPassSprite.cpp
//
//  Created by Gérald Guyomard on 12/02/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "CCMultiPassSprite.h"

NS_CC_BEGIN

    CCMultiPassSprite*
    CCMultiPassSprite::create()
    {
        CCMultiPassSprite* pRet = new CCMultiPassSprite();
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
    
    CCMultiPassSprite::CCMultiPassSprite()
    : fNbRenderingPasses(1)
    {
    }
    
    void
    CCMultiPassSprite::draw()
    {
        // Mostly copy and paste of CCSprite draw
        CC_PROFILER_START_CATEGORY(kCCProfilerCategorySprite, "CCSprite - draw");
        
        CCAssert(!m_pobBatchNode, "If CCSprite is being rendered by CCSpriteBatchNode, CCSprite#draw SHOULD NOT be called");
        
        CC_NODE_DRAW_SETUP();
        
        ccGLBlendFunc( m_sBlendFunc.src, m_sBlendFunc.dst );
        
        if (m_pobTexture != NULL)
        {
            ccGLBindTexture2D( m_pobTexture->getName() );
        }
        else
        {
            ccGLBindTexture2D(0);
        }
        
        //
        // Attributes
        //
        
        ccGLEnableVertexAttribs( kCCVertexAttribFlag_PosColorTex );
        
#define kQuadSize sizeof(m_sQuad.bl)
        long offset = (long)&m_sQuad;
        
        // vertex
        int diff = offsetof( ccV3F_C4B_T2F, vertices);
        glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));
        
        // texCoods
        diff = offsetof( ccV3F_C4B_T2F, texCoords);
        glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));
        
        // color
        diff = offsetof( ccV3F_C4B_T2F, colors);
        glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));
        
        for (uint32_t i = 0; i < fNbRenderingPasses; ++i)
        {
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);            
        }
        
        CHECK_GL_ERROR_DEBUG();
        
        
#if CC_SPRITE_DEBUG_DRAW == 1
        // draw bounding box
        CCPoint vertices[4]={
            ccp(m_sQuad.tl.vertices.x,m_sQuad.tl.vertices.y),
            ccp(m_sQuad.bl.vertices.x,m_sQuad.bl.vertices.y),
            ccp(m_sQuad.br.vertices.x,m_sQuad.br.vertices.y),
            ccp(m_sQuad.tr.vertices.x,m_sQuad.tr.vertices.y),
        };
        ccDrawPoly(vertices, 4, true);
#elif CC_SPRITE_DEBUG_DRAW == 2
        // draw texture box
        CCSize s = this->getTextureRect().size;
        CCPoint offsetPix = this->getOffsetPosition();
        CCPoint vertices[4] = {
            ccp(offsetPix.x,offsetPix.y), ccp(offsetPix.x+s.width,offsetPix.y),
            ccp(offsetPix.x+s.width,offsetPix.y+s.height), ccp(offsetPix.x,offsetPix.y+s.height)
        };
        ccDrawPoly(vertices, 4, true);
#endif // CC_SPRITE_DEBUG_DRAW
        
        CC_INCREMENT_GL_DRAWS(fNbRenderingPasses);
        
        CC_PROFILER_STOP_CATEGORY(kCCProfilerCategorySprite, "CCSprite - draw");
    }

NS_CC_END
