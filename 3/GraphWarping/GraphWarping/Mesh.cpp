#include "Mesh.h"

void Mesh::loadRectMesh(QImage * img, int n){
	START();
	
	int xMax = img->width() - 1, yMax = img->height() - 1;
	int iNum = 2 + (xMax-1) / n, jNum = 2 + (yMax-1) / n;

	// get mesh point
	for(int x=0; x<xMax; x+=n){
		for(int y=0; y<yMax; y+=n)
			V.push_back(ND_Point(x, y));
		V.push_back(ND_Point(x, yMax));
	}
	for(int y=0; y<yMax; y+=n)
		V.push_back(ND_Point(xMax,y));
	V.push_back(ND_Point(xMax,yMax));
	
	// get mesh face
	for(int i=0; i<iNum-1; i++){
		for(int j=0; j<jNum-1; j++){
			Face face;
			face.push_back(i*jNum + j);
			face.push_back((i+1)*jNum + j);
			face.push_back((i+1)*jNum + j+1);
			face.push_back(i*jNum + j+1);
			faces.push_back(face);
		}
	}

	oldV = V;
	END("Time of loading rectangle mesh");
}

void Mesh::loadTriRandMesh(QImage * img, int n){
	START();

	V = getRandP(img->width(), img->height(), n);
	oldV = V;

	genTriMesh();

	END("Time of loading triangle random mesh");
}

