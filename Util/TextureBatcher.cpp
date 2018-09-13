#include "TextureBatcher.h"

Texture2D* TextureBatcher::joinTexLink(const string& full_tex_address)
{
	Texture2D* tex = TextureCache::getInstance()->getTextureForKey(full_tex_address);

	//아직 캐쉬로드 안된 텍스쳐 일 때,
	if (getTexCnt(full_tex_address) == 0){
		TexPool.insert({ full_tex_address , 1 });
		tex = TextureCache::getInstance()->addImage(full_tex_address);
	}
	else{
		TexPool[full_tex_address]++;
	}
	assert(tex != NULL);
	return tex;
}


void TextureBatcher::detachTexLink(const string& full_tex_address)
{
	if (getTexCnt(full_tex_address) != 0)
	{
		TexPool[full_tex_address]--;
		
		//더이상 참조하는 유닛이 없을 경우, 캐쉬와 Pool에서 해제
		if (TexPool[full_tex_address] == 0){
			TextureCache::getInstance()->removeTextureForKey(full_tex_address);
			TexPool.erase(full_tex_address);
		}
	}
}

//texture pool의 address 검색을 통해, reference count를 검색한다
int TextureBatcher::getTexCnt(const string& full_tex_address) {

	if (TexPool.find(string(full_tex_address)) != TexPool.end())
	{
		assert(TexPool[full_tex_address] > 0);	//reference count는 언제나 양수여야
		return TexPool[full_tex_address];
	}
	else
		return 0;
}
