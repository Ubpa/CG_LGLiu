#include "Util.h"

void fix(Img * img, MatrixXi & flag){
	for(int i=0; i<img->width; i++){
		for( int j=0; j<img->height; j++){
			if( flag(i, j) == 0 ){
				// find neighbor
				std::vector<ND_Point> neighborVec;
				std::vector<ND_Point> colorVec;
				for(int x = i-2; x<=i+2; x++){
					for(int y = j-2; y<=j+2; y++){
						if( img->contains(x, y) && flag(x, y) != 0 ){
							neighborVec.push_back(ND_Point(x,y));
							colorVec.push_back(img->at(x,y));
						}
					}
				}
				if(neighborVec.size()==0)
					continue;
				
				// interpolation with neighbor
				IDW idw(neighborVec, colorVec);
				ND_Point newColor = idw.map(ND_Point(i, j));
				img->at(i, j) = newColor;
			}
		}
	}
}

std::vector<ND_Point> allImgP(Img & img){
	std::vector<ND_Point> points;
	for(int i=0; i<img.width; i++)
		for(int j=0; j<img.height; j++)
			points.push_back(ND_Point(i,j));
	return points;
}

void genTriMesh(std::vector<ND_Point> & V, std::vector<std::vector<int>> & faces){
	faces.clear();
	struct triangulateio in, out;

	// 初始化 in
	in.numberofpoints = V.size();
	in.pointlist = new double[2 * in.numberofpoints];
	in.numberofpointattributes = 1;
	in.pointattributelist = new double[in.numberofpoints * in.numberofpointattributes];
	in.pointmarkerlist = new int[in.numberofpoints];
	for (int i = 0; i < in.numberofpoints; i++) {
		in.pointlist[i * 2] = V[i].x;
		in.pointlist[i * 2 + 1] = V[i].y;
		in.pointattributelist[i] = 10;
		in.pointmarkerlist[i] = 1;
	}
	in.numberofsegments = 0;
	in.numberofholes = 0;
	in.numberofregions = 0;

	// 初始化 out
	out.pointlist = (TRI_REAL *)NULL;
	out.pointattributelist = (TRI_REAL *)NULL;
	out.pointmarkerlist = (int *)NULL;
	out.trianglelist = (int *)NULL;
	out.triangleattributelist = (TRI_REAL *)NULL;
	out.neighborlist = (int *)NULL;
	out.segmentlist = (int *)NULL;
	out.segmentmarkerlist = (int *)NULL;
	out.edgelist = (int *)NULL;
	out.edgemarkerlist = (int *)NULL;

	// 三角化
	triangulate("pczAenQ", &in, &out, (struct triangulateio *) NULL);

	// get faces
	for(int i = 0; i< out.numberoftriangles; i++){
		std::vector<int> face;
		for(int j = 0; j < out.numberofcorners; j++)
			face.push_back(out.trianglelist[i * out.numberofcorners + j]);
		faces.push_back(face);
	}

	// free
	free(in.pointlist);
	free(in.pointattributelist);
	free(in.pointmarkerlist);
	free(out.pointlist);
	free(out.pointattributelist);
	free(out.pointmarkerlist);
	free(out.trianglelist);
	free(out.triangleattributelist);
	free(out.neighborlist);
	free(out.segmentlist);
	free(out.segmentmarkerlist);
	free(out.edgelist);
	free(out.edgemarkerlist);
}
//FAST_API--------------
void FAST_API(Img & img, std::vector<ND_Point> & keypoints, int threshold){
	Img grayImg;
	img.genGray(grayImg);
	int size = grayImg.width*grayImg.height;
	unsigned char * imgUSC = new unsigned char[size];
	for(int i=0; i<size; i++)
		imgUSC[i] = grayImg.colors[i].x;
	std::vector<OneKeyPoint> _keypoints;

	OAST_9_16_(imgUSC, grayImg.width, grayImg.height, _keypoints, threshold);

	for(int i=0; i<_keypoints.size(); i++){
		keypoints.push_back(ND_Point((int)_keypoints[i].pt.x, (int)_keypoints[i].pt.y));
	}
	
	delete [] imgUSC;
}
//FAST---------------------------------------------------------
using namespace std;
void makeAgastOffsets(int pixel[16], int rowStride, int type)
{
    static const int offsets16[][2] =
    {
        {-3,  0}, {-3, -1}, {-2, -2}, {-1, -3}, {0, -3}, { 1, -3}, { 2, -2}, { 3, -1},
        { 3,  0}, { 3,  1}, { 2,  2}, { 1,  3}, {0,  3}, {-1,  3}, {-2,  2}, {-3,  1}
    };

    static const int offsets8[][2] =
    {
        {-1,  0}, {-1, -1}, {0, -1}, { 1, -1},
        { 1,  0}, { 1,  1}, {0,  1}, {-1,  1}
    };

    const int (*offsets)[2] = type == nOAST_9_16 ? offsets16 :
                              type == nAGAST_5_8 ? offsets8  : 0;


    int k = 0;
    for( ; k < 16; k++ )
        pixel[k] = offsets[k][0] + offsets[k][1] * rowStride;
}



