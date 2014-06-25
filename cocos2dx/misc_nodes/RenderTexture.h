//
//  RenderTexture.h
//
//  Created by Gérald Guyomard on 15/01/13.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#ifndef __RenderTexture__
#define __RenderTexture__

#include <cocos2d.h>

NS_CC_BEGIN

    class RenderTexture : public CCRenderTexture
    {
    public:
        static RenderTexture * create(int w ,int h, CCTexture2DPixelFormat eFormat, GLuint uDepthStencilFormat);
        static RenderTexture * create(int w, int h, CCTexture2DPixelFormat eFormat);
        static RenderTexture * create(int w, int h);
        
        class GfxContext
        {
        private:
            friend class RenderTexture;
            GLfloat fSavedViewport[4];
        };

        GfxContext* beginLocal(bool iFlipY = false);
        GfxContext* beginLocalWithClear(float r, float g, float b, float a, bool iFlipY = false);
        void endLocal(GfxContext* iContext);
        
        // after call, the sprite stores the rendering of iNode in local coordinates
        void renderNode(CCNode* iNode, const ccColor4B& iClearColor = ccc4(0, 0, 0, 0));
        
    protected:
        RenderTexture();
    };

NS_CC_END

#endif /* defined(__RenderTexture__) */
