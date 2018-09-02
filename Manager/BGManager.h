#ifndef __BACKGROUND_MANAGER__H__
#define __BACKGROUND_MANAGER__H__

#include "cocos2d.h"
#define KARMA_BG_FACTOR 0.014	//1000을 최종 수치라 했을 때, per unit 당 move 시켜야 할 BG 높이

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
	

	//FrameBG 리스트의 노드 -> 6개, curFrame는 밑에서 3번째 노드
	FrameBG* topFrame;
	FrameBG* bottomFrame;
	FrameBG* curFrame;		

	float curKarma = 0;		//플레이 진행률 수치화 (0 ~ 1000)
	bool isBossFlag = false;
public:

	static BGManager* getInstance();

	void loadBG();	
	void moveBG(float input_karma);
	ParallaxNode* getParallax() { return parallax; }
};


#endif // !__BACKGROUND_MANAGER__H__
