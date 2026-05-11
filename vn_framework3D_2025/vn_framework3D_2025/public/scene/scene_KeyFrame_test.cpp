#include "../../framework.h"
#include "../../framework/vn_environment.h"

//extern stMotion motion_idle;
//extern stMotion motion_walk;

vnMotionData* motion_idle;
vnMotionData* motion_walk;
vnMotionData* motion_idle_bird;

vnMotionData* motion_BoxUnity_RunF;


//初期化関数
bool SceneKeyFrameTest::initialize()
{
	//pParts[eParts::Body] = new vnModel(L"data/model/bear_part/", L"body.vnm");
	//pParts[eParts::ArmL] = new vnModel(L"data/model/bear_part/", L"arm_L.vnm");
	//pParts[eParts::ArmR] = new vnModel(L"data/model/bear_part/", L"arm_R.vnm");
	//pParts[eParts::Head] = new vnModel(L"data/model/bear_part/", L"head.vnm");
	//pParts[eParts::LegL] = new vnModel(L"data/model/bear_part/", L"leg_L.vnm");
	//pParts[eParts::LegR] = new vnModel(L"data/model/bear_part/", L"leg_R.vnm");
	//registerObject(pParts[eParts::Body]);
	//registerObject(pParts[eParts::ArmL]);
	//registerObject(pParts[eParts::ArmR]);
	//registerObject(pParts[eParts::Head]);
	//registerObject(pParts[eParts::LegL]);
	//registerObject(pParts[eParts::LegR]);

	////pParts[eParts::Body]->setParent();
	//pParts[eParts::Body]->setPosition(0.0f, 0.7f, 0.0f);
	//pParts[eParts::Body]->setRotation(0.0f, 0.0f, 0.0f);
	//pParts[eParts::Body]->setScale(1.0f, 1.0f, 1.0f);

	//pParts[eParts::ArmL]->setParent(pParts[eParts::Body]);
	//pParts[eParts::ArmL]->setPosition(0.2f, 0.75f, 0.05f);
	//pParts[eParts::ArmL]->setRotation(0.0f, 0.0f, 0.45f);
	//pParts[eParts::ArmL]->setScale(1.0f, 1.0f, 1.0f);

	//pParts[eParts::ArmR]->setParent(pParts[eParts::Body]);
	//pParts[eParts::ArmR]->setPosition(-0.2f, 0.75f, 0.025f);
	//pParts[eParts::ArmR]->setRotation(0.0f, 0.0f, 5.82f);
	//pParts[eParts::ArmR]->setScale(1.0f, 1.0f, 1.0f);

	//	
	//pParts[eParts::Head]->setParent(pParts[eParts::Body]);
	//pParts[eParts::Head]->setPosition(0.0f, 0.8f, 0.0f);
	//pParts[eParts::Head]->setRotation(0.0f, 0.0f, 0.0f);
	//pParts[eParts::Head]->setScale(1.0f, 1.0f, 1.0f);


	//pParts[eParts::LegL]->setParent(pParts[eParts::Body]);
	//pParts[eParts::LegL]->setPosition(0.2f, 0.0f, 0.1f);
	//pParts[eParts::LegL]->setRotation(0.0f, 0.0f, 0.0f);
	//pParts[eParts::LegL]->setScale(1.0f, 1.0f, 1.0f);

	//pParts[eParts::LegR]->setParent(pParts[eParts::Body]);
	//pParts[eParts::LegR]->setPosition(-0.2f, 0.0f, 0.1f);
	//pParts[eParts::LegR]->setRotation(0.0f, 0.0f, 0.0f);
	//pParts[eParts::LegR]->setScale(1.0f, 1.0f, 1.0f);

	pCube = new vnModel(L"data/model/primitive/", L"cube.vnm");

	//pCharacter = new vnCharacter(L"", L"");

	registerObject(pCube);
	pCube->setRenderEnable(false);
	radius = 15.0f;
	theta = -0.75;
	phi = 0.5f;

	Cursor = 0;
#if 0
	//モーションの初期化
	motion.length = 60.0f;
	motion.channelINum = 2;
	motion.channel = new stChannel[motion.channelINum];

	//キーフレームの初期化
	motion.channel[0].partsID = eParts::Head;
	motion.channel[0].channel = eChannel::PosZ;
	motion.channel[0].keyNum = 3;
	motion.channel[0].key = new stKeyFrame[motion.channel[0].keyNum];

	motion.channel[0].key[0].Time = 0.0f;
	motion.channel[0].key[0].Value = 0.8f;
	
	motion.channel[0].key[1].Time = 30.0f;
	motion.channel[0].key[1].Value = 1.8f;
	
	motion.channel[0].key[2].Time = 60.0f;
	motion.channel[0].key[2].Value = 0.8f;


	//キーフレームの初期化
	motion.channel[1].partsID = eParts::Head;
	motion.channel[1].channel = eChannel::RotY;
	motion.channel[1].keyNum = 2;
	motion.channel[1].key = new stKeyFrame[motion.channel[1].keyNum];

	motion.channel[1].key[0].Time = 0.0f;
	motion.channel[1].key[0].Value = 0.0f;

	motion.channel[1].key[1].Time = 60.0f;
	motion.channel[1].key[1].Value = XMConvertToRadians(360.0f);//*3.1415/180.0f

#endif

	//pBear = new cBear();
	//registerObject(pBear);	//execute()でモーション再生するので
	//for (int i = 0; i < cBear::eParts::PartsMax; i++) {
	//	registerObject(pBear->getParts(i));
	//}


	//pCharacter = new vnCharacter(L"data/model/bear_part/",L"bear_part.bone");
	pCharacter = new vnCharacter(L"data/model/Brid/brid_animation_new/", L"brid.bone");

	registerObject(pCharacter);
	for (int i = 0; i < pCharacter->getPartsNum(); i++) {
		registerObject(pCharacter->getParts(i));
	}

	motion_idle = loadMotionFile(L"data/model/bear_part/motion/motion_idle.mot");
	motion_walk = loadMotionFile(L"data/model/bear_part/motion/motion_walk.mot");
	motion_idle_bird = loadMotionFile(L"data/model/Brid/brid_new/motion/Brid_Idle1.mot");

	//registerObject(pBear);	//execute()でモーション再生するので
	//for (int i = 0; i < cBear::eParts::PartsMax; i++) {
	//	registerObject(pBear->getParts(i));
	//}


	
	return true;
}

