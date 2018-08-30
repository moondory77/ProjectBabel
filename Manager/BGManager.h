#ifndef __BACKGROUND_MANAGER__H__
#define __BACKGROUND_MANAGER__H__

#include "cocos2d.h"
#define KARMA_BG_FACTOR 0.014

USING_NS_CC;


struct FrameBG
{
	int frameIdx = -1;
	Sprite* sprFrame = NULL;	//실제 배경이 맵핑 될 스프라이트
	
	FrameBG* upper = NULL;
	FrameBG* lower = NULL;

	FrameBG() {};
	~FrameBG() {};
};


class BGManager
{
private:
	BGManager() {};
	static BGManager* BGManager_instance;
	void initBG();

	ParallaxNode *parallax;
	Layer* layerBG;	
				
	FrameBG* topFrame;
	FrameBG* bottomFrame;
	FrameBG* curFrame;


	//게임 플레이의 진행정도를 나타내는 바로미터 (0 ~ 1000)
	float curKarma = 0;
	bool isBossFlag = false;
public:

	static BGManager* getInstance();
	void loadBG();
	
	void moveBG(float input_karma);
	ParallaxNode* getParallax() { return parallax; }
};


#endif // !__BACKGROUND_MANAGER__H__
