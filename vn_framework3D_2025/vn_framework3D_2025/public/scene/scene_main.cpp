#include "../../framework.h"
#include "../../framework/vn_environment.h"
#include<algorithm>
#include <DirectXMath.h>
#include <cmath>

#include"../RigidbodyComponent.h"

// --- BGM ---
#define FILE_PATH_MAX	(256)

WCHAR seFile_Main[][FILE_PATH_MAX] =
{
	L"data/sound/灼熱のユーロビート.wav",    //ゲームプレイ中のサウンド
	L"data/sound/maou_se_battle_gun02.wav",	 //敵を倒したときになるSE
	L"data/sound/システム決定音_8.wav",		 //Enter押したときになるSE
	L"data/sound/maou_bgm_orchestra26.wav",	 //ゲームオーバーになった時に鳴らすBGM
	L"data/sound/キラキラ効果音.wav",		 //WAVEクリア時に鳴らすSE
	L"data/sound/maou_bgm_acoustic40.wav",	 //ゲームクリアになった時に鳴らすBGM
};


//extern stMotion motion_idle;
//extern stMotion motion_walk;

extern vnMotionData* motion_idle;
extern vnMotionData* motion_walk;
extern vnMotionData* motion_idle_bird;
extern vnMotionData* motion_BoxUnity_RunF;



// --- メンバ変数 ---
namespace {
	constexpr float playerModelScale = 1.2f;

	constexpr float underRespawnPos=-5.0f;
	constexpr float enemyMax = 1;
	FXMVECTOR enemyGhostModelSize = XMVectorSet(2, 2, 2, 0);
	FXMVECTOR enemyGhostColSize = XMVectorSet(2, 2, 2, 0);
	
	constexpr float minSpawnRadius = 10.0f;
	constexpr float spawnHeight	   = 40.0f;
	
	constexpr int leaderCount = 30;	//リーダーを生成する幅（leaderCountごとにリーダーを作る）

	constexpr float defalutFenceRadius = 35.0f;

	// --- UI ---
	constexpr float uiHidePosX = -5000.0f;
	constexpr float uiHidePosY = -5000.0f;

	constexpr float freamImgX = 1050.0f;
	constexpr float freamImgY = 150.0f;
	constexpr float backGroundImgX = 1020.0f;
	constexpr float backGroundImgY = 120.0f;
	constexpr float mainImgX = 110.0f;
	constexpr float mainImgY = 110.0f;

	// --- HPバー ---
	constexpr float barLeftEdgeHp = 90.0f;
	constexpr float barWidthHp = 450.0f;
	constexpr float barBackWidthHp = 458.0f;
	// 中心座標 = 左端 + (幅の半分)
	constexpr float newBarPosXHp = barLeftEdgeHp + (barWidthHp * 0.5f);      // 310.0f
	constexpr float newBarBackPosXHp = barLeftEdgeHp + (barBackWidthHp * 0.5f); // 314.0f (枠を考慮)
	constexpr float heigtYHp = 40.0f;



	// --- Expバーの設計図 ---
	constexpr float barLeftEdgeExp = 90.0f;  // HPバーの左端と合わせる
	constexpr float heightYExp = 70.0f;     // HPバーの下に来るように調整
	constexpr float maxWExp = 300.0f;        // HPバーより少し短く（HPが450なら300くらい）
	constexpr float barThick = 12.0f;        // 少し細く（18.0fから12.0fへ）

	// =================================================================
	// --- スキルのクールタイムバー（UI配置の設計図） ---
	// 
	// [画面イメージ]
	//  X座標: 90px (barLeftEdge...) が全員の左端スタートライン
	//  
	//  (Y=40)  [====== HPバー (幅450) ======]
	//  (Y=70)  [==== Expバー (幅300) ====]
	//  (Y=200) [== 範囲攻撃スキルバー (幅200) ==]  ←★ここ
	//  (Y=240) [== 引き寄せスキルバー (幅200) ==]  ←★ここ
	// =================================================================

	// --- スキルのクールタイムバー ---
	// --- 1. 範囲攻撃スキル（Area Attack）用バー ---
	// バーの「左端」のX座標（HPやExpと縦一列に揃えるために 90.0f）
	constexpr float skillBarThick = 30.0f; // しっかり太めの 30px に変更！


	constexpr float barLeftEdgeAreaSkill = 55.0f;

	// バーの「縦の位置（高さ）」を表すY座標（Expバーよりさらに下）
	constexpr float heightYAreaSkill = 330.0f;
	
	// クールタイムが最大のとき（使える状態）のバーの「最大横幅」
	constexpr float maxWAreaSkill = 250.0f;


	// --- 2. 引き寄せスキル（Pull）用バー ---
	// バーの「左端」のX座標（他のバーと綺麗に揃えるために 90.0f）
	constexpr float barLeftEdgePullSkill = 55.0f;
	
	// バーの「縦の位置（高さ）」を表すY座標（範囲攻撃バーの40px下に配置）
	constexpr float heightYPullSkill = 410.0f;
	
	// クールタイムが最大のとき（使える状態）のバーの「最大横幅」
	constexpr float maxWPullSkill = 250.0f;


	// --- カメラ関係 ---
	constexpr float defualtRadius = 30.0f;
	constexpr float defualtTheta  = 92.688f;
	constexpr float defualtPhi	  = 0.7f;

	//経験値のマネージャーが画像の番号を持ってるからそれを見て画像を入れる
	const UpgradeUIResource upgradeUIResources[5] =
	{
		{
			L"data/image/IMG_E6067.png",
			L"data/image/BackGround.png",
			L"data/image/IMG_E6067.png"
		},

		{
			L"data/image/IMG_E6067.png",
			L"data/image/BackGround.png",
			L"data/image/IMG_E6067.png"
		},

		{
			L"data/image/IMG_E6067.png",
			L"data/image/BackGround.png",
			L"data/image/IMG_E6067.png"
		},

		{
			L"data/image/IMG_E6067.png",
			L"data/image/BackGround.png",
			L"data/image/IMG_E6067.png"
		},

		{
			L"data/image/IMG_E6067.png",
			L"data/image/BackGround.png",
			L"data/image/IMG_E6067.png"
		}


	};
}

void SceneMain::SetupEnemy(NewEnemyClass* enemy, const NewEnemyClass::EnemyData& data,bool isLeader)
{
	// 1. データテーブルからパスを取得してモデルを生成
	vnCharacter* model = new vnCharacter(data.folder, data.file);
	enemy->SetModel(model);
	registerObject(model);

	model->setRenderEnable(false);

	for (int i = 0; i < model->getPartsNum(); i++)
	{
		registerObject(model->getParts(i));
		model->getParts(i)->setRenderEnable(false);
	}

	// 2. スケールを設定（floatをXMVECTORに変換）
	//XMVECTOR scaleVec = XMVectorSet(data.scale, data.scale, data.scale, 1.0f);
	model->setScale(data.scale, data.scale, data.scale);

	// 3. 当たり判定を設定（XMFLOAT3をXMVECTORに変換してロード）
	XMVECTOR colSizeVec = XMLoadFloat3(&data.colSize);
	enemy->GetCollision().SetSize(colSizeVec);

	// 中心点は高さ(Y)の半分に設定
	float centerY = XMVectorGetY(colSizeVec) / 2.0f;
	enemy->GetCollision().SetCenter(XMVectorSet(0, centerY, 0, 0));
	
	if (isLeader)
	{
		EnemyPool::GetInstance().GetLatestGroupData().push_back(
			std::make_unique<NewEnemyClass::GroupData>());


		//今追加した群れ取得
		NewEnemyClass::GroupData* group =
			EnemyPool::GetInstance().GetLatestGroupData().back().get();

		// --- 群のデータ設定 ---
		// 一時的なデータを受け取る
		NewEnemyClass::GroupColorData colorData = enemy->GetRandomGroupData();

		group->id = colorData.id;
		group->color = colorData.color;

		// colorData.colorName は一時的な変数の中にあるので指してはいけない。
		// 代わりに、静的なパレット(g_LeaderColorPalette)から直接ポインタを持ってくる。
		// IDが1から始まるなら、インデックスは [id - 1]
		int paletteIndex = colorData.id - 1;
		group->colorName = NewEnemyClass::g_LeaderColorPalette[paletteIndex].colorName;

		enemy->SettingLeader(group);
		enemy->SetFenceRadius(FenceRadius);
	}
	else
	{
		enemy->SettingOther();
	}
}


