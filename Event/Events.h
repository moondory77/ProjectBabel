#ifndef __EVENTS_H__
#define __EVENTS_H__

#include "cocos2d.h"
#include "Event/EventThread.h"
#include "Util/TextureBatcher.h"

using namespace std;
USING_NS_CC;

/* EventThread�� ��ӹ޾�, ���� �̺�Ʈ�� ��üȭ �ϴ� �� Ŭ������
   Ŭ������ 
   (��� ���(�Ҵ���) + �̺�Ʈ �̸�) */

class MainLobby_InOutMotion : public EventThread
{
	enum Targetname { HERO };
public:
	MainLobby_InOutMotion() {
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/walk_up.plist");
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/walk_down.plist");
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("default_back.plist");
	};
	~MainLobby_InOutMotion() {
		targetList.clear();
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("motions/walk_up.plist");
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("motions/walk_down.plist");
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("default_back.plist");
	};

	void initThread(initializer_list<Sprite*> targets);
};

class StatusLobby_InOutMotion : public EventThread
{
	enum TargetName {
		HERO, HERO_CIRCLE, HERO_WEAPON,
		SWORD, BAT 
	};
public:
	// �ʱ�ȭ ��, �ʿ��� ��ŭ�� Box Ȯ��
	StatusLobby_InOutMotion() {
		targetList.reserve(5);
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/WeaponSwing.plist");
	};
	~StatusLobby_InOutMotion() {
		targetList.clear();
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("motions/WeaponSwing.plist");
	};

	void initThread(initializer_list<Sprite*> targets);
};



class StatusLobby_WeaponChange : public EventThread
{
	enum TargetName { HERO, HERO_CIRCLE, HERO_WEAPON };

public:

	StatusLobby_WeaponChange() {
		targetList.reserve(3);
	};
	~StatusLobby_WeaponChange() {
		targetList.clear();
	};
	void initThread(initializer_list<Sprite*> targets);
};




class OnReady_InOutMotion : public EventThread
{
	enum TargetName { HERO };
public:

	OnReady_InOutMotion() {
		targetList.reserve(1);
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/walk_right.plist");
	}
	~OnReady_InOutMotion() {
		targetList.clear();
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("motions/walk_right.plist");
	}

	void initThread(initializer_list<Sprite*> targets);
};
class OnReady_TrialChange : EventThread
{
	enum TargetName { TRIAL };

public:

	OnReady_TrialChange() {
		targetList.reserve(1);
	}

	~OnReady_TrialChange() {
		targetList.clear();
	}
};




class Result_InOutMotion : public EventThread
{
	enum TargetName { HERO };
public:

	Result_InOutMotion() {
		targetList.reserve(1);
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/walk_left.plist");
	};
	~Result_InOutMotion() {
		targetList.clear();
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("motions/walk_left.plist");
	};

	void initThread(initializer_list<Sprite*> targets);
};
class Result_Grading : public EventThread
{
	enum TargetName { GRADE };

public:

	Result_Grading() {
		targetList.reserve(1);
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Grade.plist");
	};
	~Result_Grading() {
		targetList.clear();
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("Grade.plist");
	};

	void initThread(initializer_list<Sprite*> targets);
};





#endif // !__EVENTS_H__
