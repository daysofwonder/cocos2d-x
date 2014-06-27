//
//  SequenceAnimation.cpp
//
//  Created by Gérald Guyomard on 19/12/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "animations/SequenceAnimation.h"

NS_CC_BEGIN

    SequenceAnimationPtr
    SequenceAnimation::create()
    {
        SequenceAnimationPtr anim;
        anim << new SequenceAnimation;
        return anim;
    }

    SequenceAnimation::SequenceAnimation()
    : fQueue(AnimationQueue::create()), fNbPendingAnimations(0)
    {
        fQueue->setDelegate(this);
    }
    
    SequenceAnimation::~SequenceAnimation()
    {
        fQueue->setDelegate(nullptr);
    }
    
    void
    SequenceAnimation::addAnimation(const AnimationPtr& iAnimation)
    {
        fQueue->queueAnimation(iAnimation, false);
    }
    
    void
    SequenceAnimation::addInstantAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iInstantLambda)
    {
        fQueue->queueInstantAnimation(iInstantLambda, false);
    }
    
    void
    SequenceAnimation::addLastingAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iStartLambda, const std::function<void (const AnimationPtr& iAnimation)>& iStopLambda, bool iTouchesEnabled)
    {
        fQueue->queueLastingAnimation(iStartLambda, iStopLambda, iTouchesEnabled, false);
    }

	bool
	SequenceAnimation::canSkip()
	{
		return fQueue->canSkipCurrentAnimation();
	}

    void
    SequenceAnimation::start()
    {
        assert(fNbPendingAnimations == 0);
        fQueue->startNextAnimation();
    }
    
    void
    SequenceAnimation::stop()
    {
        fQueue->clearPendingAnimations();
    }
    
    bool
    SequenceAnimation::_pause()
    {
        fQueue->pause();
        return true;
    }
    
    void
    SequenceAnimation::_resume()
    {
        fQueue->resume();
        
        assert(fNbPendingAnimations >= 0);
        if (fNbPendingAnimations == 0)
        {
            _didStop(true);
        }
    }

    void
    SequenceAnimation::animationWillStart(const AnimationPtr &iAnim)
    {
        assert(fNbPendingAnimations >= 0);
        ++fNbPendingAnimations;
    }
    
    void
    SequenceAnimation::animationDidFinish(const AnimationPtr& iAnim, bool iFinished)
    {
        if (iFinished)
        {
            assert(fNbPendingAnimations > 0);
            if (--fNbPendingAnimations == 0)
            {
                if (!isPaused())
                {
                    _didStop(true);
                }
            }
        }
        else
        {
            _didStop(false);
        }
    }

	void
	SequenceAnimation::animationWasReplacedForSkip(const AnimationPtr& iOldAnimation, const AnimationPtr& iNewAnimation)
	{
		assert(fNbPendingAnimations > 0);
		--fNbPendingAnimations;
		fFurtherAnimsAreInvalidAfterSkip = false;
	}
	
	AnimationPtr
	SequenceAnimation::_skip()
	{
		fQueue->skipCurrentAnimation();
		return nullptr;
	}
NS_CC_END
