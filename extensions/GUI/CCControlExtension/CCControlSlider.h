/*
 * Copyright (c) 2012 cocos2d-x.org
 * http://www.cocos2d-x.org
 *
 * Copyright 2011 Yannick Loriot. All rights reserved.
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
 */


#ifndef __CCCONTROL_SLIDER_H__
#define __CCCONTROL_SLIDER_H__

#include "CCControl.h"
#include "CCInvocation.h"
#include "CCBReader/CCNodeLoaderListener.h"
#include "CCBReader/CCControlLoader.h"

NS_CC_EXT_BEGIN

/**
 * @addtogroup GUI
 * @{
 * @addtogroup control_extension
 * @{
 */

class CCControlSlider: public CCControl, public CCNodeLoaderListener
{
    //maunally put in the setters
    /** Contains the receiver¡¯s current value. */
    CC_SYNTHESIZE_READONLY(float, m_value, Value);
    virtual void setValue(float val);
    /** Contains the minimum value of the receiver. 
    * The default value of this property is 0.0. */
    CC_SYNTHESIZE_READONLY(float, m_minimumValue, MinimumValue);
    virtual void setMinimumValue(float val);
    /** Contains the maximum value of the receiver. 
    * The default value of this property is 1.0. */
    CC_SYNTHESIZE_READONLY(float, m_maximumValue, MaximumValue);
    virtual void setMaximumValue(float val);
    virtual void setEnabled(bool enabled);
    virtual bool isTouchInside(CCTouch * touch);
    CCPoint locationFromTouch(CCTouch* touch);

    CC_SYNTHESIZE(float, m_minimumAllowedValue, MinimumAllowedValue);
    CC_SYNTHESIZE(float, m_maximumAllowedValue, MaximumAllowedValue);
    CC_SYNTHESIZE_RETAIN(CCInvocation*, m_endValueChanged, EndValueChanged);

    // maybe this should be read-only
    CC_SYNTHESIZE_RETAIN(CCSprite*, m_thumbSprite, ThumbSprite);
    CC_SYNTHESIZE_RETAIN(CCSprite*, m_progressSprite, ProgressSprite);
    CC_SYNTHESIZE_RETAIN(CCSprite*, m_backgroundSprite, BackgroundSprite);

    bool    m_isVertical;
    CCRect  m_SlideInsets;

public:
    CCControlSlider();
    virtual ~CCControlSlider();

    /** 
    * Initializes a slider with a background sprite, a progress bar and a thumb
    * item.
    *
    * @param backgroundSprite  CCSprite, that is used as a background.
    * @param progressSprite    CCSprite, that is used as a progress bar.
    * @param thumbItem         CCSprite, that is used as a thumb.
    * @param isVertical        bool, indicating whether this slider is vertical or horizontal
    */
    virtual bool initWithSprites(CCSprite * backgroundSprite, CCSprite* progressSprite, CCSprite* thumbSprite, bool isVertical = false);


    /** 
    * Creates slider with a background filename, a progress filename and a 
    * thumb image filename.
    */
    static CCControlSlider* create(const char* bgFile, const char* progressFile, const char* thumbFile, bool isVertical = false);

    /** 
    * Creates a slider with a given background sprite and a progress bar and a
    * thumb item.
    *
    * @see initWithBackgroundSprite:progressSprite:thumbMenuItem:
    */
    static CCControlSlider* create(CCSprite * backgroundSprite, CCSprite* pogressSprite, CCSprite* thumbSprite, bool isVertical = false);

    bool isVertical() const { return m_isVertical; }
    CCRect slideArea();
    
    virtual void needsLayout();
    
    // Overridden from CCNodeLoaderListener
    virtual void onNodeLoaded(CCNode * pNode, CCNodeLoader * pNodeLoader);

protected:
    friend class CCControlSliderLoader;
    
    enum EComponentTags
    {
        kThumbTag = 1,
        kProgressTag = 2,
        kBackgroundTag = 3
    };
    
    void postInit();
    
    static CCControlSlider* createForLoad();
    bool initForLoad();
    
    void sliderBegan(CCPoint location);
    void sliderMoved(CCPoint location);
    void sliderEnded(CCPoint location);

    virtual bool ccTouchBegan(CCTouch* touch, CCEvent* pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

/** Returns the value for the given location. */
    float valueForLocation(CCPoint location);
};

class CCControlSliderLoader : public CCControlLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCControlSliderLoader, loader);
    
    virtual CCControlSlider* createCCNode(cocos2d::CCNode * pParent, cocos2d::extension::CCBReader * pCCBReader)
    {
        return CCControlSlider::createForLoad();
    }
    
    virtual void onHandlePropTypeCheck(CCNode * pNode, CCNode * pParent, const char * pPropertyName, bool pCheck, CCBReader * pCCBReader);
    virtual void onHandlePropTypeFloat(CCNode * pNode, CCNode * pParent, const char* pPropertyName, float pFloat, CCBReader * pCCBReader);
};

// end of GUI group
/// @}
/// @}

NS_CC_EXT_END

#endif
