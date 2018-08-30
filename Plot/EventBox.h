#ifndef __EventBox_H__
#define __EventBox_H__

/** �̺�Ʈ�� �����ϴ� �⺻�� �Ǵ� ����ü Ŭ����
 ���� �ϳ��� ������ chunk�� �Ͽ� �����ȴ�.
 ���� �̺�Ʈ �ڽ��� ��Ī
 **/
#define MAXSIZE 6
#include "cocos2d.h"
//#include "Util/LabelTypingEffect.h"
//#include "Util/MultiByteConverter.h"
#include <string>
#include <array>

using std::string;
USING_NS_CC;

class Box : public cocos2d::CCNode
{
	friend class EventList;				//** list -> box Ŭ������ ���� ���� 
	//int type;						//** Ÿ�� ���� : 1-action	  2-msg		3-effect 
	Box *next;						//** ���� �̺�Ʈ box ������
	Box *prev;

public:
	bool is_touchable = true;		//** �̺�Ʈ�ڽ� ���� ��, ��ġ ���� flag
	bool is_continuing = false;		//** �ش� �̺�Ʈ�ڽ��� ���� ���� flag (action ������ ���)

	Box(Box *my_prev, Box *my_next)
	{
		//this->type = my_type;
		this->prev = my_prev;
		this->next = my_next;
	}

	virtual void setEvent() {}
	virtual void startEvent() {}
	virtual void stopEvent() {}
};



//** Type 1) action Box
/*
setEvent :  �� Ÿ�ٿ� ���� �׼� ���� / EventScenario ���̾� �� Ÿ�� - Box�� Ÿ�� - �׼� ����
start Event : �� Ÿ�ٿ� ���� �� �׼��� ����
stop Event : �� Ÿ�ٿ� ���� �� �׼��� ����
*/
class ActionBox : public Box
{
protected:
	SpriteFrameCache *spr_fr_cache;
	Animation *animation;
	Animate *animate;

	Sprite **targets;	//scenario layer���� ������ target array
	Action **actions;	//target�� ������ �׼� �迭(�ε��� ����)
	int size;			//target�� ���� 

public:
	float time_lapse;	//** �ش� action ���൵�� �ҿ�Ǵ� �ð� lapse

	ActionBox(Box *my_prev, Box *my_next, int target_size) : Box(my_prev, my_next)
	{
		targets = new Sprite*[target_size];
		actions = new Action*[target_size];
		this->size = target_size;
	}

	//�ش� idx�� ���� target-action ����
	void setTarget(Sprite *my_target, Action *my_action, int idx)
	{
		this->targets[idx] = my_target;
		this->actions[idx] = my_action;
	}

	virtual void setEvent() {}
	virtual void startEvent()
	{
		//** �׼��� ������ Ÿ�ٵ鿡 ����, ������ �׼��� �����Ѵ�
		for (int i = 0; i < this->size; i++)
		{
			this->targets[i]->stopAction(this->actions[i]);
		}

		//** �׼��� ����
		for (int i = 0; i < this->size; i++)
		{
			this->targets[i]->runAction(this->actions[i]);
		}
		CCLOG("now Events is starting!!");
		//CCLOG("Target Size is %d, Actions Size is %d", this->size);
	}
	virtual void stopEvent()
	{
		for (int i = 0; i < this->size; i++)
		{
			this->targets[i]->stopAction(this->actions[i]);
		}
	}
};

//** Type 2) message Box
/*
setEvent :  �ʿ��� string ����(�� ����) / �ʿ��� �� ���� /
start Event : �� Ÿ�ٿ� ���� �� �׼��� ����
stop Event : �� Ÿ�ٿ� ���� �� �׼��� ����
*/
class MsgBox : public Box
{
protected:

	string msgResult[MAXSIZE];				//** �ٰ��� parsing�� �޽��� string
	Label *labelResult[MAXSIZE];			//** �ٰ��� parsing�� �޽��� label

