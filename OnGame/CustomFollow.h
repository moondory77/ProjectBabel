#ifndef __CUSTOM_FOLLOW_H__
#define __CUSTOM_FOLLOW_H__
#include "cocos2d.h"
USING_NS_CC;

enum customFollowType {
	kCustomFollowNone,
	kCustomFollowXOnly,
	kCustomFollowYOnly
};

class CustomFollow : public Follow {
private:
	customFollowType _type;
	Rect _marginRect;

	CustomFollow();
	~CustomFollow();

public:

	static CustomFollow* create(Node *followedNode);
	static CustomFollow* create(Node *followedNode, customFollowType type);
	static CustomFollow* create(Node *followedNode, Rect marginRect);

	virtual void step(float DT) override;
};
#endif
