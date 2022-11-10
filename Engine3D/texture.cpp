//#define GLEW_STATIC
// #define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"
#include "stb_image.h"
#include "../res/includes/glad/include/glad/glad.h"
#include <iostream>
#include <vector>
#include <math.h>
#include <cmath>
using namespace std;

static unsigned char *floyed (unsigned char* data){
    int len = strlen((char*)data) /4;
    int sqr = sqrt(len);

    unsigned char mat[sqr][sqr];
    for(int i =0; i<sqr; i++){
        for(int j =0; j<sqr; j++){
            mat[i][j] = (data[4 *(i * sqr + j)] + data[4 *(i * sqr + j) + 1] + data[4 *(i * sqr + j) + 2])/3;
        }
    }

    for(int i =0; i<sqr; i++){
        for(int j =0; j<sqr; j++){
            char oldPixel = mat[i][j];
            char newPixel = 16 * (oldPixel/16);
            int err = oldPixel - newPixel;
            if(j == 0){
                if(i == sqr){ //[sqr][0]
                    mat[i][j] = newPixel;
                    mat[i][j+1] = mat[i][j+1] + err*(7/16);
                }
                else{
                    mat[i][j] = newPixel;
                    mat[i][j+1] = mat[i][j+1] + err*(7/16);
                    mat[i+1][j] = mat[i+1][j] + err*(5/16);
                    mat[i+1][j+1] = mat[i+1][j+1] + err*(1/16);
                }
            }
            else if(i == sqr || j == sqr){ //edges TODO check
                if(i == sqr){
                    mat[i][j] = newPixel; //todo not
                }
                else{ //j is sqr & i != sqr
                    mat[i][j] = newPixel;
                    mat[i+1][j-1] = mat[i+1][j-1] + err*(3/16);
                    mat[i+1][j] = mat[i+1][j] + err*(5/16);
                }
            }else{
                mat[i][j] = newPixel;
                mat[i][j+1] = mat[i][j+1] + err*(7/16);
                mat[i+1][j-1] = mat[i+1][j-1] + err*(3/16);
                mat[i+1][j] = mat[i+1][j] + err*(5/16);
                mat[i+1][j+1] = mat[i+1][j+1] + err*(1/16);
            }

        }
    }
    unsigned char *data2 = new unsigned char[len*4];
    int index = 0;
    for(int i = 0; i < sqr; i++){
        for(int j =0; j < sqr; j++){
            data2[index] = mat[i][j];
            data2[index + 1] = mat[i][j];
            data2[index + 2] = mat[i][j];
            data2[index + 3] = data[4 *(i * sqr + j) + 3];
            index += 4;
        }
    }
    return data2;
}

static unsigned char *sobel (unsigned char* data) {
    int len = strlen((char *) data) / 4;
    int sqr = sqrt(len);

    unsigned char mat[sqr + 2][sqr + 2];
    for (int i = 1; i < sqr + 1; i++) {
        for (int j = 1; j < sqr + 1; j++) {
            mat[i][j] = (data[4 * (i * sqr + j)] + data[4 * (i * sqr + j) + 1] + data[4 * (i * sqr + j) + 2]) / 3;
        }
    }

    char gx[3][3] = {{-1, 0, 1},
                     {-2, 0, 2},
                     {-1, 0, 1}};
    char gy[3][3] = {{1,  2,  1},
                     {0,  0,  0},
                     {-1, -2, -1}};

    unsigned char mat2[sqr][sqr];

    for (int i = 0; i < sqr+2; i++) {
        for (int j = 0; j < sqr + 2; j++) {
            char newPixelGx = mat[i - 1][j - 1] * gx[0][0] +
                              mat[i - 1][j + 1] * gx[0][2] +
                              mat[i + 1][j - 1] * gx[2][0] +
                              mat[i + 1][j + 1] * gx[2][2] +
                              mat[i][j - 1] * gx[1][0] +
                              mat[i][j + 1] * gx[1][2];
            char newPixelGy = mat[i - 1][j - 1] * gy[0][0] +
                              mat[i - 1][j] * gy[0][1] +
                              mat[i - 1][j + 1] * gy[0][2] +
                              mat[i + 1][j - 1] * gy[2][0] +
                              mat[i + 1][j] * gy[2][1] +
                              mat[i + 1][j + 1] * gy[2][2];
            unsigned char newPixel = std::abs(newPixelGx) + std::abs(newPixelGy);
            mat2[i][j] = newPixel;
        }
    }

        unsigned char *data2 = new unsigned char[len*4];
        int index = 0;
        for(int i = 0; i < sqr; i++){
            for(int j =0; j < sqr; j++){
                data2[index] = mat2[i][j];
                data2[index + 1] = mat2[i][j];
                data2[index + 2] = mat2[i][j];
                data2[index + 3] = data[4 *(i * sqr + j) + 3];
                index += 4;
            }
        }
        return data2;
    }

