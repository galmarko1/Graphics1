#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>

class Texture
{
public:
	Texture(const std::string& fileName, int type);
	Texture(int width, int height,unsigned char *data);
    void func(const std::string& fileName);
	void Bind(int slot);
	inline int GetSlot(){return m_texture;}
	 ~Texture();
protected:
private:
	Texture(const Texture& texture) {}
	void operator=(const Texture& texture) {}
	unsigned int m_texture;
    std::vector<std::vector<unsigned char>> mat;
};

#endif
