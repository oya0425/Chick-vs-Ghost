//vn_character.h
#pragma once

#include "../framework/vn_motionData .h"

vnMotionData* loadMotionFile(const WCHAR* path);
void deleteMotionFile(vnMotionData* p);

class vnCharacter : public vnObject
{
private:
	//パーツ数
	int PartsNum;

	//パーツごとのオブジェクトデータ
	vnObject** pParts;

	//ボーンデータ(パーツ名、バインドポーズ参照用)
	vnModel_BoneData* pBoneData;


	//再生中のモーションデータ
	vnMotionData* pMotion;

	//モーションの再生位置
	float Time;

	bool isDeadMotionEnd = false; // 死亡モーションが終わったか

public:
	XMVECTOR center;	//AABBの中心(vnModel::Positionからの相対値
	XMVECTOR size;		//AABBのサイズ

	vnCharacter(const WCHAR* folder, const WCHAR* file);
	~vnCharacter();

	void execute();
	void execute(float time = 1.0f,bool isDead =true, bool isBoxUnity=false);

	//バインドポーズに戻す
	void bindPose();

	//モーションの設定
	void setMotion(vnMotionData* p);

	int getPartsNum()const;
	vnObject* getParts(int id)const;
	vnObject* getParts(const char* name);

	void SetAllPartsDiffuse(FXMVECTOR color, float a);

	XMVECTOR GetAllPartsDiffuse()const;

	//void SetPartDiffuse(const char* name, float r, float g, float b, float a);
	void SetPartDiffuse(const char* name, FXMVECTOR color, float a);
};
