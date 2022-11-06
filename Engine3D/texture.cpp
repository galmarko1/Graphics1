//#define GLEW_STATIC
// #define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"
#include "stb_image.h"
#include "../res/includes/glad/include/glad/glad.h"
#include <iostream>
#include <vector>
using namespace std;


Texture::Texture(const std::string& fileName)
{
	int width, height, numComponents;
    unsigned char* data = stbi_load((fileName).c_str(), &width, &height, &numComponents, 4);
//
//    int index = 0;
//    unsigned char mat[128][128];
//    for(int i =0; i<128; i++){
//        for(int j =0; j<128; j++){
//            mat[i][j] = (data[index] + data[index+1] + data[index+2] + data[index+3])/4;
//            index += 4;
//        }
//    }
//
//    for(int i =0; i<128; i++){
//        for(int j =0; j<128; j++){
//            char oldPixel = mat[i][j];
//            char newPixel = oldPixel/16;
//            int err = oldPixel - newPixel;
//
//            mat[i][j] = newPixel;
//            mat[i][j+1] = err*(7/16);
//            mat[i+1][j-1] = err*(3/16);
//            mat[i+1][j] = err*(5/16);
//            mat[i+1][j+1] = err*(1/16);
//        }
//    }
//    unsigned char newData[128*128];
//    for(int i =0; i< 128; i++){
//        for(int j =0; j< 128; j++){
//            newData[i+j] = mat[i][j];
//        }
//    }
//    data = newData;
//





    /// q5


    int index = 0;
    unsigned char mat[128][128];
    for(int i =0; i<128; i++){
        for(int j =0; j<128; j++){
            mat[i][j] = (data[index] + data[index+1] + data[index+2] + data[index+3])/4;
            index += 4;
        }
    }


    unsigned char newMat[512][512];
    for(int i =0; i< 128; i++){
        for(int j = 0; j< 128; j++){
            float curr = mat[i][j]/255;
            if(curr < 0.2 ){
                newMat[2*i][2*j] = 255;
                newMat[2*i + 1][2*j] = 255;
                newMat[2*i][2*j + 1] = 255;
                newMat[2*i+1][2*j+1] = 255;

            }else if(curr <0.4){
                newMat[2*i][2*j] = 255;
                newMat[2*i + 1][2*j] = 0;
                newMat[2*i][2*j + 1] = 255;
                newMat[2*i+1][2*j+1] = 255;

            }else if (curr <0.6){
                newMat[2*i][2*j] = 255;
                newMat[2*i + 1][2*j] = 0;
                newMat[2*i][2*j + 1] = 0;
                newMat[2*i+1][2*j+1] = 255;

            }else if(curr < 0.8){
                newMat[2*i][2*j] = 255;
                newMat[2*i + 1][2*j] = 0;
                newMat[2*i][2*j + 1] = 0;
                newMat[2*i+1][2*j+1] = 0;

            }else{
                newMat[2*i][2*j] = 0;
                newMat[2*i + 1][2*j] = 0;
                newMat[2*i][2*j + 1] = 0;
                newMat[2*i+1][2*j+1] = 0;

            }
        }
    }

    unsigned char newData[512 * 512];
    for(int i =0; i< 512; i++){
        for(int j =0; j< 512; j++){
            newData[i+j] = newMat[i][j];
        }
    }
    data = newData;


    if(data == NULL)
		std::cerr << "Unable to load texture: " << fileName << std::endl;
        
    glGenTextures(1, &m_texture);
    Bind(m_texture);
        
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_LOD_BIAS,-0.4f);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//    stbi_image_free(data);
}

Texture::Texture(int width,int height,unsigned char *data)
{
    glGenTextures(1, &m_texture);
    Bind(m_texture);
        
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_texture);
}

void Texture::Bind(int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

