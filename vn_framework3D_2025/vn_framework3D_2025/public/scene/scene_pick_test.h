#pragma once



class ScenePickTest : public vnScene
{
private:
	// ---  デバッグ描画用 ---
	int		  PointNum;	//4
	XMVECTOR *Point;	//new XM[PointNum]
	XMVECTOR *PointS;	//new XM[PointNum]

	//直線の情報
	XMVECTOR LinePos;	//直線上の１点（始点、位置ベクトル）
	XMVECTOR LineDir;	//直線の方向（方向ベクトル、正規化済み）

	vnModel* pHit[2];	//衝突座標表示
	vnModel* pPoint;

	XMVECTOR SphereCenter;
	float SphereRadius;

	vnModel* pSphere;

	vnModel* pModel[25];

	//平面の情報（　＾ω＾）・・・( ,,`･ω･´)ﾝﾝﾝ？
	//平面の方程式 ax + by + cz = d
	XMVECTOR PlaneNormal;	//平面の法線（a,b,c）
	float PlaneDistance;	//平面と原点との距離(d)




	// -----------------------


	//極座標
	float radius;	//半径
	float theta;	//角度（平面角/経度）
	float phi;		//角度（仰角/緯度）φ（ファイ）
	float radiusRe;


	//操作の種類
	enum eOperation
	{
		Point1,
		Point2,
		Point3,
		Point4,
		Point5,
		RotateZ,
		ScaleX,
		ScaleY,
		ScaleZ,
		CamPositonX,	//カメラの部分等々｛
		CamPositonY,
		CamPositonZ,
		CamTargetX,
		CamTargetY,
		CamTargetZ,		//｝
		RADIUS,
		THERA,
		PHI,
		OperationMax,	//何個あるかわかりやすくするため
	};


	int	Cursor;

public:
	
	bool initialize();
	void terminate();
	
	//処理関数
	//処理関数
	void execute();


	void render();
};