void Mesh::genTriMesh(std::vector<ND_Point> & V, std::vector<std::vector<int>> & faces){
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

void Mesh::loadTriFeatureMesh(QImage * img){
	START();

	// 添加边界点
	V.clear();
	int step = 20;
	int width = img->width(), height = img->height();
	for (int i = step; i < width; i += step) {
		V.push_back(ND_Point(i, 0));
		V.push_back(ND_Point(i, height - 1));
	}
	for (int i = step; i < height; i += step) {
		V.push_back(ND_Point(0, i));
		V.push_back(ND_Point(width - 1, i));
	}
	V.push_back(ND_Point(0, 0));
	V.push_back(ND_Point(height-1, 0));
	V.push_back(ND_Point(height-1, width-1));
	V.push_back(ND_Point(0, width-1));

	// 添加特征点
	cv::Mat imgMat = TypeCvt::QImageToMat(*img);
	cv::SurfFeatureDetector surf;
	std::vector<cv::KeyPoint> keypoints;
	surf.detect(imgMat, keypoints);
	for(int i=0; i<keypoints.size(); i++)
		//detector 有 bug
		if(keypoints[i].pt.x > -1000 && keypoints[i].pt.y > -1000)
			V.push_back(ND_Point(keypoints[i].pt.x, keypoints[i].pt.y));
	
	oldV = V;

	genTriMesh(V, faces);

	END("Time of loading triangule feature mesh");
}

QImage * Mesh::warp(QImage * img, Interpolation & interpolation){
	START();

	QImage * newImg = interpolation.warp(img, V);
	for(int i=0; i<V.size(); i++)
		V[i] += oldV[i];

	for(int i=0; i<faces.size(); i++){
		Face & face = faces[i];
		std::vector<ND_Point> points = polygonScan(face2PVec(face), newImg->height());
		for(int j=0; j<points.size(); j++){
			ND_Point oldP = getOriginalP(points[j], face);
			if(newImg->rect().contains(points[j]) && img->rect().contains(oldP))
				newImg->setPixel(points[j], img->pixel(oldP));
		}
	}

	END("Time of mesh warping");

	return newImg;
}

ND_Point Mesh::getOriginalP(ND_Point & p, Face & face){
	if(face.size() == 3)
		return getOriginalP_Tri(p, face);
	else
		return getOriginalP_Rect(p, face);
}

ND_Point Mesh::getOriginalP_Rect(ND_Point & p, Face & face){
	float S2[4], cotAlpha[4], cotBeta[4], w[4], sumW = 0, lambda[4], x = 0, y = 0;
	bool isCorner = false;
	int a2[4], r2[4];

	for(int k=0; k<4; k++){
		a2[k] = (V[face[k]] - V[face[(k+1)&0x3]]).mod2();
		r2[k] = (p - V[face[k]]).mod2();
		if(r2[k]==0){
			isCorner = true;
			break;
		}
		S2[k] = pow(ND_Point::crossProduct(p-V[face[k]], V[face[(k+1)&0x3]] - V[face[k]])/ 2.0, 2);
	}
	
	if(isCorner)
		return ND_Point(-1,-1);
	
	for(int k=0; k<4; k++){
		cotAlpha[k] = S2[k]==0 ? sqrt((float)(a2[k]*r2[k])) * 999999.0 : sqrt(a2[k]*r2[k]/S2[k] - 1);
		cotBeta[k] = S2[(k+3)&0x3]==0 ? sqrt((float)(a2[(k+3)&0x3] * r2[k])) * 999999.0 : sqrt(a2[(k+3)&0x3]*r2[k]/S2[(k+3)&0x3]-1);
		w[k] = (cotAlpha[k] + cotBeta[k]) / r2[k];
		sumW += w[k];
	}

	for(int k=0; k<4; k++)
		lambda[k] = w[k] / sumW;

	for(int k=0; k<4; k++){
		x += oldV[face[k]].x * lambda[k];
		y += oldV[face[k]].y * lambda[k];
	}

	return ND_Point(x, y);
}

ND_Point Mesh::getOriginalP_Tri(ND_Point & p, Face & face){
	float lambda[3];
	float SofFace = ND_Point::SofTri(V[face[0]], V[face[1]], V[face[2]]);

	lambda[0] = ND_Point::SofTri(p, V[face[1]], V[face[2]]) / SofFace;
	lambda[1] = ND_Point::SofTri(V[face[0]], p, V[face[2]]) / SofFace;
	lambda[2] = ND_Point::SofTri(V[face[0]], V[face[1]], p) / SofFace;

	float x = 0, y = 0;
	for(int k=0; k<3; k++){
		x += oldV[face[k]].x * lambda[k];
		y += oldV[face[k]].y * lambda[k];
	}
	x = x-floor(x) > 0.5 ? ceil(x) : floor(x);
	y = y-floor(y) > 0.5 ? ceil(y) : floor(y);

	return ND_Point(x, y);
}

std::vector<ND_Point> Mesh::polygonScan(std::vector<ND_Point> & polygon, int windowHeight){
	std::vector<ND_Point> rst;

	//计算最高点的y坐标
	int maxY = polygon[0].y;
	int minY = polygon[0].y;
	for(int i=1; i<polygon.size(); i++){
		if(polygon[i].y > maxY)
			maxY = polygon[i].y;
		if(polygon[i].y < minY)
			minY = polygon[i].y;
	}
	int N = 1+maxY-minY;
	//初始化ET和AET
	Edge ** ET = (Edge **)malloc(N*sizeof(Edge*));
	for (int i=0; i<N; i++){
		ET[i] = new Edge();
		ET[i]->next = nullptr;
	}
	Edge * AET = new Edge();
	AET->next = nullptr;
	
	//建立边表ET
	for(int i=0; i<polygon.size(); i++){
		//取出当前点1前后相邻的共4个点，点1与点2的连线作为本次循环处理的边，另外两个点点0和点3用于计算奇点
		int x0 = polygon[(i - 1 + polygon.size()) % polygon.size()].x;
		int y0 = polygon[(i - 1 + polygon.size()) % polygon.size()].y;
		int x1 = polygon[i].x;
		int y1 = polygon[i].y;
		int x2 = polygon[(i + 1) % polygon.size()].x;
		int y2 = polygon[(i + 1) % polygon.size()].y;
		int x3 = polygon[(i + 2) % polygon.size()].x;
		int y3 = polygon[(i + 2) % polygon.size()].y;
		//水平线直接舍弃
		if(y1 == y2)
			continue;
		//分别计算下端点y坐标、上端点y坐标、下端点x坐标和斜率倒数
		int ymin = y1 > y2 ? y2 : y1;
		int ymax = y1 > y2 ? y1 : y2;
		float x = y1 > y2 ? x2 : x1;
		float dx = ((float)(x1 - x2)) / (y1 - y2);
		//奇点特殊处理，若点2->1->0的y坐标单调递减则y1为奇点，若点1->2->3的y坐标单调递减则y2为奇点
		if((y0<y1 && y1<y2) || (y1>y2 && y2>y3)){
			ymin++;
			x += dx;
		}
		//创建新边，插入边表ET
		int idx = ymin-minY;
		if(idx>=0 && idx<N){
			Edge * p = new Edge();
			p->ymax = ymax;
			p->x = x;
			p->dx = dx;
			p->next = ET[idx]->next;
			ET[idx]->next = p;
		}
	}
	
	//扫描线从下往上扫描，y坐标每次加1
	for(int i=0; i<N; i++){
		//取出ET中当前扫描行的所有边并按x的递增顺序（若x相等则按dx的递增顺序）插入AET
		while(ET[i]->next){
			//取出ET中当前扫描行表头位置的边
			Edge *pInsert = ET[i]->next;
			Edge *p = AET;
			//在AET中搜索合适的插入位置
			while(p->next){
				if(pInsert->x > p->next->x){
					p = p->next;
					continue;
				}
				if(pInsert->x == p->next->x && pInsert->dx > p->next->dx){
					p = p->next;
					continue;
				}
				//找到位置
				break;
			}
			//将pInsert从ET中删除，并插入AET的当前位置
			ET[i]->next = pInsert->next;
			pInsert->next = p->next;
			p->next = pInsert;
		}
		
		//AET中的边两两配对并填色
		Edge *p = AET;
		while(p->next && p->next->next){
			for(int x = p->next->x; x < p->next->next->x; x++)
				rst.push_back(ND_Point(x, i+minY));
			p = p->next->next;
		}
		
		//删除AET中满足y=ymax的边
		p = AET;
		while(p->next){
			if(p->next->ymax == i+minY){
				Edge * pDelete = p->next;
				p->next = pDelete->next;
				pDelete->next = nullptr;
				delete pDelete;
			}else
				p = p->next;
		}
		
		//更新AET中边的x值，进入下一循环
		p = AET;
		while(p->next){
			p->next->x += p->next->dx;
			p = p->next;
		}
	}
	
	//此处不一定有释放干净，比如多边形在窗口外
	for(int i=0; i<N; i++)
		delete ET[i];
	free(ET);
	
	return rst;
}

std::vector<ND_Point> Mesh::getRandP(int width, int height, int n){
	std::vector<ND_Point> rst;

	// 添加边界点
	int step = 20;
	for (int i = step; i < width; i += step) {
		rst.push_back(ND_Point(i, 0));
		rst.push_back(ND_Point(i, height - 1));
	}
	for (int i = step; i < height; i += step) {
		rst.push_back(ND_Point(0, i));
		rst.push_back(ND_Point(width - 1, i));
	}
	rst.push_back(ND_Point(0, 0));
	rst.push_back(ND_Point(height-1, 0));
	rst.push_back(ND_Point(height-1, width-1));
	rst.push_back(ND_Point(0, width-1));

	// 添加内部随机点
	time_t t;
	srand((unsigned) time(&t));
	for(int i=0; i<n; i++)
		rst.push_back(ND_Point(rand()%(width-2)+1, rand()%(height-2)+1));
	return rst;
}

void Mesh::genTriMesh(){
	struct triangulateio in, out;
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
	/*
	struct triangulateio in, out;

	// points
	in.numberofpoints = V.size();
	in.pointlist = (TRI_REAL *) malloc(in.numberofpoints * 2 * sizeof(TRI_REAL));
	for(int i = 0; i < V.size(); i++){
		in.pointlist[2*i] = V[i].x;
		in.pointlist[2*i+1] = V[i].y;
	}

	// segments
	in.numberofsegments = 4;
	in.segmentlist = (int *) malloc(in.numberofsegments * 2 * sizeof(int));
	in.segmentlist[0] = 0;
	in.segmentlist[1] = 1;
	in.segmentlist[2] = 1;
	in.segmentlist[3] = 2;
	in.segmentlist[4] = 2;
	in.segmentlist[5] = 3;
	in.segmentlist[6] = 3;
	in.segmentlist[7] = 0;
	
	in.numberofpointattributes = 0;
	in.pointattributelist = (TRI_REAL *) NULL;
	in.pointmarkerlist = (int *) NULL;
	in.numberofholes = 0;
	in.numberofregions = 0;
	in.regionlist = (TRI_REAL *) NULL;
	out.trianglearealist = (TRI_REAL *) NULL;
    out.pointlist = (TRI_REAL *) NULL;
	out.pointattributelist = (TRI_REAL *) NULL;
	out.pointmarkerlist = (int *) NULL;
	out.trianglelist = (int *) NULL;
	out.triangleattributelist = (TRI_REAL *) NULL;
	out.neighborlist = (int *) NULL;
	out.segmentlist = (int *) NULL;
	out.segmentmarkerlist = (int *) NULL;
	out.edgelist = (int *) NULL;
	out.edgemarkerlist = (int *) NULL;

	triangulate("pqczeQ", &in, &out, (struct triangulateio *)NULL);
	
	// add new points
	for(int i = V.size(); i < out.numberofpoints; i++)
		V.push_back(ND_Point(out.pointlist[i*2], out.pointlist[i*2+1]));
	oldV = V;
	*/
	// faces
	for(int i = 0; i< out.numberoftriangles; i++){
		Face face;
		for(int j = 0; j < out.numberofcorners; j++)
			face.push_back(out.trianglelist[i * out.numberofcorners + j]);
		faces.push_back(face);
	}

	free(in.pointlist);
	free(in.pointattributelist);
	free(in.pointmarkerlist);
	//free(in.regionlist);
	free(out.pointlist);
	free(out.pointattributelist);
	free(out.pointmarkerlist);
	free(out.trianglelist);
	free(out.triangleattributelist);
	//free(out.trianglearealist);
	free(out.neighborlist);
	free(out.segmentlist);
	free(out.segmentmarkerlist);
	free(out.edgelist);
	free(out.edgemarkerlist);
}

std::vector<ND_Point> Mesh::face2PVec(Face & face){
	std::vector<ND_Point> rst;
	for(int i=0; i<face.size(); i++)
		rst.push_back(V[face[i]]);
	return rst;
}

/*
std::vector<ND_Point> Mesh::getPInPolygon(std::vector<ND_Point> & polygon){
	if(polygon.size()==0)
		return std::vector<ND_Point>();

	std::vector<ND_Point> rst;
	int xMax, xMin, yMax, yMin;
	xMax = xMin = polygon[0].x;
	yMax = yMin = polygon[0].y;
	
	QPolygon qPolygon;
	for(int i=0; i<polygon.size(); i++){
		qPolygon.putPoints(i, 1, polygon[i].x, polygon[i].y);
		if(polygon[i].x > xMax) xMax = polygon[i].x;
		if(polygon[i].x < xMin) xMin = polygon[i].x;
		if(polygon[i].y > yMax) yMax = polygon[i].y;
		if(polygon[i].y < yMin) yMin = polygon[i].y;
	}

	for(int x=xMin; x<=xMax; x++){
		for(int y=yMin; y<=yMax; y++){
			if(qPolygon.containsPoint(QPoint(x,y), Qt::OddEvenFill))
				rst.push_back(ND_Point(x,y));
		}
	}

	return rst;
}*/