//初期化関数
bool SceneMain::initialize()
{
	srand((unsigned int)time(nullptr));

	// --- 変数初期化 ---
	m_gameState = IdelPlay;

	radius = defualtRadius;
	theta = defualtTheta;
	phi = defualtPhi;

	m_radius = radius;
	m_theta = theta;
	m_phi = phi;

	isWaveClear = false;

	oldWaveCount = 1; // 最初はWave 1
	m_comboScale = 1.0f; // 1.0 が基準（100%の大きさ）
	m_killCounter = 0; // 敵を倒した累計（5でリセット）

	// --- 時間 ---
	totalClearTime = 0.0f;
	isTimerActive = false; // 最初のWAVE開始時にtrueにする

	backGroundBlackScale = 0.0f;
	isGameFinish = false;

	FenceRadius = defalutFenceRadius;
	//FenceRadius = 35.0f;


	// --- プレイヤー ---
	//pPlayerTest = new PlayerClass();
	//pPlayerTest->SetModel(new vnCharacter(L"data/model/Brid/brid_animation_new/", L"brid.bone"));
	//pPlayerTest->SetMeteorModel(new vnModel(L"data/model/Brid/brid_animation_new/", L"KaraDown.vnm"));
	//pPlayerTest->SetUpKaraModel(new vnModel(L"data/model/Brid/", L"KaraUp.vnm"));


	////	//プレイヤー
	//registerObject(pPlayerTest->GetModel());
	//for (int i = 0; i < pPlayerTest->GetModel()->getPartsNum(); i++) {
	//	registerObject(pPlayerTest->GetModel()->getParts(i));
	//}

	//registerObject(pPlayerTest->GetMeteorModel());
	//registerObject(pPlayerTest->GetUpKaraModel());

	m_pNewPlayer = new NewPlayerClass();
	m_pNewPlayer->SetModel(new vnCharacter(L"data/model/Brid/brid_animation_new/", L"brid.bone"));
	m_pNewPlayer->SetMeteorModel(new vnModel(L"data/model/Brid/brid_animation_new/", L"KaraDown.vnm"));
	m_pNewPlayer->SetUpKaraModel(new vnModel(L"data/model/Brid/", L"KaraUp.vnm"));

	//	//プレイヤー
	registerObject(m_pNewPlayer->GetModel());
	for (int i = 0; i < m_pNewPlayer->GetModel()->getPartsNum(); i++) {
		registerObject(m_pNewPlayer->GetModel()->getParts(i));
	}
	m_pNewPlayer->GetModel()->setScale(playerModelScale, playerModelScale, playerModelScale);
	registerObject(m_pNewPlayer->GetMeteorModel());
	registerObject(m_pNewPlayer->GetUpKaraModel());
	
	m_pBullet = new Bullet();
	m_pBullet->SetModel(new vnCharacter(L"data/model/Brid/KaraUp/", L"KaraUp.bone"));
	m_pBullet->GetModel()->SetAllPartsDiffuse(V_GAME_COLOR_YELLOW, 1);
	//m_pBullet->SetModel(new vnCharacter(L"data/model/Brid/brid_animation_new/", L"brid.bone"));
	registerObject(m_pBullet->GetModel());
	for (int i = 0; i < m_pBullet->GetModel()->getPartsNum(); i++) {
		registerObject(m_pBullet->GetModel()->getParts(i));
	}
	//m_pBullet->GetModel()->setRenderEnable(false);
	//for (int i = 0; i < m_pBullet->GetModel()->getPartsNum(); i++) {
	//	m_pBullet->GetModel()->getParts(i)->setRenderEnable(false);
	//}
	m_pBullet->GetModel()->setPositionY(-10.0f);
	m_pBullet->GetModel()->setScale(2, 2, 2);

	m_pNewPlayer->SetBulletClass(m_pBullet);

	//enemyPool = new EnemyPool();
	enemyPool = &EnemyPool::GetInstance();

	//--1.敵の種類の数ループ
	int totalEnemyCount = 0;
	for (int i = 0; i < NewEnemyClass::MasterTableCount; i++)
	{
		//今の敵の種類のデータを取得（最大数とかファイルとか）
		const auto& data = NewEnemyClass::MasterTable[i];

		//--2.その種類の最大数（maxCount）だけループして生成
		for (int j = 0; j < data.maxCount; j++)
		{
			//型に合わせたクラスをnew する
			NewEnemyClass* enemy = NewEnemyClass::CreateEnemyByType(data.type);

			if (enemy)
			{
				//10体おきにリーダーにする
				bool isLeader = (totalEnemyCount % leaderCount == 0);
				//セットアップ関数を呼ぶ
				SetupEnemy(enemy, data,isLeader);

				//プールに追加
				enemyPool->AddEnemy(enemy);

				//生成した累計をインクリメント
				totalEnemyCount++;
			}
		}
	}


	enemyPool->UnlockEnemyType(NewEnemyClass::EnemyType::GHOST);
	enemyPool->UnlockEnemyType(NewEnemyClass::EnemyType::MUSHROOM);

	// --- 地形 ---
	pGround = new vnModel(L"data/model/Ground/", L"Ground.vnm");
	pGround->setScale(50.0f, 0.5f, 50);


	//pGround->setAmbient(0.2f, 0.2f, 0.2f, 0);
	//pGround->setDiffuse(0.4f, 0.4f, 0.4f, 0);
// 影の部分（Ambient）：少し青みのある深い緑にすると綺麗
	pGround->setAmbient(0.5f, 0.7f, 0.5f, 0);

	// 光の当たる部分（Diffuse）：ここがメインの緑！
	pGround->setDiffuse(0.5f, 0.5f, 0.5f, 0);

	pSky = new vnModel(L"data/model/", L"skydome.vnm");

	pSky->setLighting(false);
	pSky->setScale(5.0f, 5.0f, 5.0f);
	pSky->setZWrite(false);

	//フェンスの配置
	for (int i = 0; i < FENCE_NUM_MAIN; i++) {
		pFence[i] = new vnModel(L"data/model/", L"fence.vnm");

		//角度
		float degree = 360.0f/ FENCE_NUM_MAIN * (float)i;
		//Degree->Radian
		float radian = degree * 3.141592f / 180.0f;

		//極座標->直交座標
		float x = sin(radian) * FenceRadius;
		float z = cos(radian) * FenceRadius;
		float angle = (2.0f * 3.14159f / FENCE_NUM_MAIN) * i;

		float degrees = -angle +3.14159f;
		pFence[i]->setPosition(x, 0.0, z);
		//pFence[i]->setRotationY(degrees + 3.141592 / 2);


		pFence[i]->setRotationY(radian);

		registerObject(pFence[i]);
	}
	//地形
    //半透明のオブジェクトは不透明オブジェクトの後に描画
	registerObject(pGround);

	m_pBlockManager = new BlockManager();
	for (int i = 0; i < BlockManager::GetMaxBlocksNum(); i++)
	{
		TerrainBlock* pBlock = new TerrainBlock();

		pBlock->SetModel(new vnCharacter(L"data/model/Block/", L"Block.bone"));

		registerObject(pBlock->GetModel());

		for (int j = 0; j < pBlock->GetModel()->getPartsNum(); j++)
		{
			registerObject(pBlock->GetModel()->getParts(j));
		}

		pBlock->GetModel()->setRenderEnable(false);
		for (int i = 0; i < pBlock->GetModel()->getPartsNum(); i++)
		{
			pBlock->GetModel()->getParts(i)->setRenderEnable(false);
		}


		m_pBlockManager->AddBlock(pBlock);

	}


	registerObject(pSky);
	//registerObject(pEmitter);
	waveManager = new WaveManager();
	waveManager->Init();

 
	// --- エフェクト ---
	//エミッターの作成
    //設定構造体
	vnEmitter::stEmitterDesc desc;
	//swprintf_s(desc.Texture, L"%s", L"data/image/cfxr aura rays.png");
	swprintf_s(desc.Texture, L"%s", L"data/image/cfxr star_new.png");
	//swprintf_s(desc.Texture, L"%s", L"data/image/cfxr magic star.png");
	//desc.ColorMax = XMVectorSet(0.9f, 0.5f, 0.4f, 1.0f);
	desc.ColorMax = V_GAME_COLOR_YELLOW;
	desc.SizeMin = 1.0f;   // 最小サイズ
	desc.SizeMax = 3.0f;  // 最大サイズ（差を大きくする）
	desc.SpeedMin = 0.08f;  
	desc.SpeedMax = 0.1f;  
	pEmitter = new vnEmitter(&desc);
	pEmitter->setEmit(false,0);
	registerObject(pEmitter);
	pEmitter->setZWrite(false);

	//pEmitter->setParent(pPlayerTest->GetModel());

	// --- 土埃 ---
	swprintf_s(desc.Texture, L"data/image/cfxr cloud blur.png");
	desc.ColorMax = V_GAME_COLOR_DARK_GRAY;
	desc.SizeMin = 0.3f;   // 最小サイズ
	desc.SizeMax = 0.5f;  // 最大サイズ（差を大きくする）
	desc.SpeedMin = 0.08f;
	desc.SpeedMax = 0.1f;
	pDustEmitter = new vnEmitter(&desc);
	pDustEmitter->setEmit(false, 0);
	registerObject(pDustEmitter);
	pDustEmitter->setZWrite(false);
	pDustEmitter->setParent(m_pNewPlayer->GetModel());


	// --- HPバー ---
	// 背景バー	Hpバー　Exp
	// 背景（黒）: 高さ22
	pHpBarBackBlack = new vnSprite(newBarPosXHp, heigtYHp, 458.0f, 22.0f, NULL);
	pHpBarBackBlack->setColor(V_GAME_COLOR_BLACK);
	pHpBarBackBlack->setSkewX(12.0f);
	registerObject(pHpBarBackBlack);

	// 背景（赤・緑）: 高さ18
	// Y座標は同じ 30.0f にしておけば、高さの差で上下に 2px ずつ黒縁が出ます
	pHpBarBack = new vnSprite(newBarPosXHp, heigtYHp, 450.0f, 18.0f, NULL);
	pHpBarBack->setColor(V_GAME_COLOR_RED);
	pHpBarBack->setSkewX(9.8f);
	registerObject(pHpBarBack);

	pHpBarFront = new vnSprite(newBarPosXHp, heigtYHp, 450.0f, 18.0f, NULL);
	pHpBarFront->setColor(V_GAME_COLOR_GREEN);
	pHpBarFront->setSkewX(9.8f);
	registerObject(pHpBarFront);



	// --- Expバーの設計図 ---
	// 背景（黒枠）: 少しだけ大きく
	pExpBarBackBlack = new vnSprite(barLeftEdgeExp + (maxWExp * 0.5f), heightYExp, maxWExp + 10.0f, barThick + 4.0f, NULL);
	pExpBarBackBlack->setColor(V_GAME_COLOR_BLACK);
	pExpBarBackBlack->setSkewX(8.0f); // 斜め度合いも少し控えめにすると綺麗
	registerObject(pExpBarBackBlack);

	// 背景（白/グレー）
	pExpBarBack = new vnSprite(barLeftEdgeExp + (maxWExp * 0.5f), heightYExp, maxWExp, barThick, NULL);
	pExpBarBack->setColor(V_GAME_COLOR_WHITE);
	pExpBarBack->setSkewX(8.0f);
	registerObject(pExpBarBack);

	// 前景（シアン）: 最初は幅0でOK
	pExpBarFront = new vnSprite(barLeftEdgeExp, heightYExp, maxWExp, barThick, NULL);
	pExpBarFront->setColor(V_GAME_COLOR_CYAN);
	pExpBarFront->setSkewX(8.0f);
	registerObject(pExpBarFront);


	//-------------------------------------------------------------------------------
	//
	//	UI関連
	//
	//-------------------------------------------------------------------------------
	
	// --- アイコン ---
	pIconPlayer = new vnSprite(40, 40, 70, 70, L"data/image/Icon.png");
	registerObject(pIconPlayer);


	setHPbarRender(false);
	SetExpbarRender(false);

	// --- コンボ表示 ---
	for (int i = 0; i < 3; i++) {       // 桁 (0:百, 1:十, 2:一)
		for (int j = 0; j < 10; j++) {  // 数字 (0〜9)
			wchar_t path[64];
			swprintf(path, 64, L"data/image/num%d.png", j);

			pComboSprites[i][j] = new vnSprite(-1000, -1000, 96, 96, path);
			registerObject(pComboSprites[i][j]);
			pComboSprites[i][j]->setColor(V_GAME_COLOR_GOLD); // RGBで指定
		}
	}

	pComboWord = new vnSprite(-1000, -1000, 128, 96, L"data/image/combo.png");
	pComboWord->setColor(V_GAME_COLOR_RED);
	registerObject(pComboWord);

	// --- 文字を見やすくするための背景 ---
	m_pUIBackGroundBlack = new vnSprite(1280 -1120, 720.0f - 320.0f, 256 * 1.2f, 512 * 1.2f, L"BackGroundBlack.png");
	m_pUIBackGroundBlack->setColor(V_GAME_COLOR_BLACK);
	m_pUIBackGroundBlack->setAlpha(0.6f);
	registerObject(m_pUIBackGroundBlack);
	m_pUIBackGroundBlack->setRenderEnable(true);

	//ポーズ中に出る背景
	m_pUIBackGroundBlackPause = new vnSprite(1280 / 2, 720 / 2, 1280, 720, L"BackGroundBlack.png");
	m_pUIBackGroundBlackPause->setColor(V_GAME_COLOR_BLACK);
	m_pUIBackGroundBlackPause->setAlpha(0.6f);
	registerObject(m_pUIBackGroundBlackPause);
	m_pUIBackGroundBlackPause->setRenderEnable(false);



	// --- WASDのキーボードの画像 ---
	pImageW = new vnSprite(-100, 200, 64, 64, L"data/image/keyboard_w_outline.png");
	registerObject(pImageW);
	pImageA = new vnSprite(-100, 200, 64, 64, L"data/image/keyboard_a_outline.png");
	registerObject(pImageA);
	pImageS = new vnSprite(-100, 200, 64, 64, L"data/image/keyboard_s_outline.png");
	registerObject(pImageS);
	pImageD = new vnSprite(-100, 200, 64, 64, L"data/image/keyboard_d_outline.png");
	registerObject(pImageD);

	pImageE = new vnSprite(-100, 200, 64 * 0.7f, 64 * 0.7f, L"data/image/keyboard_e_outline.png");
	registerObject(pImageE);
	pImageQ = new vnSprite(-100, 200, 64 * 0.7f, 64 * 0.7f, L"data/image/keyboard_q_outline.png");
	registerObject(pImageQ);



	// --- スキルのボタン設定 ---
	// 範囲攻撃
	// --- 1. 背景黒枠（少し大きく：幅+10, 縦+6） ---
	// 中心座標を正しく計算 (90 + 100 = 190)
	float centerX = barLeftEdgeAreaSkill + (maxWAreaSkill * 0.5f);

	pAreaAtkBtnBackBlack = new vnSprite(centerX, heightYAreaSkill, maxWAreaSkill + 10.0f, skillBarThick + 6.0f, NULL);
	pAreaAtkBtnBackBlack->setColor(V_GAME_COLOR_BLACK);
	registerObject(pAreaAtkBtnBackBlack);


	// --- 2. バーの背景（中身の土台：グレーなど暗い色にするのがおすすめ） ---
	pAreaAtkBtnBack = new vnSprite(centerX, heightYAreaSkill, maxWAreaSkill, skillBarThick, NULL);
	// クールダウン中（スキルが溜まっていない状態）の土台なので、暗いグレーなどにすると見やすいです
	pAreaAtkBtnBack->setColor(V_GAME_COLOR_BLUEBLACK);
	registerObject(pAreaAtkBtnBack);


	// --- 3. 前景（実際に伸縮するシアンのバー） ---
	// ★ここも最初は「背景と全く同じ中心座標とサイズ」で生成します！
	pAreaAtkBtnFront = new vnSprite(centerX, heightYAreaSkill, maxWAreaSkill, skillBarThick, NULL);
	pAreaAtkBtnFront->setColor(V_GAME_COLOR_CYAN);
	registerObject(pAreaAtkBtnFront);

	// --- 4.アイコン --- 
	pAreaSkillIcon = new vnSprite(centerX - 130, heightYAreaSkill - 15, 64*1.2f, 64*1.2f, L"data/image/areaAttackIconImg2.png");
	pAreaSkillIcon->setColor(V_GAME_COLOR_WHITE);
	pAreaSkillIcon->setAlpha(1.0f);
	registerObject(pAreaSkillIcon);

	m_areaAtkUIColor.colorBackBlack = V_GAME_COLOR_BLACK;
	m_areaAtkUIColor.colorBack		= V_GAME_COLOR_BLUEBLACK;
	m_areaAtkUIColor.colorFront		= V_GAME_COLOR_CYAN;
	m_areaAtkUIColor.colorIcon		= V_GAME_COLOR_WHITE;


	

	// 引き寄せ攻撃
	pPullBtnBackBlack = new vnSprite(centerX, heightYPullSkill, maxWAreaSkill + 10.0f, skillBarThick + 6.0f, NULL);
	pPullBtnBackBlack->setColor(V_GAME_COLOR_BLACK);
	registerObject(pPullBtnBackBlack);


	// --- 2. バーの背景（中身の土台：グレーなど暗い色にするのがおすすめ） ---
	pPullBtnBack = new vnSprite(centerX, heightYPullSkill, maxWAreaSkill, skillBarThick, NULL);
	// クールダウン中（スキルが溜まっていない状態）の土台なので、暗いグレーなどにすると見やすいです
	pPullBtnBack->setColor(V_GAME_COLOR_BLUEBLACK);
	registerObject(pPullBtnBack);


	// --- 3. 前景（実際に伸縮するシアンのバー） ---
	// ★ここも最初は「背景と全く同じ中心座標とサイズ」で生成します！
	pPullBtnFront = new vnSprite(centerX, heightYPullSkill, maxWAreaSkill, skillBarThick, NULL);
	pPullBtnFront->setColor(V_GAME_COLOR_CYAN);
	registerObject(pPullBtnFront);

	// --- 4.アイコン --- 
	pPullSkillIcon = new vnSprite(centerX - 130, heightYPullSkill-15, 64 * 1.2f, 64 * 1.2f, L"data/image/pullAttackIconImg2.png");
	pPullSkillIcon->setColor(V_GAME_COLOR_WHITE);
	pPullSkillIcon->setAlpha(1.0f);
	registerObject(pPullSkillIcon);

	m_pullUIColor.colorBackBlack = V_GAME_COLOR_BLACK;
	m_pullUIColor.colorBack = V_GAME_COLOR_BLUEBLACK;
	m_pullUIColor.colorFront = V_GAME_COLOR_CYAN;
	m_pullUIColor.colorIcon = V_GAME_COLOR_WHITE;


	SetSkillUIRender(false);


	//-------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------





	// --- 黒い幕 ---
	pBackGroundBlack = new vnSprite(1280/2, 720/2, 1280, 720, L"BackGroundBlack.png");
	pBackGroundBlack->setColor(V_GAME_COLOR_BLACK);
	registerObject(pBackGroundBlack);
	pBackGroundBlack->setRenderEnable(false);
	pBackGroundBlack->setScale(0);

	// --- レベルアップシステム ---
	m_pExpManager = new ExperienceManager;
	m_pExpManager->SetPlayer(m_pNewPlayer);



	m_pUpgradeUI = new UpgradeSelectionUI;
	for (int i = 0; i < 5; i++)
	{
		const auto& resource = upgradeUIResources[i];

		m_pUpgradeUI->SetFreamImg(i,
			new vnSprite(
				uiHidePosX,
				uiHidePosY,
				freamImgX,
				freamImgY,
				resource.framePath));

		m_pUpgradeUI->SetBackGroundImg(i,
			new vnSprite(
				uiHidePosX,
				uiHidePosY,
				backGroundImgX,
				backGroundImgY,
				resource.backGroundPath));

		m_pUpgradeUI->SetMainImg(i,
			new vnSprite(
				uiHidePosX,
				uiHidePosY,
				mainImgX,
				mainImgY,
				resource.mainPath));

		registerObject(m_pUpgradeUI->GetFreamImg(i));
		registerObject(m_pUpgradeUI->GetBackGroundImg(i));
		registerObject(m_pUpgradeUI->GetMainImg(i));
	}

	//---フォント--------
	// 
	//使用できるフォントの数を取得

	FontNum = vnFont::getFontNum();

	//作成したフォント用の情報を確保しておく変数を必要な数作成
	textFormat= new IDWriteTextFormat * [FontNum];

	for (int i = 0; i < FontNum; i++)
	{
		//フォント名とサイズを指定してフォントを作成(フォント名は直接指定することも可能)
		textFormat[i] = vnFont::create(vnFont::getFontName(i), 50);
	}

	// --- BGM ---
	fileNum = sizeof(seFile_Main) / (sizeof(WCHAR) * FILE_PATH_MAX);

	pSound = new vnSound * [fileNum];
	for (int i = 0; i < fileNum; i++)
	{
		pSound[i] = new vnSound(seFile_Main[i]);
	}
	// --- はじめからBGMを鳴らす ---
	pSound[0]->play(true);
	pSound[0]->setVolume(1.0f);



	return true;
}
//終了関数
void SceneMain::terminate()
{
	// GPUの完了待ち（これを最初に入れるのが最強の安全策）
	vnDirect3D::waitForGpu();
	//プレイヤー
	//if (pPlayerTest) {
	//	// 1. パーツなどの「中身」を先に掃除する（元のコードにあった処理）
	//	if (pPlayerTest->GetModel()) {
	//		for (int i = 0; i < pPlayerTest->GetModel()->getPartsNum(); i++) {
	//			deleteObject(pPlayerTest->GetModel()->getParts(i));
	//		}
	//		deleteObject(pPlayerTest->GetModel());
	//	}
	//	deleteObject(pPlayerTest->GetMeteorModel());
	//	deleteObject(pPlayerTest->GetUpKaraModel());

	//	// 2. 本体を消す
	//	delete pPlayerTest;
	//	pPlayerTest = nullptr; // ★最強のNULL
	//}	

	if (m_pBullet) {
		// 1. パーツなどの「中身」を先に掃除する（元のコードにあった処理）
		if (m_pBullet->GetModel()) {
			for (int i = 0; i < m_pBullet->GetModel()->getPartsNum(); i++) {
				deleteObject(m_pBullet->GetModel()->getParts(i));
			}
			deleteObject(m_pBullet->GetModel());
		}

		// 2. 本体を消す
		delete m_pBullet;
		m_pBullet = nullptr;
	}

	if (m_pNewPlayer) {
		// 1. パーツなどの「中身」を先に掃除する（元のコードにあった処理）
		if (m_pNewPlayer->GetModel()) {
			for (int i = 0; i < m_pNewPlayer->GetModel()->getPartsNum(); i++) {
				deleteObject(m_pNewPlayer->GetModel()->getParts(i));
			}
			deleteObject(m_pNewPlayer->GetModel());
		}
		deleteObject(m_pNewPlayer->GetMeteorModel());
		deleteObject(m_pNewPlayer->GetUpKaraModel());

		// 2. 本体を消す
		delete m_pNewPlayer;
		m_pNewPlayer = nullptr; 
	}


	// --- 敵 (完全修正版) ---
	if (enemyPool) {
		auto& enemies = enemyPool->GetEnemies();
		for (auto enemy : enemies) {
			if (!enemy) continue; // 念のため空チェック

			// 1. 敵のモデルとパーツを先に削除
			if (enemy->GetModel()) {
				for (int i = 0; i < enemy->GetModel()->getPartsNum(); i++) {
					deleteObject(enemy->GetModel()->getParts(i));
				}
				deleteObject(enemy->GetModel());
			}

			// 2. 敵本体を削除
			delete enemy;
		}
		// 3. リストを空にしてゴミを掃除
		enemies.clear();

		// 4. プール本体を削除してNULLにする
		//delete enemyPool;
		enemyPool = nullptr;
	}

	// --- 背景・ギミック ---
	for (int i = 0; i < FENCE_NUM_MAIN; i++) {
		deleteObject(pFence[i]);
		pFence[i] = nullptr;
	}
	deleteObject(pGround); pGround = nullptr;
	deleteObject(pSky);    pSky = nullptr;

	// --- ブロック ---
	if (m_pBlockManager) {
		auto& blocks = m_pBlockManager->GetAllBlocks();
		for (auto block : blocks) {
			if (!block) continue; // 念のため空チェック

			// 1. ブロックのモデルとパーツを先に削除
			if (block->GetModel()) {
				for (int i = 0; i < block->GetModel()->getPartsNum(); i++) {
					deleteObject(block->GetModel()->getParts(i));
				}
				deleteObject(block->GetModel());
			}

			// 2. ブロック本体を削除
			delete block;
		}
		// 3. リストを空にしてゴミを掃除
		blocks.clear();

		// 4. プール本体を削除してNULLにする
		delete m_pBlockManager;
		m_pBlockManager = nullptr;
	}


	// --- その他 ---
	delete waveManager;
	waveManager = nullptr;

	deleteObject(pEmitter);
	pEmitter = nullptr;

	deleteObject(pDustEmitter);
	pDustEmitter = nullptr;

	deleteObject(pHpBarBackBlack);
	pHpBarBackBlack = nullptr;
	deleteObject(pHpBarBack);
	pHpBarBack = nullptr;
	deleteObject(pHpBarFront);
	pHpBarFront = nullptr;

	deleteObject(pIconPlayer);
	pIconPlayer = nullptr;

	//経験値バー
	deleteObject(pExpBarBackBlack);
	pExpBarBackBlack = nullptr;
	deleteObject(pExpBarBack);
	pExpBarBack = nullptr;
	deleteObject(pExpBarFront);
	pExpBarFront = nullptr;

	//範囲攻撃バー
	deleteObject(pAreaAtkBtnBackBlack);
	deleteObject(pAreaAtkBtnBack);
	deleteObject(pAreaAtkBtnFront);
	deleteObject(pAreaSkillIcon);

	//引き寄せ攻撃バー
	deleteObject(pPullBtnBackBlack);
	deleteObject(pPullBtnBack);
	deleteObject(pPullBtnFront);
	deleteObject(pPullSkillIcon);



	//for (int i = 0; i < 3; i++) {
	//	for (int j = 0; j < 10; j++) {
	//		if (pComboSprites[i][j]) {
	//			delete pComboSprites[i][j];
	//			pComboSprites[i][j] = nullptr;
	//		}
	//	}
	//}
	deleteObject(pComboWord);

	// --- WASDのキーボードの画像 ---
	deleteObject(pImageW);
	deleteObject(pImageA);
	deleteObject(pImageS);
	deleteObject(pImageD);
	deleteObject(pImageE);
	deleteObject(pImageQ);

	pImageW = nullptr;
	pImageA = nullptr;
	pImageS = nullptr;
	pImageD = nullptr;
	pImageE = nullptr;
	pImageQ = nullptr;

	deleteObject(m_pUIBackGroundBlack);
	m_pUIBackGroundBlack = nullptr;

	deleteObject(m_pUIBackGroundBlackPause);
	m_pUIBackGroundBlackPause = nullptr;


	deleteObject(pBackGroundBlack);
	pBackGroundBlack = nullptr;




	for (int i = 0; i < 3; i++)
	{
		deleteObject(m_pUpgradeUI->GetFreamImg(i));
		deleteObject(m_pUpgradeUI->GetBackGroundImg(i));
		deleteObject(m_pUpgradeUI->GetMainImg(i));
	}
	delete m_pUpgradeUI;
	m_pUpgradeUI = nullptr;

	// --- レベルアップシステム ---
	delete m_pExpManager;
	m_pExpManager = nullptr;

	// ...フォント処理...
	delete[] textFormat;
	textFormat = nullptr;

	if (pSound != NULL)
	{
		for (int i = 0; i < fileNum; i++)
		{
			if (pSound[i] == NULL)continue;
			delete pSound[i];
			pSound[i] = NULL;
		}
		delete[] pSound;
		pSound = NULL;
	}

}
//処理関数
void SceneMain::execute()
{
	if (!pSound[0]->isPlaying())
	{
		pSound[0]->play(true);

	}

	float dt = vnScene::getDeltaTime();
	switch (m_gameState)
	{
	case IdelPlay:
	{
		UpdateIdel();
		// 入力待ち
		break;
	}

	case Play:
	{
		UpdatePlay(dt);
		
		break;
	}

	case LevelUp:
		m_levelUpCameraTargetTheta =
			m_pNewPlayer->GetModel()->getRotationY()
			+ XM_PI
			+ XM_PIDIV2;

		UpdateLevelUp();
		Common::UpdateCameraLevelUp(
			m_pNewPlayer->GetModel()->getPosition(),
			m_levelUpCameraTargetTheta,
			dt,
			m_phi,
			m_radius,
			m_theta);
		break;

	case Pause:
		UpdatePause();
		break;

	case GameOver:
		UpdateGameOver();


		break;

	case GameClear:
		UpdateGameClear();
		
		break;
	case TimeStop:
		break;
	}

	//左側の表示
	if(m_gameState==GameState::Play)m_pUIBackGroundBlack->setRenderEnable(true);
	else m_pUIBackGroundBlack->setRenderEnable(false);

	
	if (m_gameState == GameState::Play)
	{
		pImageW->setRenderEnable(true);
		pImageA->setRenderEnable(true);
		pImageS->setRenderEnable(true);
		pImageD->setRenderEnable(true);
		pImageE->setRenderEnable(true);
		pImageQ->setRenderEnable(true);
		

	}
	else
	{
		pImageW->setRenderEnable(false);
		pImageA->setRenderEnable(false);
		pImageS->setRenderEnable(false);
		pImageD->setRenderEnable(false);
		pImageE->setRenderEnable(false);
		pImageQ->setRenderEnable(false);
		SetSkillUIRender(false);
	}

	// --- ポーズ中に黒い画面にする ---
	if (m_gameState == GameState::Pause)
	{
		m_pUIBackGroundBlackPause->setRenderEnable(true);
	}
	else
	{
		m_pUIBackGroundBlackPause->setRenderEnable(false);
	}

	// --- WAVEの状態の切り替え(WAVEクリア→次のWAVEとか) ---
	UpdateWaveTransition();

	//デバッグ表示
#if _DEBUG
	//XMVECTOR ropecenter = XMVectorAdd(*pPlayerTest->GetModel()->getPosition(), pPlayerTest->GetModel()->center);
	//vnDebugDraw::Box(ropecenter, pPlayerTest->GetModel()->size, GAME_COLOR_SILVER);
#endif
	//if (vnKeyboard::trg(DIK_R)) {
	//	m_gameState = GameClear;
	//}

	//if (vnKeyboard::trg(DIK_T)) {
	//	m_gameState = GameOver;
	//}

	// --- デバッグ用：現在のゲーム状態を表示 ---
	//const wchar_t* stateName = L"Unknown";
	//switch (m_gameState)
	//{
	//case IdelPlay:  stateName = L"IdelPlay (Waiting...)"; break;
	//case Play:      stateName = L"Play (Running)";       break;
	//case LevelUp:   stateName = L"LevelUp (Paused)";     break;
	//case GameOver:  stateName = L"GameOver";             break;
	//case GameClear: stateName = L"GameClear";            break;
	//}

	//// 画面の左上（または見やすい位置）に出す
	//vnFont::print(20, 400, L"Current State: %s", stateName);

	// --- ライト設定 ---

	// 環境光：全体の底上げ（入れすぎない）
	vnLight::setAmbient(0.35f, 0.35f, 0.35f);
	//vnLight::setAmbient(0, 0, 0);

	// キャラ個別の環境光：ほぼ補助レベル
	//pBrid->setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

	// 平行光源：少し斜め前上から当てる
	vnLight::setLightDir(0.3f, -1.0f, 0.3f);

	// 光の色：完全な白より少しだけ抑える
	//vnLight::setLightCol(0.95f, 0.95f, 0.95f);
	vnLight::setLightCol(0.5f, 0.5f, 0.5f);

	//vnDebugDraw::Grid();
	//vnDebugDraw::Axis();
	vnScene::execute();
}

