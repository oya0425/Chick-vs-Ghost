//vn
#include"../framework.h"
#include"../framework/vn_environment.h"

namespace
{
	const wchar_t* sCube = L"cube";

}

vnCharacter::vnCharacter(const WCHAR* folder, const WCHAR* file)
{
	size = XMVectorSet(1.5f, 3.0f, 1.5f, 0.0f);
	center = XMVectorSet(0.0f, 1.3f, 0.0f, 0.0f);


	FILE* fp = nullptr;

	// bone ファイルパス作成
	WCHAR bonePath[256];
	swprintf_s(bonePath,L"%s%s", folder, file);

	if (_wfopen_s(&fp, bonePath, L"rb") != 0)
	{
		assert(false);
		return;
	}

	// ファイルサイズ取得
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// パーツ数決定
	PartsNum = size / sizeof(vnModel_BoneData);

	pBoneData = new vnModel_BoneData[PartsNum];
	fread(pBoneData, sizeof(vnModel_BoneData), PartsNum, fp);

	fclose(fp);

	// パーツ配列確保（← 正しい位置）
	pParts = new vnObject * [PartsNum];

	// 階層構築
	for (int i = 0; i < PartsNum; i++)
	{
		WCHAR name[256];
		size_t ret;
		mbstowcs_s(&ret, name, 256,
			pBoneData[i].Name,
			strlen(pBoneData[i].Name));
		wcscat_s(name, L".vnm");

		WCHAR vnmPath[256];
		swprintf_s(vnmPath, 256, L"%s%s", folder, name);

		bool isSimple =
			(wcscmp(name, L"Cube.vnm") == 0 ||
				wcscmp(name, L"box.vnm") == 0);

		FILE* fpVnm = nullptr;
		if (!isSimple)
		{
			if (_wfopen_s(&fpVnm, vnmPath, L"rb") == 0)
			{
				fclose(fpVnm);
				pParts[i] = new vnModel(folder, name);
			}
			else
			{
				pParts[i] = new vnObject();
			}
		}
		else {
			//continue;
			pParts[i] = new vnObject();

		}
	

		//ファイル名として完成させる
		//vnmファイルがないパーツがある
		//pBoneData[i].Name+"vnm"
		//vnmファイルがないパーツがある
		//→有るか無いか調べる
		/*
		if (fopen())
		{	成功（ファイルがある）
		pParts[i]=new vnModel(L"data/model/bear_part/",name);
		}else {	失敗（ファイルはない）
			new vnObject();
		}
		//※親子関係
		if(pBoneData[i].ParentID==-1)
		{	//親はvnCharacter自体
			pParts[i]->setParent(this);
		}else
		{


		}
		
		*/
		//※親子関係
		if (pBoneData[i].ParentID == -1)
		{	//親はvnCharacter自体
			pParts[i]->setParent(this);
		}
		else
		{
			pParts[i]->setParent(pParts[pBoneData[i].ParentID]);

		}
		pParts[i]->setPosition(pBoneData[i].pos[0], pBoneData[i].pos[1], pBoneData[i].pos[2]);
		pParts[i]->setRotation(pBoneData[i].rot[0], pBoneData[i].rot[1], pBoneData[i].rot[2]);
		pParts[i]->setScale(pBoneData[i].scl[0], pBoneData[i].scl[1], pBoneData[i].scl[2]);
			//pParts[i] = new vnModel(L"data/model/bear_part/", name);
	}
	pMotion = NULL;
	Time = 0.0f;

}
vnCharacter::~vnCharacter() 
{
	delete[] pBoneData;
	delete[] pParts;

}

int vnCharacter::getPartsNum()const
{
	return PartsNum;
}

void vnCharacter::setMotion(vnMotionData *p)
{
	if (pMotion == p)return;
	pMotion = p;
	Time = 0.0f;
	bindPose();
}
void vnCharacter::bindPose() 
{
	for (int i = 0; i < PartsNum; i++) {
		pParts[i]->setPosition(pBoneData[i].pos[0], pBoneData[i].pos[1], pBoneData[i].pos[2]);
		pParts[i]->setRotation(pBoneData[i].rot[0], pBoneData[i].rot[1], pBoneData[i].rot[2]);
		pParts[i]->setScale(pBoneData[i].scl[0], pBoneData[i].scl[1], pBoneData[i].scl[2]);

	}
}
vnObject* vnCharacter::getParts(int id)const {
	if (id < 0 || id >= PartsNum) {
		return nullptr;

	}
	else {
		return pParts[id];

	}
}

vnObject* vnCharacter::getParts(const char* name)
{
	if (!name)return NULL;
	for (int i = 0; i < PartsNum; i++) {
		if (strcmp(name,pBoneData[i].Name)==0) {	//ポインタ同士の比較
			return pParts[i];
		}
	}
	return NULL;
}

vnMotionData* loadMotionFile(const WCHAR* path)
{
	vnMotionData* ret = NULL;
	FILE* fp = NULL;
	if (_wfopen_s(&fp, path, L"rb") != 0) {
		vnFont::output(L"cannot open file : % s\n", path);
		assert(false);
		return ret;
	}
	long size = 0;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	ret = (vnMotionData*)new BYTE[size];

	fread(ret, size, 1, fp);

	fclose(fp);


	return ret;

}
void vnCharacter::execute()
{
	execute(0.0f,true ,false);
}

