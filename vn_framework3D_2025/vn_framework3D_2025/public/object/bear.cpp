//bear.cpp
#include"../../framework.h"
#include"../../framework/vn_environment.h"
#include"bear.h"

extern void createBearMotion();	//extern他の場所にあるからそれを探してくる
extern void deleteBearMotion();	//extern他の場所にあるからそれを探してくる


cBear::cBear() 
{
	createBearMotion();
	size = XMVectorSet(1.5f, 3.0f, 1.5f, 0.0f);
	center = XMVectorSet(0.0f, 1.3f, 0.0f, 0.0f);

	pParts[eParts::Body] = new vnModel(L"data/model/bear_part/", L"body.vnm");
	pParts[eParts::ArmL] = new vnModel(L"data/model/bear_part/", L"arm_L.vnm");
	pParts[eParts::ArmR] = new vnModel(L"data/model/bear_part/", L"arm_R.vnm");
	pParts[eParts::Head] = new vnModel(L"data/model/bear_part/", L"head.vnm");
	pParts[eParts::LegL] = new vnModel(L"data/model/bear_part/", L"leg_L.vnm");
	pParts[eParts::LegR] = new vnModel(L"data/model/bear_part/", L"leg_R.vnm");

	//pParts[eParts::Body]->setParent();
	pParts[eParts::Body]->setParent(this);

	pParts[eParts::Body]->setPosition(0.0f, 0.7f, 0.0f);
	pParts[eParts::Body]->setRotation(0.0f, 0.0f, 0.0f);
	pParts[eParts::Body]->setScale(1.0f, 1.0f, 1.0f);


	pParts[eParts::ArmL]->setParent(pParts[eParts::Body]);
	pParts[eParts::ArmL]->setPosition(0.2f, 0.75f, 0.05f);
	pParts[eParts::ArmL]->setRotation(0.0f, 0.0f, 0.45f);
	pParts[eParts::ArmL]->setScale(1.0f, 1.0f, 1.0f);

	pParts[eParts::ArmR]->setParent(pParts[eParts::Body]);
	pParts[eParts::ArmR]->setPosition(-0.2f, 0.75f, 0.025f);
	pParts[eParts::ArmR]->setRotation(0.0f, 0.0f, 5.82f);
	pParts[eParts::ArmR]->setScale(1.0f, 1.0f, 1.0f);


	pParts[eParts::Head]->setParent(pParts[eParts::Body]);
	pParts[eParts::Head]->setPosition(0.0f, 0.8f, 0.0f);
	pParts[eParts::Head]->setRotation(0.0f, 0.0f, 0.0f);
	pParts[eParts::Head]->setScale(1.0f, 1.0f, 1.0f);


	pParts[eParts::LegL]->setParent(pParts[eParts::Body]);
	pParts[eParts::LegL]->setPosition(0.2f, 0.0f, 0.1f);
	pParts[eParts::LegL]->setRotation(0.0f, 0.0f, 0.0f);
	pParts[eParts::LegL]->setScale(1.0f, 1.0f, 1.0f);

	pParts[eParts::LegR]->setParent(pParts[eParts::Body]);
	pParts[eParts::LegR]->setPosition(-0.2f, 0.0f, 0.1f);
	pParts[eParts::LegR]->setRotation(0.0f, 0.0f, 0.0f);
	pParts[eParts::LegR]->setScale(1.0f, 1.0f, 1.0f);

	time = 0;
	pMotion = nullptr;
	
}