//描画関数
void SceneMain::render()
{
	// --- 移動表示 ---
	// 基準 x = 20, y = 160
	int baseX = 50;
	int baseY = 130;

	// --- 画面表示 ---
	switch (m_gameState)
	{
	case IdelPlay:
	{
		// 影のズレ幅（ピクセル）
		float off = 3.0f;

		// --- 1. 下中央：スタート案内（点滅） ---
		blinkCounter++;
		float alpha = (sinf(blinkCounter * 0.1f) + 1.0f) * 0.5f;
		unsigned int blinkColor = ((unsigned int)(alpha * 255) << 24) | (0x00FFFFFF & GAME_COLOR_WHITE);
		unsigned int shadowAlpha = ((unsigned int)(alpha * 255) << 24) | 0x00000000; // 影も一緒に点滅

		//vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 60));		
		vnFont::setFontSize(38, 60);

		// 影を描画
		vnFont::print(340 + off, 600 + off, shadowAlpha, L"[RIGHT CLICK] TO START");
		// 本体を描画
		vnFont::print(340, 600, blinkColor, L"[RIGHT CLICK] TO START");

  
		// --- 2. 左上：操作説明 ---
		int opX = 50;
		int opY = 50;
		//vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 50));
		vnFont::setFontSize(38, 50);
		vnFont::print(opX + off, opY + off, 0xFF000000, L"【操作説明】"); // 影
		vnFont::print(opX, opY, GAME_COLOR_LIME, L"【操作説明】");

		//vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 30));
		vnFont::setFontSize(38, 30);

		// 各行に影を入れる
		auto printShadow = [&](float x, float y, unsigned int col, const wchar_t* txt) {
			vnFont::print(x + off, y + off, 0xFF000000, txt); // 影
			vnFont::print(x, y, col, txt); // 本体
			};

		printShadow(opX, opY + 70, GAME_COLOR_WHITE, L"移動      : W, A, S, D");
		printShadow(opX, opY + 120, GAME_COLOR_WHITE, L"カメラ    : MOUSE");
		printShadow(opX, opY + 170, GAME_COLOR_WHITE, L"ズーム    : WHEEL");


		// --- 3. 右側：ルール説明 ---
		int ruleX = 820; // 少し左に寄せました
		int ruleY = 30;
		//vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 50));
		vnFont::setFontSize(38, 50);

		printShadow(ruleX, ruleY, GAME_COLOR_YELLOW, L"【ルール】");

		//vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 30));
		vnFont::setFontSize(38, 30);

		printShadow(ruleX, ruleY + 60, GAME_COLOR_WHITE, L"・敵を倒してWaveを生き残れ");
		printShadow(ruleX, ruleY + 110, GAME_COLOR_WHITE, L"・コンボを繋ぐとHPが回復！");
		printShadow(ruleX, ruleY + 160, GAME_COLOR_WHITE, L"・Waveが進むほど敵は速く、");
		printShadow(ruleX, ruleY + 200, GAME_COLOR_WHITE, L"  リングは広くなるぞ");
		printShadow(ruleX, ruleY + 260, 0xFFFF0000, L"※HPが0になるとゲームオーバー");

		break;
	}
		break;
	case Play:
		// 通常HUD描画
		// 
		// --- 時間表示 ---
	{
		float off = 3.0f;
		unsigned int shadowCol = 0xFF000000; // 不透明な黒

		// --- タイム表示 ---
		int minutes = (int)(totalClearTime / 60);
		int seconds = (int)totalClearTime % 60;
		//vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 50));
		vnFont::setFontSize(38, 50);

		vnFont::print(950.0f + off, 20.0f + off, shadowCol, L"TIME: %02d:%02d", minutes, seconds); // 影
		vnFont::print(950.0f, 20.0f, GAME_COLOR_GREEN, L"TIME: %02d:%02d", minutes, seconds); // 本体


		// --- 現在のWAVE数 ---
		//vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 40));
		vnFont::setFontSize(38, 40);
		vnFont::print(950.0f + off, 70.0f + off, shadowCol, L"WAVE %d", waveManager->GetCurrentWave());
		vnFont::print(950.0f, 70.0f, GAME_COLOR_LIME, L"WAVE %d", waveManager->GetCurrentWave());

		// --- 撃破数関係 ---
		vnFont::print(950.0f + off, 110.0f + off, shadowCol, L"%d / %d 体撃破", waveManager->GetKillCount(), waveManager->GetKillTargetCount());
		vnFont::print(950.0f, 110.0f, GAME_COLOR_NEON_GREEN, L"%d / %d 体撃破", waveManager->GetKillCount(), waveManager->GetKillTargetCount());


		// --- 待機中のテキスト ---
		if (waveManager->IsWaitingForNext() && (waveManager->GetCurrentWave() < 5))
		{
			//vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 80));
			vnFont::setFontSize(38, 80);

			// メインのクリア表示 (影あり)
			vnFont::print(320.0f + off, 200.0f + off, shadowCol, L"WAVE %d CLEAR!!", waveManager->GetCurrentWave());
			vnFont::print(320.0f, 200.0f, GAME_COLOR_WHITE, L"WAVE %d CLEAR!!", waveManager->GetCurrentWave());

			//vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 40));
			vnFont::setFontSize(38, 40);

			// NEXT表示 (影あり)
			vnFont::print(300.0f + off, 320.0f + off, shadowCol, L"NEXT : フィールド拡大 ＆ 速度上昇！");
			vnFont::print(300.0f, 320.0f, GAME_COLOR_LIME, L"NEXT : フィールド拡大 ＆ 速度上昇！");

			// ノルマ表示 (影あり)
			vnFont::print(295.0f + off, 370.0f + off, shadowCol, L"ノルマ: %d 体 撃破", waveManager->GetNextKillTargetCount());
			vnFont::print(295.0f, 370.0f, GAME_COLOR_YELLOW, L"ノルマ: %d 体 撃破", waveManager->GetNextKillTargetCount());

			// --- 点滅処理 ---
			blinkCounter++;
			float alpha = (sinf(blinkCounter * 0.1f) + 1.0f) * 0.5f;

			// アルファ値を影の色にも合成（影も一緒にふわふわ消えるようにする）
			unsigned int blinkShadow = ((unsigned int)(alpha * 255) << 24) | 0x00000000;
			unsigned int blinkColor = ((unsigned int)(alpha * 255) << 24) | (GAME_COLOR_CYAN & 0x00FFFFFF);

			vnFont::print(320.0f + off, 450.0f + off, blinkShadow, L"[RIGHT CLICK] NEXT WAVE"); // 点滅する影
			vnFont::print(320.0f, 450.0f, blinkColor, L"[RIGHT CLICK] NEXT WAVE"); // 点滅する本体
		}
		{
			// --- 操作説明 ---
			bool aPressed = vnKeyboard::on(DIK_A);       // DIK_* はキーコード
			bool dPressed = vnKeyboard::on(DIK_D);
			bool wPressed = vnKeyboard::on(DIK_W);
			bool sPressed = vnKeyboard::on(DIK_S);
			bool ePressed = vnKeyboard::on(DIK_E);
			bool qPressed = vnKeyboard::on(DIK_Q);

			bool spacePressed = vnKeyboard::on(DIK_SPACE);
			bool mousePressed = vnMouse::onL(); // 0 = 左クリック


			// 5. フォント設定
			//vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 25));
			vnFont::setFontSize(38, 25);


			// 「移動：」は常に白
			vnFont::print(baseX, baseY - 20, GAME_COLOR_YELLOW, L"移動：");
			//// 「W」は押していたら黄色
			//vnFont::print(baseX + 35, baseY - 10, wPressed ? GAME_COLOR_GOLD : GAME_COLOR_WHITE, L"\nW");

			//// 「A」は押していたら黄色
			//vnFont::print(baseX + 15, baseY + 20, aPressed ? GAME_COLOR_GOLD : GAME_COLOR_WHITE, L"\nA");

			//// 「S」は押していたら黄色
			//vnFont::print(baseX + 35, baseY + 20, sPressed ? GAME_COLOR_GOLD : GAME_COLOR_WHITE, L"\nS");

			//// 「D」は押していたら黄色
			//vnFont::print(baseX + 55, baseY + 20, dPressed ? GAME_COLOR_GOLD : GAME_COLOR_WHITE, L"\nD");

			pImageW->setPos(baseX + 75, baseY + 35);
			pImageW->setColor(wPressed ? V_GAME_COLOR_GOLD : V_GAME_COLOR_WHITE);

			pImageA->setPos(baseX + 15, baseY + 100);
			pImageA->setColor(aPressed ? V_GAME_COLOR_GOLD : V_GAME_COLOR_WHITE);

			pImageS->setPos(baseX + 75, baseY + 100);
			pImageS->setColor(sPressed ? V_GAME_COLOR_GOLD : V_GAME_COLOR_WHITE);

			pImageD->setPos(baseX + 135, baseY + 100);
			pImageD->setColor(dPressed ? V_GAME_COLOR_GOLD : V_GAME_COLOR_WHITE);

			pImageE->setPos(baseX + 55.5f, baseY + 162.5f);
			pImageE->setColor(ePressed ? V_GAME_COLOR_GOLD : V_GAME_COLOR_WHITE);
			
			pImageQ->setPos(baseX + 55.5f, baseY + 242.5f);
			pImageQ->setColor(qPressed ? V_GAME_COLOR_GOLD : V_GAME_COLOR_WHITE);

		}

		{
			// --- HPバーの表示 ---
			setHPbarRender(true);
			//float hpRatio = (float)pPlayerTest->getCurrentHp() / pPlayerTest->getMaxHp();
			float hpRatio = (float)m_pNewPlayer->GetCurrentHp() / m_pNewPlayer->GetMaxHp();
			//float maxW = 450.0f;
			//float barLeftEdge = 90.0f; // 初期化で決めた左端の位置

			// スケール適用
			pHpBarFront->setScaleX(hpRatio);

			// 座標補正：左端(barLeftEdge)に固定しつつ、今の幅の半分だけ右にずらした位置を中心にする
			float currentPosX = barLeftEdgeHp + (barWidthHp * hpRatio * 0.5f);

			pHpBarFront->setPos(currentPosX, heigtYHp); // Y座標もアイコンに合わせて40に調整
		}

		{
			// --- Expバーの表示更新 ---
			SetExpbarRender(true);

			float expRatio = (float)m_pExpManager->GetCurrentExp() / m_pExpManager->GetNeedExp();
			if (expRatio > 1.0f) expRatio = 1.0f; // 100%超え防止

			// 1. スケールを適用
			pExpBarFront->setScaleX(expRatio);

			// 2. 座標計算：左端 + (最大幅 * 比率 * 0.5)
			// これで「左端を固定したまま右に伸びる」動きになります
			float currentPosX = barLeftEdgeExp + (maxWExp * expRatio * 0.5f);

			pExpBarFront->setPos(currentPosX, heightYExp); // Y座標も初期化時に合わせた位置に

		}

		{
			// --- スキルバーの表示更新 ---
			SetSkillUIRender(true);
			float currentCoolTime = (float)m_pNewPlayer->GetAreaAttackCoolTime();
			float maxCoolTime = (float)m_pNewPlayer->GetAreaAttackMaxCoolTime();

			// 安全に割り算を行うためのチェック
			float areaRatio = 0.0f;
			if (maxCoolTime > 0.0f)
			{
				// 例：残り10秒/最大10秒 = 1.0  → 1.0 - 1.0 = 0.0 (空っぽ)
				// 例：残り 3秒/最大10秒 = 0.3  → 1.0 - 0.3 = 0.7 (7割溜まった)
				// 例：残り 0秒/最大10秒 = 0.0  → 1.0 - 0.0 = 1.0 (満タン！)
				areaRatio = 1.0f - (currentCoolTime / maxCoolTime);
			}
			if (areaRatio > 1.0f)
			{
				areaRatio = 1.0f;
			}
			if (areaRatio >= 1.0f)
			{
				pAreaSkillIcon->setAlpha(1.0f);
				if (!m_bIsAreaSkillMaxPrev)
				{
					m_bIsAreaSkillMaxPrev = true;

					//アイコンを拡大する
					m_areaSkillIconScale = 1.5f;
					m_areaSkillIargetScale = 1.0f;
				}
				m_areaSkillIconScale += (m_areaSkillIargetScale - m_areaSkillIconScale) * 0.05f;
			}
			else
			{
				pAreaSkillIcon->setAlpha(0.7f);
				m_bIsAreaSkillMaxPrev = false;
				m_areaSkillIconScale = 1.0f;
				m_areaSkillIargetScale = 1.0f;
			}
			pAreaSkillIcon->setScale(m_areaSkillIconScale);
			pAreaAtkBtnFront->setScaleX(areaRatio);
			float currentPosX = barLeftEdgeAreaSkill + (maxWAreaSkill * areaRatio * 0.5f);
			pAreaAtkBtnFront->setPos(currentPosX, heightYAreaSkill);


		}
		{
			// --- スキルバーの表示更新 ---
			SetSkillUIRender(true);
			float currentCoolTime = (float)m_pNewPlayer->GetPullAttackCoolTime();
			float maxCoolTime = (float)m_pNewPlayer->GetPullAttackMaxCoolTime();

			// 安全に割り算を行うためのチェック
			float pullRatio = 0.0f;
			if (maxCoolTime > 0.0f)
			{
				// 例：残り10秒/最大10秒 = 1.0  → 1.0 - 1.0 = 0.0 (空っぽ)
				// 例：残り 3秒/最大10秒 = 0.3  → 1.0 - 0.3 = 0.7 (7割溜まった)
				// 例：残り 0秒/最大10秒 = 0.0  → 1.0 - 0.0 = 1.0 (満タン！)
				pullRatio = 1.0f - (currentCoolTime / maxCoolTime);
			}
			if (pullRatio > 1.0f)
			{
				pullRatio = 1.0f;
			}
			if (pullRatio >= 1.0f)
			{
				pPullSkillIcon->setAlpha(1.0f);
				if (!m_bIsPullSkillMaxPrev)
				{
					m_bIsPullSkillMaxPrev = true;

					//アイコンを拡大する
					m_pullSkillIconScale = 1.5f;
					m_pullSkillIargetScale = 1.0f;
				}
				m_pullSkillIconScale += (m_pullSkillIargetScale - m_pullSkillIconScale) * 0.05f;
			}
			else
			{
				pPullSkillIcon->setAlpha(0.7f);
				m_bIsPullSkillMaxPrev = false;
				m_pullSkillIconScale = 1.0f;
				m_pullSkillIconScale = 1.0f;
			}
			pPullSkillIcon->setScale(m_pullSkillIconScale);
			pPullBtnFront->setScaleX(pullRatio);
			float currentPosX = barLeftEdgeAreaSkill + (maxWAreaSkill * pullRatio * 0.5f);
			pPullBtnFront->setPos(currentPosX, heightYPullSkill);


		}

	}
	break;

	case GameOver:
	{
		float off = 3.0f; // 影のズレ幅
		unsigned int shadowCol = 0xFF000000;

		// --- 上下移動（ふわふわ）の計算 ---
		float offsetY = sinf(blinkCounter * 0.08f) * 15.0f;

		// GAME OVER の表示（影 → 本体の順で描画）
		//vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 100));
		vnFont::setFontSize(38, 100);

		// 影も offsetY を足して一緒に動かす
		vnFont::print(350 + off, 250 + offsetY + off, shadowCol, L"GAME OVER");
		vnFont::print(350, 250 + offsetY, GAME_COLOR_RED, L"GAME OVER");

		// --- 点滅処理 ---
		blinkCounter++;
		float alpha = (sinf(blinkCounter * 0.1f) + 1.0f) * 0.5f;

		// アルファ値を影と本体に適用
		unsigned int blinkShadow = ((unsigned int)(alpha * 255) << 24) | 0x00000000;
		unsigned int blinkColor = ((unsigned int)(alpha * 255) << 24) | (0x00FFFFFF & GAME_COLOR_CYAN);

		// タイトルに戻る案内
		//vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 40));
		vnFont::setFontSize(38, 40);

		// 影も一緒に点滅させる
		vnFont::print(400.0f + off, 450.0f + off, blinkShadow, L"[ENTER]  BACK TITLE");
		vnFont::print(400.0f, 450.0f, blinkColor, L"[ENTER]  BACK TITLE");

		break;

	}

	case GameClear:
		{	
		float off = 3.0f; // 影のズレ幅
		unsigned int shadowCol = 0xFF000000;

		blinkCounter++;
		// クリア文字を上下に「ふわふわ」させる
		float offsetY = sinf(blinkCounter * 0.08f) * 15.0f;

		// --- ALL WAVE CLEAR!! (一番大きく、影もしっかり) ---
		//vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 120));
		vnFont::setFontSize(38, 120);

		vnFont::print(100 + off, 200 + offsetY + off, shadowCol, L"ALL WAVE CLEAR!!");
		vnFont::print(100, 200 + offsetY, GAME_COLOR_GOLD, L"ALL WAVE CLEAR!!");

		// --- TOTAL TIME ---
		int minutes = (int)(totalClearTime / 60);
		int seconds = (int)totalClearTime % 60;
		//vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 50));
		vnFont::setFontSize(38, 50);

		vnFont::print(340 + off, 400 + off, shadowCol, L"～TOTAL TIME: %02d:%02d～", minutes, seconds);
		vnFont::print(340, 400, GAME_COLOR_WHITE, L"～TOTAL TIME: %02d:%02d～", minutes, seconds);

		// --- THANK YOU FOR PLAYING! ---
		vnFont::print(330 + off, 500 + off, shadowCol, L"THANK YOU FOR PLAYING!");
		vnFont::print(330, 500, GAME_COLOR_LIME, L"THANK YOU FOR PLAYING!");

		// --- [ENTER] BACK TITLE (点滅 + 影) ---
		float alpha = (sinf(blinkCounter * 0.1f) + 1.0f) * 0.5f;
		unsigned int blinkShadow = ((unsigned int)(alpha * 255) << 24) | 0x00000000;
		unsigned int blinkColor = ((unsigned int)(alpha * 255) << 24) | (GAME_COLOR_CYAN & 0x00FFFFFF);

		vnFont::print(370.0f + off, 600.0f + off, blinkShadow, L"[ENTER]  BACK TITLE");
		vnFont::print(370.0f, 600.0f, blinkColor, L"[ENTER]  BACK TITLE");


		break;

		}
	case GameFinish:
		{
		 if (isGameFinish)
		 {
			pBackGroundBlack->setRenderEnable(true);
			backGroundBlackScale += (1.1f - backGroundBlackScale) * 0.05f;
			pBackGroundBlack->setScale(backGroundBlackScale);
			if (backGroundBlackScale >= 1)
			{
				for (auto& enemy : enemyPool->GetEnemies())
				{
					enemy->ReStartEnemy();
				}
				enemyPool->ReStartEnemyGroupData();
				switchScene(TITEL);
			}
		 }
		}
	}
	//--プレイヤーのHP



	vnScene::render();
}