	Layer *target;		//** �޽����� ��µ� target(�ϴ� layer. ��ǳ�� sprite�� ����) 
	Point *location;	//**  target�� ��ġ �� ��ġ
	int size;			//�ٰ��� parsing�� ����

public:
	MsgBox(Box* my_prev, Box* my_next) : Box(my_prev, my_next)
	{
	}

	//** string�� ��ū ������ msgResult�� ���� ����
	void setBoxMsg(string *my_msg)
	{
		int cutAt;				//** ���ڿ� �ڸ��� ��ġ
		size = 0;			//** ���ڿ��� �ε���

		string tmp = *my_msg;
		//msgResult = new string[MAXINDEX];

		//** '_'�������� ��Ʈ���� �κ� �ڸ� �� ������ �ݺ�
		while ((cutAt = tmp.find_first_of('_')) != tmp.npos)
		{
			//** ��ū ��Ʈ���� �����ϸ�,
			if (cutAt > 0)
			{
				msgResult[size++] = tmp.substr(0, cutAt);
			}
			//** cut ������ string���� tmp�� ����
			tmp = tmp.substr(cutAt + 1);
		}

		//** ������ '_' ���� ��Ʈ�� ��ū ó��
		if (tmp.length() > 0)
		{
			//** �������� ��� �迭�� �߰��Ѵ�.
			msgResult[size++] = tmp.substr(0, cutAt);
		}
	}
	void setTarget(Layer* my_target) { this->target = my_target; }
	void setPrintLocation(Point *my_location) { this->location = my_location; }


	virtual void setEvent() {}
	virtual void startEvent()
	{
		for (int i = 0; i < size; i++)
		{
			//LabelTypingEffect::typeMessage(labelResult[i], _AtoU8(&msgResult[i][0]));
			//CCLOG("labelEffect complete!");
		}
	}
	virtual void stopEvent() {}
};







//*************************** #Scene - Lobby	***************************************// 

//�κ�ȭ�鿡 �� ������/�ΰ� ���̵� ��
class Menu_Lobby_Intro_0 : public ActionBox{
public :
	Menu_Lobby_Intro_0(Box* my_prev, Box* my_next) : ActionBox(my_prev, my_next, 5) {}

	virtual void setEvent();
};
//���ΰ� �ɾ� �ö��
class Menu_Lobby_Intro_1 : public ActionBox
{
public:
	Menu_Lobby_Intro_1(Box* my_prev, Box* my_next) : ActionBox(my_prev, my_next, 1) {}

	virtual void setEvent();
};

//�κ�ȭ�� icon & logo ���̵� �ƿ�
class Menu_Lobby_Outro_0 : public ActionBox
{
public:
	Menu_Lobby_Outro_0(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 5) {}

	virtual void setEvent();
};
//���ΰ� �ɾ� ������
class Menu_Lobby_Outro_1 : public ActionBox {

public:
	Menu_Lobby_Outro_1(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 1) {}

	virtual void setEvent();
};

/*
class Prac_Box_1 : public ActionBox {

public:
	Prac_Box_1(Box *my_next) : ActionBox(my_next) {}

	virtual void setEvent();	//�ִϸ��̼� �غ�
	virtual void startEvent();	//�ִϸ��̼� ����
	virtual void stopEvent();
};
class Prac_Box_2 : public ActionBox {
public:
	Prac_Box_2(Box *my_next) : ActionBox(my_next) {}

	virtual void setEvent();		//�ִϸ��̼� �غ�
	virtual void startEvent();	//�ִϸ��̼� ����
	virtual void stopEvent();
};
class Prac_Box_3 : public MsgBox {
public:
	Prac_Box_3(Box *my_next) :MsgBox(my_next) {}

	virtual void setEvent();		//�޽��� �غ�
	virtual void startEvent();	//�޽��� ����
	virtual void stopEvent();
};
*/


//*************************** #Scene - CharacterInfo	**************************************// 