static unsigned char *halftone (unsigned char* data) {
    int len = strlen((char *) data) / 4;
    int sqr = sqrt(len);

    unsigned char mat[sqr][sqr];
    for (int i = 0; i < sqr ; i++) {
        for (int j = 0; j < sqr; j++) {
            mat[i][j] = (data[4 * (i * sqr + j)] + data[4 * (i * sqr + j) + 1] + data[4 * (i * sqr + j) + 2]) / 3;
        }
    }



    unsigned char newMat[2*sqr][2*sqr];

    for(int i =0; i< sqr; i++) {
        for (int j = 0; j < sqr; j++) {

            float curr = (float) mat[i][j] / (float) 255;
            if (curr < 0.2) {
                newMat[2 * i][2 * j] = 255;
                newMat[2 * i + 1][2 * j] = 255;
                newMat[2 * i][2 * j + 1] = 255;
                newMat[2 * i + 1][2 * j + 1] = 255;

            } else if (curr < 0.4) {
                newMat[2 * i][2 * j] = 255;
                newMat[2 * i + 1][2 * j] = 0;
                newMat[2 * i][2 * j + 1] = 255;
                newMat[2 * i + 1][2 * j + 1] = 255;

            } else if (curr < 0.6) {
                newMat[2 * i][2 * j] = 255;
                newMat[2 * i + 1][2 * j] = 0;
                newMat[2 * i][2 * j + 1] = 0;
                newMat[2 * i + 1][2 * j + 1] = 255;

            } else if (curr < 0.8) {
                newMat[2 * i][2 * j] = 255;
                newMat[2 * i + 1][2 * j] = 0;
                newMat[2 * i][2 * j + 1] = 0;
                newMat[2 * i + 1][2 * j + 1] = 0;

            } else {
                newMat[2 * i][2 * j] = 0;
                newMat[2 * i + 1][2 * j] = 0;
                newMat[2 * i][2 * j + 1] = 0;
                newMat[2 * i + 1][2 * j + 1] = 0;

            }
        }
    }

        unsigned char *data2 = new unsigned char[len*16];
        int index = 0;
        for(int i = 0; i < sqr * 2; i++){
            for(int j =0; j < sqr * 2; j++){
                data2[index] = newMat[i][j];
                data2[index + 1] = newMat[i][j];
                data2[index + 2] = newMat[i][j];
                data2[index + 3] = data[4 *(((i/2) * sqr) + j/2) + 3];
                index += 4;
            }
        }
        return data2;
    }


Texture::Texture(const std::string& fileName, int type)
{
	int width, height, numComponents;
    unsigned char* data = stbi_load((fileName).c_str(), &width, &height, &numComponents, 4);
    switch (type) {
        case 0:
            data = sobel(data);
            break;
        case 1:
            data = halftone(data);
            width *= 2;
            height *= 2;
            break;
        case 2:
            data = floyed(data);
            break;
    }

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