//フェンスの外に出ないようにする関数
void SceneMain::InFence(vnCharacter* pObject)
{
	//if (pObject == nullptr) return;

	XMVECTOR vPos = *pObject->getPosition();  // 現在位置
	float length = XMVectorGetX(XMVector3Length(vPos));  // 原点からの距離

	if (length > FenceRadius - 1.0f)
	{
		XMVECTOR dir = XMVector3Normalize(vPos);
		XMVECTOR newPos = XMVectorScale(dir, FenceRadius - 1.0f);
		pObject->setPosition(&newPos);
	}
}
bool SceneMain::CheckFenceReflection(vnCharacter* pObject)
{
	XMVECTOR vPos = *pObject->getPosition();
	float length = XMVectorGetX(XMVector3Length(vPos));

	if (length > FenceRadius - 1.0f)
	{
		// ① 座標をフェンス内に押し戻す
		XMVECTOR dir = XMVector3Normalize(vPos);
		XMVECTOR newPos = XMVectorScale(dir, FenceRadius - 2.0f);
		pObject->setPosition(&newPos);

		return true; // 当たった！
	}
	return false; // 当たってない
}
// --- フェンスのサイズを更新 ---
void SceneMain::UpdateFencePositions() {
	for (int i = 0; i < FENCE_NUM_MAIN; i++) {
		// 角度の計算（初期化時と同じ）
		float degree = 360.0f / FENCE_NUM_MAIN * (float)i;
		float radian = degree * 3.141592f / 180.0f;

		// 新しい FenceRadius を使って座標を再計算
		float x = sin(radian) * FenceRadius;
		float z = cos(radian) * FenceRadius;

		// モデルの位置を更新
		pFence[i]->setPosition(x, 0.0, z);
		pFence[i]->setRotationY(radian);
	}
}

