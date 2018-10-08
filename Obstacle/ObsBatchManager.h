#ifndef __OBSTACLE_BATCH_MANAGER_H__
#define __OBSTACLE_BATCH_MANAGER_H__

#include "cocos2d.h"
#include "Util/TextureBatcher.h"

USING_NS_CC;
using namespace std;

//� �����췯(�̺�Ʈ ť)�� ���� ����� �� 
enum ObsMainType {
	BUILDING = 0, ORDEAL = 1, BOSS = 2
};

//� �ؽ��ĸ� ������ ��..
enum ObsTexType {
	COMPANY = 0,
	APARTMENT = 1,
	GOTHIC = 2,
	MYSTERY = 3,

	METEOR = 30,
	MUSE = 50,
};



//Ÿ������ �ϴ� Obstacle�� �ؽ��ĸ� batch drawing
class ObsBatcher : public SpriteBatchNode
{
	static const int DEFAULT_CAPACITY = 29;
public:

	static ObsBatcher* createWithTexture(ObsTexType type, Texture2D* tex, ssize_t capacity = DEFAULT_CAPACITY);
	Vector<SpriteFrame*> unitFrame = {};
	Size unitSize = Size::ZERO;	//��ֹ� ������Ʈ �ϳ��� �⺻ frame size

	ObsBatcher(ObsTexType type, Texture2D* tex);
	virtual ~ObsBatcher();
};




class ObsBatchManager : public CCNode
{
private:
	static bool isExist;
	TextureBatcher* texPool;
	map<ObsTexType, ObsBatcher*> BatchTable = {};	//(�ε� ��) ��� �� obstacle�� ���ҽ��� ����

public:

	//�ѹ��� 1���� �ν��ͽ��� �����ϰ� ����
	ObsBatchManager() {
		assert(!isExist);
		texPool = new TextureBatcher();
		isExist = true;
	}
	~ObsBatchManager() {

		while (!BatchTable.empty()) {
			removeObsBatcher(BatchTable.begin()->first);
		}
		//CCLOG(TextureCache::getInstance()->getCachedTextureInfo().c_str());
		texPool->release();
		isExist = false;
	};

	void initObsBatcher(ObsTexType obs_tex_type);
	void removeObsBatcher(ObsTexType obs_tex_type);


	ObsBatcher& getBatcher(ObsTexType obs_tex_type) {
		if (BatchTable.find(obs_tex_type) == BatchTable.end()) {
			initObsBatcher(obs_tex_type);
		}
		return *BatchTable[obs_tex_type];
	}

};

#endif