int agast_cornerScore_9_16_(const unsigned char* ptr, const int pixel[], int threshold)
{
    int bmin = threshold;
    int bmax = 255;
    int b_test = (bmax + bmin) / 2;
    register short offset[16] = { 0 };
    offset[0]= (short)pixel[0];
    offset[1]= (short)pixel[1];
    offset[2]= (short)pixel[2];
    offset[3]= (short)pixel[3];
    offset[4]= (short)pixel[4];
    offset[5]= (short)pixel[5];
    offset[6]= (short)pixel[6];
    offset[7]= (short)pixel[7];
    offset[8]= (short)pixel[8];
    offset[9]= (short)pixel[9];
    offset[10] = (short)pixel[10];
    offset[11] = (short)pixel[11];
    offset[12] = (short)pixel[12];
    offset[13] = (short)pixel[13];
    offset[14] = (short)pixel[14];
    offset[15] = (short)pixel[15];

    while (true)
    {
        register const int cb = *ptr + b_test;
        register const int c_b = *ptr - b_test;
        int flag[32] = { 0 };
        for (int k = 0; k < 16; k++)
        {
            if (ptr[offset[k]] > cb)
            {
                flag[k] = 1;
                flag[k + 16] = 1;

            }
            else if (ptr[offset[k]] < c_b)
            {
                flag[k] = -1;
                flag[k + 16] = -1;
            }
            else
            {
                flag[k] = 0;
                flag[k + 16] = 0;
            }

        }
        int temp = 0;
        int count = 0;
        for (int k = 0; k < 31; k++)
        {
            temp = flag[k] * flag[k + 1];
            if (temp > 0)
            {
                count++;
            }
            else
            {
                count = 0;
            }
            if (count >= 8)
            {
                goto is_a_corner;
            }
        }
        goto is_not_a_corner;

        is_a_corner:
            bmin = b_test;
            goto end;

        is_not_a_corner:
            bmax = b_test;
            goto end;

        end:

            if (bmin == bmax - 1 || bmin == bmax)
                return bmin;
            b_test = (bmin + bmax) / 2;
    }
}