// --- 当たり判定（地面とキャラクター）
void SceneMain::OnCollider(vnCharacter* pCharacter,vnModel *pGround, float footOffset,RigidbodyComponent &rigidBody)
{
	XMVECTOR LinePos = *pCharacter->getPosition();
	//LineDir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

	// --- モデルデータから内部情報を取得 ---
	int vnum = pGround->getVertexNum();	//頂点数を獲得
	int inum = pGround->getIndexNum();	//インデックス数

	//メッシュ単位で走査するため、メッシュデータを取得
	int meshNum = pGround->getMeshNum();
	vnModel_MeshData* pMesh = pGround->getMesh();

	vnVertex3D* vtx = pGround->getVertex();	//頂点配列
	unsigned short* idx = pGround->getIndex();	//インデックス配列
	//ワールドマトリクス
	XMMATRIX world = *pGround->getWorld();

	float highestY = -10000.0f; // 初期値は極端に低く
	int hitMeshID = -1;

	//地面
	vnCollide::stSegment seg;
	float safetyMargin = 0.2f;

	seg.Pos = *pCharacter->getPosition() + XMVectorSet(0, footOffset, 0, 0);
	seg.Dir = XMVectorSet(0, -1, 0, 0);
	seg.Length = footOffset + safetyMargin;
	//seg.Pos = *pPlayerTest->GetModel()->getPosition() + XMVectorSet(0.0f, 20.5f, 0.0f, 0.0f);
	//seg.Dir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	//seg.Length = 0.6f;

	for (int m = 0; m < meshNum; m++)
	{
		int m_inum = pMesh[m].IndexNum;
		int m_sidx = pMesh[m].StartIndex;

		//for(int i=sidx;i<sidx+inum;i+=3)
		for (int i = 0; i < m_inum; i += 3)
		{
			XMVECTOR v1 = XMVector3TransformCoord(
				XMVectorSet(vtx[idx[m_sidx + i + 0]].x,
					vtx[idx[m_sidx + i + 0]].y,
					vtx[idx[m_sidx + i + 0]].z, 0.0f),
				world);

			XMVECTOR v2 = XMVector3TransformCoord(
				XMVectorSet(vtx[idx[m_sidx + i + 1]].x,
					vtx[idx[m_sidx + i + 1]].y,
					vtx[idx[m_sidx + i + 1]].z, 0.0f),
				world);

			XMVECTOR v3 = XMVector3TransformCoord(
				XMVectorSet(vtx[idx[m_sidx + i + 2]].x,
					vtx[idx[m_sidx + i + 2]].y,
					vtx[idx[m_sidx + i + 2]].z, 0.0f),
				world);

			// ★ ここで vnCollide 用の三角形を作る
			vnCollide::stTriangle tri;
			tri.fromPoints(&v1, &v2, &v3);

			// ★ ここで Segment と当てる
			XMVECTOR hit;
			if (vnCollide::isCollide(&hit, &seg, &tri))
			{
				float y = XMVectorGetY(hit);
				if (y > highestY)
				{
					highestY = y;
				}
			}
		}
	}

	if (highestY > -10000.0f)
	{
		//pEnemy->SetIsGround(true);
		rigidBody.SetVerticalVelocity(0.0f);
		rigidBody.SetIsGround(true);
		rigidBody.SetIsUseGravity(false);

		pCharacter->setPositionY(highestY + GROUND_OFFSET);
	}
	else {
		//pEnemy->SetIsGround(false);
		rigidBody.SetIsUseGravity(true);
		rigidBody.SetIsGround(false);

	}


}

