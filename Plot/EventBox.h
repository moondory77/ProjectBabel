#ifndef __EventBox_H__
#define __EventBox_H__

/** 이벤트를 구성하는 기본이 되는 구조체 클래스
 각각 하나의 동작을 chunk로 하여 구성된다.
 이하 이벤트 박스라 명칭
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
	friend class EventList;				//** list -> box 클래스에 접근 가능 
	//int type;						//** 타입 구분 : 1-action	  2-msg		3-effect 
	Box *next;						//** 다음 이벤트 box 포인터
	Box *prev;

public:
	bool is_touchable = true;		//** 이벤트박스 진행 중, 터치 가능 flag
	bool is_continuing = false;		//** 해당 이벤트박스의 지속 여부 flag (action 프레임 경우)

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
setEvent :  각 타겟에 대한 액션 생성 / EventScenario 레이어 위 타겟 - Box의 타겟 - 액션 연결
start Event : 각 타겟에 대해 각 액션을 실행
stop Event : 각 타겟에 대해 각 액션을 중지
*/
class ActionBox : public Box
{
protected:
	SpriteFrameCache *spr_fr_cache;
	Animation *animation;
	Animate *animate;

	Sprite **targets;	//scenario layer에서 움직일 target array
	Action **actions;	//target들 각각의 액션 배열(인덱스 동일)
	int size;			//target의 갯수 

public:
	float time_lapse;	//** 해당 action 수행도중 소요되는 시간 lapse

	ActionBox(Box *my_prev, Box *my_next, int target_size) : Box(my_prev, my_next)
	{
		targets = new Sprite*[target_size];
		actions = new Action*[target_size];
		this->size = target_size;
	}

	//해당 idx에 대한 target-action 장착
	void setTarget(Sprite *my_target, Action *my_action, int idx)
	{
		this->targets[idx] = my_target;
		this->actions[idx] = my_action;
	}

