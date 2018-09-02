#ifndef __BACKGROUND_MANAGER__H__
#define __BACKGROUND_MANAGER__H__

#include "cocos2d.h"
#define KARMA_BG_FACTOR 0.014	//1000�� ���� ��ġ�� ���� ��, per unit �� move ���Ѿ� �� BG ����

USING_NS_CC;


struct FrameBG
{
	int frameIdx = -1;
	Sprite* sprFrame = NULL;	//���� ����� ���� �� ��������Ʈ
	
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
	

	//FrameBG ����Ʈ�� ��� -> 6��, curFrame�� �ؿ��� 3��° ���
	FrameBG* topFrame;
	FrameBG* bottomFrame;
	FrameBG* curFrame;		

	float curKarma = 0;		//�÷��� ����� ��ġȭ (0 ~ 1000)
	bool isBossFlag = false;
public:

	static BGManager* getInstance();

	void loadBG();	
	void moveBG(float input_karma);
	ParallaxNode* getParallax() { return parallax; }
};


#endif // !__BACKGROUND_MANAGER__H__
