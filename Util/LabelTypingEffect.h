#ifndef LabelTypingEffect_h
#define LabelTypingEffect_h

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;


class LabelTypingEffect {
public:
	static void typeMessage(Label *txtBox, const std::string &strChatMessage);
	static void typeMessage(Label *txtBox, const std::string &strChatMessage, Color3B txtColor);
	static void typeMessage(Label *txtBox, const std::string &strChatMessage, float interval);
	static void typeMessage(Label *txtBox, const std::string &strChatMessage, float interval, Color3B txtColor);
	static void typeMessage(Label *txtBox, const std::string &strChatMessage, float interval, const std::function<void()> &fncCallback);
	static void typeMessage(Label *txtBox, const std::string &strChatMessage, float interval, Color3B txtColor, const std::function<void()> &fncCallback);
};

#endif /* LabelTypingEffect_h */