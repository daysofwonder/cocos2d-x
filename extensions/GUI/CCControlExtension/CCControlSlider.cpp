/*
 * Copyright (c) 2012 cocos2d-x.org
 * http://www.cocos2d-x.org
 *
 * Copyright 2011 Yannick Loriot.
 * http://yannickloriot.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Converted to c++ / cocos2d-x by Angus C
*
 */

#include "CCControlSlider.h"
#include "support/CCPointExtension.h"
#include "touch_dispatcher/CCTouch.h"
#include "CCDirector.h"

NS_CC_EXT_BEGIN

CCControlSlider::CCControlSlider()
: m_value(0.0f)
, m_minimumValue(0.0f)
, m_maximumValue(0.0f)
, m_minimumAllowedValue(0.0f)
, m_maximumAllowedValue(0.0f)
, m_thumbSprite(NULL)
, m_progressSprite(NULL)
, m_backgroundSprite(NULL)
, m_isVertical(false)
, m_endValueChanged(NULL)
{

}

CCControlSlider::~CCControlSlider()
{
    CC_SAFE_RELEASE(m_thumbSprite);
    CC_SAFE_RELEASE(m_progressSprite);
    CC_SAFE_RELEASE(m_backgroundSprite);
    CC_SAFE_RELEASE(m_endValueChanged);
}

CCControlSlider* CCControlSlider::create(const char* bgFile, const char* progressFile, const char* thumbFile, bool isVertical)
{
    // Prepare background for slider
    CCSprite *backgroundSprite      = CCSprite::create(bgFile);
    
    // Prepare progress for slider
    CCSprite *progressSprite        = CCSprite::create(progressFile);
    
    // Prepare thumb (menuItem) for slider
    CCSprite *thumbSprite           = CCSprite::create(thumbFile);
    
    return CCControlSlider::create(backgroundSprite, progressSprite, thumbSprite, isVertical);
}

CCControlSlider* CCControlSlider::create(CCSprite * backgroundSprite, CCSprite* pogressSprite, CCSprite* thumbSprite, bool isVertical)
{
    CCControlSlider *pRet = new CCControlSlider();
    pRet->initWithSprites(backgroundSprite, pogressSprite, thumbSprite, isVertical);
    pRet->autorelease();
    return pRet;
}

CCControlSlider* CCControlSlider::createForLoad()
{
    CCControlSlider *pRet = new CCControlSlider();
    pRet->initForLoad();
    pRet->autorelease();
    return pRet;
}

CCRect CCControlSlider::slideArea()
{
    CCRect area;
    area.size = getContentSize();
    
    area.origin.x += m_SlideInsets.origin.x;
    area.origin.y += m_SlideInsets.origin.y;
    
    area.size.width -= m_SlideInsets.getMaxX();
    area.size.height -= m_SlideInsets.getMaxY();
    
    return area;
}

 bool CCControlSlider::initWithSprites(CCSprite * backgroundSprite, CCSprite* progressSprite, CCSprite* thumbSprite, bool isVertical)
 {
     if (CCControl::init())
     {
         CCAssert(backgroundSprite,  "Background sprite must be not nil");
         CCAssert(progressSprite,    "Progress sprite must be not nil");
         CCAssert(thumbSprite,       "Thumb sprite must be not nil");
         
         ignoreAnchorPointForPosition(false);
         setTouchEnabled(true);
         
         this->setBackgroundSprite(backgroundSprite);
         this->setProgressSprite(progressSprite);
         this->setThumbSprite(thumbSprite);
         
         addChild(m_backgroundSprite);
         addChild(m_progressSprite);
         addChild(m_thumbSprite);
         
         m_isVertical = isVertical;
         
         m_backgroundSprite->ignoreAnchorPointForPosition(false);
         m_backgroundSprite->setAnchorPoint(ccp(0.5f, 0.5f));
         
         CCRect maxRect   = CCControlUtils::CCRectUnion(m_backgroundSprite->boundingBox(), m_thumbSprite->boundingBox());
         
         setContentSize(CCSizeMake(maxRect.size.width, maxRect.size.height));
         
         postInit();
         
         // Init default values
         m_minimumValue                   = 0.0f;
         m_maximumValue                   = 1.0f;
         
         setValue(m_minimumValue);
         return true;
     }
     
     return false;
 }

