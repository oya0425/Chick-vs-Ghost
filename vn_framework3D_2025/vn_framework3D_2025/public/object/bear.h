//bear.h
#pragma once	//インクルードガード

//キーフレーム
struct stKeyframe
{
	float time;	//時間
	float value;//値
};

struct stMotionChannel
{
	int partsID;
	int channelID;
	int keyframeNum;
	stKeyframe* pKey;
	
};

struct stMotion
{
	enum eChannel {
		PosX,
		PosY,
		PosZ,
		RotX,
		RotY,
		RotZ,
		ChannelMax,
	};
	float Length;
	int ChannelNum;
	stMotionChannel* pChannel;
};

class cBear : public vnObject
{
public:

//#if 0
	enum eParts {
		Body,
		ArmL,
		ArmR,
		Head,
		LegL,
		LegR,
		PartsMax,
	};

//#endif
	cBear* pBear;


private:
	vnModel* pParts[eParts::PartsMax];
	float time;

	stMotion motion_idle;
	stMotion motion_walk;

	stMotion* pMotion;	//再生中のモーションデータ

public:
	XMVECTOR center;	//AABBの中心(vnModel::Positionからの相対値
	XMVECTOR size;		//AABBのサイズ

	cBear();
	~cBear();
	void execute();
	void setMotion(stMotion* p);

	void bindPose();	//バインドポーズ（標準の状態）に戻す(動きの初期化)

	vnModel* getParts(int i);

};