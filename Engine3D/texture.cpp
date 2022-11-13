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
#include <numbers>

using namespace std;

#define N 3

void multiply(int arr1[][N], int arr2[][N], int arr3[][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            arr3[i][j] = 0;
            for (int k = 0; k < N; k++) {
                arr3[i][j] += arr1[i][k] * arr2[k][j];
            }
        }
    }
}

//
//static void gaussKernel(int size, int sigma = 1, int arr3[3][3]){
//    size = size/2;
//    int gausKernelx[3][3] = {{-1,-1,-1},
//                             {0,0,0},
//                             {1,1,1}};
//    int gausKernely[3][3] = {{-1,0,1},
//                             {-1,0,1},
//                             {-1,0,1}};
//
//    int normal = 1/(2.0 * 3.14159265358979323846 * pow(sigma,2));
//
//    int xx[3][3];
//    int yy[3][3];
//    multiply(gausKernelx, gausKernelx, xx);
//    multiply(gausKernely, gausKernely, yy);
////    int g[3][3]; //todo exp
//
//
//}





static unsigned char *floyed(unsigned char *data) {
    int len = strlen((char *) data) / 4;
    int sqr = sqrt(len);

    unsigned char mat[sqr][sqr];
    for (int i = 0; i < sqr; i++) {
        for (int j = 0; j < sqr; j++) {
            mat[i][j] = (data[4 * (i * sqr + j)] + data[4 * (i * sqr + j) + 1] + data[4 * (i * sqr + j) + 2]) / 3;
        }
    }

    for (int i = 0; i < sqr; i++) {
        for (int j = 0; j < sqr; j++) {
            char oldPixel = mat[i][j];
            char newPixel = 16 * (oldPixel / 16);
            int err = oldPixel - newPixel;
            if (j == 0) {
                if (i == sqr) { //[sqr][0]
                    mat[i][j] = newPixel;
                    mat[i][j + 1] = mat[i][j + 1] + err * (7 / 16);
                } else {
                    mat[i][j] = newPixel;
                    mat[i][j + 1] = mat[i][j + 1] + err * (7 / 16);
                    mat[i + 1][j] = mat[i + 1][j] + err * (5 / 16);
                    mat[i + 1][j + 1] = mat[i + 1][j + 1] + err * (1 / 16);
                }
            } else if (i == sqr || j == sqr) { //edges TODO check
                if (i == sqr) {
                    mat[i][j] = newPixel; //todo not
                } else { //j is sqr & i != sqr
                    mat[i][j] = newPixel;
                    mat[i + 1][j - 1] = mat[i + 1][j - 1] + err * (3 / 16);
                    mat[i + 1][j] = mat[i + 1][j] + err * (5 / 16);
                }
            } else {
                mat[i][j] = newPixel;
                mat[i][j + 1] = mat[i][j + 1] + err * (7 / 16);
                mat[i + 1][j - 1] = mat[i + 1][j - 1] + err * (3 / 16);
                mat[i + 1][j] = mat[i + 1][j] + err * (5 / 16);
                mat[i + 1][j + 1] = mat[i + 1][j + 1] + err * (1 / 16);
            }

        }
    }
    unsigned char *data2 = new unsigned char[len * 4];
    int index = 0;
    for (int i = 0; i < sqr; i++) {
        for (int j = 0; j < sqr; j++) {
            data2[index] = mat[i][j];
            data2[index + 1] = mat[i][j];
            data2[index + 2] = mat[i][j];
            data2[index + 3] = data[4 * (i * sqr + j) + 3];
            index += 4;
        }
    }
    return data2;
}