int agast_cornerScore_5_8_(const unsigned char* ptr, const int pixel[], int threshold)
{
    int bmin = threshold;
    int bmax = 255;
    int b_test = (bmax + bmin) / 2;


    register short offset[8] = { 0 };
    offset[0] = (short)pixel[0];
    offset[1] = (short)pixel[1];
    offset[2] = (short)pixel[2];
    offset[3] = (short)pixel[3];
    offset[4] = (short)pixel[4];
    offset[5] = (short)pixel[5];
    offset[6] = (short)pixel[6];
    offset[7] = (short)pixel[7];

    while (true)
    {
        register const int cb = *ptr + b_test;
        register const int c_b = *ptr - b_test;
        int flag[16] = { 0 };
        for (int k = 0; k < 8; k++)
        {
            if (ptr[offset[k]] > cb)
            {
                flag[k] = 1;
                flag[k + 8] = 1;

            }
            else if (ptr[offset[k]] < c_b)
            {
                flag[k] = -1;
                flag[k + 8] = -1;
            }
            else
            {
                flag[k] = 0;
                flag[k + 8] = 0;
            }

        }
        int temp = 0;
        int count = 0;
        for (int k = 0; k < 15; k++)
        {
            temp = flag[k] * flag[k + 1];
            if (temp > 0)
            {
                count++;
            }
            else
            {
                count = 0;
            }
            if (count >= 4)
            {
                goto is_a_corner;
            }
        }
        goto is_not_a_corner;

    is_a_corner:
        bmin = b_test;
        goto end;

    is_not_a_corner:
        bmax = b_test;
        goto end;

    end:

        if (bmin == bmax - 1 || bmin == bmax)
            return bmin;
        b_test = (bmin + bmax) / 2;
    }
}


void OAST_9_16_(unsigned char * img, int mWidth, int mHeight, std::vector<OneKeyPoint>& keypoints, int threshold)
{


        size_t total = 0;
        int xsize = mWidth;
        int ysize = mHeight;
        size_t nExpectedCorners = keypoints.capacity();
        register int x, y;
        register int xsizeB = xsize - 4;
        register int ysizeB = ysize - 3;
        register int width;

        keypoints.resize(0);

        int pixel_9_16_[16];
        makeAgastOffsets(pixel_9_16_, xsize, nOAST_9_16);
        register short offset[16] = { 0 };
        offset[0] = (short)pixel_9_16_[0];
        offset[1] = (short)pixel_9_16_[1];
        offset[2] = (short)pixel_9_16_[2];
        offset[3] = (short)pixel_9_16_[3];
        offset[4] = (short)pixel_9_16_[4];
        offset[5] = (short)pixel_9_16_[5];
        offset[6] = (short)pixel_9_16_[6];
        offset[7] = (short)pixel_9_16_[7];
        offset[8] = (short)pixel_9_16_[8];
        offset[9] = (short)pixel_9_16_[9];
        offset[10] = (short)pixel_9_16_[10];
        offset[11] = (short)pixel_9_16_[11];
        offset[12] = (short)pixel_9_16_[12];
        offset[13] = (short)pixel_9_16_[13];
        offset[14] = (short)pixel_9_16_[14];
        offset[15] = (short)pixel_9_16_[15];

        width = xsize;
        int flag[32] = { 0 };
        for (y = 3; y < ysizeB; y++)
        {
            for (x = 3; x < xsizeB + 1; x++)
            {
                register const unsigned char* const ptr = img + y*width + x;
                register const int cb = *ptr + threshold;
                register const int c_b = *ptr - threshold;
                for (int k = 0; k < 16; k++)
                {
                    if (ptr[offset[k]] > cb)
                    {
                        flag[k] = 1;
                        flag[k + 16] = 1;

                    }
                    else if (ptr[offset[k]] < c_b)
                    {
                        flag[k] = -1;
                        flag[k + 16] = -1;
                    }
                    else
                    {
                        flag[k] = 0;
                        flag[k + 16] = 0;
                    }

                }
                int temp = 0;
                int count = 0;
                for (int k = 0; k < 31; k++)
                {
                    temp = flag[k] * flag[k + 1];
                    if (temp > 0)
                    {
                        count++;
                    }
                    else
                    {
                        count = 0;
                    }
                    if (count >= 8)
                    {
                        OneKeyPoint tempKeyPoint;
                        tempKeyPoint.pt.x = (float)x;
                        tempKeyPoint.pt.y = (float)y;
                        tempKeyPoint.size = 1.0f;
                        tempKeyPoint.response = (float)agast_cornerScore_9_16_(ptr, pixel_9_16_, threshold);

                        keypoints.push_back(tempKeyPoint);
                        //printf("x = %d, y = %d\n", x, y);
                        break;
                    }
                }

            }

        }
}