	virtual void setEvent() {}
	virtual void startEvent()
	{
		//** 액션을 진행할 타겟들에 대해, 이전의 액션은 중지한다
		for (int i = 0; i < this->size; i++)
		{
			this->targets[i]->stopAction(this->actions[i]);
		}

		//** 액션을 실행
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
setEvent :  필요한 string 생성(줄 단위) / 필요한 라벨 생성 /
start Event : 각 타겟에 대해 각 액션을 실행
stop Event : 각 타겟에 대해 각 액션을 중지
*/
class MsgBox : public Box
{
protected:

	string msgResult[MAXSIZE];				//** 줄간격 parsing된 메시지 string
	Label *labelResult[MAXSIZE];			//** 줄간격 parsing된 메시지 label

	Layer *target;		//** 메시지가 출력될 target(일단 layer. 말풍선 sprite라 생각) 
	Point *location;	//**  target이 배치 될 위치
	int size;			//줄간격 parsing의 갯수

public:
	MsgBox(Box* my_prev, Box* my_next) : Box(my_prev, my_next)
	{
	}

	//** string을 토큰 분할해 msgResult에 순차 저장
	void setBoxMsg(string *my_msg)
	{
		int cutAt;				//** 문자열 자르는 위치
		size = 0;			//** 문자열의 인덱스

		string tmp = *my_msg;
		//msgResult = new string[MAXINDEX];

		//** '_'기준으로 스트링을 부분 자를 수 있으면 반복
		while ((cutAt = tmp.find_first_of('_')) != tmp.npos)
		{
			//** 토큰 스트링이 존재하면,
			if (cutAt > 0)
			{
				msgResult[size++] = tmp.substr(0, cutAt);
			}
			//** cut 이후의 string으로 tmp를 갱신
			tmp = tmp.substr(cutAt + 1);
		}

		//** 마지막 '_' 후의 스트링 토큰 처리
		if (tmp.length() > 0)
		{
			//** 나머지를 결과 배열에 추가한다.
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

//로비화면에 각 아이콘/로고 페이드 인
class Menu_Lobby_Intro_0 : public ActionBox{
public :
	Menu_Lobby_Intro_0(Box* my_prev, Box* my_next) : ActionBox(my_prev, my_next, 5) {}

	virtual void setEvent();
};
//주인공 걸어 올라옴
class Menu_Lobby_Intro_1 : public ActionBox
{
public:
	Menu_Lobby_Intro_1(Box* my_prev, Box* my_next) : ActionBox(my_prev, my_next, 1) {}

	virtual void setEvent();
};

//로비화면 icon & logo 페이드 아웃
class Menu_Lobby_Outro_0 : public ActionBox
{
public:
	Menu_Lobby_Outro_0(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 5) {}

	virtual void setEvent();
};
//주인공 걸어 내려감
class Menu_Lobby_Outro_1 : public ActionBox {

public:
	Menu_Lobby_Outro_1(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 1) {}

	virtual void setEvent();
};

/*
class Prac_Box_1 : public ActionBox {

public:
	Prac_Box_1(Box *my_next) : ActionBox(my_next) {}

	virtual void setEvent();	//애니메이션 준비
	virtual void startEvent();	//애니메이션 실행
	virtual void stopEvent();
};
class Prac_Box_2 : public ActionBox {
public:
	Prac_Box_2(Box *my_next) : ActionBox(my_next) {}

	virtual void setEvent();		//애니메이션 준비
	virtual void startEvent();	//애니메이션 실행
	virtual void stopEvent();
};
class Prac_Box_3 : public MsgBox {
public:
	Prac_Box_3(Box *my_next) :MsgBox(my_next) {}

	virtual void setEvent();		//메시지 준비
	virtual void startEvent();	//메시지 실행
	virtual void stopEvent();
};
*/


//*************************** #Scene - CharacterInfo	**************************************// 

// 주인공 칼 휘두름 - intro(2.22초 소모)
class Menu_Character_Intro_0 : public ActionBox {

public:
	Menu_Character_Intro_0(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 1) {}

	virtual void setEvent();
};
//무기, 뒤로가기 탭 fade in 
class Menu_Character_Intro_1 : public ActionBox {

public:
	Menu_Character_Intro_1(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 2) {}

	virtual void setEvent();
};

//칼/방망이 탭 fade in
class Menu_Character_WTab_0 : public ActionBox {

public:
	Menu_Character_WTab_0(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 4) {}

	virtual void setEvent();
};
//칼/방망이 탭 fade out
class Menu_Character_WTab_1 : public ActionBox {

public:
	Menu_Character_WTab_1(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 4) {}

	virtual void setEvent();
};

//주인공 칼 집어넣음 - outro
class Menu_Character_Outro_0 : public ActionBox {

public:
	Menu_Character_Outro_0(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 1) {}

	virtual void setEvent();
};
//무기, 뒤로가기 탭 fade out
class Menu_Character_Outro_1 : public ActionBox {

public:
	Menu_Character_Outro_1(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 2) {}

	virtual void setEvent();
};



//*************************** #Scene - OnReady	***************************************// 

//씬 진입 시, 캐릭터 왼쪽에서 천천히 걸어 나옴 (2.16초 소모)
class Menu_OnReady_Intro_0 : public ActionBox
{
public:
	Menu_OnReady_Intro_0(Box* my_prev, Box *my_next) :ActionBox(my_prev, my_next, 1) {}
	virtual void setEvent();
};		


//운석의 시련에 대한, 액션을 실행시켜주는 이벤트 
class Menu_OnReady_Meteor_0 : public ActionBox
{
public:
	Menu_OnReady_Meteor_0(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 1) {}
	virtual void setEvent();
};
//운석의 시련에 대한, Message 출력 (시련 종류 설명)
class Menu_OnReady_Meteor_1 : public MsgBox
{
public:
	Menu_OnReady_Meteor_1(Box* my_prev, Box* my_next) : MsgBox(my_prev, my_next) {}
	virtual void setEvent();		
};


//톱니바퀴의 시련에 대한, Action
class Menu_OnReady_Gear_0 : public ActionBox
{
public:
	Menu_OnReady_Gear_0(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 1) {}
	virtual void setEvent();
};
//톱니바퀴의 시련에 대한, Message 
class Menu_OnReady_Gear_1 : public MsgBox
{
public:
	Menu_OnReady_Gear_1(Box* my_prev, Box *my_next) : MsgBox(my_prev, my_next) {}
	virtual void setEvent();		//메시지 준비
};


//출격버튼을 눌렀을 때, 캐릭터 오른쪽으로 빠르게 걸어나가는 모션 (2.88초 소모)
class Menu_OnReady_Outro_0 : public ActionBox
{
public:
	Menu_OnReady_Outro_0(Box* my_prev, Box *my_next) :ActionBox(my_prev, my_next, 1) {}
	virtual void setEvent();
};



//*************************** #Scene - Result	***************************************// 

// 씬 진입 시, 캐릭터 언덕 아래 오른쪽에서 천천히 걸어나옴
class Menu_Result_Intro_0 : public ActionBox
{
public:
	Menu_Result_Intro_0(Box* my_prev, Box* my_next) :ActionBox(my_prev, my_next, 1) {}
	virtual void setEvent();
};

//** Result 씬에서, 한번 터치 했을 때, 게임 플레이 등급이 표시
class Menu_Result_Grading_0 : public ActionBox
{
public:
	Menu_Result_Grading_0(Box* my_prev, Box *my_next) : ActionBox(my_prev, my_next, 2) {}
	virtual void setEvent();
};

//왼쪽 위로 걸어나감
class Menu_Result_Outro_0 : public ActionBox
{
public:
	Menu_Result_Outro_0(Box* my_prev, Box *my_next) :ActionBox(my_prev, my_next, 1) {}
	virtual void setEvent();
};



#endif 