void vnCharacter::execute(float time,bool isDead, bool isBoxUnity)//vnCharactorにisLoopとかをstructで持たせてplayerとかで設定してそれを参照する感じ
{
	if (pMotion == NULL)return;

	//時間の進行
	Time += 1.0f*time;
	if (Time >= pMotion->Length&&!isDead)
	{
		Time = 0.0f;
	}
	//BoxUnityがいいところで止まる（倒れるところ）
	else if(Time >= pMotion->Length &&isDead) {
		Time = pMotion->Length;
		return;
	}
	else if (Time >= pMotion->Length-90 && isDead&&isBoxUnity)
	{
		Time = pMotion->Length - 90;
	}
	//モーションの再生

	//データからチャンネルとキーフレームの先頭アドレスを取得
	vnMotionData_Channel* channel = reinterpret_cast<vnMotionData_Channel*>(reinterpret_cast<__int64>(pMotion) + pMotion->ChannelAccess);
	vnMotionData_KeyFrame* key = reinterpret_cast<vnMotionData_KeyFrame*>(reinterpret_cast<__int64>(pMotion) + pMotion->KeyFrameAccess);

	vnObject* pObj = this;	//再生対象オブジェクト

	for (int i = 0; i < pMotion->ChannelNum; i++)
	{
		if (channel[i].Name[0] == '\0')	//名前の設定がないチャンネルはオブジェクト自体のモーションとして扱う
		{
			pObj = this;
		}
		else
		{
			pObj = getParts(channel[i].Name);
		}
		if (!pObj)continue;

		//キーフレームの先頭アドレスと数を取得
		vnMotionData_KeyFrame* keys = key + channel[i].StartIndex;
		int n = channel[i].KeyFrameNum;

		//値を評価
		float v = 0.0f;
		bool hit = false;
		for (int k = 0; k < n; k++)
		{
			if (Time == keys[k].Time)
			{
				v = keys[k].Value;
				hit = true;
				break;
			}
			else if (Time < keys[k].Time)
			{
				float rate = (Time - keys[k - 1].Time) / (keys[k].Time - keys[k - 1].Time);
				float d = keys[k].Value - keys[k - 1].Value;
				v = d * rate + keys[k - 1].Value;
				hit = true;
				break;
			}
		}
		if (!hit)continue;	//キーフレームがTimeまで設定されていなかった場合の対策
		//評価された値を適用
		switch (channel[i].ChannelID)
		{
		case eMotionChannel::PosX:	pObj->setPositionX(v);	break;
		case eMotionChannel::PosY:	pObj->setPositionY(v);	break;
		case eMotionChannel::PosZ:	pObj->setPositionZ(v);	break;
		case eMotionChannel::RotX:	pObj->setRotationX(v);	break;
		case eMotionChannel::RotY:	pObj->setRotationY(v);	break;
		case eMotionChannel::RotZ:	pObj->setRotationZ(v);	break;
		case eMotionChannel::SclX:	pObj->setScaleX(v);		break;
		case eMotionChannel::SclY:	pObj->setScaleY(v);		break;
		case eMotionChannel::SclZ:	pObj->setScaleZ(v);		break;
		}
	}
}

void deleteMotionFile(vnMotionData* p)
{
	if (!p)return;
	delete[]p;

}

void vnCharacter::SetAllPartsDiffuse(FXMVECTOR color, float a) {
	// 色を変えるループの中などで
	for (int i = 0; i < getPartsNum(); i++) {
		vnObject* obj = getParts(i);
		if (obj == nullptr) continue;

		// 安全のために、ここで vnModel かどうかをチェックしたい
		// もし dynamic_cast が使える設定（RTTI有効）なら：
		vnModel* model = dynamic_cast<vnModel*>(obj);

		if (model) {
			// ここで落ちるなら pModelData が不正
			int num = model->getMaterialNum();
			model->setDiffuse(XMVectorGetX(color), XMVectorGetY(color), XMVectorGetZ(color), a);
		}
	}
}
//色を返す
XMVECTOR vnCharacter::GetAllPartsDiffuse()const
{
	for (int i = 0; i < getPartsNum(); i++)
	{
		vnObject* obj = getParts(i);
		
		if (!obj)continue;
		if (auto*model =dynamic_cast<vnModel*>(obj))
		{
			return *model->getDiffuse();
		}
	}
	return XMVectorZero();
}

//void vnCharacter::SetPartDiffuse(const char* name, float r, float g, float b, float a)
//{
//	//名前からパーツ取ってきて色を変更
//	vnObject* obj = getParts(name);
//	if (obj == nullptr)return;
//
//	vnModel* model = dynamic_cast<vnModel*>(obj);
//
//	if (model)
//	{
//		model->setDiffuse(r, g, b, a);
//	}
//
//}
void vnCharacter::SetPartDiffuse(const char* name, FXMVECTOR color, float a=1.0f)
{
	//名前からパーツ取ってきて色を変更
	vnObject* obj = getParts(name);
	if (obj == nullptr)return;

	vnModel* model = dynamic_cast<vnModel*>(obj);

	if (model)
	{
		model->setDiffuse(XMVectorGetX(color), XMVectorGetY(color), XMVectorGetZ(color), a);
	}

}