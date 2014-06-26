#include "CCNodeLoaderLibrary.h"

#include "CCLayerLoader.h"
#include "CCLayerColorLoader.h"
#include "CCLayerGradientLoader.h"
#include "CCLabelBMFontLoader.h"
#include "CCLabelTTFLoader.h"
#include "CCSpriteLoader.h"
#include "CCMultiPassSpriteLoader.h"
#include "CCScale9SpriteLoader.h"
#include "CCBFileLoader.h"
#include "CCMenuLoader.h"
#include "CCMenuItemLoader.h"
#include "CCMenuItemImageLoader.h"
#include "CCControlButtonLoader.h"
#include "CCParticleSystemQuadLoader.h"
#include "CCScrollViewLoader.h"
#include "CCEnhancedLabelTTFLoader.h"
#include "GUI/CCControlExtension/CCControlSlider.h"
#include "CCTableViewLoader.h"
#include "GUI/CCScrollView/CCEnhancedTableView.h"
#include "GUI/CCControlExtension/CCEnhancedControlButton.h"
#include "GUI/CCControlExtension/CCGenericButton.h"
#include "CCSmartLayerLoader.h"
#include "sprite_nodes/CCLoadableSpriteBatchNode.h"
#include "layers_scenes_transitions_nodes/CCAutoScrollableEditBoxContainer.h"
#include "layers_scenes_transitions_nodes/CCLayout.h"
#include "layers_scenes_transitions_nodes/CCDoubleSidedLayer.h"

#include "GUI/NativeNodes/CCWebView.h"
#include "GUI/NativeNodes/CCYoutubeView.h"
#include "GUI/NativeNodes/CCMultilineTextBox.h"


NS_CC_EXT_BEGIN

CCNodeLoaderLibrary::CCNodeLoaderLibrary() {

}

CCNodeLoaderLibrary::~CCNodeLoaderLibrary() {
    this->purge(true);
}

static std::vector<CCNodeLoaderLibrary::TRegistrationCallback> s_RegistrationCallbacks;


void CCNodeLoaderLibrary::registerDefaultCCNodeLoaders() {
    this->registerCCNodeLoader("CCNode", CCNodeLoader::loader());
    this->registerCCNodeLoader("CCLayer", CCLayerLoader::loader());
    this->registerCCNodeLoader("CCLayerColor", CCLayerColorLoader::loader());
    this->registerCCNodeLoader("CCLayerGradient", CCLayerGradientLoader::loader());
    this->registerCCNodeLoader("CCSprite", CCSpriteLoader::loader());
    this->registerCCNodeLoader("CCLabelBMFont", CCLabelBMFontLoader::loader());
    this->registerCCNodeLoader("CCLabelTTF", CCLabelTTFLoader::loader());
    this->registerCCNodeLoader("CCScale9Sprite", CCScale9SpriteLoader::loader());
    this->registerCCNodeLoader("CCScrollView", CCScrollViewLoader::loader());
    this->registerCCNodeLoader("CCBFile", CCBFileLoader::loader());
    this->registerCCNodeLoader("CCMenu", CCMenuLoader::loader());
    this->registerCCNodeLoader("CCMenuItemImage", CCMenuItemImageLoader::loader());
    this->registerCCNodeLoader("CCControlButton", CCControlButtonLoader::loader());
    this->registerCCNodeLoader("CCParticleSystemQuad", CCParticleSystemQuadLoader::loader());
    this->registerCCNodeLoader("CCControlSlider", CCControlSliderLoader::loader());
    
    this->registerCCNodeLoader("CCMultiPassSprite", CCMultiPassSpriteLoader::loader());
    this->registerCCNodeLoader("CCTableView", CCTableViewLoader::loader());
    this->registerCCNodeLoader("CCEnhancedTableView", CCEnhancedTableViewLoader::loader());

    this->registerCCNodeLoader("CCEnhancedLabelTTF", CCEnhancedLabelTTFLoader::loader());
    this->registerCCNodeLoader("CCEnhancedControlButton", CCEnhancedControlButtonLoader::loader());
    this->registerCCNodeLoader("CCGenericButton", CCGenericButtonLoader::loader());
    
    this->registerCCNodeLoader("CCSmartLayer", CCSmartLayerLoader::loader());
    this->registerCCNodeLoader("CCLoadableSpriteBatchNode", CCLoadableSpriteBatchNodeLoader::loader());

    this->registerCCNodeLoader("CCAutoScrollableEditBoxContainer", CCAutoScrollableEditBoxContainerLoader::loader());

    this->registerCCNodeLoader("CCWebView", CCWebViewLoader::loader());
    this->registerCCNodeLoader("CCYoutubeView", CCYoutubeViewLoader::loader());
    this->registerCCNodeLoader("CCMultilineTextBox", CCMultilineTextBoxLoader::loader());
    this->registerCCNodeLoader("CCLayout", CCLayoutLoader::loader());
    this->registerCCNodeLoader("CCDoubleSidedLayer", CCDoubleSidedLayerLoader::loader());

    for (const auto& cb : s_RegistrationCallbacks)
    {
        cb(*this);
    }
}

