#ifndef __TEXTURE_BATCHER__H_
#define __TEXTURE_BATCHER__H_

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

//�̱��� Ŭ������ TextureCache�κ���, �ؽ��ĸ� �ε����ִ� Hub Ŭ����. 
//texture ��� ���� ������ ����Ƚ���� �����ϱ� ����
class TextureBatcher : public CCNode
{
private:
	map<string, int> TexPool = {};

public:
	TextureBatcher() {};

	~TextureBatcher() {
		while (!TexPool.empty()){
			detachTexLink(TexPool.begin()->first);
		}
	};

	Texture2D* joinTexLink(string full_tex_address);
	void detachTexLink(string full_tex_address);
	int getTexCnt(string full_tex_address);
};


#endif // !__TEXTURE_BATCHER__H_