// --- 当たり判定（キャラクターとキャラクター）
SceneMain::eDirection SceneMain::colliderCtoC(CharacterBase* p1, CharacterBase* p2)
{
	eDirection ret = eDirection::None;

	if (!p1 || !p2) return ret;
	
	XMVECTOR range = XMVectorAdd(p1->GetCollision().GetSize() * 0.5f, p2->GetCollision().GetSize() * 0.5f);
	float rx = XMVectorGetX(range);
	float ry = XMVectorGetY(range);
	float rz = XMVectorGetZ(range);

	XMVECTOR center1 = XMVectorAdd(*p1->GetModel()->getPosition(), p1->GetCollision().GetCenter());
	XMVECTOR center2 = XMVectorAdd(*p2->GetModel()->getPosition(), p2->GetCollision().GetCenter());

	XMVECTOR dif = XMVectorAbs(center1 - center2);

	float dx = XMVectorGetX(dif);
	float dy = XMVectorGetY(dif);
	float dz = XMVectorGetZ(dif);

	if (dx < rx && dy < ry && dz < rz)
	{
		float sx = rx - dx;
		float sy = ry - dy;
		float sz = rz - dz;

		if (sx < sy && sx < sz)
		{
			if (XMVectorGetX(center1) < XMVectorGetX(center2))
			{
				p1->GetModel()->addPositionX(-sx);
				ret = X_Neg;
			}
			else
			{
				p1->GetModel()->addPositionX(sx);
				ret = X_Pos;
			}
		}
		else if (sy < sz)
		{
			if (XMVectorGetY(center1) < XMVectorGetY(center2))
			{
				p1->GetModel()->addPositionY(-sy);
				ret = Y_Neg;
			}
			else
			{
				p1->GetModel()->addPositionY(sy);
				p1->GetRigidbody().SetIsGround(true);
				ret = Y_Pos;
			}
		}
		else
		{
			if (XMVectorGetZ(center1) < XMVectorGetZ(center2))
			{
				p1->GetModel()->addPositionZ(-sz);
				ret = Z_Neg;
			}
			else
			{
				p1->GetModel()->addPositionZ(sz);
				ret = Z_Pos;
			}
		}
	}
	
	return ret;
}

// 球体同士の判定と押し戻し
SceneMain::eDirection SceneMain::colliderStoS(CharacterBase* p1, CharacterBase* p2)
{
	eDirection ret = eDirection::None;
	if (!p1 || !p2) return ret;

	auto& col1 = p1->GetCollision();
	auto& col2 = p2->GetCollision();

	// 1. 半径の取得（size.xを直径として扱う、またはradiusを追加）
	float r1 = p1->GetEffectiveRadius();
	float r2 = p2->GetEffectiveRadius();
	float sumRadii = r1 + r2;

	// 2. 世界座標での中心位置
	XMVECTOR center1 = XMVectorAdd(*p1->GetModel()->getPosition(), col1.GetCenter());
	XMVECTOR center2 = XMVectorAdd(*p2->GetModel()->getPosition(), col2.GetCenter());

	// 3. 距離の計算
	XMVECTOR diff = center2 - center1;
	XMVECTOR distSqVec = XMVector3LengthSq(diff);
	float distSq = XMVectorGetX(distSqVec);

	// 4. 衝突判定
	if (distSq < sumRadii * sumRadii)
	{
		float dist = sqrtf(distSq);
		if (dist < 0.0001f) return ret; // 重なりすぎ防止

		float overlap = (sumRadii - dist)*1.1f;
		XMVECTOR pushDir = XMVector3Normalize(diff * -1.0f); // p1を押し戻す方向

		// --- 範囲攻撃かどうかの分岐をここに入れる ---
		//if (p1->IsAttacking()) {
		//	// 攻撃中なら敵(p2)を吹っ飛ばす！
		//	XMVECTOR knockbackDir = XMVector3Normalize(diff);
		//	p2->GetRigidbody().AddImpulse(knockbackDir * 25.0f);
		//	p2->ApplyDamage(10);
		//}
		//else 
		{
			// 通常時は位置を補正
			XMVECTOR pushVector = pushDir * overlap;
			p1->GetModel()->addPosition(&pushVector);
		}

		ret = X_Pos; // 戻り値は必要に応じて調整
	}
	return ret;
}

void SceneMain::setHPbarRender(bool on)

{

	pHpBarBackBlack->setRenderEnable(on);

	pHpBarFront->setRenderEnable(on);

	pHpBarBack->setRenderEnable(on);



	pIconPlayer->setRenderEnable(on);

}

void SceneMain::SetExpbarRender(bool on)
{
	pExpBarBack->setRenderEnable(on);
	pExpBarFront->setRenderEnable(on);
	pExpBarBackBlack->setRenderEnable(on);

}

//スキルボタンのUIの表示・非表示
void SceneMain::SetSkillUIRender(bool on)
{
	pAreaAtkBtnBackBlack ->setRenderEnable(on);
	pAreaAtkBtnBack		 ->setRenderEnable(on);
	pAreaAtkBtnFront	 ->setRenderEnable(on);
	pAreaSkillIcon		 ->setRenderEnable(on);
	pPullBtnBackBlack	 ->setRenderEnable(on);
	pPullBtnBack		 ->setRenderEnable(on);
	pPullBtnFront		 ->setRenderEnable(on);
	pPullSkillIcon		 ->setRenderEnable(on);

	if (!m_pNewPlayer->GetIsHaveAreaAtkSkill())
	{
		pAreaAtkBtnBackBlack->setColor(V_GAME_COLOR_BLACK);
		pAreaAtkBtnBack		->setColor(V_GAME_COLOR_BLACK);
		pAreaAtkBtnFront	->setColor(V_GAME_COLOR_BLACK);
		pAreaSkillIcon		->setColor(V_GAME_COLOR_BLACK);
	}
	else
	{
		pAreaAtkBtnBackBlack->setColor(m_areaAtkUIColor.colorBackBlack);
		pAreaAtkBtnBack		->setColor(m_areaAtkUIColor.colorBack);
		pAreaAtkBtnFront	->setColor(m_areaAtkUIColor.colorFront);
		pAreaSkillIcon		->setColor(m_areaAtkUIColor.colorIcon);
	}

	if (!m_pNewPlayer->GetIsHavePullSkill())
	{
		pPullBtnBackBlack->setColor(V_GAME_COLOR_BLACK);
		pPullBtnBack	 ->setColor(V_GAME_COLOR_BLACK);
		pPullBtnFront	 ->setColor(V_GAME_COLOR_BLACK);
		pPullSkillIcon	 ->setColor(V_GAME_COLOR_BLACK);
	}
	else
	{
		pPullBtnBackBlack->setColor(m_pullUIColor.colorBackBlack);
		pPullBtnBack	 ->setColor(m_pullUIColor.colorBack);
		pPullBtnFront	 ->setColor(m_pullUIColor.colorFront);
		pPullSkillIcon	 ->setColor(m_pullUIColor.colorIcon);
	}

}

//----------------------------------------

// --- 待機状態 ---

//----------------------------------------

void SceneMain::UpdateIdel()
{
	//--カメラの初期位置設定
	StartCameraRote();
	if (vnMouse::onR() || vnKeyboard::on(DIK_RETURN))
	{
		m_gameState = Play;
		pSound[2]->play();

		waveManager->Init();   //ここでWave開始
		m_pBlockManager->RespawnBlocks(waveManager->GetCurrentWave(), FenceRadius);

	}

}

void SceneMain::StartCameraRote()
{
	//カメラの座標の計算
	float camX = m_radius * cosf(m_phi) * cosf(m_theta);
	float camY = m_radius * sinf(m_phi);
	float camZ = m_radius * cosf(m_phi) * sinf(m_theta);

	XMVECTOR camPos = XMVectorSet(camX, camY, camZ, 0.0f);
	//プレイヤーの位置を取得してオフセット
	camPos = XMVectorAdd(camPos, *m_pNewPlayer->GetModel()->getPosition());

	vnCamera::setPosition(&camPos);
	vnCamera::setTarget(m_pNewPlayer->GetModel()->getPosition());
}
// ---------------------------------------
//  ポーズ中
// ---------------------------------------
void SceneMain::UpdatePause()
{
	if (vnKeyboard::trg(DIK_TAB))
	{
		m_gameState = GameState::Play;
	}

	//基本の役割：群の情報を出す
	enemyPool->DebugPause();
}





//----------------------------------------

// --- ゲーム中 ---

//----------------------------------------
void SceneMain::UpdatePlay(float deltaTime)
{
	//1.プレイヤーの更新（移動、エフェクト、落下チェック、HP減少など）
	UpdatePlayer(deltaTime);
	UpdateBlocksCollision();
	//2.敵の出現管理（WaveManagerの状態を確認してスポーン）
	SpawnEnemies(deltaTime);

	//3.敵全体の更新（移動、敵同士の衝突、プレイヤーとの衝突・撃破）
	UpdateEnemies(deltaTime);

	//4.コンボと回復の更新（コンボのタイマー処理、コンボの画像表示）
	UpdateCombo(deltaTime);

	//5.グローバルな更新（フェンスの拡大、カメラ追従、タイマー加算）
	UpdateGlobalSystems(deltaTime);

	//レベルアップチェック
	if (m_pExpManager && m_pExpManager->GetLevelUpStock() > 0)
	{
		m_gameState = LevelUp;
		// 3つの選択肢を表示
		for (int i = 0; i < 3; i++)
		{
			// UI用データの取得（本来はGetterを作るのが理想）
			// ここでは仮に直接参照するか、公開されたデータを使います
			auto& data = m_pExpManager->GetUIDisplayChoices()[i];
			int index = m_pExpManager->GetChoiceIndex();
			float posY = 300.0f + (i * 100.0f);
			//UIを設定
			m_pUpgradeUI->SettingUI(data, i, index);
		}

		if (!m_isLevelUpStarted)
		{
			// 【解説】
	// 1. getRotationY(): モデルの現在の向き（背中側）。
	// 2. + XM_PI: 180度反転させて「正面側」の位置にする。
	// 3. + XM_PIDIV2: モデル固有の初期姿勢（Zマイナス向き等）による90度のズレを補正。
	// ※ 最終的な theta 計算時に符号を反転 (-targetTheta) することで、
	//    モデルの回転方向とカメラの回り込み方向を同期させている。
			// 開始した瞬間のモデルの向きに180度足して「正面」の目標を作る
			//m_levelUpCameraTargetTheta =
			//	m_pNewPlayer->GetModel()->getRotationY()
			//	+ XM_PI
			//	+ XM_PIDIV2;
			m_isLevelUpStarted = true;
		}
	}


}
// --- プレイヤー挙動・衝突判定 ---
void SceneMain::UpdatePlayer(float deltaTime)
{
	m_pNewPlayer->Update(deltaTime);

	XMVECTOR movePlayer = m_pNewPlayer->GetRigidbody().getMoveDelta();
	if (!XMVector3Equal(movePlayer, XMVectorZero()))
	{
		pDustEmitter->Desc.SizeMax = 0.5 * (waveManager->GetCurrentWave());
		pDustEmitter->Desc.SizeMin = 0.3 * (waveManager->GetCurrentWave());

		pDustEmitter->setEmit(true, 0);

	}
	else {
		pDustEmitter->setEmit(false, 0);
	}
	//vnFont::print(10.0f, 200.0f, L"movePlayer : % 3f,% 3f,% 3f",
	//	XMVectorGetX(movePlayer),
	//	XMVectorGetY(movePlayer),
	//	XMVectorGetZ(movePlayer));
	OnCollider(m_pNewPlayer->GetModel(), pGround, 1.0f, m_pNewPlayer->GetRigidbody());
	if (m_pNewPlayer->GetModel()->getPositionY() < -30.0f) {
		//プレイヤーが一定以上落下したら
		m_pNewPlayer->GetModel()->setPosition(0.0f, 1.0f, 0.0f);
	}


	InFence(m_pNewPlayer->GetModel());

	if (waveManager->GetState() == WaveManager::WaveState::InProgress)
	{
		//pPlayerTest->applyDamage(0.05f * (waveManager->GetCurrentWave() * 0.8));
		m_pNewPlayer->Damage(0.05f * (waveManager->GetCurrentWave() * 0.8));
		// --- HPが０になったらGameOver ---
		//if (pPlayerTest->getCurrentHp() <= 0)
		//{
		//	m_gameState = GameOver;
		//}
		if (m_pNewPlayer->GetCurrentHp() <= 0)
		{
			m_gameState = GameOver;
			CleanUpScene();

		}

	}

#pragma region 回復処理
	// --- 敵を倒した瞬間の処理（SceneMain内） ---
	if (waveManager->GetState() == WaveManager::WaveState::InProgress)
	{
		// 5体倒したかチェック
		if (m_killCounter >= 1) {
			// 1. 基礎係数を 0.5 -> 0.2 に落とす（これでも十分強力です）
			float baseEffect = (float)m_comboCount * 0.2f;

			// 2. 減衰を (currentWave + 1) ではなく、2乗に近い形にする
			// Wave1: 1.0 / 1.5 = 0.66
			// Wave5: 1.0 / 5.5 = 0.18
			float waveMitigation = 1.0f / ((float)waveManager->GetCurrentWave() * 0.25f + 0.75f);

			float totalRecovery = baseEffect * waveMitigation;

			// 3. 【重要】1回の回復に「上限」を作る（ここがバランスの肝！）
			// これを入れないと、100コンボ超えの時に一気にHPが全快してしまいます
			float maxHealCap = 5.0f; // Waveごとのダメージ数秒分に抑える
			if (totalRecovery > maxHealCap) {
				totalRecovery = maxHealCap;
			}

			//pPlayerTest->addHP(totalRecovery);
			m_pNewPlayer->AddHP(totalRecovery);
			m_killCounter = 0;
		}
	}

#pragma endregion

}