static unsigned char *sobel(unsigned char *data) {
    int len = strlen((char *) data) / 4;
    int sqr = sqrt(len);

    unsigned char mat[sqr + 2][sqr + 2];
    for (int i = 1; i < sqr + 1; i++) {
        for (int j = 1; j < sqr + 1; j++) {
            mat[i][j] = (data[4 * (i * sqr + j)] + data[4 * (i * sqr + j) + 1] + data[4 * (i * sqr + j) + 2]) / 3;
        }
    }

    float mat2[sqr + 2][sqr + 2];


    /////insert gausian

    float gaussKernel[3][3] = {{1, 2, 1},
                              {2, 4, 2},
                              {1, 2, 1}};

    for (int i = 1; i < sqr + 2; i++) {
        for (int j = 1; j < sqr + 2; j++) {
            float newPixel = mat[i - 1][j - 1] * gaussKernel[0][0] +
                            mat[i - 1][j] * gaussKernel[0][1] +
                            mat[i - 1][j + 1] * gaussKernel[0][2] +
                            mat[i][j - 1] * gaussKernel[1][0] +
                            mat[i][j] * gaussKernel[1][1] +
                            mat[i][j + 1] * gaussKernel[1][2] +
                            mat[i + 1][j - 1] * gaussKernel[2][0] +
                            mat[i + 1][j] * gaussKernel[2][1] +
                            mat[i + 1][j + 1] * gaussKernel[2][2];

            mat2[i][j] = mat[i-1][j-1] + newPixel/16;
        }
    }




    ////end gausian
    //sobel
//
    float gx[3][3] = {{-1, 0, 1},
                     {-2, 0, 2},
                     {-1, 0, 1}};
    float gy[3][3] = {{1,  2,  1},
                     {0,  0,  0},
                     {-1, -2, -1}};



    float mat3[sqr + 2][sqr + 2];
    float angles[sqr + 2][sqr + 2];

    for (int i = 1; i < sqr + 2; i++) {
        for (int j = 1; j < sqr + 2; j++) {
            float newPixelGx = mat2[i - 1][j - 1] * gx[0][0] +
                              mat2[i - 1][j + 1] * gx[0][2] +
                              mat2[i + 1][j - 1] * gx[2][0] +
                              mat2[i + 1][j + 1] * gx[2][2] +
                              mat2[i][j - 1] * gx[1][0] +
                              mat2[i][j + 1] * gx[1][2];
            float newPixelGy = mat2[i - 1][j - 1] * gy[0][0] +
                              mat2[i - 1][j] * gy[0][1] +
                              mat2[i - 1][j + 1] * gy[0][2] +
                              mat2[i + 1][j - 1] * gy[2][0] +
                              mat2[i + 1][j] * gy[2][1] +
                              mat2[i + 1][j + 1] * gy[2][2];
            float newPixel = std::sqrt(newPixelGx * newPixelGx + newPixelGy * newPixelGy);

            if(newPixel <0)
                mat3[i][j] = 0;
            else if(newPixel > 255)
                mat3[i][j] = 255;
            else
                mat3[i][j] = newPixel;


            if (newPixelGx == 0)
                angles[i][j] = 0;
            else
                angles[i][j] = std::atan(newPixelGy/newPixelGx);

        }
    }






//    ////////////// end sobel
//
    /////non max

    float mat4[sqr + 2][sqr + 2];

    float max  = -2000;
    for (int i = 1; i < sqr + 2; i++) {
        for (int j = 1; j < sqr + 2; j++) {
            float p1;
            float p2;
            float curr = mat3[i][j];
            if ((angles[i][j] <= 22.5 && angles[i][j] >= -22.5) || (angles[i][j] < -157.5 && angles[i][j] >= -180)) {
                p1 = mat3[i][j - 1];
                p2 = mat3[i][j + 1];
            } else if ((angles[i][j] <= 67.5 && angles[i][j] >= 22.5) || (angles[i][j] >= -157.5 && angles[i][j] < -112.5)) {
                p1 = mat3[i - 1][j - 1];
                p2 = mat3[i + 1][j + 1];
            } else if ((angles[i][j] <= 112.5 && angles[i][j] >= 67.5) || (angles[i][j] <-67.5 && angles[i][j] >= -112.5)) {
                p1 = mat3[i - 1][j];
                p2 = mat3[i + 1][j];
            } else if ((angles[i][j] <= 157.5 && angles[i][j] >= 112.5) || (angles[i][j] <-22.5 && angles[i][j] >= -67.5)) {
                p1 = mat3[i + 1][j - 1];
                p2 = mat3[i - 1][j + 1];
            }
//            } else {
//                p1 = mat3[i][j - 1];
//                p2 = mat3[i][j + 1];
//            }
            if (curr >= p1 && curr >= p2) {
                if(curr > max)
                    max = curr;

                mat4[i][j] = curr;
            } else {
                mat4[i][j] = 0;
            }
        }
    }

    float highThreshold = 255 * 0.2;
    float lowThreshhold = highThreshold * 0.05;
//    float newMat[sqr+2][sqr+2];
    for(int i =0; i < sqr+1; i++){
        for (int j = 0; j< sqr + 1; j++){
            if(mat4[i][j] >= highThreshold){
                mat4[i][j] = 255;
            }else if ((mat4[i][j] <= highThreshold) & (mat4[i][j] >= lowThreshhold)){
                mat4[i][j] = 25;
            }
        }
    }


    for(int i =0; i< sqr+1; i++){
        for(int j=0; j< sqr+1; j++){
            if (mat4[i][j] == 25){
                if ((mat4[i+1][j-1] == 255) || (mat4[i+1][j] == 255) || (mat4[i+1][j+1] == 255)
                        || (mat4[i][j-1] == 255) || (mat4[i][j+1] == 255)
                        || (mat4[i-1][j-1] == 255) || (mat4[i-1][j] == 255) || (mat4[i-1][j+1] == 255))
                        mat4[i][j] = 255;
                else
                    mat4[i][j] = 0;
//
            }
//
//
        }
    }



//  M, N = img.shape
//    for i in range(1, M-1):
//        for j in range(1, N-1):
//            if (img[i,j] == weak):
//                try:
//                    if ((img[i+1, j-1] == strong) or (img[i+1, j] == strong) or (img[i+1, j+1] == strong)
//                        or (img[i, j-1] == strong) or (img[i, j+1] == strong)
//                        or (img[i-1, j-1] == strong) or (img[i-1, j] == strong) or (img[i-1, j+1] == strong)):
//                        img[i, j] = strong
//                    else:
//                        img[i, j] = 0
//                except IndexError as e:
//                    pass
//    return img
//

    ///////////////


    unsigned char *data2 = new unsigned char[len * 4];
    int index = 0;
    for (int i = 1; i < sqr + 1; i++) {
        for (int j = 1; j < sqr + 1; j++) {
            data2[index] = mat4[i][j];
            data2[index + 1] = mat4[i][j];
            data2[index + 2] = mat4[i][j];
            data2[index + 3] = data[4 * (i * sqr + j) + 3];
            index += 4;
        }
    }
    return data2;
}

