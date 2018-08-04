#ifndef UTIL_H
#define UTIL_H

#include "IDW.h"
#include "HE_mesh\Mesh3D.h"
#include "triangle.h"
#include <algorithm>
#include <vector>

void fix(Img * img, MatrixXi & flag);
std::vector<ND_Point> allImgP(Img & img);
void genTriMesh(std::vector<ND_Point> & V, std::vector<std::vector<int>> & faces);
void genTriMesh(std::vector<point2> & V, std::vector<std::vector<int>> & faces);
//FAST_API--------------
void FAST_API(Img & img, std::vector<ND_Point> & keypoints, int threshold);
//FAST---------------------------------------------
struct OnePoint2f
{
    float x;
    float y;
};
typedef struct __KeyPoint_
{
    OnePoint2f pt; 
    float size; 
    float angle;

    float response; 
    int octave;

}OneKeyPoint;

enum MyEnum
{
    start = -1,
    nOAST_9_16,
    nAGAST_5_8,

};

// @para img »Ò¶ÈÍ¼
// @para mWidth Í¼Ïñ¿í
// @para mHeight Í¼Ïñ¸ß
// @para keypoints ½á¹û
// @para threshold ãÐÖµ
void OAST_9_16_(unsigned char * img, int mWidth, int mHeight, std::vector<OneKeyPoint>& keypoints, int threshold);

#endif UTIL_H