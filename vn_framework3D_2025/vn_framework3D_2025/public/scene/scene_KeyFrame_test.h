#pragma once

#include"../object/bear.h"

class SceneKeyFrameTest : public vnScene
{
private:
	// ---  デバッグ描画用 ---

	XMFLOAT3 pos;

	// -----------------------

	vnModel* pCube;
	float time;	//経過時間

	cBear* pBear;

	vnCharacter* pCharacter;

	//enum eParts {
	//	Body,
	//	ArmL,
	//	ArmR,
	//	Head,
	//	LegL,
	//	LegR,
	//	PartsMax,
	//};
	//vnModel* pParts[PartsMax];

	//極座標
	float radius;	//半径
	float theta;	//角度（平面角/経度）
	float phi;		//角度（仰角/緯度）φ（ファイ）
	float radiusRe;


	int	Cursor;
#if 0
	struct stKeyFrame
	{
		float Time;		//時間
		float Value;	//値

	};
	enum eChannel
	{
		PosX,
		PosY,
		PosZ,
		RotX,
		RotY,
		RotZ,
		SclX,
		SclY,
		SclZ,
		//Red,どこが変わるかを入れる
	};

	struct stChannel
	{
		int partsID;
		eChannel channel;
		int keyNum;
		stKeyFrame* key;

	};

	struct stMotion
	{
		float length;
		int channelINum;
		stChannel* channel;
	};
	stMotion motion;//増やしていく


	stChannel channel[2]; 
#endif
	//=
	//{
	//	{eChannel::PosX,		//channel
	//		{
	//		{  0.0f,0.0f},		//key[0]
	//		{ 60.0f,1.0f},		//key[1]
	//		{120.0f,5.0f},
	//		{180.0f,5.0f},
	//		{240.0f,0.0f},
	//		}
	//	},
	//	{eChannel::PosZ,
	//		{
	//		{  0.0f,0.0f},
	//		{ 60.0f,1.0f},
	//		{120.0f,5.0f},
	//		{180.0f,5.0f},
	//		{240.0f,0.0f},
	//		}
	//	}

	//};
	//stKeyFrame keyPx[5] =
//{
//	{0.0f,0.0f},
//	{60.0f,1.0f},	//[1]
//	{120.0f,5.0f},	//[2]
//	{180.0f,5.0f},	//[3]
//	{240.0f,0.0f},	//[4]
//
//};
//stKeyFrame keyPz[5] =
//{
//	{0.0f,0.0f},
//	{60.0f,0.0f},	//[1]
//	{120.0f,0.0f},	//[2]
//	{180.0f,5.0f},	//[3]
//	{240.0f,0.0f},	//[4]
//
//};



public:



	bool initialize();
	void terminate();

	//処理関数
	//処理関数
	void execute();


	void render();
};