// --- 敵の出現管理 ---
void SceneMain::SpawnEnemies(float deltaTime)
{
	// === 出現 ===
	waveManager->Update(deltaTime);
	//int totalCount = (int)enemyPool->GetEnemies().size();
	int activeCount = enemyPool->GetActiveCount();
	if (activeCount < waveManager->GetMaxSpawnLimit() && waveManager->GetState() == WaveManager::WaveState::InProgress)
	{
		int spawnNum = waveManager->GetMaxSpawnLimit() - activeCount;
		//vnFont::print(30, 400, L"spawnNum %d", spawnNum);

		for (int i = 0; i < spawnNum; i++)
		{
			// ウェーブの残り出現枠がある時だけスポーン
			if (waveManager->CanSpawn())
			{
				float minRadius = 1.0f;
				float maxRadius = FenceRadius * 3; // フェンスの8割まで

				float r1 = (float)rand() / RAND_MAX;
				float r2 = (float)rand() / RAND_MAX;
				float angle = r1 * XM_2PI;
				// ドーナツ型分布で計算
				float dist = sqrtf(r2 * (maxRadius * maxRadius - minRadius * minRadius) + minRadius * minRadius);


				float x = cosf(angle) * dist;
				float y = spawnHeight;
				float z = sinf(angle) * dist;
				XMVECTOR pos = XMVectorSet(x, y, z, 0.0f);
				enemyPool->Spawn(pos);
				waveManager->OnEnemySpawned(); // カウントを増やす


			}
		}
	}

	//vnFont::print(30, 450, L"activeCount %d", activeCount);

}

// --- 敵の移動・衝突・プレイヤーとの判定 ---
void SceneMain::UpdateEnemies(float deltaTime)
{
	// プレイヤー座標を全敵に渡す
	//enemyPool->SetPlayerPosAll(*pPlayerTest->GetModel()->getPosition());
	enemyPool->SetPlayerPosAll(*m_pNewPlayer);

	// 更新
	enemyPool->Update(deltaTime);
	// 移動反映＋地面衝突
	auto& enemies = enemyPool->GetEnemies();
	// --- WAVEをクリアしたら敵を消す ---
	//if (waveManager->GetKillTargetCount() <= waveManager->GetKillCount())
	//{
	//	for (auto enemy : enemies)
	//	{
	//		enemy->DeSpawn();
	//	}
	//}
	if (waveManager->GetWaveTimer() >= waveManager->GetWaveTimeLimit())
	{
		for (auto enemy : enemies)
		{
			enemy->DeSpawn();
		}
	}

	for (auto enemy : enemies)
	{

		if (!enemy->GetActive()) continue;
		// ------------------------------------
		// --- ここで引き寄せ判定を流し込む ---
		// ------------------------------------

		XMVECTOR enemyPos = *enemy->GetModel()->getPosition();
		XMVECTOR toPlayerVec = *m_pNewPlayer->GetModel()->getPosition() - enemyPos;
		float dist = XMVectorGetX(XMVector3Length(toPlayerVec));

		// 敵に「今は吸い込み中か？」を判断させる
		enemy->CheckPullTrigger(m_pNewPlayer->IsPulling(), m_pNewPlayer->GetPullRadius(), dist);



		// --- 移動 ---

		//enemy->ChangeSpeed(waveManager->GetCurrentWave() * 0.5);

		// 移動量取得
		XMVECTOR moveEnemy = enemy->GetRigidbody().getMoveDelta();

		// 座標反映
		enemy->GetModel()->addPosition(&moveEnemy);
		// 地面判定
		OnCollider(enemy->GetModel(), pGround, 1.0f, enemy->GetRigidbody());




	}
	// --- 敵同士の衝突分離 ---
	for (size_t i = 0; i < enemyPool->GetEnemies().size(); ++i)
	{
		NewEnemyClass* a = enemyPool->GetEnemies()[i];
		if (!a->GetActive() || !a->GetRigidbody().GetIsGround()) continue;

		for (size_t j = i + 1; j < enemyPool->GetEnemies().size(); ++j)
		{
			NewEnemyClass* b = enemyPool->GetEnemies()[j];
			if (!b->GetActive() || !b->GetRigidbody().GetIsGround()) continue;
			//if (!a->IsAttracted() && !b->IsAttracted())continue;
			if (a->IsAttracted() || b->IsAttracted())
			{
				continue;
			}
			//// 1. まずは「お互いにリーダー」か「お互いにザコ」なら、両方押し合う
			//if (a->GetIsLeader() == b->GetIsLeader())
			//{
			//	colliderCtoC(a, b);
			//	colliderCtoC(b, a);
			//}
			//// 2. どちらか片方だけがリーダーなら、リーダーじゃない方（ザコ）を動かす
			//else if (b->GetIsLeader())
			//{
			//	colliderCtoC(a, b); // bがリーダーなので、aがどく
			//}
			//else
			//{
			//	colliderCtoC(b, a); // aがリーダーなので、bがどく
			//}
			bool aIsL = a->GetIsLeader();
			bool bIsL = b->GetIsLeader();
			bool sameGroup = (a->GetGroupID() == b->GetGroupID());

			// 1. リーダー同士なら（群れが違っても）当たる
			// 2. 同じ群れの配下同士（リーダーじゃない者同士）なら当たる
			// ⇒ これにより「リーダーと配下」は同じ群れでも当たらなくなる
			if ((aIsL && bIsL) || (sameGroup && !aIsL && !bIsL))
			{
				colliderCtoC(a, b);
				colliderCtoC(b, a);
			}

		}
	}

	//敵と味方の当たり判定
	for (size_t i = 0; i < enemyPool->GetEnemies().size(); ++i)
	{
		NewEnemyClass* enemy = enemyPool->GetEnemies()[i];

		if (!enemy->GetActive()) continue;

		if (enemy->GetState() != NewEnemyClass::eState::KnockBack)
		{
			auto dir = colliderStoS(enemy, m_pNewPlayer);
			NewEnemyClass::DamageSource source = NewEnemyClass::DamageSource::Melee;

			if (dir != None)
			{
				//if (!m_pNewPlayer->GetRigidbody().GetIsGround())
				//{
				//	if (dir == Y_Pos || dir == Y_Neg)
				//		return; // 空中のYだけ無視
				//}

				// --- 倒したとき ---
				pSound[1]->play(true);


				pEmitter->setPosition(enemy->GetModel()->getPosition());
				pEmitter->setPositionY(enemy->GetModel()->getPositionY() + 2.0f);
				int index = rand() % (sizeof(vnEmitter::colors) / sizeof(vnEmitter::colors[0]));
				pEmitter->SetColor(vnEmitter::colors[index]);
				pEmitter->setEmit(true, 0.3f);
				AddCombo();
				
				//死因を記録
				if (m_pNewPlayer->IsAreaAttack())
				{
					source = NewEnemyClass::DamageSource::AreaAttack;
				}
				if (m_pNewPlayer->IsPulling())
				{
					source = NewEnemyClass::DamageSource::PullAttack;
				}
				if (enemy->GetIsLeader())
				{
					enemy->OnDie(source);
				}
				enemy->SetIsHitPlayer(true);
				waveManager->OnEnemyKilled();
			}
			else
			{
				//enemy->SetIsHitPlayer(false);
				//--当たった時に枠外に飛ばせる
				InFence(enemy->GetModel());

			}

			auto dirEtoB = colliderStoS(enemy, m_pBullet);

			if (dirEtoB != None)
			{
				//if (!m_pNewPlayer->GetRigidbody().GetIsGround())
				//{
				//	if (dir == Y_Pos || dir == Y_Neg)
				//		return; // 空中のYだけ無視
				//}

				// --- 倒したとき ---
				pSound[1]->play(true);


				pEmitter->setPosition(enemy->GetModel()->getPosition());
				pEmitter->setPositionY(enemy->GetModel()->getPositionY() + 2.0f);
				int index = rand() % (sizeof(vnEmitter::colors) / sizeof(vnEmitter::colors[0]));
				pEmitter->SetColor(vnEmitter::colors[index]);
				pEmitter->setEmit(true, 0.3f);
				AddCombo();

				enemy->SetIsHitPlayer(true);
				waveManager->OnEnemyKilled();
			}
			else
			{
				//enemy->SetIsHitPlayer(false);
				//--当たった時に枠外に飛ばせる
				InFence(enemy->GetModel());

			}

		}
	}
	//// enemyPoolの全個体を監視（!enemy->GetActive() の continue を外す）
	//for (size_t i = 0; i < enemyPool->GetEnemies().size(); ++i)
	//{
	//	NewEnemyClass* enemy = enemyPool->GetEnemies()[i];

	//	// 1. アクティブ状態を文字列にする
	//	const wchar_t* activeStr = enemy->GetActive() ? L"ALIVE" : L"DEAD ";

	//	// 2. ステートに応じた文字列の判定
	//	const wchar_t* stateStr = L"UNKNOWN";
	//	switch (enemy->GetState())
	//	{
	//	case NewEnemyClass::eState::Idel:      stateStr = L"IDLE";      break;
	//	case NewEnemyClass::eState::Panic:     stateStr = L"PANIC";     break;
	//	case NewEnemyClass::eState::Charge:    stateStr = L"CHARGE";    break;
	//	case NewEnemyClass::eState::KnockBack: stateStr = L"KNOCKBACK"; break;
	//	}

	//	// 3. 画面に表示
	//	// 生死状態(activeStr)も一緒に出すことで、
	//	// 「DEADなのにPANICになっている個体」がいないかチェック
	//	vnFont::print(20, 100 + (static_cast<int>(i) * 25), L"[%s] Enemy[%d] State: %s",
	//		activeStr, i, stateStr);
	//}
}

