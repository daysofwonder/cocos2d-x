#ifndef __CCB_CCSEQUENCE_H__
#define __CCB_CCSEQUENCE_H__

#include <string>
#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "CCBSequenceProperty.h"

NS_CC_BEGIN

class CCNode;

NS_CC_END

NS_CC_EXT_BEGIN

class CCBSequence : public CCObject
{
private:
    float mDuration;
    std::string mName;
    int mSequenceId;
    int mChainedSequenceId;
    CCBSequenceProperty* mCallbackChannel;
    CCBSequenceProperty* mSoundChannel;

public:
    CCBSequence();
    ~CCBSequence();
    float getDuration();
    void setDuration(float fDuration);
    
    CCBSequenceProperty* getCallbackChannel();
    void setCallbackChannel(CCBSequenceProperty* callbackChannel);

    CCBSequenceProperty* getSoundChannel();
    void setSoundChannel(CCBSequenceProperty* soundChannel);
    
    const char* getName();
    void setName(const char *pName);
    
    int getSequenceId();
    void setSequenceId(int nSequenceId);
    
    int getChainedSequenceId();
    void setChainedSequenceId(int nChainedSequenceId);
    
    // CCB Animations
    static CCBSequence* animSequence(CCNode* iNode, const std::string& iAnimName);
};


NS_CC_EXT_END


#endif // __CCB_CCSEQUENCE_H__
