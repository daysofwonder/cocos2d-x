//
//  AnimationQueue.cpp
//
//  Created by Gérald Guyomard on 18/10/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "animations/AnimationQueue.h"
#include "animations/LambdaAnimation.h"
#include <cassert>

NS_CC_BEGIN

AnimationQueuePtr
AnimationQueue::create()
{
    AnimationQueuePtr queue;
    queue << new AnimationQueue;
    return queue;
}
    
AnimationQueue::AnimationQueue()
: fDelegate(nullptr), fPauseCounter(0), fIsValid(true)
{}
    
AnimationQueue::~AnimationQueue()
{
        
}

void
AnimationQueue::queueAnimation(const AnimationPtr& iAnimation, bool iStart)
{
    if (iAnimation != nullptr)
    {
        fQueue.push_back(iAnimation);
        
        if (iStart)
        {
            startNextAnimation();            
        }
    }
}
    
void
AnimationQueue::queueInstantAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iStartLambda, bool iStart)
{
    AnimationPtr anim = LambdaAnimation::createInstantAnimation(iStartLambda);
    queueAnimation(anim, iStart);
}
    
void
AnimationQueue::queueLastingAnimation(const std::function<void (const AnimationPtr& iAnimation)>& iStartLambda, const std::function<void (const AnimationPtr& iAnimation)>& iStopLambda, bool iTouchesEnabled, bool iStart)
{
    AnimationPtr anim = LambdaAnimation::createLastingAnimation(iStartLambda, iStopLambda, iTouchesEnabled);
    queueAnimation(anim, iStart);
}

void
AnimationQueue::clearPendingAnimations(bool iInvalidateQueue)
{
    fIsValid = !iInvalidateQueue;
    
    for (auto& anim : fQueue)
    {
        anim->setDelegate(nullptr);
    }
    
    fQueue.clear();

    if (fAnimationInProgress != nullptr)
    {
        AnimationPtr anim = fAnimationInProgress;

        anim->setDelegate(nullptr);
        anim->stop();
        
        if (fDelegate != nullptr)
        {
            fDelegate->animationDidFinish(anim, false);
        }
    }
}
    
void
AnimationQueue::startNextAnimation()
{
    if (isValid() && !isPaused() && !fAnimationInProgress && !fQueue.empty())
    {
        fAnimationInProgress = fQueue[0];
        fQueue.erase(fQueue.begin());
        
        if (fDelegate != nullptr)
        {
            fDelegate->enableUserInteraction(fAnimationInProgress->touchesEnabled());
            fDelegate->animationWillStart(fAnimationInProgress);
        }
        
        fAnimationInProgress->setDelegate(this);
        fAnimationInProgress->start();
        
        if (fDelegate != nullptr)
        {
            fDelegate->animationDidStart(fAnimationInProgress);
        }
    }
}

void
AnimationQueue::animationDidStop(const AnimationPtr& iAnimation, bool iFinished)
{
    assert(iAnimation == fAnimationInProgress);
    
    AnimationPtr anim = fAnimationInProgress;
        
    fAnimationInProgress->retain();
    fAnimationInProgress->autorelease();
    fAnimationInProgress = nullptr;
    
    if (fDelegate != nullptr)
    {
        fDelegate->enableUserInteraction(true);
    }
    
	if (anim->skipping() && anim->furtherAnimationsAreInvalidAfterSkip())
	{
		fQueue.clear();
	}
	else
	{
		startNextAnimation();
	}
    
    if (fDelegate != nullptr)
    {
        // Tell delegate AFTER starting possible next animations
        // to block events
        fDelegate->animationDidFinish(anim, iFinished);
    }
}

bool
AnimationQueue::iterateAnimations(const TIterator& iIterator)
{
    if (fAnimationInProgress != nullptr)
    {
        if (iIterator(fAnimationInProgress))
        {
            return true;
        }
    }
    
    for (const auto& anim : fQueue)
    {
        if (iIterator(anim))
        {
            return true;
        }
    }
    
    return false;
}
    
bool
AnimationQueue::isPaused() const
{
    assert(fPauseCounter >= 0);
    return fPauseCounter > 0;
}
    
void
AnimationQueue::pause()
{
    assert(fPauseCounter >= 0);
    if (fPauseCounter++ == 0)
    {
        // Just paused
        if (fDelegate != nullptr)
        {
            fDelegate->animationQueueWillPause(*this);
        }
        
        if (fAnimationInProgress != nullptr)
        {
            fAnimationInProgress->pause();
        }
    }
}
    
    
void
AnimationQueue::resume()
{
    assert(fPauseCounter > 0);

    if (--fPauseCounter == 0)
    {
        // Just resumed
        if (fAnimationInProgress != nullptr)
        {
            assert(fAnimationInProgress->isPaused());
            fAnimationInProgress->resume();
        }
        else
        {
            startNextAnimation();
        }
        
        if (fDelegate != nullptr)
        {
            fDelegate->animationQueueDidResume(*this);
        }
    }
}

bool
AnimationQueue::canSkipCurrentAnimation() const
{
    return (fAnimationInProgress != nullptr) && fAnimationInProgress->canSkip() && !fAnimationInProgress->skipping();
}
    
bool
AnimationQueue::animationAllowedToSkip(const AnimationPtr& iAnimation)
{
	return (fDelegate != nullptr) ? fDelegate->animationAllowedToSkip(*this, iAnimation) : false;
}

void
AnimationQueue::skipCurrentAnimation()
{
	if (canSkipCurrentAnimation())
	{
		if (fDelegate != nullptr)
		{
			fDelegate->animationQueueWillSkip(*this);
		}

		auto currentAnim = fAnimationInProgress;
		auto replacementAnim = currentAnim->skip();
		if (replacementAnim != nullptr)
		{
			fAnimationInProgress = nullptr;

			currentAnim->setDelegate(nullptr);
			currentAnim->stop();

			replacementAnim->setDelegate(this);

			if (fDelegate != nullptr)
			{
				fDelegate->animationWasReplacedForSkip(currentAnim, replacementAnim);
			}

			fQueue.insert(fQueue.begin(), replacementAnim);

			startNextAnimation();
		}

		if (fDelegate != nullptr)
		{
			fDelegate->animationQueueDidSkip(*this);
		}
	}
}
    
NS_CC_END