// --- コンボ計算・回復 ---
void SceneMain::UpdateCombo(float deltaTime)
{
#pragma region コンボ処理（数の増加のみ敵の処理にある）
	// --- コンボ ---
	//--Wave状況に応じたコンボ限界時間の更新 ---
	m_currentComboLimit = COMBO_BASE_TIME - (waveManager->GetCurrentWave() * COMBO_DECREASE);
	if (m_currentComboLimit < COMBO_MIN_TIME) {
		m_currentComboLimit = COMBO_MIN_TIME;
	}

	// --- コンボタイマーのカウントダウン ---
	if (waveManager->GetState() == WaveManager::WaveState::InProgress) //プレイ中のみコンボリセット
	{
		if (m_comboCount > 0) {
			m_comboTimer -= 1.0f / 60.0f; // 1フレーム分減らす
			if (m_comboTimer <= 0.0f) {
				m_comboCount = 0;   // コンボ終了
				m_comboTimer = 0.0f;
			}
		}
	}

	// --- スケール演出の更新 ---
	if (m_comboScale > 2.0f) { // 上限
		m_comboScale = 2.0f;
	}
	if (m_comboScale > 1.0f) {
		m_comboScale -= 0.05f; // 1フレームごとに0.05ずつ小さくする
	}
	else {
		m_comboScale = 1.0f; // 1.0より小さくならないように固定
	}

	// --- 透明度（フェード）の計算 ---
	float currentAlpha = 1.0f;
	// 残り時間が1.0秒を切ったら、タイマーに合わせて透明にする
	if (m_comboTimer < 1.0f) {
		currentAlpha = m_comboTimer;
	}

	// 1. まず30個全員を画面外へ飛ばす
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 10; j++) {
			pComboSprites[i][j]->setPos(-1000, -1000);
			// ついでに透明度もリセットしておく（次に備えて）
			pComboSprites[i][j]->setAlpha(1.0f);
		}
	}

	if (m_comboCount > 0) {
		// 各桁の数字を計算
		int digits[3];
		digits[0] = (m_comboCount / 100) % 10; // 百の位
		digits[1] = (m_comboCount / 10) % 10;  // 十の位
		digits[2] = m_comboCount % 10;          // 一の位

		bool showing = false; // 上位桁に数字が出たかどうかのフラグ

		for (int i = 0; i < 3; i++) {
			// 0抑制
			if (i < 2 && digits[i] == 0 && !showing) continue;
			showing = true;

			// 表示位置の調整
			float startX = 500.0f + 500.0f;
			float spacing = (128.0f * 0.5f) * m_comboScale / 1.5;
			float posX = startX + (i * spacing);

			// スプライトを取得して設定
			vnSprite* s = pComboSprites[i][digits[i]];
			s->setPos(posX, 220);
			s->setScale(0.5f * m_comboScale);

			// 【追加】透明度を適用
			s->setAlpha(currentAlpha);
		}

		// Combo!! の表示
		pComboWord->setPos(800 + 400, 220);
		pComboWord->setScale(m_comboScale);

		// 【追加】Combo!!も一緒にフェード
		pComboWord->setAlpha(currentAlpha);

	}
	else {
		// コンボが0なら、Combo!!も画面外に隠す
		pComboWord->setPos(-1000, -1000);
	}

	// --- WAVEクリアしたらコンボTimerを最大にしとく ---
	if (waveManager->GetState() != WaveManager::WaveState::InProgress)
	{
		m_comboTimer = m_currentComboLimit;
	}

#pragma endregion
}
// --- コンボ加算 ---
void SceneMain::AddCombo()
{
	// コンボ加算！
	m_comboCount++;
	// Waveごとに短くなっている「現在の限界時間」をセット
	m_comboTimer = m_currentComboLimit;
	m_comboScale = 1.5f; // 一瞬で1.5倍の大きさに跳ねさせる！(コンボの文字)
	m_killCounter++; // 回復用カウンターを増やす
	if (m_comboCount %100==0)
	{
		
		Common::StartCameraShake(3.5f, 3.5f, 1.0f);
	}
	//経験値を獲得
	if (m_pExpManager)
	{
		float expAmount = 1.0f * (1.0f + (waveManager->GetCurrentWave() * 0.2f));

		m_pExpManager->GainExp(expAmount);
	}

}

// --- フェンス・タイマー・カメラ ---
void SceneMain::UpdateGlobalSystems(float deltaTime)
{
	// --- フェンスの半径WAVEごとに５ずつ大きくする ---
	int currentWave = waveManager->GetCurrentWave(); // WaveManagerにWave取得関数があると仮定

	if (currentWave > oldWaveCount) {
		// Waveが増えた瞬間だけここを通る
		FenceRadius += 3.0f;

		for (auto& obj : enemyPool->GetEnemies()) // 管理しているリストの形式に合わせてください
		{
			// NewEnemyClassにキャスト（変換）でき、かつリーダーなら
			auto enemy = dynamic_cast<NewEnemyClass*>(obj);
			if (enemy && enemy->GetIsLeader())
			{
				enemy->SetFenceRadius(FenceRadius); // 最新の半径に更新！
			}
		}

		// 実際にフェンスモデルの座標やスケールを更新する処理をここに書く
		UpdateFencePositions();
		oldWaveCount = currentWave; // 記録を更新
	}


	// --- 時間経過 ---
	if (waveManager->GetState() == WaveManager::WaveState::InProgress)
	{
		isTimerActive = true;
	}
	else {
		isTimerActive = false;
	}

	if (isTimerActive) {
		totalClearTime += 1.0f / 60.0f; // 60FPS想定
	}
	// --- カメラ操作 ---
	//Common::UpdateCameraByMouse(theta, phi, radius, m_pNewPlayer->GetModel()->getPosition());
	//Common::UpdateFlexibleCamera(pPlayerTest->GetModel()->getPosition(), phi, radius*2, theta);
	Common::UpdateFlexibleCamera(m_pNewPlayer->GetModel()->getPosition(), phi, radius * 1.5, theta, FenceRadius);
	m_isLevelUpStarted = false;


	if (vnKeyboard::trg(DIK_TAB))
	{
		m_gameState = GameState::Pause;
	}

}

// --- レベルアップ画面の更新 ---
void SceneMain::UpdateLevelUp()
{
	//敵を全て消す
	enemyPool->HideAllActiveEnemies();

	// 共通の更新（演出など）
	m_pNewPlayer->UpdateLevelUp();
	OnCollider(m_pNewPlayer->GetModel(), pGround, 1.0f, m_pNewPlayer->GetRigidbody());
	m_pUpgradeUI->UpdateUI();

	//vnFont::print(500, 100, L"--- LEVEL UP! SELECT UPGRADE ---", GAME_COLOR_GOLD);

	// --- 1. 入力判定（UIがまだ消えていない時だけ受け付ける） ---
	if (!m_pUpgradeUI->GetIsClosingUI()) // UIが消去演出中でなければ
	{
		int selectedIndex = -1;
		if (vnKeyboard::trg(DIK_1)) selectedIndex = 0;
		if (vnKeyboard::trg(DIK_2)) selectedIndex = 1;
		if (vnKeyboard::trg(DIK_3)) selectedIndex = 2;

		if (selectedIndex != -1)
		{
			m_pExpManager->ApplyUpgrade(selectedIndex);
			pSound[2]->play();
			m_pUpgradeUI->HideUI(); // ここでアニメーション開始（IsHidingがtrueになる想定）
		}
	}

	// --- 2. アニメーション終了待ち判定（入力とは独立させる） ---
	// HideUIが呼ばれた後、アニメーションが終わったかどうかを毎フレームチェックする
	if (m_pUpgradeUI->GetIsClosingUI() && m_pUpgradeUI->GetIsFinishAnim())
	{
		if (m_pExpManager->GetLevelUpStock() <= 0)
		{
			// Playに戻る
			m_gameState = Play;
			m_theta = theta;
			m_radius = radius;
			m_phi = phi;

			enemyPool->ShowAllEnemies();
			m_pNewPlayer->FinishLevelUp();
		}
		else
		{
			// まだストックがあるなら再抽選して、UIを再度表示状態に戻す
			// m_pExpManager->GenerateLevelUpOptions();
			// m_pUpgradeUI->ShowUI(); // 再表示
		}
	}

}




//----------------------------------------

// --- ゲームオーバー ---

//----------------------------------------
void SceneMain::UpdateGameOver()
{

	if (!pSound[3]->isPlaying())
	{
		pSound[3]->play();

	}
	if (vnKeyboard::trg(DIK_RETURN))
	{
		pSound[2]->play();
		isGameFinish = true;
		m_gameState = GameFinish;
	}

}



//----------------------------------------

// --- ゲームクリア ---

//----------------------------------------
void SceneMain::UpdateGameClear()
{

	if (!pSound[5]->isPlaying())
	{
		pSound[5]->play();

	}
	if (vnKeyboard::trg(DIK_RETURN))
	{
		pSound[2]->play();
		isGameFinish = true;
		m_gameState = GameFinish;
	}


}


//--敵の消去など共通の処理
void SceneMain::CleanUpScene()
{
	pDustEmitter->setEmit(false, 0);
	pSound[0]->stop();
	
	//敵を全て消す
	for (size_t i = 0; i < enemyPool->GetEnemies().size(); ++i)
	{
		NewEnemyClass* enemy = enemyPool->GetEnemies()[i];
		enemy->DeSpawn();
	}

	//コンボUIを隠す
	{
		int digits[3];
		digits[0] = (m_comboCount / 100) % 10; // 百の位
		digits[1] = (m_comboCount / 10) % 10;  // 十の位
		digits[2] = m_comboCount % 10;          // 一の位
		pComboWord->setPos(-1000, -1000);
		for (int i = 0; i < 3; i++) {
			// スプライトを取得して設定
			vnSprite* s = pComboSprites[i][digits[i]];
			s->setPos(-1000, -1000);
		}

	}
}

void SceneMain::UpdateWaveTransition()
{
	// --- WAVEの状態の切り替え(WAVEクリア→次のWAVEとか) ---
	if ((waveManager->GetState() == WaveManager::WaveState::ClearWait) && waveManager->GetCurrentWave() < 5)
	{
		if (isWaveClear == false) // まだ「乗っている状態」に切り替わる前なら
		{
			pSound[4]->play();   // 鳴らす
		}
		isWaveClear = true;

		if (vnKeyboard::on(DIK_RETURN) || vnMouse::onR())
		{

			pSound[2]->play();
			waveManager->GoNextWave();
			isWaveClear = false;
			m_pBlockManager->RespawnBlocks(waveManager->GetCurrentWave(), FenceRadius);
		}
	}
	if (waveManager->GetState() == WaveManager::WaveState::ClearWait && waveManager->GetCurrentWave() == 5)
	{
		if (!isGameFinish)
			m_gameState = GameClear;
		CleanUpScene();
	}

}
void SceneMain::UpdateBlocksCollision()
{
	// --- 地形との当たり判定（ブロック） ---
	// プレイヤー vs ブロック
	for (auto* block : m_pBlockManager->GetAllBlocks()) {
		// ブロックが表示（有効）なときだけ判定
		//if (block->GetModel()->getRenderEnable()) {
		colliderCtoC(m_pNewPlayer, block);
		colliderCtoC(block, m_pNewPlayer);
		//}
	}

	// 敵 vs ブロック
	for (auto* enemy : enemyPool->GetEnemies()) {
		if (!enemy->GetActive()) continue;
		if (enemy->IsAttracted()) {
			continue;
		}
		for (auto* block : m_pBlockManager->GetAllBlocks()) {
			if (colliderCtoC(enemy, block)) {
				// 当たった敵にはフラグを立てる
				enemy->SetWallHit(true);

				// 1つのブロックと当たっていれば十分なので、このループを抜ける（高速化）
				break;
			}

			//if (block->GetModel()->getRenderEnable()) {
			colliderCtoC(enemy, block);
			//}
		}
	}

	for (auto* block : m_pBlockManager->GetAllBlocks()) {
		if (colliderCtoC(m_pBullet, block)) {
			m_pBullet->SetIsHitWall(true);
			break; // 何か一つに当たっていればOK
		}
	}
	if (m_pBullet->IsActive())
	{
		if (CheckFenceReflection(m_pBullet->GetModel()))
		{
			m_pBullet->SetIsHitWall(true);
		}
	}
	//vnFont::print(10.0f, 500, L"X %.f,Y%.f,Z%.f",
	//	m_pBullet->GetModel()->getPositionX(),
	//	m_pBullet->GetModel()->getPositionY(),
	//	m_pBullet->GetModel()->getPositionZ());


	//vnFont::print(10, 600, L"HitWall: %s", m_pBullet->GetIsHitWall() ? L"true" : L"false");


	//vnFont::print(10.0f, 500, GAME_COLOR_YELLOW,L"X %.f,Y%.f,Z%.f",
	//	vnCamera::getPositionX(),
	//	vnCamera::getPositionY(),
	//	vnCamera::getPositionZ());

	//vnFont::print(10.0f, 600, L"radius %.f,phi%.f,theta%.f",
	//	vnCamera::get(),
	//	vnCamera::getPositionY(),
	//	vnCamera::getPositionZ());



	//// ループが終わった後に、フラグを更新する
	//// 前のフレームで当たっていなくて、今当たった場合だけ反射させる、といった制御ができる
	//if (isHittingAnyBlock && !m_pBullet->GetIsHitWall()) {
	//	//m_pBullet->Reflect(); // 反射関数を呼ぶ
	//}
	//m_pBullet->SetIsHitWall(isHittingAnyBlock);

}
