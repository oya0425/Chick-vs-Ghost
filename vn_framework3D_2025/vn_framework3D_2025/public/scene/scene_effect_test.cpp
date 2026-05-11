#include "../../framework.h"
#include "../../framework/vn_environment.h"

//初期化関数
bool SceneEffectTest::initialize()
{
	XMVECTOR point = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	radius = 10.0f;
	theta = -XM_PI/2;
	phi = XM_PI/6;
	vnEmitter::stEmitterDesc desc;
	swprintf_s(desc.Texture, L"data/image/cfxr aura rays.png");

	desc.LifeMin = 30.0f;
	desc.LifeMax = 60.0f;
	desc.ColorMin = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	desc.ColorMax = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	desc.ColorMax =	V_GAME_COLOR_BLUE;

	desc.SizeMin = 30.0f;
	desc.SizeMax = 60.0f;
	desc.SpeedMin = 0.1f;
	desc.SpeedMax = 0.2f;

	pEmitter = new vnEmitter(&desc);
	registerObject(pEmitter);
	return true;
}

//終了関数
void SceneEffectTest::terminate()
{
	//オブジェクトの削除
	deleteObject(pEmitter);
}

void SceneEffectTest::execute() {

	//エミッター
	//パーティクルを放出させるオブジェクト
	//放出させる設定を持つ（数）
	//※エミッター自体は見えない
	
	//パーティクル（Particle）
	//エミッターから放出させる粒
	//基本にはビルボードポリゴンとして描画される
	//不定の数を大量に扱う


	Common::UpdateCameraByMouse(theta, phi, radius, &point);


		float moveValue = 0.1f;

		switch (Cursor)
		{
			// --- ここからPoint操作 ---
		case Point1: // Point[0]

			break;


		}




		int line = 0;
		vnFont::print(80.0f, (float)(100 + Cursor * 16), L"→");
		// 選択中の頂点やカメラ操作対象を表示
		//vnFont::print(100.0f, 150.0f + (float)(line++ * 16), L"Phi: %.3f", phi);


		vnDebugDraw::Grid();
		vnDebugDraw::Axis();

		vnScene::execute();
	
}

//描画関数
void SceneEffectTest::render()
{


	vnScene::render();
}
