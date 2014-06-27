//
//  Animation.cpp
//
//  Created by Gérald Guyomard on 18/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "animations/Animation.h"
#include "CCDirector.h"

NS_CC_BEGIN
    
Animation::Animation()
: fDelegate(nullptr), fIsPaused(false), fSkipping(false)
{
}
    
Animation::~Animation()
{        
}
    
void
Animation::_didStop(bool iFinished)
{
	if (skipping())
	{
		_didSkip(iFinished);
	}

    if (fDelegate != nullptr)
    {
		fDelegate->animationDidStop(this, iFinished);
    }
}
        
bool
Animation::touchesEnabled() const
{
    return false;
}
    
void
Animation::finish()
{
    _didStop(true);
}

bool
Animation::pause()
{
    if (isPaused())
    {
        return true;
    }
    else
    {
        fIsPaused = true;
        return _pause();
    }
}
    
void
Animation::resume()
{
    if (isPaused())
    {
        _resume();
        fIsPaused = false;
    }
}

bool
Animation::canSkip()
{
	return (fDelegate != nullptr) ? fDelegate->animationAllowedToSkip(this) : false;

}

AnimationPtr
Animation::skip()
{
	if (!fSkipping)
	{
		fSkipping = true;
		return _skip();
	}

	return nullptr;
}

AnimationPtr
Animation::_skip()
{
	// Default: let the animation finish...
	return nullptr;
}

CCScheduler*
Animation::scheduler() const
{
    return CCDirector::sharedDirector()->getScheduler();
}

void
Animation::schedule(SEL_SCHEDULE selector, float interval, unsigned int repeat, float delay)
{
    CCAssert( selector, "Argument must be non-nil");
    CCAssert( interval >=0, "Argument must be positive");
    
    // void scheduleSelector(SEL_SCHEDULE pfnSelector, CCObject *pTarget, float fInterval, unsigned int repeat, float delay, bool bPaused);
    scheduler()->scheduleSelector(selector, this, interval, repeat, delay, false);
}

void
Animation::scheduleOnce(SEL_SCHEDULE selector, float delay)
{
    this->schedule(selector, 0.0f, 0, delay);
}

void
Animation::unschedule(SEL_SCHEDULE selector)
{
    // explicit nil handling
    if (selector == 0)
        return;
    
    scheduler()->unscheduleSelector(selector, this);
}

NS_CC_END
