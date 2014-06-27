//
//  SpawnAnimation.cpp
//
//  Created by Gérald Guyomard on 19/11/12.
// Copyright (c) 2013 Days of Wonder Inc.
//
//

#include "animations/SpawnAnimation.h"

#include <cassert>

#include <algorithm>
#undef min
#undef max

NS_CC_BEGIN
    
SpawnAnimationPtr
SpawnAnimation::create(const std::vector<AnimationPtr>& iAnimations)
{
    SpawnAnimationPtr anim;
    anim << new SpawnAnimation(iAnimations);
    return anim;
}

SpawnAnimationPtr
SpawnAnimation::create()
{
    SpawnAnimationPtr anim;
    anim << new SpawnAnimation;
    return anim;
}
    
SpawnAnimation::SpawnAnimation()
: fPrepareStart(nullptr)
{}
    
SpawnAnimation::SpawnAnimation(const std::vector<AnimationPtr>& iAnimations)
: fAnimations(iAnimations), fPrepareStart(nullptr)
{
}
    
SpawnAnimation::~SpawnAnimation()
{
    
}
    
void
SpawnAnimation::addAnimation(const AnimationPtr& iAnimation)
{
	if (iAnimation != nullptr)
	{
		fAnimations.push_back(iAnimation);
	}
}
    
void
SpawnAnimation::setPrepareStart(const TPrepareStartLambda& iLambda)
{
    fPrepareStart = iLambda;
}
    
void
SpawnAnimation::start()
{
    if (fPrepareStart != nullptr)
    {
        fPrepareStart(this);
    }
    
    for (auto& anim : fAnimations)
    {
        anim->setDelegate(this);
    }
    
    if (!fAnimations.empty())
    {
        std::vector<AnimationPtr> animations = fAnimations; // copy to iterate safely
        for (auto& anim : animations)
        {
            anim->start();
        }
    }
    else
    {
        finish();
    }
}
    
void
SpawnAnimation::stop()
{
    std::vector<AnimationPtr> animations = fAnimations; // copy to iterate safely
    for (auto& anim : animations)
    {
        anim->stop();
    }
    fAnimations.clear();
}

bool
SpawnAnimation::_pause()
{
    for (auto& anim : fAnimations)
    {
        anim->pause();
    }
    
    return true;
}
    
void
SpawnAnimation::_resume()
{
    for (auto& anim : fAnimations)
    {
        anim->resume();
    }    
}

void
SpawnAnimation::animationDidStop(const AnimationPtr& iAnimation, bool iFinished)
{    
    std::vector<AnimationPtr>::iterator it = std::find(fAnimations.begin(), fAnimations.end(), iAnimation);
    assert(it != fAnimations.end());
    
    iAnimation->setDelegate(nullptr);
    
    if (it != fAnimations.end())
    {
        fAnimations.erase(it);
    }
    
    if (fAnimations.empty())
    {
        finish();
    }
}

NS_CC_END
