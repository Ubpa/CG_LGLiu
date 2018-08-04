#ifndef IMG_H
#define IMG_H

#include "ND_Point.h"
#include <vector>

using namespace std;

class Img
{
public:
	Img():width(0), height(0){}

	Img(int _width, int _height){
		init(_width, _height);
	}

	bool contains(ND_Point & p){
		if(p.x >= 0 && p.x < width && p.y >= 0 && p.y < height)
			return true;
		else
			return false;
	}

	bool contains(int x, int y){
		return contains(ND_Point(x,y));
	}

	ND_Point & at(ND_Point & p){
		return at(p.x, p.y);;
	}

	ND_Point & at(int x, int y){
		return colors[x*width + y];
	}

	void init(int _width, int _height){
		width = _width;
		height = _height;
		colors.clear();
		colors.resize(width*height, ND_Point(0,0,0));
	}
	
	void genGray(Img & img){
		img.init(width, height);
		for(int i=0; i<colors.size(); i++)
			img.colors[i].x = (colors[i].x*299 + colors[i].y*587 + colors[i].z*114 + 500) / 1000;
	}

	int width;
	int height;
	vector<ND_Point> colors;
};

#endif // IMG_H