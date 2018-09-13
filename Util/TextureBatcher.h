#ifndef __TEXTURE_BATCHER__H_
#define __TEXTURE_BATCHER__H_

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

//싱글톤 클래스인 TextureCache로부터, 텍스쳐를 로드해주는 Hub 클래스. 
//texture 사용 유닛 단위로 참조횟수를 관리하기 위함
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

	Texture2D* joinTexLink(const string& full_tex_address);
	void detachTexLink(const string& full_tex_address);
	int getTexCnt(const string& full_tex_address);
};


#endif // !__TEXTURE_BATCHER__H_
