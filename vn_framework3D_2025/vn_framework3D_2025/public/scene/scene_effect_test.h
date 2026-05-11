#pragma once



class SceneEffectTest : public vnScene
{
private:
	// ---  デバッグ描画用 ---
	vnEmitter* pEmitter;

	XMVECTOR point;

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
