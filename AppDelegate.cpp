#include "AppDelegate.h"
#include "Scene/_Scene_Lobby.h"
#include "Scene/_Scene_OnReady.h"
#include "OnGame/_Scene_OnGame.h"

USING_NS_CC;

//** 테스트용 화면 사이즈
//static cocos2d::Size designResolutionSize = cocos2d::Size(480, 320);
//static cocos2d::Size designResolutionSize = cocos2d::Size(1024, 768);
//static cocos2d::Size designResolutionSize = cocos2d::Size(360, 720);


//static cocos2d::Size smallResolutionSize = cocos2d::Size(1440, 2560);//480,320
//static cocos2d::Size mediumResolutionSize = cocos2d::Size(1440, 2560);//1024, 768
//static cocos2d::Size largeResolutionSize = cocos2d::Size(1440, 2560);//2048, 1536



//** 타겟기종(내 핸드폰 v20 기준) 화면 사이즈
///갤럭시 s8 2960
//static cocos2d::Size designResolutionSize = cocos2d::Size(480, 320);
//static cocos2d::Size designResolutionSize = cocos2d::Size(1680, 2440);

static cocos2d::Size designResolutionSize = cocos2d::Size(1440, 2560);
//static cocos2d::Size designResolutionSize = cocos2d::Size(360, 720);
//static cocos2d::Size designResolutionSize = cocos2d::Size(840, 960);


static cocos2d::Size smallResolutionSize = cocos2d::Size(1440, 2560);//480,320
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1440, 2560);//1024, 768
static cocos2d::Size largeResolutionSize = cocos2d::Size(1440, 2560);//2048, 1536




/*
static cocos2d::Size designResolutionSize = cocos2d::Size(360, 740);
//static cocos2d::Size designResolutionSize = cocos2d::Size(360, 720);
static cocos2d::Size smallResolutionSize = cocos2d::Size(360, 740);//480,320
static cocos2d::Size mediumResolutionSize = cocos2d::Size(360, 740);//1024, 768
static cocos2d::Size largeResolutionSize = cocos2d::Size(360, 740);//2048, 1536
*/


AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate()
{
}

//if you wyant a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
	//set OpenGL context attributions,now can only set six attributions:
	//red,green,blue,alpha,depth,stencil
	GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };
	//GLContextAttrs glContextAttrs = { 0, 0, 0, 0, 0, 8 };


	GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
	return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() 
{

	// initialize director
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();

	if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		glview = GLViewImpl::createWithRect("Real", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
		glview = GLViewImpl::create("Real");
#endif
		director->setOpenGLView(glview);
	}

	// turn on display FPS
	director->setDisplayStats(true);
	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0 /60.f);
	// Set the design resolution
	glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, 
		ResolutionPolicy::SHOW_ALL);
	
	Size frameSize = glview->getFrameSize();
	director->setContentScaleFactor(frameSize.width);
	
	// if the frame's height is larger than the height of medium size.
	if (frameSize.height > mediumResolutionSize.height)
	{
		director->setContentScaleFactor(MIN(largeResolutionSize.height / designResolutionSize.height, largeResolutionSize.width / designResolutionSize.width));
	}
	// if the frame's height is larger than the height of small size.
	else if (frameSize.height > smallResolutionSize.height)
	{
		director->setContentScaleFactor(MIN(mediumResolutionSize.height / designResolutionSize.height, mediumResolutionSize.width / designResolutionSize.width));
	}
	// if the frame's height is smaller than the height of medium size.
	else
	{
		director->setContentScaleFactor(MIN(smallResolutionSize.height / designResolutionSize.height, smallResolutionSize.width / designResolutionSize.width));
	}
	
	register_all_packages();

	// create a scene. it's an autorelease object
	auto scene = Lobby::createScene();
	//auto scene = OnGame::createScene();

	// run
	director->runWithScene(scene);

	return true;
}




// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
	Director::getInstance()->stopAnimation();

	// if you use SimpleAudioEngine, it must be pause
	// SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
	Director::getInstance()->startAnimation();

	// if you use SimpleAudioEngine, it must resume here
	// SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

