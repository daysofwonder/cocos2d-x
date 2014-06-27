//
//  NumberRasterizer.cpp
//
//  Created by Gérald Guyomard on 26/02/13.
//  Copyright (c) 2013 Days of Wonder. All rights reserved.

//
//

#include "misc/NumberRasterizer.h"
#include "DOWFoundation/StringUtils.h"
#include <algorithm>
#include "sprite_nodes/CCSpriteFrameCache.h"
#include "sprite_nodes/CCSpriteBatchNode.h"
#include "sprite_nodes/CCSprite.h"

NS_CC_BEGIN

    NumberRasterizer::NumberRasterizer(const std::string& iAtlasPListName)
    : fAtlasPListName(iAtlasPListName)
    {}

    void
    NumberRasterizer::prefetchResources()
    {
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(fAtlasPListName.c_str());
    }

    CCObjectPtr<CCNode>
    NumberRasterizer::rasterize(const int32_t iScore)
    {
        prefetchResources();

        CCSpriteFrameCache* frameCache = CCSpriteFrameCache::sharedSpriteFrameCache();

        CCSpriteFrame* frame = frameCache->spriteFrameByName("1.png");
        assert(frame != nullptr);

        // Create the digits
        std::string score;
        DOW::StringUtils::format(score, "%d", iScore);

        CCObjectPtr<CCSpriteBatchNode> label = CCSpriteBatchNode::createWithTexture(frame->getTexture());
        CCSize labelSize;

        for (char c : score)
        {
            CCSpriteFrame* glyphFrame = nullptr;

            if (c == '+')
            {
                glyphFrame = frameCache->spriteFrameByName("plus.png");
            }
            else if (c == '-')
            {
                glyphFrame = frameCache->spriteFrameByName("moins.png");
            }
            else
            {
                assert(c >= '0');
                assert(c <= '9');

                std::string name;
                DOW::StringUtils::format(name, "%c.png", c);
                glyphFrame = frameCache->spriteFrameByName(name.c_str());
            }

            assert(glyphFrame != nullptr);
            CCSprite* glyph = CCSprite::createWithSpriteFrame(glyphFrame);
            glyph->ignoreAnchorPointForPosition(true);

            const CCSize& glyphSize = glyph->getContentSize();
            labelSize.width += glyphSize.width;
            labelSize.height = std::max(labelSize.height, glyphSize.height);

            label->addChild(glyph);
        }

        label->setContentSize(labelSize);

        // Position glyphes inside label
        CCObject* child;
        CCPoint pos;
        CCARRAY_FOREACH(label->getChildren(), child)
        {
            CCNode* glyph = static_cast<CCNode*>(child);
            const CCSize& glyphSize = glyph->getContentSize();

            pos.y = (labelSize.height - glyphSize.height) / 2;
            glyph->setPosition(pos);

            pos.x += glyphSize.width;
        }

        return label;
    }

    CCObjectPtr<CCNode>
    NumberRasterizer::rasterize(int32_t iScore, CCNode* iParent, float iCoveragePercent)
    {
        CCObjectPtr<CCNode> label = rasterize(iScore);

        assert(iParent != nullptr);
        const CCSize& size = iParent->getContentSize();

        CCSize maxSize(size.width * iCoveragePercent, size.height * iCoveragePercent);

        const CCSize& labelSize = label->getContentSize();

        const float ratioX = maxSize.width / labelSize.width;
        const float ratioY = maxSize.height / labelSize.height;
        const float scale = std::min(std::min(ratioX, ratioY), 1.f);

        //Center label
        label->ignoreAnchorPointForPosition(false);
        label->setAnchorPoint(CCPoint(0.5f, 0.5f));
        label->setPosition(CCPoint(size.width / 2, size.height / 2));
        label->setScale(scale);

        iParent->addChild(label);

        return label;
    }

NS_CC_END

