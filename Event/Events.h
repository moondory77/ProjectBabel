#ifndef __EVENTS_H__
#define __EVENTS_H__

#include "cocos2d.h"
#include "Event/EventThread.h"
#include "Util/TextureBatcher.h"

using namespace std;
USING_NS_CC;

//EventThread�� ��ӹ޾�, ���� �̺�Ʈ�� ��üȭ �ϴ� �� Ŭ������
//Ŭ������ (��� ���(�Ҵ���) + �̺�Ʈ �̸�)


class MainLobby_HeroInOut : public EventThread
{
	enum Targetname {
		HERO,
	};

public:
	MainLobby_HeroInOut() {
		targetList.reserve(2);
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/for_events/walk_up/walk_up.plist");
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/for_events/walk_down/walk_down.plist");
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("default_back.plist");
	};
	~MainLobby_HeroInOut() {
		targetList.clear();
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("motions/for_events/walk_up/walk_up.plist");
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("motions/for_events/walk_down/walk_down.plist");
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("default_back.plist");
	};

	void initThread(initializer_list<Sprite*> targets);
};


class StatusLobby_HeroInOut : public EventThread
{
	enum TargetName {
		HERO,
		SWORD,
		BAT
	};

public:

	// �ʱ�ȭ ��, �ʿ��� ��ŭ�� Box Ȯ��
	StatusLobby_HeroInOut(){
		targetList.reserve(3);
		SpriteFrameCache::getInstance()
			->addSpriteFramesWithFile("motions/for_events/menu_cha/menu_character.plist");
	};

	~StatusLobby_HeroInOut() {
		targetList.clear();
		SpriteFrameCache::getInstance()
			->removeSpriteFramesFromFile("motions/for_events/menu_cha/menu_character.plist");
	};

	void initThread(initializer_list<Sprite*> targets);
};








#endif // !__EVENTS_H__