void CCControlSlider::postInit()
{    
    const CCRect area = slideArea();
    
    // Add the progress bar
    if (m_progressSprite != NULL)
    {
        m_progressSprite->ignoreAnchorPointForPosition(false);
        
        if (isVertical())
        {
            m_progressSprite->setAnchorPoint(ccp(0.5f, 0.0f));
            m_progressSprite->setPosition(ccp(area.origin.x + (area.size.width / 2), area.origin.y));
        }
        else
        {
            m_progressSprite->setAnchorPoint(ccp(0.0f, 0.5f));
            m_progressSprite->setPosition(ccp(area.origin.x, area.origin.y + (area.size.height / 2)));
        }
    }
    
    m_thumbSprite->ignoreAnchorPointForPosition(false);
    m_thumbSprite->setAnchorPoint(CCPoint(0.5f, 0.5f));
    
    // Add the slider thumb
    if (isVertical())
    {
        m_thumbSprite->setPosition(ccp(area.origin.x + (area.size.width / 2), area.origin.y));
    }
    else
    {
        m_thumbSprite->setPosition(ccp(area.origin.x, area.origin.y + (area.size.height / 2)));
    }
}

bool CCControlSlider::initForLoad()
{
    if (CCControl::init())
    {
        ignoreAnchorPointForPosition(false);
        setTouchEnabled(true);
        
        // Init default values
        m_minimumValue                   = 0.0f;
        m_maximumValue                   = 1.0f;

        return true;
    }
    else
    {
        return false;
    }
}

void CCControlSlider::onNodeLoaded(CCNode * pNode, CCNodeLoader * pNodeLoader)
{
    CCAssert(pNode == this, "onNodeLoaded called not for me");
    
    // Find the background, thumb and progress in our children
    CCSprite* thumb = dynamic_cast<CCSprite*>(getChildByTag(kThumbTag));
    CCAssert(thumb != NULL, "There should be a thumb as child");
    
    setThumbSprite(thumb);
    
    // optional
    CCSprite* progress = dynamic_cast<CCSprite*>(getChildByTag(kProgressTag));
    setProgressSprite(progress);

    CCSprite* background = dynamic_cast<CCSprite*>(getChildByTag(kBackgroundTag));
    setBackgroundSprite(background);
    
    postInit();
}

void CCControlSlider::setEnabled(bool enabled)
{
    CCControl::setEnabled(enabled);
    if (m_thumbSprite != NULL) 
    {
        m_thumbSprite->setOpacity((enabled) ? 255 : 128);
    }
}

 void CCControlSlider::setValue(float value)
 {
     // set new value with sentinel
     if (value < m_minimumValue)
     {
         value = m_minimumValue;
     }

     if (value > m_maximumValue) 
     {
         value = m_maximumValue;
     }

     m_value = value;

     this->needsLayout();

     this->sendActionsForControlEvents(CCControlEventValueChanged);
 }

 void CCControlSlider::setMinimumValue(float minimumValue)
 {
     m_minimumValue=minimumValue;
     m_minimumAllowedValue = minimumValue;
     if (m_minimumValue >= m_maximumValue)    
     {
        m_maximumValue   = m_minimumValue + 1.0f;
     }
     setValue(m_value);
 }

 void CCControlSlider::setMaximumValue(float maximumValue)
 {
     m_maximumValue=maximumValue;
     m_maximumAllowedValue = maximumValue;
     if (m_maximumValue <= m_minimumValue)   
     {
        m_minimumValue   = m_maximumValue - 1.0f;
     }
     setValue(m_value);
 }

bool CCControlSlider::isTouchInside(CCTouch * touch)
{
    CCPoint touchLocation   = touch->getLocation();
    touchLocation           = convertToNodeSpace(touchLocation);
    
    CCRect rect             = slideArea();
    
    if (isVertical())
    {
        rect.size.height         += m_thumbSprite->getContentSize().height;
        rect.origin.y           -= m_thumbSprite->getContentSize().height / 2;
    }
    else
    {
        rect.size.width         += m_thumbSprite->getContentSize().width;
        rect.origin.x           -= m_thumbSprite->getContentSize().width / 2;
    }
    
    return rect.containsPoint(touchLocation);
}

CCPoint CCControlSlider::locationFromTouch(CCTouch* touch)
{
    CCPoint touchLocation   = touch->getLocation();                      // Get the touch position
    touchLocation           = this->convertToNodeSpace(touchLocation);                  // Convert to the node space of this class
    
    CCRect rect             = slideArea();
    
    if (isVertical())
    {
        if (touchLocation.y < rect.origin.y)
        {
            touchLocation.y     = rect.origin.y;
        }
        else if (touchLocation.y > rect.getMaxY())
        {
            touchLocation.y     = rect.getMaxY();
        }
    }
    else
    {
        if (touchLocation.x < rect.origin.x)
        {
            touchLocation.x = rect.origin.x;
        }
        else if (touchLocation.x > rect.getMaxX())
        {
            touchLocation.x = rect.getMaxX();
        }
    }
    
    return touchLocation;
}