//終了関数
void SceneKeyFrameTest::terminate()
{
	//オブジェクトの削除
	//delete pCharacter;

	for (int i = 0; i < pCharacter->getPartsNum();i++) {
	deleteObject(pCharacter->getParts(i));
	}
	deleteObject(pCharacter);

	//for (int i = 0; i < cBear::eParts::PartsMax;i++) {
	//	deleteObject(pBear->getParts(i));
	//}
	//deleteObject(pBear);	//こっちを先に消すとパーツも一緒に消えるのでメモリがおかしくなるので本体は後で消す

	deleteMotionFile(motion_idle);
	deleteMotionFile(motion_walk);
}



void SceneKeyFrameTest::execute() {

	pCharacter->getParts("KaraDown")->addRotationY(0.05f);

	const XMVECTOR zeroPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	Common::UpdateCameraByMouse(theta, phi, radius, &zeroPos);

	int keyNum = 5;//= sizeof(keyPx) / sizeof(keyPx[0]);
	/*
	time	0	  15	 30		  60	  |  60	   90	120
	rate  0.0	0.25	0.5		 1.0(100%)| 0.0   0.5	1.0
	value	0	0.25	0.5		   1	  |   1	  3.0	5.0

	<key>  [0]								 [1]		 [2]
	Time	0								 60			120
	Value	0								  1			  5
	

	
	*/
#if 0
	//時間
	time += 1.0f;
	//if (time >= keyPx[keyNum-1].Time) {
	//	time = 0.0f;
	//}
	if (time >=60) {
		time = 0.0f;
	}
	if (time >=motion.length) {
		time = 0.0f;
	}

	for (int i = 0; i < 2/*ここ変更*/; i++) {	//iはchannel
		float v = 0.0f;
		for (int j = 0; j < motion.channel[i].keyNum; j++) {	//keyframe
			if (motion.channel[i].key[j].Time > time) {
				//
				int prev = j - 1;	//現時刻を挟む前のキーフレーム番号
				int next = j;		//現時刻を挟む次のキーフレーム番号

				//時間の幅（割合の為の分母）
				float interval = motion.channel[i].key[next].Time - motion.channel[i].key[prev].Time;

				//幅の中での割合を計算
				float rate = (time - motion.channel[i].key[prev].Time) / interval;
				//値の幅（差分）
				float d = motion.channel[i].key[next].Value - motion.channel[i].key[prev].Value;
				//値計算(線形補間　/ Linear Inputpolation,Lerp)
				//線形補間の計算
				v = d * rate + motion.channel[i].key[prev].Value;

				break;
			}

		}	
		switch (channel[i].channel)
		{
		case eChannel::PosX:pParts[motion.channel[i].partsID]->setPositionX(v); break;
		case eChannel::PosY:pParts[motion.channel[i].partsID]->setPositionX(v); break;
		case eChannel::PosZ:pParts[motion.channel[i].partsID]->setPositionX(v); break;
		case eChannel::RotX:pParts[motion.channel[i].partsID]->setRotationX(v); break;
		case eChannel::RotY:pParts[motion.channel[i].partsID]->setRotationY(v); break;
		case eChannel::RotZ:pParts[motion.channel[i].partsID]->setRotationZ(v); break;
		case eChannel::SclX:pParts[motion.channel[i].partsID]->setScaleX(v); break;
		case eChannel::SclY:pParts[motion.channel[i].partsID]->setScaleY(v); break;
		case eChannel::SclZ:pParts[motion.channel[i].partsID]->setScaleZ(v); break;
			
		}

	}
	if (vnKeyboard::on(DIK_A)) {
		pParts[0]->addPositionX(-0.1f);
	}
	else if(vnKeyboard::on(DIK_D)) {
		pParts[0]->addPositionX(0.1f);
	}
#endif

	//float v = 0.0f;
	//for (int i = 1; i < keyNum; ++i) {

	//	if (keyPx[i].Time > time) {
	//		//
	//		int prev = i - 1;	//現時刻を挟む前のキーフレーム番号
	//		int next = i;		//現時刻を挟む次のキーフレーム番号

	//		//時間の幅（割合の為の分母）
	//		float interval = keyPx[next].Time - keyPx[prev].Time;			 

	//		//幅の中での割合を計算
	//		 float rate = (time - keyPx[prev].Time)/interval;
	//		//値の幅（差分）
	//		float d = keyPx[next].Value - keyPx[prev].Value;
	//		//値計算(線形補間　/ Linear Inputpolation,Lerp)
	//		//線形補間の計算
	//		v =d* rate + keyPx[prev].Value;

	//		break;
	//	}
	//}

	//float z = 0.0f;
	//for (int i = 1; i < keyNum; ++i) {

	//	if (keyPz[i].Time > time) {
	//		//
	//		int prev = i - 1;	//現時刻を挟む前のキーフレーム番号
	//		int next = i;		//現時刻を挟む次のキーフレーム番号

	//		//時間の幅（割合の為の分母）
	//		float interval = keyPz[next].Time - keyPz[prev].Time;

	//		//幅の中での割合を計算
	//		float rate = (time - keyPz[prev].Time) / interval;
	//		//値の幅（差分）
	//		float d = keyPz[next].Value - keyPz[prev].Value;
	//		//値計算(線形補間　/ Linear Inputpolation,Lerp)
	//		//線形補間の計算
	//		z = d * rate + keyPz[prev].Value;

	//		break;
	//	}
	//}


	//float rate = time / keyPx[1].Time;
	//float value = rate * keyPx[1].Value;

	//pParts[0]->setPositionZ(v);

	//pCube->setPositionX(v);
	//pCube->setRotationX(v*100);
	//pCube->setRotationZ(z*100);
	
	//pCube->setPositionZ(z);

		// --- モーション再生 ---
	if (vnKeyboard::trg(DIK_1)) {
		pCharacter->setMotion(NULL);
	}
	if (vnKeyboard::trg(DIK_2)) {
		//pBear->setMotion(&motion_idle);
		pCharacter->setMotion(motion_idle_bird);

	}
	if (vnKeyboard::trg(DIK_3)) {
		//pBear->setMotion(&motion_walk);
		pCharacter->setMotion(motion_walk);
	}


	vnFont::print(20.0f, 20.0f, L"%.3f", time);

		int line = 0;
		vnFont::print(80.0f, (float)(100 + Cursor * 16), L"→");

		// 選択中の頂点やカメラ操作対象を表示
		//vnFont::print(100.0f, 150.0f + (float)(line++ * 16), L"CamPositionX: %.3f", vnCamera::getPositionX());
		//vnFont::print(100.0f, 150.0f + (float)(line++ * 16), L"CamPositionY: %.3f", vnCamera::getPositionY());
		//vnFont::print(100.0f, 150.0f + (float)(line++ * 16), L"CamPositionZ: %.3f", vnCamera::getPositionZ());
		//vnFont::print(100.0f, 150.0f + (float)(line++ * 16), L"CamTargetX: %.3f", vnCamera::getTargetX());
		//vnFont::print(100.0f, 150.0f + (float)(line++ * 16), L"CamTargetY: %.3f", vnCamera::getTargetY());
		//vnFont::print(100.0f, 150.0f + (float)(line++ * 16), L"CamTargetZ: %.3f", vnCamera::getTargetZ());
		//vnFont::print(100.0f, 150.0f + (float)(line++ * 16), L"Radius: %.3f", radius);
		//vnFont::print(100.0f, 150.0f + (float)(line++ * 16), L"Theta: %.3f", theta);
		//vnFont::print(100.0f, 150.0f + (float)(line++ * 16), L"Phi: %.3f", phi);


		vnDebugDraw::Grid();
		vnDebugDraw::Axis();

		vnScene::execute();
	
}

//描画関数
void SceneKeyFrameTest::render()
{


	vnScene::render();
}
