#ifndef __OBSTACLE_TEXTURE_BATCHER_H__
#define __OBSTACLE_TEXTURE_BATCHER_H__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;


//� �����췯(�̺�Ʈ ť)�� ���� ����� �� 
enum ObsMainType
{
	BUILDING = 0,
	ORDEAL = 1,
	BOSS = 2
};

//� �ؽ��ĸ� ������ ��..
enum ObsTexType
{
	COMPANY = 0,
	APARTMENT = 1,
	GOTHIC = 2,
	MYSTERY = 3,

	METEOR = 30,
	MUSE = 50,
};



struct BatchElement
{
	Texture2D* texture = NULL;
	Vector<SpriteFrame*> frames = {};
	Size frameSize = Size::ZERO;

	SpriteBatchNode* batchNode = NULL;	//�ش� �ؽ��ĸ� �������ϴ� ��ġ���
	BatchElement(ObsTexType type);
	~BatchElement();
};



class ObsTexBatcher : public CCNode
{
private:
	static bool isExist;
	map<ObsTexType, BatchElement*> texBatchTable = {};	//(�ε� ��) ��� �� obstacle�� ���ҽ��� ����

public:

	ObsTexBatcher() {
		assert(!isExist);
		isExist = true;
	}

	~ObsTexBatcher() {
		for (auto iter = texBatchTable.begin(); iter != texBatchTable.end(); iter++) {
			//�ʱ�ȭ �Ǿ��ִ� ��� batchElement�� ����
			if ((*iter).second != NULL)
				delete((*iter).second);
		}
		isExist = false;
	};

	void initTex(ObsTexType texture_type);

	BatchElement& getTexBatcher(ObsTexType tex_type) {
		if (texBatchTable.find(tex_type) == texBatchTable.end())
			initTex(tex_type);

		return *texBatchTable[tex_type];
	}

};

#endif