void
CCNodeLoaderLibrary::addRegistrationCallback(const TRegistrationCallback& iCallback)
{
    s_RegistrationCallbacks.push_back(iCallback);
}

void CCNodeLoaderLibrary::registerCCNodeLoader(const char * pClassName, CCNodeLoader * pCCNodeLoader) {
    pCCNodeLoader->retain();
    this->mCCNodeLoaders.insert(CCNodeLoaderMapEntry(pClassName, pCCNodeLoader));
}

void CCNodeLoaderLibrary::unregisterCCNodeLoader(const char * pClassName) {
    CCNodeLoaderMap::iterator ccNodeLoadersIterator = this->mCCNodeLoaders.find(pClassName);
    if (ccNodeLoadersIterator != this->mCCNodeLoaders.end())
    {
        ccNodeLoadersIterator->second->release();
        mCCNodeLoaders.erase(ccNodeLoadersIterator);
    }
    else
    {
        CCLOG("The loader (%s) doesn't exist", pClassName);
    }
}

CCNodeLoader * CCNodeLoaderLibrary::getCCNodeLoader(const char* pClassName) {
    CCNodeLoaderMap::iterator ccNodeLoadersIterator = this->mCCNodeLoaders.find(pClassName);
    assert(ccNodeLoadersIterator != this->mCCNodeLoaders.end());
    return ccNodeLoadersIterator->second;
}

void CCNodeLoaderLibrary::purge(bool pReleaseCCNodeLoaders) {
    if(pReleaseCCNodeLoaders) {
        for(CCNodeLoaderMap::iterator it = this->mCCNodeLoaders.begin(); it != this->mCCNodeLoaders.end(); it++) {
            it->second->release();
        }
    }
    this->mCCNodeLoaders.clear();
}



static CCNodeLoaderLibrary * sSharedCCNodeLoaderLibrary = NULL;

CCNodeLoaderLibrary * CCNodeLoaderLibrary::sharedCCNodeLoaderLibrary() {
    if(sSharedCCNodeLoaderLibrary == NULL) {
        sSharedCCNodeLoaderLibrary = new CCNodeLoaderLibrary();

        sSharedCCNodeLoaderLibrary->registerDefaultCCNodeLoaders();
    }
    return sSharedCCNodeLoaderLibrary;
}

void CCNodeLoaderLibrary::purgeSharedCCNodeLoaderLibrary() {
    CC_SAFE_DELETE(sSharedCCNodeLoaderLibrary);
}

CCNodeLoaderLibrary * CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary() {
    CCNodeLoaderLibrary * ccNodeLoaderLibrary = CCNodeLoaderLibrary::library();
    
    ccNodeLoaderLibrary->registerDefaultCCNodeLoaders();

    return ccNodeLoaderLibrary;
}

NS_CC_EXT_END