bool CCControlSlider::ccTouchBegan(CCTouch* touch, CCEvent* pEvent)
{
    if (!isTouchInside(touch) || !isEnabled() || !isVisible())
    {
        return false;
    }

    CCPoint location = locationFromTouch(touch);
    sliderBegan(location);
    return true;
}

void CCControlSlider::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint location = locationFromTouch(pTouch);
    sliderMoved(location);
}

void CCControlSlider::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    sliderEnded(CCPointZero);
}

void CCControlSlider::needsLayout()
{
    if (NULL == m_thumbSprite)
    {
        return;
    }
    // Update thumb position for new value
    float percent               = (m_value - m_minimumValue) / (m_maximumValue - m_minimumValue);
    
    CCPoint pos                 = m_thumbSprite->getPosition();
    const CCRect area = slideArea();
    
    if (isVertical())
    {
        pos.y = area.origin.y + (percent * area.size.height);
    }
    else
    {
        pos.x = area.origin.x + (percent * area.size.width);
    }
    
    m_thumbSprite->setPosition(pos);
    
    if (m_progressSprite != NULL)
    {
        // Stretches content proportional to newLevel
        CCRect textureRect          = m_progressSprite->getTextureRect();
        
        if (isVertical())
        {
            textureRect = CCRectMake(textureRect.origin.x, textureRect.origin.y, textureRect.size.width, pos.y);
        }
        else
        {
            textureRect = CCRectMake(textureRect.origin.x, textureRect.origin.y, pos.x, textureRect.size.height);
        }
        
        m_progressSprite->setTextureRect(textureRect, m_progressSprite->isTextureRectRotated(), textureRect.size);
    }

}

void CCControlSlider::sliderBegan(CCPoint location)
{
    this->setSelected(true);
    this->getThumbSprite()->setColor(ccGRAY);
    setValue(valueForLocation(location));
}

void CCControlSlider::sliderMoved(CCPoint location)
{
    setValue(valueForLocation(location));
}

void CCControlSlider::sliderEnded(CCPoint location)
{
    if (this->isSelected())
    {
        setValue(valueForLocation(m_thumbSprite->getPosition()));
        if (m_endValueChanged != NULL)
        {
            m_endValueChanged->invoke(this);
        }
    }
    this->getThumbSprite()->setColor(ccWHITE);
    this->setSelected(false);
}

float CCControlSlider::valueForLocation(CCPoint location)
{
    const CCRect area = slideArea();
    
    float percent;
    
    if (isVertical())
    {
        percent = (location.y - area.origin.y) / area.size.height;
    }
    else
    {
        percent = (location.x - area.origin.x) / area.size.width;
    }
    
    return MAX(MIN(m_minimumValue + percent * (m_maximumValue - m_minimumValue), m_maximumAllowedValue), m_minimumAllowedValue);
}

void CCControlSliderLoader::onHandlePropTypeCheck(CCNode * pNode, CCNode * pParent, const char * pPropertyName, bool pCheck, CCBReader * pCCBReader)
{
    CCControlSlider* slider = dynamic_cast<CCControlSlider*>(pNode);
    CCAssert(slider != NULL, "CCControlSliderLoader: can only manage CCControlSlider");
    
    if (strcmp(pPropertyName, "isVertical") == 0)
    {
        slider->m_isVertical = pCheck;
    }
    else
    {
        CCControlLoader::onHandlePropTypeCheck(pNode, pParent, pPropertyName, pCheck, pCCBReader);
    }
}

void CCControlSliderLoader::onHandlePropTypeFloat(CCNode * pNode, CCNode * pParent, const char* pPropertyName, float pFloat, CCBReader * pCCBReader)
{
    CCControlSlider* slider = dynamic_cast<CCControlSlider*>(pNode);
    CCAssert(slider != NULL, "CCControlSliderLoader: can only manage CCControlSlider");

    if (strcmp(pPropertyName, "minimumValue") == 0)
    {
        slider->setMinimumValue(pFloat);
    }
    else if (strcmp(pPropertyName, "maximumValue") == 0)
    {
        slider->setMaximumValue(pFloat);
    }
    else
    {
        CCControlLoader::onHandlePropTypeFloat(pNode, pParent, pPropertyName, pFloat, pCCBReader);
    }
    
}

NS_CC_EXT_END