// ���ΰ� Į �ֵθ� - intro(2.22�� �Ҹ�)
class Menu_Character_Intro_0 : public ActionBox {

public:
	Menu_Character_Intro_0(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 1) {}

	virtual void setEvent();
};
//����, �ڷΰ��� �� fade in 
class Menu_Character_Intro_1 : public ActionBox {

public:
	Menu_Character_Intro_1(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 2) {}

	virtual void setEvent();
};

//Į/����� �� fade in
class Menu_Character_WTab_0 : public ActionBox {

public:
	Menu_Character_WTab_0(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 4) {}

	virtual void setEvent();
};
//Į/����� �� fade out
class Menu_Character_WTab_1 : public ActionBox {

public:
	Menu_Character_WTab_1(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 4) {}

	virtual void setEvent();
};

//���ΰ� Į ������� - outro
class Menu_Character_Outro_0 : public ActionBox {

public:
	Menu_Character_Outro_0(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 1) {}

	virtual void setEvent();
};
//����, �ڷΰ��� �� fade out
class Menu_Character_Outro_1 : public ActionBox {

public:
	Menu_Character_Outro_1(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 2) {}

	virtual void setEvent();
};



//*************************** #Scene - OnReady	***************************************// 

//�� ���� ��, ĳ���� ���ʿ��� õõ�� �ɾ� ���� (2.16�� �Ҹ�)
class Menu_OnReady_Intro_0 : public ActionBox
{
public:
	Menu_OnReady_Intro_0(Box* my_prev, Box *my_next) :ActionBox(my_prev, my_next, 1) {}
	virtual void setEvent();
};		


//��� �÷ÿ� ����, �׼��� ��������ִ� �̺�Ʈ 
class Menu_OnReady_Meteor_0 : public ActionBox
{
public:
	Menu_OnReady_Meteor_0(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 1) {}
	virtual void setEvent();
};
//��� �÷ÿ� ����, Message ��� (�÷� ���� ����)
class Menu_OnReady_Meteor_1 : public MsgBox
{
public:
	Menu_OnReady_Meteor_1(Box* my_prev, Box* my_next) : MsgBox(my_prev, my_next) {}
	virtual void setEvent();		
};


//��Ϲ����� �÷ÿ� ����, Action
class Menu_OnReady_Gear_0 : public ActionBox
{
public:
	Menu_OnReady_Gear_0(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 1) {}
	virtual void setEvent();
};
//��Ϲ����� �÷ÿ� ����, Message 
class Menu_OnReady_Gear_1 : public MsgBox
{
public:
	Menu_OnReady_Gear_1(Box* my_prev, Box *my_next) : MsgBox(my_prev, my_next) {}
	virtual void setEvent();		//�޽��� �غ�
};


//��ݹ�ư�� ������ ��, ĳ���� ���������� ������ �ɾ���� ��� (2.88�� �Ҹ�)
class Menu_OnReady_Outro_0 : public ActionBox
{
public:
	Menu_OnReady_Outro_0(Box* my_prev, Box *my_next) :ActionBox(my_prev, my_next, 1) {}
	virtual void setEvent();
};



//*************************** #Scene - Result	***************************************// 

// �� ���� ��, ĳ���� ��� �Ʒ� �����ʿ��� õõ�� �ɾ��
class Menu_Result_Intro_0 : public ActionBox
{
public:
	Menu_Result_Intro_0(Box* my_prev, Box* my_next) :ActionBox(my_prev, my_next, 1) {}
	virtual void setEvent();
};

//** Result ������, �ѹ� ��ġ ���� ��, ���� �÷��� ����� ǥ��
class Menu_Result_Grading_0 : public ActionBox
{
public:
	Menu_Result_Grading_0(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 2) {}
	virtual void setEvent();
};

//���� ���� �ɾ��
class Menu_Result_Outro_0 : public ActionBox
{
public:
	Menu_Result_Outro_0(Box* my_prev, Box *my_next) :ActionBox(my_prev, my_next, 1) {}
	virtual void setEvent();
};



#endif 