void cBear::execute()
{
	if (pMotion == NULL)return;

	//時間
	time += 1.0f;
	//if (time >= keyPx[keyNum-1].Time) {
	//	time = 0.0f;
	//}
	if (time >= pMotion->Length) {
		time = 0.0f;
	}

	for (int i = 0; i < pMotion->ChannelNum; i++) {	//iはchannel
		float v = 0.0f;
		for (int j = 1; j < pMotion->pChannel[i].keyframeNum; j++) {	//keyframe
			if (pMotion->pChannel[i].pKey[j].time > time) {
				//
				int prev = j - 1;	//現時刻を挟む前のキーフレーム番号
				int next = j;		//現時刻を挟む次のキーフレーム番号

				//時間の幅（割合の為の分母）
				float interval = pMotion->pChannel[i].pKey[next].time - pMotion->pChannel[i].pKey[prev].time;

				//幅の中での割合を計算
				float rate = (time - pMotion->pChannel[i].pKey[prev].time) / interval;
				//値の幅（差分）
				float d = pMotion->pChannel[i].pKey[next].value - pMotion->pChannel[i].pKey[prev].value;
				//値計算(線形補間　/ Linear Inputpolation,Lerp)
				//線形補間の計算
				v = d * rate + pMotion->pChannel[i].pKey[prev].value;

				switch (pMotion->pChannel[i].channelID)
				{
				case stMotion::eChannel::PosX:pParts[pMotion->pChannel[i].partsID]->setPositionX(v); break;
				case stMotion::eChannel::PosY:pParts[pMotion->pChannel[i].partsID]->setPositionY(v); break;
				case stMotion::eChannel::PosZ:pParts[pMotion->pChannel[i].partsID]->setPositionZ(v); break;
				case stMotion::eChannel::RotX:pParts[pMotion->pChannel[i].partsID]->setRotationX(v); break;
				case stMotion::eChannel::RotY:pParts[pMotion->pChannel[i].partsID]->setRotationY(v); break;
				case stMotion::eChannel::RotZ:pParts[pMotion->pChannel[i].partsID]->setRotationZ(v); break;

				}

				break;
			}
			//vnFont::print(20, 20, L"%.3f/%.3f", time, pMotion->pChannel[i].pKey.time);

		}

		//if (v != 0) {
		//	switch (pMotion->pChannel[i].channelID)
		//	{
		//	case stMotion::eChannel::PosX:pParts[pMotion->pChannel[i].partsID]->setPositionX(v); break;
		//	case stMotion::eChannel::PosY:pParts[pMotion->pChannel[i].partsID]->setPositionX(v); break;
		//	case stMotion::eChannel::PosZ:pParts[pMotion->pChannel[i].partsID]->setPositionX(v); break;
		//	case stMotion::eChannel::RotX:pParts[pMotion->pChannel[i].partsID]->setRotationX(v); break;
		//	case stMotion::eChannel::RotY:pParts[pMotion->pChannel[i].partsID]->setRotationY(v); break;
		//	case stMotion::eChannel::RotZ:pParts[pMotion->pChannel[i].partsID]->setRotationZ(v); break;

		//	}

		//}

	}

}

cBear::~cBear()
{
	deleteBearMotion();
}

void cBear::setMotion(stMotion* p) 
{
	if (pMotion == p)return;
	pMotion = p;
	time = 0.0f;
	bindPose();
	//if(p==nullptr)
	//pParts[eParts::Body]->setParent();


}

vnModel* cBear::getParts(int i) {
	if (i < 0 || i >= eParts::PartsMax) {
		return nullptr;

	}else{
		return pParts[i];

	}
}
void cBear::bindPose() {
	pParts[eParts::Body]->setPosition(0.0f, 0.7f, 0.0f);
	pParts[eParts::Body]->setRotation(0.0f, 0.0f, 0.0f);
	pParts[eParts::Body]->setScale(1.0f, 1.0f, 1.0f);

	pParts[eParts::ArmL]->setParent(pParts[eParts::Body]);
	pParts[eParts::ArmL]->setPosition(0.2f, 0.75f, 0.05f);
	pParts[eParts::ArmL]->setRotation(0.0f, 0.0f, 0.45f);
	pParts[eParts::ArmL]->setScale(1.0f, 1.0f, 1.0f);

	pParts[eParts::ArmR]->setParent(pParts[eParts::Body]);
	pParts[eParts::ArmR]->setPosition(-0.2f, 0.75f, 0.025f);
	pParts[eParts::ArmR]->setRotation(0.0f, 0.0f, 5.82f);
	pParts[eParts::ArmR]->setScale(1.0f, 1.0f, 1.0f);


	pParts[eParts::Head]->setParent(pParts[eParts::Body]);
	pParts[eParts::Head]->setPosition(0.0f, 0.8f, 0.0f);
	pParts[eParts::Head]->setRotation(0.0f, 0.0f, 0.0f);
	pParts[eParts::Head]->setScale(1.0f, 1.0f, 1.0f);


	pParts[eParts::LegL]->setParent(pParts[eParts::Body]);
	pParts[eParts::LegL]->setPosition(0.2f, 0.0f, 0.1f);
	pParts[eParts::LegL]->setRotation(0.0f, 0.0f, 0.0f);
	pParts[eParts::LegL]->setScale(1.0f, 1.0f, 1.0f);

	pParts[eParts::LegR]->setParent(pParts[eParts::Body]);
	pParts[eParts::LegR]->setPosition(-0.2f, 0.0f, 0.1f);
	pParts[eParts::LegR]->setRotation(0.0f, 0.0f, 0.0f);
	pParts[eParts::LegR]->setScale(1.0f, 1.0f, 1.0f);

}

