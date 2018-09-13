#include "TextureBatcher.h"

Texture2D* TextureBatcher::joinTexLink(const string& full_tex_address)
{
	Texture2D* tex = TextureCache::getInstance()->getTextureForKey(full_tex_address);

	//���� ĳ���ε� �ȵ� �ؽ��� �� ��,
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
		
		//���̻� �����ϴ� ������ ���� ���, ĳ���� Pool���� ����
		if (TexPool[full_tex_address] == 0){
			TextureCache::getInstance()->removeTextureForKey(full_tex_address);
			TexPool.erase(full_tex_address);
		}
	}
}

//texture pool�� address �˻��� ����, reference count�� �˻��Ѵ�
int TextureBatcher::getTexCnt(const string& full_tex_address) {

	if (TexPool.find(string(full_tex_address)) != TexPool.end())
	{
		assert(TexPool[full_tex_address] > 0);	//reference count�� ������ �������
		return TexPool[full_tex_address];
	}
	else
		return 0;
}