static unsigned char *halftone(unsigned char *data) {
    int len = strlen((char *) data) / 4;
    int sqr = sqrt(len);

    unsigned char mat[sqr][sqr];
    for (int i = 0; i < sqr; i++) {
        for (int j = 0; j < sqr; j++) {
            mat[i][j] = (data[4 * (i * sqr + j)] + data[4 * (i * sqr + j) + 1] + data[4 * (i * sqr + j) + 2]) / 3;
        }
    }


    unsigned char newMat[2 * sqr][2 * sqr];

    for (int i = 0; i < sqr; i++) {
        for (int j = 0; j < sqr; j++) {

            float curr = (float) mat[i][j] / (float) 255;
            if (curr < 0.2) {
                newMat[2 * i][2 * j] = 0;
                newMat[2 * i + 1][2 * j] = 0;
                newMat[2 * i][2 * j + 1] = 0;
                newMat[2 * i + 1][2 * j + 1] = 0;

            } else if (curr < 0.4) {
                newMat[2 * i][2 * j] = 0;
                newMat[2 * i + 1][2 * j] = 255;
                newMat[2 * i][2 * j + 1] = 0;
                newMat[2 * i + 1][2 * j + 1] = 0;

            } else if (curr < 0.6) {
                newMat[2 * i][2 * j] = 0;
                newMat[2 * i + 1][2 * j] = 255;
                newMat[2 * i][2 * j + 1] = 255;
                newMat[2 * i + 1][2 * j + 1] = 0;

            } else if (curr < 0.8) {
                newMat[2 * i][2 * j] = 0;
                newMat[2 * i + 1][2 * j] = 255;
                newMat[2 * i][2 * j + 1] = 255;
                newMat[2 * i + 1][2 * j + 1] = 255;

            } else {
                newMat[2 * i][2 * j] = 255;
                newMat[2 * i + 1][2 * j] = 255;
                newMat[2 * i][2 * j + 1] = 255;
                newMat[2 * i + 1][2 * j + 1] = 255;
            }
        }
    }

    unsigned char *data2 = new unsigned char[len * 16];
    int index = 0;
    for (int i = 0; i < sqr * 2; i++) {
        for (int j = 0; j < sqr * 2; j++) {
            data2[index] = newMat[i][j];
            data2[index + 1] = newMat[i][j];
            data2[index + 2] = newMat[i][j];
            data2[index + 3] = data[4 * (((i / 2) * sqr) + (j / 2)) + 3];
            index += 4;
        }
    }
    return data2;
}


Texture::Texture(const std::string &fileName, int type) {
    int width, height, numComponents;
    unsigned char *data = stbi_load((fileName).c_str(), &width, &height, &numComponents, 4);
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

    if (data == NULL)
        std::cerr << "Unable to load texture: " << fileName << std::endl;

    glGenTextures(1, &m_texture);
    Bind(m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//    stbi_image_free(data);
}

Texture::Texture(int width, int height, unsigned char *data) {
    glGenTextures(1, &m_texture);
    Bind(m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

}

Texture::~Texture() {
    glDeleteTextures(1, &m_texture);
}

void Texture::Bind(int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

