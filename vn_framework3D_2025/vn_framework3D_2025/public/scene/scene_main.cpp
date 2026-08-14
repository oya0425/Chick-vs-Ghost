#include "../../framework.h"
#include "../../framework/vn_environment.h"
#include<algorithm>
#include <DirectXMath.h>
#include <cmath>
  
#include"../RigidbodyComponent.h"
#define ENABLE_TREE_DELETE (1)  // 1: 解放する、0: 解放しない

//グローバル変数
extern bool g_isTutorial;	//タイトルシーンのチュートリアルをするかどうかの変数（trueでする）
extern bool g_isEndless;	//タイトルシーンのエンドレスモードにするかどうかの変数（trueでする）




// --- メンバ変数 ---
namespace {

	//=====================================================
	// --- ボタンの設定 ---
	//=====================================================
	//タイトルに戻るボタン
	constexpr float titleBack_button_x = 1150.0f;
	constexpr float titleBack_button_y = 650.0f;

	//閉じるボタン
	constexpr float message_close_button_x = 950.0f;
	constexpr float message_close_button_y = 650.0f;

	//戻るボタン
	constexpr float message_left_button_x = 450.0f;
	constexpr float message_left_button_y = 650.0f;

	//進むボタン
	constexpr float message_right_button_x = 950.0f;
	constexpr float message_right_button_y = 650.0f;

	//はいボタン
	constexpr float message_yes_button_x = 850.0f;
	constexpr float message_yes_button_y = 600.0f;

	//いいえボタン
	constexpr float message_no_button_x = 450.0f;
	constexpr float message_no_button_y = 600.0f;

	//ゲームに戻るボタン
	constexpr float playgame_back_button_x = 1150.0f;
	constexpr float playgame_back_button_y = 450.0f;

	//チュートリアル振り返りボタン
	constexpr float tutorial_review_button_x = 1150.0f;
	constexpr float tutorial_review_button_y = 550.0f;

	//振り返りボタンを押した後に出る戻るボタン
	constexpr float tutorial_back_button_x = 1150.0f;
	constexpr float tutorial_back_button_y = 550.0f;

	//敵の説明ボタン
	constexpr float tutorial_enemy_leader_button_x = 400.0f;
	constexpr float tutorial_enemy_leader_button_y = 300.0f;

	//経験値の説明ボタン
	constexpr float tutorial_exp_button_x = 650.0f;
	constexpr float tutorial_exp_button_y = 300.0f;

	//レベルアップの説明ボタン
	constexpr float tutorial_level_up_button_x = 900.0f;
	constexpr float tutorial_level_up_button_y = 300.0f;

	//スキルの説明ボタン
	constexpr float tutorial_skills_button_x = 400.0f;
	constexpr float tutorial_skills_button_y = 450.0f;

	//プレイヤー操作の説明ボタン
	constexpr float tutorial_player_operation_button_x = 650.0f;
	constexpr float tutorial_player_operation_button_y = 450.0f;

	//プレイヤーダメージの説明ボタン
	constexpr float tutorial_player_damage_button_x = 900.0f;
	constexpr float tutorial_player_damage_button_y = 450.0f;

	//全ボタンサイズ
	constexpr float button_w = 200;
	constexpr float button_h = 70;

	//=====================================================
	// --- 説明画像の設定 ---
	//=====================================================







	constexpr float underRespawnPos = -5.0f;

	constexpr float playerModelScale = 1.2f;
	constexpr float blackBackSpeed = 0.1f;	//黒い背景の拡大縮小する速度

	constexpr float offScreen = -1000.0f;	//2Dにおける画面外（縦、横共通）
	constexpr float operationUI_Y = 20.0f;	//操作説明のUIの基準の高さ
	
	
	FXMVECTOR enemyGhostModelSize = XMVectorSet(2, 2, 2, 0);
	FXMVECTOR enemyGhostColSize = XMVectorSet(2, 2, 2, 0);
	
	constexpr float spawnHeight	   = 40.0f;	//敵のリスポーンする時の高さ
	
	constexpr int leaderCount = 50;	//リーダーを生成する幅（leaderCountごとにリーダーを作る）

	//半径設定（Fenceと木）
	constexpr float defalutFenceRadius = 35.0f;
	constexpr float treeRadius = 6.0f;


	// --- UI ---
	constexpr float uiHidePosX = -5000.0f;
	constexpr float uiHidePosY = -5000.0f;

	//constexpr float freamImgX = 1050.0f;
	//constexpr float freamImgY = 150.0f;
	//constexpr float backGroundImgX = 1020.0f;
	//constexpr float backGroundImgY = 120.0f;
	//constexpr float mainImgX = 110.0f;
	//constexpr float mainImgY = 110.0f;

	//レベルアップ時の選択肢のUI設定
	constexpr float freamImgW = 1050.0f*0.9f;
	constexpr float freamImgH = 150.0f*0.9f;
	constexpr float backGroundImgW = 1020.0f*0.9f;
	constexpr float backGroundImgH = 120.0f*0.9f;
	constexpr float mainImgW = 110.0f*0.9f;
	constexpr float mainImgH = 110.0f*0.9f;

	//時間表示の後ろの背景の設定（画像の縦横、位置xは画面の中心）
	constexpr float timeBackGround_w = 150;
	constexpr float timeBackGround_h = 150;
	constexpr float timeBackGround_y = 120;


	// --- Comboimgと数値の位置 ---
	constexpr float combo_num_x = 1000.0f;
	constexpr float combo_num_y = 300.0f;
	constexpr float combo_img_x = 1200.0f;
	constexpr float combo_img_y = 300.0f;


	//UIバー設定（位置、サイズ）
	// --- HPバー ---
	constexpr float barLeftEdgeHp = 90.0f;
	constexpr float barWidthHp = 400.0f;
	constexpr float barHeightHp = 18.0f;

	constexpr float barBackWidthHp = 408.0f;
	constexpr float barBackHeightHp = 22.0f;

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
	constexpr float heightYAreaSkill = 320.0f + operationUI_Y;

	// クールタイムが最大のとき（使える状態）のバーの「最大横幅」
	constexpr float maxWAreaSkill = 250.0f;


	// --- 2. 引き寄せスキル（Pull）用バー ---
	// バーの「左端」のX座標（他のバーと綺麗に揃えるために 90.0f）
	constexpr float barLeftEdgePullSkill = 55.0f;

	// バーの「縦の位置（高さ）」を表すY座標（範囲攻撃バーの40px下に配置）
	constexpr float heightYPullSkill = 400.0f + operationUI_Y;

	// クールタイムが最大のとき（使える状態）のバーの「最大横幅」
	constexpr float maxWPullSkill = 250.0f;


	// ポーズ中のバーの基準
	constexpr float baseBarWidth = 300.0f;
	constexpr float baseBarHeight = 12.0f;




	// --- カメラ関係 ---
	constexpr float defualtRadius = 30.0f;
	constexpr float defualtTheta  = 92.688f;
	constexpr float defualtPhi	  = 0.7f;


	//
	constexpr float defualtDamage = 2.0f;

	//
	constexpr float finalChargeEnemyNum = 2;	//最終WAVEの特攻状態で出てくる敵の数を増やす倍率（ボスの近接耐性に掛ける）

	//経験値のマネージャーが画像の番号を持ってるからそれを見て画像を入れる
	// 1.フレーム
	// 2.後ろの背景
	// 3.メインのアイコン
	const UpgradeUIResource upgradeUIResources[5] =
	{
		//移動速度アップ
		{
			L"data/image/フレーム１.png",
			L"data/image/BackGroundWhite.png",
			L"data/image/移動速度アップのアイコン.png"
		},
		//範囲攻撃
		{
			L"data/image/フレーム１.png",
			L"data/image/BackGroundWhite.png",
			L"data/image/範囲攻撃のアイコン.png"
		},
		//引き寄せ攻撃
		{
			L"data/image/フレーム１.png",
			L"data/image/BackGroundWhite.png",
			L"data/image/引き寄せ攻撃のアイコン.png"
		},

		{
			L"data/image/IMG_E6067.png",
			L"data/image/BackGroundWhite.png",
			L"data/image/IMG_E6067.png"
		},

		{
			L"data/image/IMG_E6067.png",
			L"data/image/BackGround.png",
			L"data/image/IMG_E6067.png"
		}


	};
}

void SceneMain::SetupEnemy(NewEnemyClass* enemy, const NewEnemyClass::EnemyData& data,bool isLeader,bool isBoss)
{
	// 1. データテーブルからパスを取得してモデルを生成
	vnCharacter* model = new vnCharacter(data.folder, data.file);
	enemy->SetModel(model);

	RegisterCharacter(model);

	model->setRenderEnable(false);

	for (int i = 0; i < model->getPartsNum(); i++)
	{
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

	// ビルボードを生成
	
	enemy->SetChargeMark(new vnSprite(1.1f, 1.1f, 28, 28, L"data/image/怒り.png"));
	enemy->SetPanicMark(new vnSprite(1.1f, 1.1f, 32, 32, L"data/image/汗.png"));

	registerObject(enemy->GetChargeMark());
	registerObject(enemy->GetPanicMark());

	//ボスの設定
	if (isLeader && isBoss)
	{
		EnemyPool::GetInstance().GetBossGroupData() = std::make_unique<NewEnemyClass::GroupData>();
		
		NewEnemyClass::GroupData* group = EnemyPool::GetInstance().GetBossGroupData().get();

		NewEnemyClass::GroupColorData colorData = enemy->GetRandomGroupData();

		group->id = colorData.id;
		group->color = colorData.color;

		group->isLeaderAlive = false;		//出てきていないリーダーTabのリーダー情報殻外すため


		int paletteIndex = colorData.id - 1;
		group->colorName = NewEnemyClass::g_LeaderColorPalette[paletteIndex].colorName;
		enemy->SettingBoss(group);
		enemy->SetFenceRadius(FenceRadius);
	}
	else
	{
		//リーダーの設定
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
			
			group->isLeaderAlive = false;		//出てきていないリーダーTabのリーダー情報殻外すため

			enemy->SettingLeader(group);
			enemy->SetFenceRadius(FenceRadius);
		}
		//その他の敵の設定
		else
		{
			enemy->SettingOther();
		}
	}

}

//UIBarの設定
void SceneMain::CreateUIBar(
	EnemyPool::UIBar& ui,
	float centerX,
	float posY,
	float width,
	float height) 
{

	ui.pBackBlack = new vnSprite(
		centerX,
		posY,
		width + 5.0f,
		height + 6.0f,
		NULL);

	ui.pBackBlack->setColor(V_GAME_COLOR_BLACK);
	registerObject(ui.pBackBlack);

	ui.pBack = new vnSprite(
		centerX,
		posY,
		width,
		height,
		NULL);

	ui.pBack->setColor(V_GAME_COLOR_BLUEBLACK);
	registerObject(ui.pBack);

	ui.pFront = new vnSprite(
		centerX,
		posY,
		width,
		height,
		NULL);
	ui.pFront->setScaleX(0.0f);
	ui.pFront->setColor(V_GAME_COLOR_CYAN);
	registerObject(ui.pFront);


}

//？マークと吹き出しの設定
EnemyPool::UIQuestionExplain SceneMain::CreateQuestionUI(const WCHAR* text, DWORD color, float offsetSize)
{
	EnemyPool::UIQuestionExplain qus;

	// スプライト生成（共通の設定）
	qus.pQuestionIcon = new vnSprite(offScreen, 200, 64 * offsetSize, 64 * offsetSize, L"data/image/keyboard_question.png");
	qus.pBalloonBg = new vnSprite(offScreen, 200, 256*1.5, 128 , L"data/image/吹き出し.png");

	registerObject(qus.pQuestionIcon);
	registerObject(qus.pBalloonBg);

	// 固有のデータと色の設定
	qus.explainText = text;
	qus.textColor = color;

	// 判定用サイズや初期座標の保存
	qus.qW = 64.0f * offsetSize;
	qus.qH = 64.0f * offsetSize;
	qus.qX = qus.pQuestionIcon->posX;
	qus.qY = qus.pQuestionIcon->posY;
	qus.textX = qus.qX;
	qus.textY = qus.qY;

	// フラグの初期化
	qus.isHovered = false;
	qus.isShowExplain = false;

	return qus;
}

//初期化関数
bool SceneMain::initialize()
{
	srand((unsigned int)time(nullptr));
	// --- ウェーブマネージャー ---
	waveManager = new WaveManager();

	InitializeVariables();
	InitializePlayer();
	InitializeEnemies();
	InitializeField();
	InitializeEffects();
	InitializeUI();
	InitializeMissionUI();
	InitializePauseUI();
	InitializeUpgradeUI();
	InitializeExplanationUI();
	InitializeFont();
	InitializeSound();
	InitializeTutorial();

	return true;
}
//=========================================
// 初期化関数
//=========================================

void SceneMain::RegisterCharacter(vnCharacter* character)
{
	registerObject(character);

	for (int i = 0; i < character->getPartsNum(); i++)
	{
		registerObject(character->getParts(i));
	}
}

//変数の初期化
void SceneMain::InitializeVariables()
{	
	m_gameState = Opening;

	m_isEndless = g_isEndless;

	radius = defualtRadius;
	theta = defualtTheta;
	phi = defualtPhi;

	m_radius = radius;
	m_theta = theta;
	m_phi = phi;

	isWaveClear = false;

	oldWaveCount = 1;
	m_comboCount = 0;
	m_comboScale = 1.0f;
	m_killCounter = 0;

	totalClearTime = 0.0f;
	isTimerActive = false;

	backGroundBlackScale = 1.0f;
	isGameFinish = false;

	FenceRadius = defalutFenceRadius;


}

//プレイヤーの初期化
void SceneMain::InitializePlayer()
{
	// --- プレイヤー ---
	m_pNewPlayer = new NewPlayerClass();
	m_pNewPlayer->SetModel(new vnCharacter(L"data/model/Brid/brid_animation_new/", L"brid.bone"));
	m_pNewPlayer->SetMeteorModel(new vnModel(L"data/model/Brid/brid_animation_new/", L"KaraDown.vnm"));
	m_pNewPlayer->SetUpKaraModel(new vnModel(L"data/model/Brid/", L"KaraUp.vnm"));

	RegisterCharacter(m_pNewPlayer->GetModel());

	m_pNewPlayer->GetModel()->setScale(playerModelScale, playerModelScale, playerModelScale);

	registerObject(m_pNewPlayer->GetMeteorModel());
	registerObject(m_pNewPlayer->GetUpKaraModel());

	m_pGameOverPlayer = new vnSprite(offScreen, 200, 128, 128, L"data/image/プレイヤーゲームオーバー.png");
	registerObject(m_pGameOverPlayer);

	// --- 弾 ---
	m_pBullet = new Bullet();
	m_pBullet->SetModel(new vnCharacter(L"data/model/Brid/KaraUp/", L"KaraUp.bone"));
	m_pBullet->GetModel()->SetAllPartsDiffuse(V_GAME_COLOR_YELLOW, 1);

	RegisterCharacter(m_pBullet->GetModel());

	m_pBullet->GetModel()->setPositionY(-10.0f);
	m_pBullet->GetModel()->setScale(2, 2, 2);

	m_pNewPlayer->SetBulletClass(m_pBullet);

}

//敵の初期化
void SceneMain::InitializeEnemies()
{
	enemyPool = &EnemyPool::GetInstance();

	int totalEnemyCount = 0;

	for (int i = 0; i < NewEnemyClass::MasterTableCount; i++)
	{
		const auto& data = NewEnemyClass::MasterTable[i];

		for (int j = 0; j < data.maxCount + 1; j++)
		{
			NewEnemyClass* enemy =
				NewEnemyClass::CreateEnemyByType(data.type);

			if (!enemy)
			{
				continue;
			}

			if (j < data.maxCount)
			{
				bool isLeader = (totalEnemyCount % leaderCount == 0);

				if (isLeader)
				{
					m_leaderCount++;
				}

				SetupEnemy(enemy, data, isLeader, false);

				enemyPool->AddEnemy(enemy);
				totalEnemyCount++;
			}
			else
			{
				SetupEnemy(enemy, data, true, true);
				enemyPool->AddEnemy(enemy);
			}
		}
	}

	enemyPool->UnlockEnemyType(NewEnemyClass::EnemyType::GHOST);
	enemyPool->UnlockEnemyType(NewEnemyClass::EnemyType::MUSHROOM);
}

//地形の初期化
void SceneMain::InitializeField()
{
	// --- スカイドーム ---
	pSky = new vnModel(L"data/model/", L"skydome.vnm");
	pSky->setLighting(false);
	pSky->setScale(5.0f, 5.0f, 5.0f);
	pSky->setZWrite(false);
	registerObject(pSky);

	// --- フェンス ---
	for (int i = 0; i < FENCE_NUM_MAIN; i++) {
		pFence[i] = new vnModel(L"data/model/", L"fence.vnm");

		float degree = 360.0f / FENCE_NUM_MAIN * (float)i;
		float radian = degree * 3.141592f / 180.0f;

		float x = sin(radian) * FenceRadius;
		float z = cos(radian) * FenceRadius;

		pFence[i]->setPosition(x, 0.0, z);
		pFence[i]->setRotationY(radian);

		registerObject(pFence[i]);
	}

	// --- 木の生成 ---
#if ENABLE_TREE_DELETE
	for (int line = 0; line < 2; line++)
	{
		for (int i = 0; i < TREE_NUM; i++)
		{
			int index = line * TREE_NUM + i;
			pTree[index] = new vnModel(L"data/model/ST_Tree_01_LOD0/", L"ST_Tree_01_LOD0.vnm");

			float radian = (2.0f * 3.14159f / (float)TREE_NUM) * (float)i;
			if (line == 1)
			{
				float halfGap = (2.0f * 3.14159f / (float)TREE_NUM / 2.0f);
				radian += halfGap;
			}

			float lineOffset = (line == 0) ? 0.0f : 7.0f;
			float totalRadius = FenceRadius + treeRadius + lineOffset;

			float x = sinf(radian) * totalRadius;
			float z = cosf(radian) * totalRadius;

			pTree[index]->setPosition(x, 0.0f, z);
			pTree[index]->setRotationY(radian);

			float treeSize = 3.5f;
			pTree[index]->setScale(treeSize, treeSize, treeSize);

			registerObject(pTree[index]);
		}
	}
#else
	for (int i = 0; i < TREE_NUM; i++)
	{
		pTree[i] = nullptr;
	}
	for (int i = 0; i < TREE_NUM; i++)
	{
		int index = i;
		pTree[index] = new vnModel(L"data/model/ST_Tree_01_LOD0/", L"ST_Tree_01_LOD0.vnm");

		float radian = (2.0f * 3.14159f / (float)TREE_NUM) * (float)i;
		float totalRadius = FenceRadius + treeRadius;

		float x = sinf(radian) * totalRadius;
		float z = cosf(radian) * totalRadius;

		pTree[index]->setPosition(x, 0.0f, z);
		pTree[index]->setRotationY(radian);

		float treeSize = 4.0f;
		pTree[index]->setScale(treeSize, treeSize, treeSize);

		registerObject(pTree[index]);
	}
#endif

	// --- 地面 ---
	pGround = new vnModel(L"data/model/Ground/", L"Ground.vnm");
	//pGround = new vnModel(L"data/model/magma/", L"magma.vnm");
	pGround->setScale(30.0f, 0.5f, 30.0f);
	registerObject(pGround);

	// --- ブロックマネージャー ---
	m_pBlockManager = new BlockManager();
	//通常ブロックを生成
	for (int i = 0; i < BlockManager::GetMaxBlocksNum(); i++)
	{
		TerrainBlock* pBlock = new TerrainBlock();
		pBlock->SetModel(new vnCharacter(L"data/model/Block/", L"Block.bone"));
		pBlock->SetIsMagma(false);

		registerObject(pBlock->GetModel());
		for (int j = 0; j < pBlock->GetModel()->getPartsNum(); j++)
		{
			registerObject(pBlock->GetModel()->getParts(j));
		}

		pBlock->GetModel()->setRenderEnable(false);
		for (int j = 0; j < pBlock->GetModel()->getPartsNum(); j++)
		{
			pBlock->GetModel()->getParts(j)->setRenderEnable(false);
		}

		m_pBlockManager->AddBlock(pBlock);
	}
	//マグマブロックの生成
	for (int i = 0; i < BlockManager::GetMaxBlocksNum(); i++)
	{
		TerrainBlock* pBlock = new TerrainBlock();
		pBlock->SetModel(new vnCharacter(L"data/model/magma/", L"magma.bone"));

		pBlock->SetIsMagma(true);
		registerObject(pBlock->GetModel());
		for (int j = 0; j < pBlock->GetModel()->getPartsNum(); j++)
		{
			registerObject(pBlock->GetModel()->getParts(j));
		}

		pBlock->GetModel()->setRenderEnable(false);
		for (int j = 0; j < pBlock->GetModel()->getPartsNum(); j++)
		{
			pBlock->GetModel()->getParts(j)->setRenderEnable(false);
		}

		m_pBlockManager->AddMagmaBlock(pBlock);
	}


}

//エフェクトの初期化
void SceneMain::InitializeEffects()
{
	vnEmitter::stEmitterDesc desc;

	// 通常エミッター（星など）
	swprintf_s(desc.Texture, L"%s", L"data/image/cfxr star_new.png");
	desc.ColorMax = V_GAME_COLOR_YELLOW;
	desc.SizeMin = 1.0f;
	desc.SizeMax = 3.0f;
	desc.SpeedMin = 0.08f;
	desc.SpeedMax = 0.1f;
	pEmitter = new vnEmitter(&desc);
	pEmitter->setEmit(false, 0);
	registerObject(pEmitter);
	pEmitter->setZWrite(false);

	// 土埃エミッター
	swprintf_s(desc.Texture, L"data/image/cfxr cloud blur.png");
	desc.ColorMax = V_GAME_COLOR_DARK_GRAY;
	desc.SizeMin = 0.3f;
	desc.SizeMax = 0.5f;
	desc.SpeedMin = 0.08f;
	desc.SpeedMax = 0.1f;
	pDustEmitter = new vnEmitter(&desc);
	pDustEmitter->setEmit(false, 0);
	registerObject(pDustEmitter);
	pDustEmitter->setZWrite(false);
	pDustEmitter->setParent(m_pNewPlayer->GetModel());
}

//UIの初期化
void SceneMain::InitializeUI()
{
	float screenWidth = vnMainFrame::screenWidth;
	float screenHeight = vnMainFrame::screenHeight;


	//================================================
	// --- HPバー ---
	//================================================
	pHpBarBackBlack = new vnSprite(newBarPosXHp, heigtYHp, barBackWidthHp, barBackHeightHp, NULL);
	pHpBarBackBlack->setColor(V_GAME_COLOR_BLACK);
	pHpBarBackBlack->setSkewX(12.0f);
	registerObject(pHpBarBackBlack);

	pHpBarBack = new vnSprite(newBarPosXHp, heigtYHp, barWidthHp, barHeightHp, NULL);
	pHpBarBack->setColor(V_GAME_COLOR_RED);
	pHpBarBack->setSkewX(9.8f); 
	registerObject(pHpBarBack);

	pHpBarFront = new vnSprite(newBarPosXHp, heigtYHp, barWidthHp, barHeightHp, NULL);
	pHpBarFront->setColor(V_GAME_COLOR_GREEN);
	pHpBarFront->setSkewX(9.8f);
	registerObject(pHpBarFront);
	
	//================================================
	// --- Expバー ---
	//================================================
	pExpBarBackBlack = new vnSprite(barLeftEdgeExp + (maxWExp * 0.5f), heightYExp, maxWExp + 10.0f, barThick + 4.0f, NULL);
	pExpBarBackBlack->setColor(V_GAME_COLOR_BLACK);
	pExpBarBackBlack->setSkewX(8.0f);
	registerObject(pExpBarBackBlack);

	pExpBarBack = new vnSprite(barLeftEdgeExp + (maxWExp * 0.5f), heightYExp, maxWExp, barThick, NULL);
	pExpBarBack->setColor(V_GAME_COLOR_WHITE);
	pExpBarBack->setSkewX(8.0f);
	registerObject(pExpBarBack);

	pExpBarFront = new vnSprite(barLeftEdgeExp, heightYExp, maxWExp, barThick, NULL);
	pExpBarFront->setColor(V_GAME_COLOR_CYAN);
	pExpBarFront->setSkewX(8.0f);
	registerObject(pExpBarFront);


	//================================================
	// --- プレイヤーアイコン ---
	//================================================
	pIconPlayer = new vnSprite(40, 40, 70, 70, L"data/image/Icon.png");
	registerObject(pIconPlayer);

	setHPbarRender(false);
	SetExpbarRender(false);
	
	//===============================================================
	// --- コンボ表示 ---
	//===============================================================

	//コンボ（Combo）の文字
	pComboWord = new vnSprite(offScreen, offScreen, 128, 96, L"data/image/combo.png");
	pComboWord->setColor(V_GAME_COLOR_RED);
	registerObject(pComboWord);
	//数値（３桁まで）
	for (int i = 0; i < 3; i++) {       // 桁 (0:百, 1:十, 2:一)
		for (int j = 0; j < 10; j++) {  // 数字 (0〜9)
			wchar_t path[64];
			swprintf(path, 64, L"data/image/num%d.png", j);

			pComboSprites[i][j] = new vnSprite(offScreen, offScreen, 96, 96, path);
			registerObject(pComboSprites[i][j]);
			pComboSprites[i][j]->setColor(V_GAME_COLOR_GOLD);
		}
	}



	//===============================================================
	// --- 文字用背景(左側にある操作説明の後ろにある背景)---
	//===============================================================
	m_pUIBackGroundBlack[0] = new vnSprite(1280 - 1120, 720.0f - 450.0f, 256 * 1.2f, 512 * 0.7, L"BackGroundBlack.png");
	m_pUIBackGroundBlack[0]->setColor(V_GAME_COLOR_BLACK);
	m_pUIBackGroundBlack[0]->setAlpha(0.6f);
	registerObject(m_pUIBackGroundBlack[0]);
	m_pUIBackGroundBlack[0]->setRenderEnable(true);

	//右下側にあるメッセージの表示の背景
	m_pUIBackGroundBlack[1] = new vnSprite(1280 - 170, 720.0f - 110.0f, 256 * 1.2f, 512 * 0.3, L"BackGroundBlack.png");
	m_pUIBackGroundBlack[1]->setColor(V_GAME_COLOR_BLACK);
	m_pUIBackGroundBlack[1]->setAlpha(0.6f);
	registerObject(m_pUIBackGroundBlack[1]);
	m_pUIBackGroundBlack[1]->setRenderEnable(true);
	enemyPool->SetUIBackGroundBlack(m_pUIBackGroundBlack[1]);


	//============================================================
	// --- キーボード画像 ---
	//============================================================
	pImageW   = new vnSprite(-100, 200, 64, 64, L"data/image/keyboard_w_outline.png"); registerObject(pImageW);
	pImageA   = new vnSprite(-100, 200, 64, 64, L"data/image/keyboard_a_outline.png"); registerObject(pImageA);
	pImageS   = new vnSprite(-100, 200, 64, 64, L"data/image/keyboard_s_outline.png"); registerObject(pImageS);
	pImageD   = new vnSprite(-100, 200, 64, 64, L"data/image/keyboard_d_outline.png"); registerObject(pImageD);
	pImageE   = new vnSprite(-100, 200, 64 * 0.7f, 64 * 0.7f, L"data/image/keyboard_e_outline.png"); registerObject(pImageE);
	pImageQ	  = new vnSprite(-100, 200, 64 * 0.7f, 64 * 0.7f, L"data/image/keyboard_q_outline.png"); registerObject(pImageQ);
	pImageTab = new vnSprite(-100, 200, 64 * 1.5f, 64 * 1.5f, L"data/image/keyboard_tab.png"); registerObject(pImageTab);



	//===================================================================
	// --- スキルUI設定 ---
	//===================================================================
	float centerX = barLeftEdgeAreaSkill + (maxWAreaSkill * 0.5f);

	//============================================================
	// 範囲攻撃(スキルバー)
	//============================================================
	pAreaAtkBtnBackBlack = new vnSprite(centerX, heightYAreaSkill, maxWAreaSkill + 10.0f, skillBarThick + 6.0f, NULL);
	pAreaAtkBtnBackBlack->setColor(V_GAME_COLOR_BLACK);
	registerObject(pAreaAtkBtnBackBlack);

	pAreaAtkBtnBack = new vnSprite(centerX, heightYAreaSkill, maxWAreaSkill, skillBarThick, NULL);
	pAreaAtkBtnBack->setColor(V_GAME_COLOR_BLUEBLACK);
	registerObject(pAreaAtkBtnBack);

	pAreaAtkBtnFront = new vnSprite(centerX, heightYAreaSkill, maxWAreaSkill, skillBarThick, NULL);
	pAreaAtkBtnFront->setColor(V_GAME_COLOR_CYAN);
	registerObject(pAreaAtkBtnFront);

	pAreaSkillIcon = new vnSprite(centerX - 130, heightYAreaSkill - 15, 64 * 1.2f, 64 * 1.2f, L"data/image/areaAttackIconImg2.png");
	pAreaSkillIcon->setColor(V_GAME_COLOR_WHITE);
	pAreaSkillIcon->setAlpha(1.0f);
	registerObject(pAreaSkillIcon);

	//初期状態では黒に塗りつぶす為初期値を保存しておく
	m_areaAtkUIColor.colorBackBlack = V_GAME_COLOR_BLACK;
	m_areaAtkUIColor.colorBack = V_GAME_COLOR_BLUEBLACK;
	m_areaAtkUIColor.colorFront = V_GAME_COLOR_CYAN;
	m_areaAtkUIColor.colorIcon = V_GAME_COLOR_WHITE;

	//============================================================
	// 引き寄せ攻撃(スキルUIバー)
	//============================================================
	pPullBtnBackBlack = new vnSprite(centerX, heightYPullSkill, maxWAreaSkill + 10.0f, skillBarThick + 6.0f, NULL);
	pPullBtnBackBlack->setColor(V_GAME_COLOR_BLACK);
	registerObject(pPullBtnBackBlack);

	pPullBtnBack = new vnSprite(centerX, heightYPullSkill, maxWAreaSkill, skillBarThick, NULL);
	pPullBtnBack->setColor(V_GAME_COLOR_BLUEBLACK);
	registerObject(pPullBtnBack);

	pPullBtnFront = new vnSprite(centerX, heightYPullSkill, maxWAreaSkill, skillBarThick, NULL);
	pPullBtnFront->setColor(V_GAME_COLOR_CYAN);
	registerObject(pPullBtnFront);

	pPullSkillIcon = new vnSprite(centerX - 130, heightYPullSkill - 15, 64 * 1.2f, 64 * 1.2f, L"data/image/pullAttackIconImg2.png");
	pPullSkillIcon->setColor(V_GAME_COLOR_WHITE);
	pPullSkillIcon->setAlpha(1.0f);
	registerObject(pPullSkillIcon);

	//初期状態では黒に塗りつぶす為初期値を保存しておく
	m_pullUIColor.colorBackBlack = V_GAME_COLOR_BLACK;
	m_pullUIColor.colorBack = V_GAME_COLOR_BLUEBLACK;
	m_pullUIColor.colorFront = V_GAME_COLOR_CYAN;
	m_pullUIColor.colorIcon = V_GAME_COLOR_WHITE;
	//==============================================================

	//スキルバーの非表示
	SetSkillUIRender(false);

	//===================================================================


	//===============================================================
	//時間表示の後ろの背景
	//===============================================================
	pTimeBackGround = new vnSprite(screenWidth / 2, timeBackGround_y, timeBackGround_w, timeBackGround_h, L"data/image/TimeBackGround.png");
	registerObject(pTimeBackGround);
	pTimeBackGround->setRenderEnable(false);

}

//ミッションのUIの初期化
void SceneMain::InitializeMissionUI()
{
	AddMissionImage();
	// ミッション内容のテキストを入れる
	m_missionUI[(int)TutorialMission::StartMessage].text = L"敵を１体倒そう！";
	m_missionUI[(int)TutorialMission::WaitSkill_Area].text = L"範囲攻撃を使おう！";
	m_missionUI[(int)TutorialMission::WaitSkill_Pull].text = L"引き寄せ攻撃を使おう！";
	m_missionUI[(int)TutorialMission::WaitLevelUp].text = L"レベルアップしよう！";
	m_missionUI[(int)TutorialMission::WaitEnemyRange].text = L"敵を特攻状態にしよう！";
	m_missionUI[(int)TutorialMission::WaitEnemyCount].text = L"敵を合計200体倒そう！";


}

//ポーズ中に出るUI
void SceneMain::InitializePauseUI()
{
	float centerX = barLeftEdgeAreaSkill + (maxWAreaSkill * 0.5f);
	float screenWidth= vnMainFrame::screenWidth;
	float screenHeight = vnMainFrame::screenHeight;

	// ポーズ背景幕
	m_pUIBackGroundBlackPause = new vnSprite(screenWidth / 2, screenHeight / 2, screenWidth, screenHeight, L"BackGroundBlack.png");
	m_pUIBackGroundBlackPause->setColor(V_GAME_COLOR_BLACK);
	m_pUIBackGroundBlackPause->setAlpha(0.6f);
	registerObject(m_pUIBackGroundBlackPause);
	m_pUIBackGroundBlackPause->setRenderEnable(false);

	// ポーズフレーム
	m_pPauseFrame = new vnSprite(screenWidth / 2, screenHeight / 2, screenWidth, screenHeight, L"data/image/Pauseframe.png");
	registerObject(m_pPauseFrame);
	m_pPauseFrame->setRenderEnable(false);

	m_pPauseFrame2 = new vnSprite(screenWidth / 2, screenHeight / 2, screenWidth, screenHeight, L"data/image/Pauseframe2.png");
	registerObject(m_pPauseFrame2);
	m_pPauseFrame2->setRenderEnable(false);

	// グラフバー設定
	EnemyPool::UIBar meleeUIbar;
	EnemyPool::UIBar rangeUIbar;
	EnemyPool::UIBar pullUIbar;

	CreateUIBar(meleeUIbar, centerX + 450, 380, baseBarWidth, baseBarHeight + 2);
	CreateUIBar(rangeUIbar, centerX + 450, 415, baseBarWidth, baseBarHeight + 2);
	CreateUIBar(pullUIbar, centerX + 450, 450, baseBarWidth, baseBarHeight + 2);

	enemyPool->SetMeleeBar(meleeUIbar);
	enemyPool->SetRangeBar(rangeUIbar);
	enemyPool->SetPullBar(pullUIbar);

	// 各種アイコン
	float offsetSize = 0.8f;
	float offsetSizeGhost = 1.5f;
	enemyPool->SetImageTab(new vnSprite(-100, 200, 64 * 1.5f, 64 * 1.5f, L"data/image/keyboard_tab.png"));
	//enemyPool->SetImageTab(pImageTab);
	enemyPool->SetImageA(new vnSprite(-100, 200, 64 * offsetSize, 64 * offsetSize, L"data/image/keyboard_a.png"));
	enemyPool->SetImageD(new vnSprite(-100, 200, 64 * offsetSize, 64 * offsetSize, L"data/image/keyboard_d.png"));
	enemyPool->SetImageSlash(new vnSprite(-100, 200, 64 * offsetSize, 64 * offsetSize, L"data/image/flair_disabled_line_outline.png"));
	enemyPool->SetImageGhost(new vnSprite(-100, 200, 64 * offsetSizeGhost, 64 * offsetSizeGhost, L"data/image/ghostImage.png"));
	registerObject(enemyPool->GetImageTab());
	registerObject(enemyPool->GetImageA());
	registerObject(enemyPool->GetImageD());
	registerObject(enemyPool->GetImageSlash());
	registerObject(enemyPool->GetImageGhost());

	// 吹き出し
	enemyPool->SetMeleeQus(CreateQuestionUI(L"：特攻確率に加算", GAME_COLOR_AMBER, 0.6f));
	enemyPool->SetRangeQus(CreateQuestionUI(L"：基本速度に加算", GAME_COLOR_NEON_MAGENTA, 0.6f));
	enemyPool->SetPullQus(CreateQuestionUI(L"：無効確率に加算", GAME_COLOR_SKY_NEON, 0.6f));




}



//レベルアップ時に出るUI関連
void SceneMain::InitializeUpgradeUI()
{
	float screenWidth = vnMainFrame::screenWidth;
	float screenHeight = vnMainFrame::screenHeight;

	// 黒い幕
	pBackGroundBlack = new vnSprite(screenWidth / 2, screenHeight / 2, screenWidth, screenHeight, L"BackGroundBlack.png");
	pBackGroundBlack->setColor(V_GAME_COLOR_BLACK);
	registerObject(pBackGroundBlack);
	pBackGroundBlack->setRenderEnable(false);
	pBackGroundBlack->setScale(0);

	// 経験値マネージャー
	m_pExpManager = new ExperienceManager;
	m_pExpManager->SetPlayer(m_pNewPlayer);

	// アップグレードUI
	m_pUpgradeUI = new UpgradeSelectionUI;
	for (int i = 0; i < 3; i++)
	{
		const auto& resource = upgradeUIResources[i];

		m_pUpgradeUI->SetFreamImg(i, new vnSprite(uiHidePosX, uiHidePosY, freamImgW, freamImgH, resource.framePath));
		m_pUpgradeUI->SetBackGroundImg(i, new vnSprite(uiHidePosX, uiHidePosY, backGroundImgW, backGroundImgH, resource.backGroundPath));
		m_pUpgradeUI->SetMainImg(i, new vnSprite(uiHidePosX, uiHidePosY, mainImgW, mainImgH, resource.mainPath));

		registerObject(m_pUpgradeUI->GetFreamImg(i));
		registerObject(m_pUpgradeUI->GetBackGroundImg(i));
		registerObject(m_pUpgradeUI->GetMainImg(i));
	}
}

//説明に関するUI
void SceneMain::InitializeExplanationUI()
{
	float screenWidth = vnMainFrame::screenWidth;
	float screenHeight = vnMainFrame::screenHeight;


	//===============================================================
	// --- 説明に関するUI ---
	//===============================================================

	//後ろの背景
	m_messageBackground = new vnSprite(screenWidth / 2, screenHeight / 2, screenWidth, screenHeight, L"BackGroundBlack_maru.png");
	registerObject(m_messageBackground);
	m_messageBackground->setRenderEnable(false);
	m_messageBackground->setColor(V_GAME_COLOR_BLACK);
	m_messageBackground->setAlpha(0.6f);


	//=================================================================
	// --- チュートリアル中に出てくる説明の画像 ---
	//=================================================================
	//EnemyLeader,		//リーダーを倒したときに出てくる敵の説明
	//Exp,				//敵を倒したときに出てくる説明（経験値獲得）
	//LevelUp,			//レベルアップ時に出てくる
	//Skills,			//スキルを獲得したときの説明
	//PlayerOperation,	//プレイヤーの基本操作説明
	//PlayerDamage,		//プレイヤーがダメージを受けたときの説明

	AddExplanationImage(ExplanationType::EnemyLeader, L"data/image/tutorial_ExplainEnemyLeader.png");
	AddExplanationImage(ExplanationType::EnemyLeader, L"data/image/tutorial_ExplainEnemyLeader2.png");

	AddExplanationImage(ExplanationType::Exp, L"data/image/tutorial_ExplainExp.png");
	
	AddExplanationImage(ExplanationType::LevelUp, L"data/image/tutorial_ExplainLevelUp.png");
	AddExplanationImage(ExplanationType::LevelUp, L"data/image/tutorial_ExplainLevelUp2.png");

	AddExplanationImage(ExplanationType::Skills, L"data/image/tutorial_ExplainSkills.png");

	AddExplanationImage(ExplanationType::PlayerOperation, L"data/image/tutorial_ExplainPlayerOperation.png");
	AddExplanationImage(ExplanationType::PlayerOperation, L"data/image/tutorial_ExplainPlayerOperation2.png");

	AddExplanationImage(ExplanationType::PlayerDamage, L"data/image/tutorial_ExplainPlayerDamage.png");

	//設定した説明画像をまとめて登録
	for (int i = 0; i < (int)ExplanationType::MaxNum; i++)
	{
		for (size_t j = 0; j < m_explanationUI[i].images.size(); j++)
		{
			registerObject(m_explanationUI[i].images[j]);
		}
		m_explanationUI[i].position_x = screenWidth / 2.0f;
		m_explanationUI[i].position_y = screenHeight / 2.0f;
		m_explanationUI[i].visible = false;
	}



	//===============================================================
	// --- ボタン設定（タイトルに戻るボタンなど）
	//===============================================================
	//基本ボタン
	float threeLetters = -15.0f;
	{
		//３文字のもののフォント位置の調整

		//タイトルに戻るボタン
		InitButton(UIButton::TITLEBACK,
			titleBack_button_x, titleBack_button_y, L"タイトル");
		//閉じるボタン
		InitButton(UIButton::MESSAGE_CLOSE,
			message_close_button_x, message_close_button_y, L"閉じる", threeLetters);
		//戻るボタン
		InitButton(UIButton::MESSAGE_LEFT,
			message_left_button_x, message_left_button_y, L"戻る", -10.0f);
		//進むボタン
		InitButton(UIButton::MESSAGE_RIGHT,
			message_right_button_x, message_right_button_y, L"次へ", -10.0f);
		//はいボタン
		InitButton(UIButton::MESSAGE_YES,
			message_yes_button_x, message_yes_button_y, L"はい" ,- 10.0f);
		//いいえボタン
		InitButton(UIButton::MESSAGE_NO,
			message_no_button_x, message_no_button_y, L"いいえ", threeLetters);
		//ポーズ中のゲームに戻るボタン
		InitButton(UIButton::PLAYGAME_BACK,
			playgame_back_button_x, playgame_back_button_y, L"ゲームへ");
		//ポーズ中のゲームに戻るボタン
		InitButton(UIButton::MESSAGE_GAMEPLAY,
			playgame_back_button_x, playgame_back_button_y, L"本番へ");
	}
	//チュートリアル振り返りボタン
	{
		//チュートリアル振り返り
		InitButton(UIButton::TUTORIAL_REVIEW,
			tutorial_review_button_x, tutorial_review_button_y, L"振り返り");

		//振り返りボタンを押した後に出る戻るボタン
		InitButton(UIButton::TUTORIAL_BACK,
			tutorial_back_button_x, tutorial_back_button_y, L"戻る",-10.0f);

		//敵の説明
		InitButton(UIButton::TUTORIAL_ENEMY_LEADER,
			tutorial_enemy_leader_button_x, tutorial_enemy_leader_button_y, L"敵",-5.0f);

		//経験値の説明
		InitButton(UIButton::TUTORIAL_EXP,
			tutorial_exp_button_x, tutorial_exp_button_y, L"経験値", threeLetters);

		//レベルアップの説明
		InitButton(UIButton::TUTORIAL_LEVEL_UP,
			tutorial_level_up_button_x, tutorial_level_up_button_y, L"レベルアップ",-30.0f);

		//スキルの説明
		InitButton(UIButton::TUTORIAL_SKILLS,
			tutorial_skills_button_x, tutorial_skills_button_y, L"スキル", threeLetters);

		//プレイヤー操作の説明
		InitButton(UIButton::TUTORIAL_PLAYER_OPERATION,
			tutorial_player_operation_button_x, tutorial_player_operation_button_y, L"基本説明");

		//プレイヤーダメージの説明
		InitButton(UIButton::TUTORIAL_PLAYER_DAMAGE,
			tutorial_player_damage_button_x, tutorial_player_damage_button_y, L"ダメージ");
	}


}

//ボタンの初期化
void SceneMain::InitButton(
	UIButton type,
	float x, float y,
	const wchar_t* text,
	float fontOffsetX)
{

	m_buttonData[(int)type].sprite =
		new vnSprite(x, y, button_w, button_h, L"data/image/無題.png");

	registerObject(m_buttonData[(int)type].sprite);

	m_buttonData[(int)type].sprite->setRenderEnable(false);
	m_buttonData[(int)type].position_x = x;
	m_buttonData[(int)type].position_y = y;
	m_buttonData[(int)type].font_pos_offset_x = fontOffsetX;
	m_buttonData[(int)type].text = text;
	m_buttonData[(int)type].se_id = SE_TITLE_CHANGEPAGE;
	m_buttonData[(int)type].visible = false;
}

//チュートリアルの説明の画像の入れる
void SceneMain::AddExplanationImage(
	ExplanationType type,
	const wchar_t* path)
{
	float screenWidth = vnMainFrame::screenWidth;
	float screenHeight = vnMainFrame::screenHeight;

	float offset_UI_size = 0.85f;

	vnSprite* image = new vnSprite(
		screenWidth / 2,
		screenHeight / 2,
		screenWidth * offset_UI_size,
		screenHeight * offset_UI_size,
		path);

	image->setRenderEnable(false);

	m_explanationUI[(int)type].images.push_back(image);


}
//チュートリアルのミッションUIの画像の入れる
void SceneMain::AddMissionImage()
{
	//ミッションのUIは同じ画像を使う
	float screenWidth = vnMainFrame::screenWidth;
	float screenHeight = vnMainFrame::screenHeight;

	float offset_UI_size = 0.85f;
	float sizeY = 40;
	float posX = 1080;
	for (int i = 0; i < (int)TutorialMission::MaxNum; i++)
	{
		// 背景＋フレーム
		m_missionUI[i].sprite_back = new vnSprite(
			posX,
			30+(i* sizeY),
			400,
			sizeY,
			L"data/image/mission_frame.png");

		m_missionUI[i].sprite_back->setRenderEnable(false);
		m_missionUI[i].sprite_back->setAlpha(0.6f);
		registerObject(m_missionUI[i].sprite_back);

		m_missionUI[i].position_x = posX;
		m_missionUI[i].position_y = 30 + (i * sizeY);

		// チェック画像
		m_missionUI[i].sprite_check = new vnSprite(
			m_missionUI[i].position_x-150,
			m_missionUI[i].position_y,
			sizeY,
			sizeY,
			L"data/image/mark_check.png");

		m_missionUI[i].sprite_check->setRenderEnable(false);
		m_missionUI[i].sprite_check->setAlpha(0.6f);
		registerObject(m_missionUI[i].sprite_check);
	}
}



//フォントの初期化
void SceneMain::InitializeFont()
{
	FontNum = vnFont::getFontNum();
	textFormat = new IDWriteTextFormat * [FontNum];

	for (int i = 0; i < FontNum; i++)
	{
		textFormat[i] = vnFont::create(vnFont::getFontName(i), 50);
	}
}

//音の初期化
void SceneMain::InitializeSound()
{
	soundManager = std::make_unique<SoundManager>();
	enemyPool->SetSoundManager(soundManager.get());
	m_pNewPlayer->SetSoundManager(soundManager.get());
}

void SceneMain::InitializeTutorial()
{
	// タイトル画面の設定を引き継ぐ
	m_isTutorial = g_isTutorial;

	if (!m_isTutorial)
	{
		for (int i = 0; i < (int)ExplanationType::MaxNum; i++)
		{
			m_explanationUI[i].isOne = true;
		}

		return;
	}

	// チュートリアル開始
	m_state_tutorial = TutorialState::None;
}


//初期化==============================================================================



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
	deleteObject(m_pGameOverPlayer);
	m_pGameOverPlayer = nullptr;


	// --- ポーズ中の棒グラフ ---
	auto& meleebar = enemyPool->GetMeleeBar();
	deleteObject(meleebar.pFront);
	deleteObject(meleebar.pBack);
	deleteObject(meleebar.pBackBlack);

	auto& rangebar = enemyPool->GetMeleeBar();
	deleteObject(rangebar.pFront);
	deleteObject(rangebar.pBack);
	deleteObject(rangebar.pBackBlack);

	auto& pullbar = enemyPool->GetMeleeBar();
	deleteObject(pullbar.pFront);
	deleteObject(pullbar.pBack);
	deleteObject(pullbar.pBackBlack);

	deleteObject(enemyPool->GetImageTab());
	deleteObject(enemyPool->GetImageA());
	deleteObject(enemyPool->GetImageD());
	deleteObject(enemyPool->GetImageSlash());
	deleteObject(enemyPool->GetImageGhost());

	enemyPool->ReleaseQuestionUI(this);

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
			if (enemy->GetChargeMark())
			{
				deleteObject(enemy->GetChargeMark());
			}
			if (enemy->GetPanicMark())
			{
				deleteObject(enemy->GetPanicMark());
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

#if ENABLE_TREE_DELETE
	for (int i = 0; i < TREE_NUM*2; i++) {
		deleteObject(pTree[i]);
		pTree[i] = nullptr;
	}
#else
	for (int i = 0; i < TREE_NUM; i++) {
		deleteObject(pTree[i]);
		pTree[i] = nullptr;
	}
#endif

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
	deleteObject(pImageTab);

	pImageW = nullptr;
	pImageA = nullptr;
	pImageS = nullptr;
	pImageD = nullptr;
	pImageE = nullptr;
	pImageQ = nullptr;
	pImageTab = nullptr;

	for (int i = 0; i < 2; i++)
	{
		deleteObject(m_pUIBackGroundBlack[i]);
		m_pUIBackGroundBlack[i] = nullptr;
	}

	//ミッションの画像
	for (int i = 0; i < (int)TutorialMission::MaxNum; i++)
	{
		if (m_missionUI[i].sprite_back != nullptr)
		{
			deleteObject(m_missionUI[i].sprite_back);
			m_missionUI[i].sprite_back= nullptr;
		}
		if (m_missionUI[i].sprite_check!= nullptr)
		{
			deleteObject(m_missionUI[i].sprite_check);
			m_missionUI[i].sprite_check=nullptr;
		}
	}

	// --- ポーズ中に出る奴 ---
	deleteObject(m_pUIBackGroundBlackPause);
	m_pUIBackGroundBlackPause = nullptr;
	deleteObject(m_pPauseFrame);
	m_pPauseFrame = nullptr;
	deleteObject(m_pPauseFrame2);
	m_pPauseFrame2 = nullptr;

	deleteObject(pBackGroundBlack);
	pBackGroundBlack = nullptr;

	deleteObject(pTimeBackGround);
	pTimeBackGround = nullptr;

	deleteObject(m_messageBackground);
	m_messageBackground = nullptr;

	//ボタン
	for (int i = 0; i < (int)UIButton::MaxNum; i++)
	{
		deleteObject(m_buttonData[i].sprite);
	}

	for (int i = 0; i < (int)ExplanationType::MaxNum; i++)
	{
		for (size_t j = 0; j < m_explanationUI[i].images.size(); j++)
		{
			deleteObject(m_explanationUI[i].images[j]);
		}
	}

	//レベルアップ時に出るUI
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


}
//処理関数
void SceneMain::execute()
{

	float deltaTime = vnScene::getDeltaTime();
	switch (m_gameState)
	{
	case Opening:
		//--カメラの初期位置設定
		StartCameraRote();

		HandleBackgroundFade(false, backGroundBlackScale,blackBackSpeed);

		break;
	case IdelPlay:
	{
		soundManager->PlayBGM(BGM_GAME);

		UpdateIdel();
		// 入力待ち
		break;
	}

	case Play:
	{
		soundManager->PlayBGM(BGM_GAME);

		UpdatePlay(deltaTime);
		
		break;
	}

	case LevelUp:
		//プレイヤーの正面にカメラが行くように基準を取る
		m_levelUpCameraTargetTheta =
			m_pNewPlayer->GetModel()->getRotationY()
			+ XM_PI
			+ XM_PIDIV2;

		UpdateLevelUp();
		Common::UpdateCameraLevelUp(
			m_pNewPlayer->GetModel()->getPosition(),
			m_levelUpCameraTargetTheta,
			deltaTime,
			m_phi,
			m_radius,
			m_theta);
		break;

	case Pause:
		UpdatePause();
		break;
	case BossPause:
		UpdateBossPause();
		break;
	case GameOver:
		UpdateGameOver();


		break;

	case GameClear:
		UpdateGameClear();
		
		break;
	case TimeStop:
		break;
	case Restart:
		HandleBackgroundFade(false, backGroundBlackScale, blackBackSpeed);

		break;

	case GameFinish:
		HandleBackgroundFade(true, backGroundBlackScale, blackBackSpeed/2);
		break;
	}

	// チュートリアル更新
	UpdateTutorial(deltaTime);

	//ゲーム開始前とゲーム中のみ操作説明を出す

	//左側の表示
	if (m_gameState == GameState::Play)
	{
		m_pUIBackGroundBlack[0]->setRenderEnable(true);
		//m_pUIBackGroundBlack[1]->setRenderEnable(true);
		//m_pUIBackGroundBlack[1]->setRenderEnable(false);
	}
	else
	{
		m_pUIBackGroundBlack[0]->setRenderEnable(false);
		m_pUIBackGroundBlack[1]->setRenderEnable(false);
	}
	
	if (m_gameState == GameState::Play)
	{
		pImageW->setRenderEnable(true);
		pImageA->setRenderEnable(true);
		pImageS->setRenderEnable(true);
		pImageD->setRenderEnable(true);
		pImageE->setRenderEnable(true);
		pImageQ->setRenderEnable(true);
		pImageTab->setRenderEnable(true);
		pTimeBackGround->setRenderEnable(true);
		enemyPool->ResetQuestionUI();


	}
	else
	{
		pImageW->setRenderEnable(false);
		pImageA->setRenderEnable(false);
		pImageS->setRenderEnable(false);
		pImageD->setRenderEnable(false);
		pImageE->setRenderEnable(false);
		pImageQ->setRenderEnable(false);
		pImageTab->setRenderEnable(false);
		pTimeBackGround->setRenderEnable(false);

		SetSkillUIRender(false);
	}

	// --- ポーズ中に黒い画面にする ---
	if (m_gameState == GameState::Pause)
	{
		m_pUIBackGroundBlackPause->setRenderEnable(true);
		m_pPauseFrame->setRenderEnable(true);
		m_pPauseFrame2->setRenderEnable(true);

		if (m_returnTitleState == ReturnTitleState::None &&
			m_tutorialReviewState == TutorialReviewState::None) 
		{
			enemyPool->ShowHideUI(true);
		}
		else
		{
			enemyPool->ShowHideUI(false);
		}


	}
	else
	{
		m_pUIBackGroundBlackPause->setRenderEnable(false);
		m_pPauseFrame->setRenderEnable(false);
		m_pPauseFrame2->setRenderEnable(false);
		enemyPool->ShowHideUI(false);

		//ポーズ中以外は「タイトルに戻るボタン」を非表示にしておく
		m_buttonData[(int)UIButton::TITLEBACK].visible = false;
		m_buttonData[(int)UIButton::TUTORIAL_REVIEW].visible = false;


	}


	//ボタンの表示非表示管理
	for (int i = 0; i < (int)UIButton::MaxNum; i++)
	{
		m_buttonData[i].sprite->setRenderEnable(m_buttonData[i].visible);
	}

	//説明画像の表示非表示
	//チュートリアル中の画像のみを表示
	UpdateExplanationImages();

	UpdateMission();

	//ミッションUIの表示
	//ゲームプレイ中且つチュートリアルの時のみ
	if (m_gameState == GameState::Play&&m_isTutorial)
	{
		for (int i = 0; i < (int)TutorialMission::MaxNum; i++)
		{
			//ミッションをクリアするまで表示する
			m_missionUI[i].sprite_back->setRenderEnable(!m_missionUI[i].isClear);
			m_missionUI[i].sprite_check->setRenderEnable(!m_missionUI[i].isClear);
		}
	}
	else if(m_gameState!=GameState::Play)
	{
		for (int i = 0; i < (int)TutorialMission::MaxNum; i++)
		{
			//プレイ中以外は表示しない
			m_missionUI[i].sprite_back->setRenderEnable(false);
			m_missionUI[i].sprite_check->setRenderEnable(false);
		}
	}

	// --- WAVEの状態の切り替え(WAVEクリア→次のWAVEとか) ---
	UpdateWaveTransition();

	//デバッグ表示
	//if (vnKeyboard::trg(DIK_R)) {
	//	m_gameState = GameClear;

	//}

	//if (vnKeyboard::trg(DIK_T)) {
	//	m_gameState = GameOver;
	//}

	// --- ライト設定 ---

	// 環境光：全体の底上げ（入れすぎない）
	vnLight::setAmbient(0.35f, 0.35f, 0.35f);
	//vnLight::setAmbient(0, 0, 0);


	// 平行光源：少し斜め前上から当てる
	vnLight::setLightDir(0.3f, -1.0f, 0.3f);

	// 光の色：完全な白より少しだけ抑える
	//vnLight::setLightCol(0.95f, 0.95f, 0.95f);
	vnLight::setLightCol(0.7f, 0.5f, 0.5f);

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

	int baseX_WASD = 120;	//WASDの画像の位置（X）
	int baseY = operationUI_Y+130;

	float text_RIGHT_CLICK_x = 380.0f;

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
		vnFont::setFontSize(38, 40);

		// 影を描画
		vnFont::print(text_RIGHT_CLICK_x + off, 600 + off, shadowAlpha, L"[RIGHT CLICK] TO START");
		// 本体を描画
		vnFont::print(text_RIGHT_CLICK_x, 600, blinkColor, L"[RIGHT CLICK] TO START");

  
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
		printShadow(opX, opY + 120, GAME_COLOR_WHITE, L"ジャンプ    : MOUSE");
		printShadow(opX, opY + 170, GAME_COLOR_WHITE, L"ポーズ: TAB");


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
		printShadow(ruleX, ruleY + 160, GAME_COLOR_WHITE, L"・Waveが進むほど");
		printShadow(ruleX, ruleY + 200, GAME_COLOR_WHITE, L"  リングは広くなるぞ");
		printShadow(ruleX, ruleY + 260, 0xFFFF0000, L"※HPが0になるとゲームオーバー");

		break;
	}
		break;
	case Play:
	{
		float screenWidth = vnMainFrame::screenWidth;
		float screenHeight = vnMainFrame::screenHeight;

		float off = 3.0f;
		unsigned int shadowCol = 0xFF000000; // 不透明な黒

		//==========================================================
		// --- タイム表示 ---
		//==========================================================
		int minutes = (int)(totalClearTime / 60);
		int seconds = (int)totalClearTime % 60;
		//vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 50));
		vnFont::setFontSize(38, 40);
		int remainTime = static_cast<int>(waveManager->GetWaveTimer());

		unsigned int time_textColor = (remainTime <= 3) ? GAME_COLOR_RED : GAME_COLOR_WHITE;

		if (waveManager->GetFinalWave()||m_isTutorial||m_isEndless)
		{
			vnFont::print(screenWidth / 2 - 34 + off, 100.0f + off, shadowCol, L" ∞");

			vnFont::print(screenWidth / 2 - 34, 100.0f, GAME_COLOR_WHITE, L" ∞");
		}
		else
		{
			vnFont::print(screenWidth / 2 - 35 + off, 100.0f + off, shadowCol, L"%02ds", remainTime);

			vnFont::print(screenWidth / 2 - 35, 100.0f, time_textColor, L"%02ds", remainTime);


		}


		//============================================================
		// --- 倒した敵の合計 ---
		//============================================================
		vnFont::setFontSize(38, 25);

		vnFont::print(420 + off, 70 + off, shadowCol, L"撃破数：%d体", waveManager->GetTotalKillCount());
		vnFont::print(420, 70, GAME_COLOR_YELLOW, L"撃破数：%d体", waveManager->GetTotalKillCount());

		//vnFont::print(950.0f + off, 20.0f + off, shadowCol, L"TIME: %02d:%02d", minutes, seconds); // 影
		//vnFont::print(950.0f, 20.0f, GAME_COLOR_GREEN, L"TIME: %02d:%02d", minutes, seconds); // 本体
		//vnFont::print(screenWidth / 2 - 35 + off, 100.0f + off, shadowCol, L"%02ds", static_cast<int>(waveManager->GetWaveTimer())); // 影
		//vnFont::print(screenWidth / 2 - 35, 100.0f, GAME_COLOR_WHITE, L"%02ds", static_cast<int>(waveManager->GetWaveTimer())); // 本体


		//============================================================
		// --- 現在のWAVE数 ---
		//============================================================
		vnFont::setFontSize(38, 25);
		
		vnFont::print(screenWidth / 2 - 25 + off, 10.0f + off, shadowCol, L"%d/%d", waveManager->GetCurrentWave(), waveManager->GetMaxWave());
		vnFont::print(screenWidth / 2 - 25 , 10.0f, GAME_COLOR_WHITE, L"%d/%d", waveManager->GetCurrentWave(), waveManager->GetMaxWave());

		vnFont::print(screenWidth / 2 - 55 + off, 35.0f + off, shadowCol, L"WAVE進捗");
		vnFont::print(screenWidth / 2 - 55, 35.0f, GAME_COLOR_SILVER, L"WAVE進捗");

		// --- 撃破数関係 ---
		//vnFont::print(950.0f + off, 110.0f + off, shadowCol, L"%d / %d 体撃破", waveManager->GetKillCount(), waveManager->GetKillTargetCount());
		//vnFont::print(950.0f, 110.0f, GAME_COLOR_NEON_GREEN, L"%d / %d 体撃破", waveManager->GetKillCount(), waveManager->GetKillTargetCount());


		// --- 待機中のテキスト ---
		if (waveManager->IsWaitingForNext() && (waveManager->GetCurrentWave() < waveManager->GetMaxWave()))
		{
			vnFont::setFontSize(38, 80);
			// メインのクリア表示 (影あり)
			vnFont::print(350.0f + off, 200.0f + off, shadowCol, L"WAVE %d CLEAR!!", waveManager->GetCurrentWave());
			vnFont::print(350.0f, 200.0f, GAME_COLOR_WHITE, L"WAVE %d CLEAR!!", waveManager->GetCurrentWave());

			vnFont::setFontSize(38, 40);

			// NEXT表示 (影あり)
			vnFont::print(430.0f + off, 320.0f + off, shadowCol, L"NEXT : フィールド拡大");
			vnFont::print(430.0f, 320.0f, GAME_COLOR_LIME, L"NEXT : フィールド拡大");

			// --- 点滅処理 ---
			blinkCounter++;
			float alpha = (sinf(blinkCounter * 0.1f) + 1.0f) * 0.5f;

			// アルファ値を影の色にも合成（影も一緒にふわふわ消えるようにする）
			unsigned int blinkShadow = ((unsigned int)(alpha * 255) << 24) | 0x00000000;
			unsigned int blinkColor = ((unsigned int)(alpha * 255) << 24) | (GAME_COLOR_CYAN & 0x00FFFFFF);

			vnFont::print(text_RIGHT_CLICK_x + off, 450.0f + off, blinkShadow, L"[RIGHT CLICK] NEXT WAVE"); // 点滅する影
			vnFont::print(text_RIGHT_CLICK_x, 450.0f, blinkColor, L"[RIGHT CLICK] NEXT WAVE"); // 点滅する本体
		}
		{
			// --- 操作説明 ---
			bool aPressed = vnKeyboard::on(DIK_A);       // DIK_* はキーコード
			bool dPressed = vnKeyboard::on(DIK_D);
			bool wPressed = vnKeyboard::on(DIK_W);
			bool sPressed = vnKeyboard::on(DIK_S);
			bool ePressed = vnKeyboard::on(DIK_E);
			bool qPressed = vnKeyboard::on(DIK_Q);
			bool tabPressed = vnKeyboard::on(DIK_TAB);

			bool spacePressed = vnKeyboard::on(DIK_SPACE);
			bool mousePressed = vnMouse::onL(); // 0 = 左クリック


			// 5. フォント設定
			vnFont::setFontSize(38, 25);

			// 「移動：」は常に白
			vnFont::print(baseX-25, baseY +10 , GAME_COLOR_YELLOW, L"移動：");
			pImageW->setPos(baseX_WASD + 75, baseY + 35);
			pImageW->setColor(wPressed ? V_GAME_COLOR_GOLD : V_GAME_COLOR_WHITE);

			pImageA->setPos(baseX_WASD + 15, baseY + 100);
			pImageA->setColor(aPressed ? V_GAME_COLOR_GOLD : V_GAME_COLOR_WHITE);

			pImageS->setPos(baseX_WASD + 75, baseY + 100);
			pImageS->setColor(sPressed ? V_GAME_COLOR_GOLD : V_GAME_COLOR_WHITE);

			pImageD->setPos(baseX_WASD + 135, baseY + 100);
			pImageD->setColor(dPressed ? V_GAME_COLOR_GOLD : V_GAME_COLOR_WHITE);

			pImageE->setPos(baseX + 55.5f, baseY + 152.5f);
			pImageE->setColor(ePressed ? V_GAME_COLOR_GOLD : V_GAME_COLOR_WHITE);
			
			pImageQ->setPos(baseX + 55.5f, baseY + 232.5f);
			pImageQ->setColor(qPressed ? V_GAME_COLOR_GOLD : V_GAME_COLOR_WHITE);

			vnFont::print(baseX-25, baseY-35 , GAME_COLOR_YELLOW, L"ポーズ：");
			pImageTab->setPos(baseX +150, baseY -25);
			pImageTab->setColor(tabPressed ? V_GAME_COLOR_GOLD : V_GAME_COLOR_WHITE);

		}

		{
			// --- HPバーの表示 ---
			setHPbarRender(true);
			// 1. シェイクの揺れ量（オフセット）を取得
			float shakeX = 0.0f;
			float shakeY = 0.0f;
			ShakeHpBar(m_pNewPlayer->GetCurrentHp(), vnScene::getDeltaTime(), shakeX, shakeY);

			// 2. HP割合の計算とスケール適用
			float hpRatio = (float)m_pNewPlayer->GetCurrentHp() / m_pNewPlayer->GetMaxHp();
			pHpBarFront->setScaleX(hpRatio);

			// 3. 各パーツ本来の位置を計算（前バーは伸縮補正入り）
			float frontPosX = barLeftEdgeHp + (barWidthHp * hpRatio * 0.5f); // 伸縮調整あり
			float backPosX = barLeftEdgeHp + (barWidthHp * 0.5f);           // 背景系は固定長（満タン時の中心）

			// 4. 本来の位置 + シェイクの揺れ量をセット！
			pHpBarFront->setPos(frontPosX + shakeX, heigtYHp + shakeY);
			pHpBarBackBlack->setPos(backPosX + shakeX, heigtYHp + shakeY);
			pHpBarBack->setPos(backPosX + shakeX, heigtYHp + shakeY);
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

		// --- スキルバーの表示更新 ---
		{
			//範囲攻撃スキルバー、アイコン
			UpdateSkillBar(
				m_pNewPlayer->GetAreaAttackCoolTime(),
				m_pNewPlayer->GetAreaAttackMaxCoolTime(),
				pAreaSkillIcon,
				pAreaAtkBtnFront,
				m_bIsAreaSkillMaxPrev,
				m_areaSkillIconScale,
				m_areaSkillTargetScale,
				barLeftEdgeAreaSkill,
				maxWAreaSkill,
				heightYAreaSkill
			);
			//引き寄せスキルバー、アイコン
			UpdateSkillBar(
				m_pNewPlayer->GetPullAttackCoolTime(),
				m_pNewPlayer->GetPullAttackMaxCoolTime(),
				pPullSkillIcon,
				pPullBtnFront,
				m_bIsPullSkillMaxPrev,
				m_pullSkillIconScale,
				m_pullSkillTargetScale,
				barLeftEdgeAreaSkill,
				maxWAreaSkill,
				heightYPullSkill
			);
		}

		//=============================
		// ボス出現演出
		//=============================

		// FinalWaveになった最初の1回だけ表示開始
		if (waveManager->GetFinalWave() && !m_isBossAppearanceTriggered)
		{
			m_isBossAppearanceTriggered = true;
			m_showBossText = true;
			m_bossTextTimer = 2.0f;
		}

		// 表示中のみ更新・描画
		if (m_showBossText)
		{
			float dt = vnScene::getDeltaTime();
			m_bossTextTimer -= dt;

			// 表示時間終了
			if (m_bossTextTimer <= 0.0f)
			{
				m_showBossText = false;
			}

			//=============================
			// 拡大アニメーション
			//=============================

			float bossButtonScale = 1.5f; // 最終倍率

			if (m_bossTextTimer > 1.5f)
			{
				// 表示開始から0.5秒間だけ拡大する
				float progressTime = 2.0f - m_bossTextTimer;
				float rate = progressTime / 0.5f;

				// 0.2倍 → 1.5倍
				bossButtonScale = 0.2f + (1.5f - 0.2f) * rate;
			}

			//=============================
			// フォント設定
			//=============================

			float currentFontSize = 50.0f * bossButtonScale;
			vnFont::setTextFormat(
				vnFont::create(vnFont::getFontName(38), (int)currentFontSize));

			// 文字サイズから描画位置を計算
			float textWidth = currentFontSize * 1.5f;
			float textHeight = currentFontSize * 0.5f;

			float actualTextWidth = currentFontSize * 3.1f;
			float actualTextHeight = currentFontSize * 0.5f;

			float tx = (vnMainFrame::screenWidth / 2.0f) - actualTextWidth;
			float ty = (vnMainFrame::screenHeight / 2.0f) - 50 - actualTextHeight;

			//=============================
			// 描画
			//=============================

			float off = 4.0f; // 影のずらし量

			vnFont::print(tx + off, ty + off, shadowCol, L"～ボス出現～");
			vnFont::print(tx, ty, GAME_COLOR_RED, L"～ボス出現～");
		}
		//ボスの撃破必要カウント
		if (waveManager->GetFinalWave())
		{
			vnFont::setFontSize(38, 50);

			float off = 4.0f; // 影のずらし量
			unsigned int shadowCol = 0xFF000000;
			// 影も offsetY を足して一緒に動かす
			vnFont::print(950.0f + off, 120 + off, shadowCol, L"残り：%d",waveManager->GetKillBossCountTarget());
			vnFont::print(950.0f, 120 , GAME_COLOR_RED, L"残り：%d", waveManager->GetKillBossCountTarget());


		}


		//============================================
		//ミッションの文字の表示
		if (m_isTutorial)
		{
			vnFont::setFontSize(38, 25);
			float missionTextX = m_missionUI[0].position_x - 125;
			for (int i = 0; i < (int)TutorialMission::MaxNum; i++)
			{
				auto& mission = m_missionUI[i];
				//クリアしていないもののみ表示する
				if (mission.isClear)
				{
					continue;
				}

				//影
				vnFont::print(
					missionTextX + mission.font_pos_offset_x + off,
					mission.position_y - 10 + off,
					GAME_COLOR_WHITE,
					mission.text);
				//本体
				vnFont::print(
					missionTextX + mission.font_pos_offset_x,
					mission.position_y - 10,
					GAME_COLOR_BLACK,
					mission.text);


			}

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
		vnFont::print(text_RIGHT_CLICK_x + off, 450.0f + off, blinkShadow, L"[RIGHT CLICK]  BACK TITLE");
		vnFont::print(text_RIGHT_CLICK_x, 450.0f, blinkColor, L"[RIGHT CLICK]  BACK TITLE");

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

		//vnFont::print(340 + off, 400 + off, shadowCol, L"～TOTAL TIME: %02d:%02d～", minutes, seconds);
		//vnFont::print(340, 400, GAME_COLOR_WHITE, L"～TOTAL TIME: %02d:%02d～", minutes, seconds);

		// --- THANK YOU FOR PLAYING! ---
		vnFont::print(330 + off, 500 + off, shadowCol, L"THANK YOU FOR PLAYING!");
		vnFont::print(330, 500, GAME_COLOR_LIME, L"THANK YOU FOR PLAYING!");

		// --- [ENTER] BACK TITLE (点滅 + 影) ---
		float alpha = (sinf(blinkCounter * 0.1f) + 1.0f) * 0.5f;
		unsigned int blinkShadow = ((unsigned int)(alpha * 255) << 24) | 0x00000000;
		unsigned int blinkColor = ((unsigned int)(alpha * 255) << 24) | (GAME_COLOR_CYAN & 0x00FFFFFF);
		vnFont::setFontSize(38, 40);

		vnFont::print(text_RIGHT_CLICK_x + off, 600.0f + off, blinkShadow, L"[RIGHT CLICK]  BACK TITLE");
		vnFont::print(text_RIGHT_CLICK_x, 600.0f, blinkColor, L"[RIGHT CLICK]  BACK TITLE");


		break;

		}

	case Pause:
	{
	}
		break;
	}

	//ボタンが表示されてるとにそれに合った文字を表示する
	for (int i = 0; i < (int)UIButton::MaxNum; i++)
	{
		auto& button = m_buttonData[i];

		if (!button.visible)
		{
			continue;
		}

		Common::ChangeButtonTextSize(
			button.position_x+button.font_pos_offset_x,
			button.position_y,
			button.scale,
			button.isOn,
			button.text);
	}


	vnScene::render();
}





//黒い画像を拡大・縮小する
// 拡大縮小とシーン遷移を一括管理する関数
void SceneMain::HandleBackgroundFade(bool isFadeOut, float& scale, float speed = 0.05f)
{
	pBackGroundBlack->setRenderEnable(true);

	if (isFadeOut)
	{
		// 拡大（終了時）目標値: 1.1f
		scale += (1.1f - scale) * speed;
		pBackGroundBlack->setScale(scale);

		// 画面を覆い尽くしたらタイトルへ
		if (scale >= 1.0f)
		{
			// 敵のリセット処理
			for (auto& enemy : enemyPool->GetEnemies())
			{
				enemy->ReStartEnemy();
			}
			enemyPool->ReStartEnemyGroupData();

			switchScene(TITEL); // タイトルシーンへ遷移
		}
	}
	else
	{
		// 縮小（開始時）目標値: 0.0f
		scale += (0.0f - scale) * speed;
		pBackGroundBlack->setScale(scale);

		// 消え去ったらゲームスタート（プレイ状態）へ
		if (scale <= 0.01f)
		{
			scale = 0.0f;
			pBackGroundBlack->setScale(scale);
			pBackGroundBlack->setRenderEnable(false); // 完全に非表示にする

			m_gameState=IdelPlay; // ゲーム本編（プレイ状態）へ遷移
		}
	}
}



//==================================================
// --- チュートリアルの画像の制御 ---
//==================================================
void SceneMain::UpdateExplanationImages()
{
	if (m_currentExplanationType == ExplanationType::None)
	{
		return;
	}

	auto& explanation =
		m_explanationUI[(int)m_currentExplanationType];

	for (size_t j = 0; j < explanation.images.size(); j++)
	{
		bool isVisible = false;

		// アニメーション中
		if (m_explanationSlideState != ExplanationSlideState::None)
		{
			// 現在のページ
			if (j == m_explanationSlidePage)
			{
				isVisible = true;
			}

			// 進むなら次のページも表示
			if (m_explanationSlideState == ExplanationSlideState::SlideLeft &&
				j == m_explanationSlidePage + 1)
			{
				isVisible = true;
			}

			// 戻るなら前のページも表示
			if (m_explanationSlideState == ExplanationSlideState::SlideRight &&
				j == m_explanationSlidePage - 1)
			{
				isVisible = true;
			}
		}
		else
		{
			// 通常時は現在のページだけ
			isVisible = explanation.visible &&
				j == m_explanationPage;
		}

		explanation.images[j]->setRenderEnable(isVisible);
	}
}


//=====================================================================
// スキルなどのバー関数（Update）
//=====================================================================
void SceneMain::UpdateSkillBar(
	float currentCoolTime,
	float maxCoolTime,
	vnSprite* pSkillIcon,
	vnSprite* pSkillBar,
	bool& isSkillMaxPrev,
	float& skillIconScale,
	float& skillTargetScale,
	float barLeftEdge,
	float maxWidth,
	float heightY
)
{
	SetSkillUIRender(true);

	// 安全に割り算を行うためのチェック
	float skillRatio = 0.0f;
	if (maxCoolTime > 0.0f)
	{
		// 例：残り10秒/最大10秒 = 1.0  → 1.0 - 1.0 = 0.0 (空っぽ)
		// 例：残り 3秒/最大10秒 = 0.3  → 1.0 - 0.3 = 0.7 (7割溜まった)
		// 例：残り 0秒/最大10秒 = 0.0  → 1.0 - 0.0 = 1.0 (満タン！)
		skillRatio = 1.0f - (currentCoolTime / maxCoolTime);
	}
	if (skillRatio > 1.0f)
	{
		skillRatio = 1.0f;
	}
	if (skillRatio >= 1.0f)
	{
		if (!isSkillMaxPrev)
		{
			isSkillMaxPrev = true;

			//アイコンを拡大する
			skillIconScale = 1.5f;
			skillTargetScale = 1.0f;
		}
		skillIconScale += (skillTargetScale - skillIconScale) * 0.05f;
	}
	else
	{
		isSkillMaxPrev = false;
		skillIconScale = 1.0f;
		skillTargetScale = 1.0f;

	}
	pSkillIcon->setScale(skillIconScale);
	pSkillBar->setScaleX(skillRatio);
	float currentPosX = barLeftEdge + (maxWidth * skillRatio * 0.5f);
	pSkillBar->setPos(currentPosX, heightY);

}
void SceneMain::ShakeHpBar(
	float currentHp,
	float deltaTime,
	float& outOffsetX,
	float& outOffsetY
)
{
	if (m_prevHp < 0.0f)
	{
		m_prevHp = currentHp;
	}

	// ダメージ検知
	if (currentHp < m_prevHp)
	{
		m_hpShakeTimer = m_hpShakeDuration;
	}
	m_prevHp = currentHp;

	outOffsetX = 0.0f;
	outOffsetY = 0.0f;

	if (m_hpShakeTimer > 0.0f)
	{
		m_hpShakeTimer -= deltaTime;

		float damping = m_hpShakeTimer / m_hpShakeDuration;
		float randomX = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
		float randomY = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;

		outOffsetX = randomX * m_hpShakeIntensity * damping;
		outOffsetY = randomY * m_hpShakeIntensity * damping;
	}
}


//======================================================================
// --- ボタンの判定 ---
//======================================================================
bool SceneMain::UpdateButton(UIButton id)
{
	auto& button = m_buttonData[(int)id];
	return Common::UpdateButton(
		button.position_x,
		button.position_y,
		button_w,
		button_h,
		button.sprite,
		button.isOn,
		button.scale,
		soundManager.get());
}

//======================================================================
// --- 説明文の表示 ---
//======================================================================
bool SceneMain::UpdateMessageWindow(float targetScale,const WCHAR* text,WindowMode mode, ExplanationUIData* explanation)
{
	float screenWidth = vnMainFrame::screenWidth;
	float screenHeight = vnMainFrame::screenHeight;

	switch (mode)
	{
		case WindowMode::Open:
		{
			m_messageBackground->setRenderEnable(true);

			m_windowOpenTimer += vnScene::getDeltaTime();
			//一定時間で拡大
			const float openTime = 0.3f;

			float t = m_windowOpenTimer / openTime;

			if (t >= 1.0f)
			{
				t = 1.0f;
			}

			m_windowScale = targetScale * t;

			m_messageBackground->setScale(m_windowScale);
			if (t >= 1.0f)
			{
				m_windowScale = targetScale;
				m_messageBackground->setScale(m_windowScale);

				//「本当に戻りますか？」を表示
				if (m_returnTitleState == ReturnTitleState::Confirm)
				{
					vnFont::setFontSize(38, 40);
					vnFont::print(
						screenWidth / 2 - 175,
						screenHeight / 2 - 25,
						GAME_COLOR_WHITE,
						text);
				}

				// 説明画像
				else
				{
					if (explanation != nullptr && !explanation->images.empty())
					{
						explanation->visible = true;
					}
				}

				return true;
			}


			break;
		}
		case WindowMode::Close:
		{
			if (explanation != nullptr && !explanation->images.empty())
			{
				explanation->visible = false;
			}


			m_windowScale += (0.0f - m_windowScale) * 0.2f;
			m_messageBackground->setScale(m_windowScale);

			if (m_windowScale <= 0.01f)
			{
				m_windowOpenTimer = 0.0f;
				m_windowScale = 0.0f;
				m_messageBackground->setScale(m_windowScale);
				m_messageBackground->setRenderEnable(false);

				return true;
			}

			break;
		}
	}

	return false;

}


//===============================================================
// --- チュートリアルの説明を出してボタンの押して閉じるまで ---
//===============================================================
void SceneMain::UpdateExplanation(ExplanationType type)
{
	if (m_windowMode == WindowMode::Open)
	{
		if (UpdateMessageWindow(
			0.9f,
			L"",
			WindowMode::Open,
			&m_explanationUI[(int)type]))
		{
			auto& rightButton = m_buttonData[(int)UIButton::MESSAGE_RIGHT];
			auto& leftButton = m_buttonData[(int)UIButton::MESSAGE_LEFT];
			auto& closeButton = m_buttonData[(int)UIButton::MESSAGE_CLOSE];

			UpdateExplanationButtons(type);
		}
	}
	else if (m_windowMode == WindowMode::Close)
	{
		if (UpdateMessageWindow(
			0.0f,
			L"",
			WindowMode::Close,
			&m_explanationUI[(int)type]))
		{

			// ページを最初に戻す
			m_explanationPage = 0;

			// ボタンを非表示
			m_buttonData[(int)UIButton::MESSAGE_RIGHT].visible = false;
			m_buttonData[(int)UIButton::MESSAGE_LEFT].visible = false;
			m_buttonData[(int)UIButton::MESSAGE_CLOSE].visible = false;

			m_explanationUI[(int)type].isOne = true;	//一度見たしるしをつける
			m_gameState = currentState;
			m_windowMode = WindowMode::None;

			//ポーズ中に振り返り画面に戻る
			if (m_tutorialReviewState != TutorialReviewState::None)
			{
				m_tutorialReviewState = TutorialReviewState::Select;
			}

			//モードを初期に戻す
			m_state_tutorial = TutorialState::None;
		}
	}

}

//チュートリアル中のボタンの制御
void SceneMain::UpdateExplanationButtons(ExplanationType type)
{
	auto& rightButton = m_buttonData[(int)UIButton::MESSAGE_RIGHT];
	auto& leftButton = m_buttonData[(int)UIButton::MESSAGE_LEFT];
	auto& closeButton = m_buttonData[(int)UIButton::MESSAGE_CLOSE];

	int pageCount = (int)m_explanationUI[(int)type].images.size();
	auto& explanation = m_explanationUI[(int)type];


	if (pageCount <= 1)
	{
		rightButton.visible = false;
		leftButton.visible = false;
		closeButton.visible = true;
	}
	else
	{
		rightButton.visible = (m_explanationPage < pageCount - 1);
		leftButton.visible = (m_explanationPage > 0);
		closeButton.visible = (m_explanationPage + 1 >= pageCount);
	}

	//アニメーションしてないときだけボタンを押せる
	if (m_explanationSlideState == ExplanationSlideState::None)
	{
		// 進む
		if (rightButton.visible && (UpdateButton(UIButton::MESSAGE_RIGHT) || vnKeyboard::trg(DIK_D)))
		{
			soundManager->PlaySE(rightButton.se_id);

			//m_explanationPage++;
			m_explanationSlidePage = m_explanationPage;
			explanation.position_new_x = explanation.position_x;
			explanation.position_new_y = explanation.position_y;
			m_explanationSlideState = ExplanationSlideState::SlideLeft;
			m_explanationSlideTime = 0.0f;
		}

		// 戻る
		if (leftButton.visible && (UpdateButton(UIButton::MESSAGE_LEFT)||vnKeyboard::trg(DIK_A)))
		{
			soundManager->PlaySE(leftButton.se_id);

			//m_explanationPage--;
			m_explanationSlidePage = m_explanationPage;
			explanation.position_new_x = explanation.position_x;
			explanation.position_new_y = explanation.position_y;

			m_explanationSlideState = ExplanationSlideState::SlideRight;
			m_explanationSlideTime = 0.0f;
		}

		// 閉じる
		if (closeButton.visible && (UpdateButton(UIButton::MESSAGE_CLOSE) || vnKeyboard::trg(DIK_SPACE)))
		{
			soundManager->PlaySE(closeButton.se_id);

			m_windowMode = WindowMode::Close;
		}

	}



	// アニメーション更新
	UpdateExplanationSlide(type);
}

//---説明画像のアニメーション ---
void SceneMain::UpdateExplanationSlide(ExplanationType type)
{
	if (m_explanationSlideState == ExplanationSlideState::None)
		return;

	//チュートリアルの種類を取る
	auto& explanation = m_explanationUI[(int)type];

	//アニメーションの時間を足す
	m_explanationSlideTime += vnScene::getDeltaTime();

	float t = m_explanationSlideTime / 0.5f;

	if (t >= 1.0f)
		t = 1.0f;

	//移動量
	float moveDistance = 1800.0f;

	float x = explanation.position_new_x;

	auto* currentImage = explanation.images[m_explanationSlidePage];
	// 前の画像
	auto* prevImage = explanation.images[m_explanationSlidePage - 1];

	// 進む（今表示されてる画像は左に行く、表示したい画像は右（画面外）から左に来る）
	if (m_explanationSlideState == ExplanationSlideState::SlideLeft)
	{
		// 現在の画像
		currentImage->setPos(
			explanation.position_new_x - moveDistance * t,
			explanation.position_new_y
		);

		currentImage->setAlpha(1.0f - t);

		// 次の画像
		auto* nextImage = explanation.images[m_explanationSlidePage + 1];

		nextImage->setPos(
			explanation.position_new_x + moveDistance - moveDistance * t,
			explanation.position_new_y
		);

		//nextImage->setAlpha(t);
	}

	// 戻る（今表示されてる画像は右に行く、表示したい画像は左（画面外）から右に来る）
	else if (m_explanationSlideState == ExplanationSlideState::SlideRight)
	{
		// 現在の画像
		currentImage->setPos(
			explanation.position_new_x + moveDistance * t,
			explanation.position_new_y
		);

		currentImage->setAlpha(1.0f - t);


		prevImage->setPos(
			explanation.position_new_x - moveDistance + moveDistance * t,
			explanation.position_new_y
		);

		//prevImage->setAlpha(t);
	}


	// アニメーション終了
	if (t >= 1.0f)
	{
		if (m_explanationSlideState == ExplanationSlideState::SlideLeft)
		{

			m_explanationPage++;
			currentImage->setAlpha(t);
		}
		else if (m_explanationSlideState == ExplanationSlideState::SlideRight)
		{
			m_explanationPage--;
			currentImage->setAlpha(t);
		}

		m_explanationSlideState = ExplanationSlideState::None;

	}


	//アニメーションが完全に終了したら初期化で画像をもとに戻す
	if (m_explanationSlideState == ExplanationSlideState::None)
	{
		for (auto* image : m_explanationUI[(int)type].images)
		{
			if (image != nullptr)
			{
				image->setPos(
					m_explanationUI[(int)type].position_x,
					m_explanationUI[(int)type].position_y
				);

				image->setAlpha(1.0f);
			}
		}
	}

}


//説明UIがある状態の変更
void SceneMain::ChangeTutorialState(ExplanationType type, bool isReview)
{
	if (m_state_tutorial != TutorialState::None)
	{
		return;
	}
	//チュートリアルなら説明を出す
	if ((m_isTutorial && !m_explanationUI[(int)type].isOne) || isReview)
	{
		//もとのゲームの状態を保存
		currentState = m_gameState;
		//時間停止する
		m_gameState = TimeStop;

		// 現在表示する説明を保存
		m_currentExplanationType = type;

		switch (type)
		{
		case ExplanationType::EnemyLeader:
			m_state_tutorial = TutorialState::ExplainEnemyLeader;
			break;

		case ExplanationType::Exp:
			m_state_tutorial = TutorialState::ExplainExp;
			for (int i = 0; i < (int)TutorialMission::MaxNum; i++)
			{
				m_missionUI[i].isClear = true;
			}
			break;

		case ExplanationType::LevelUp:
			m_state_tutorial = TutorialState::ExplainLevelUp;
			break;

		case ExplanationType::Skills:
			m_state_tutorial = TutorialState::ExplainSkills;
			break;
		case ExplanationType::PlayerOperation:
			m_state_tutorial = TutorialState::ExplainPlayerOperation;
			break;
		case ExplanationType::PlayerDamage:
			m_state_tutorial = TutorialState::ExplainPlayerDamage;
			break;

		default:
			return;
		}

		//黒い画面を拡大する
		m_windowMode = WindowMode::Open;

	}

}

//振り返りボタンを押した後のボタンの表示非表示
void SceneMain::SetTutorialReviewButtonVisible(bool visible)
{
	m_buttonData[(int)UIButton::TUTORIAL_ENEMY_LEADER].visible = visible;
	m_buttonData[(int)UIButton::TUTORIAL_EXP].visible = visible;
	m_buttonData[(int)UIButton::TUTORIAL_LEVEL_UP].visible = visible;
	m_buttonData[(int)UIButton::TUTORIAL_SKILLS].visible = visible;
	m_buttonData[(int)UIButton::TUTORIAL_PLAYER_OPERATION].visible = visible;
	m_buttonData[(int)UIButton::TUTORIAL_PLAYER_DAMAGE].visible = visible;
	m_buttonData[(int)UIButton::TUTORIAL_BACK].visible = visible;
}



//==========================================================================================
// --- チュートリアルのミッションが全て達成出来たら「タイトルに戻る」か「ゲームに移動」---
//==========================================================================================
//全てのミッションをクリアしたかどうか
bool SceneMain::IsAllMissionClear()
{
	for (int i = 0; i < (int)TutorialMission::MaxNum; i++)
	{
		if (!m_missionUI[i].isClear)
		{
			return false;
		}
	}

	return true;
}
void SceneMain::UpdateMission()
{
	if (IsAllMissionClear())
	{
		// ボタン表示


		if (vnKeyboard::trg(DIK_RETURN))
		{
			soundManager->StopBGM(BGM_GAME);

			m_isTutorial = false;
			CleanUpScene();
			InitializeVariables();
			m_pNewPlayer->Init();
			enemyPool->AllDataReset();
			SetExpbarRender(false);
			setHPbarRender(false);
		}


	}

}

#pragma region UIの表示非表示

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
	pAreaAtkBtnBackBlack->setRenderEnable(on);
	pAreaAtkBtnBack->setRenderEnable(on);
	pAreaAtkBtnFront->setRenderEnable(on);
	pAreaSkillIcon->setRenderEnable(on);
	pPullBtnBackBlack->setRenderEnable(on);
	pPullBtnBack->setRenderEnable(on);
	pPullBtnFront->setRenderEnable(on);
	pPullSkillIcon->setRenderEnable(on);

	if (!m_pNewPlayer->GetIsHaveAreaAtkSkill())
	{
		pAreaAtkBtnBackBlack->setColor(V_GAME_COLOR_BLACK);
		pAreaAtkBtnBack->setColor(V_GAME_COLOR_BLACK);
		pAreaAtkBtnFront->setColor(V_GAME_COLOR_BLACK);
		pAreaSkillIcon->setColor(V_GAME_COLOR_BLACK);
	}
	else
	{
		pAreaAtkBtnBackBlack->setColor(m_areaAtkUIColor.colorBackBlack);
		pAreaAtkBtnBack->setColor(m_areaAtkUIColor.colorBack);
		pAreaAtkBtnFront->setColor(m_areaAtkUIColor.colorFront);
		pAreaSkillIcon->setColor(m_areaAtkUIColor.colorIcon);
	}

	if (!m_pNewPlayer->GetIsHavePullSkill())
	{
		pPullBtnBackBlack->setColor(V_GAME_COLOR_BLACK);
		pPullBtnBack->setColor(V_GAME_COLOR_BLACK);
		pPullBtnFront->setColor(V_GAME_COLOR_BLACK);
		pPullSkillIcon->setColor(V_GAME_COLOR_BLACK);
	}
	else
	{
		pPullBtnBackBlack->setColor(m_pullUIColor.colorBackBlack);
		pPullBtnBack->setColor(m_pullUIColor.colorBack);
		pPullBtnFront->setColor(m_pullUIColor.colorFront);
		pPullSkillIcon->setColor(m_pullUIColor.colorIcon);
	}

}

#pragma endregion



//----------------------------------------

// --- 待機状態 ---

//----------------------------------------

void SceneMain::UpdateIdel()
{
	//ボタン押してゲームスタート
	if (vnMouse::onR() || vnKeyboard::on(DIK_RETURN))
	{
		m_gameState = Play;
		//pSound[2]->play();
		soundManager->PlaySE(SE_ENTER);

		//スキルレベルなどの初期化
		m_pExpManager->AllLevelReset();

		waveManager->Init(m_isTutorial, m_isEndless);   //ここでWave開始
		m_pBlockManager->RespawnBlocks(waveManager->GetCurrentWave(), FenceRadius, waveManager->GetFinalWave());

		//開始と同時にプレイヤーの基本操作のチュートリアルを出す
		
		{
			//チュートリアルのみ通る
			ChangeTutorialState(ExplanationType::PlayerOperation,false);
		}

	}

}

//最初のカメラ位置
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
	//ゲームに戻るボタン
	auto& playBackButton = m_buttonData[(int)UIButton::PLAYGAME_BACK];

	if (m_returnTitleState == ReturnTitleState::None&&m_tutorialReviewState==TutorialReviewState::None)
	{
		playBackButton.visible = true;
		if (vnKeyboard::trg(DIK_TAB) || UpdateButton(UIButton::PLAYGAME_BACK))
		{
			soundManager->PlaySE(playBackButton.se_id);
			playBackButton.visible = false;
			m_gameState = GameState::Play;		

		}
	}

	//基本の役割：群の情報を出す
	if (m_returnTitleState == ReturnTitleState::None && m_tutorialReviewState == TutorialReviewState::None)
	{
		if (waveManager->GetFinalWave())
		{
			enemyPool->DebugBossPause();
		}
		else
		{
			enemyPool->DebugPause();
		}
	}
	else
	{
		playBackButton.visible = false;
	}

	//=================================
	// --- タイトルに戻るボタン ---
	//=================================
	auto& titleBack = m_buttonData[(int)UIButton::TITLEBACK];
	auto& yesButton = m_buttonData[(int)UIButton::MESSAGE_YES];
	auto& noButton = m_buttonData[(int)UIButton::MESSAGE_NO];

	//振り返りボタン
	auto& tutorialReviewButton = m_buttonData[(int)UIButton::TUTORIAL_REVIEW];



	switch (m_returnTitleState)
	{
	case ReturnTitleState::None:
	{
		// タイトルに戻るボタンの表示
		if (m_tutorialReviewState == TutorialReviewState::None)
		{
			titleBack.visible = true;
			//===============================
			// --- タイトルボタンを押す ---
			//===============================
			if (UpdateButton(UIButton::TITLEBACK))
			{
				soundManager->PlaySE(titleBack.se_id);



				// 黒い背景を拡大して表示
				m_windowMode = WindowMode::Open;

				// タイトルに戻るかの確認するモードに切り替え
				m_returnTitleState = ReturnTitleState::Confirm;

				// タイトルボタンを隠して「Yes」「No」ボタンを表示
				titleBack.visible = false;
				yesButton.visible = true;
				noButton.visible = true;
			}

		}
		else
		{
			titleBack.visible = false;
		}


		break;
	}
	case ReturnTitleState::Confirm:
	{
		if (m_windowMode == WindowMode::Open)
		{
			// 黒い背景が目標値に到達したら文字を表示し
			// 「Yes」「No」ボタンを押せるようにする
			if (UpdateMessageWindow(0.3f, L"本当に戻りますか？", WindowMode::Open,
				&m_explanationUI[(int)ExplanationType::Exp]))
			{
				if (UpdateButton(UIButton::MESSAGE_YES))
				{
					soundManager->PlaySE(yesButton.se_id);

					yesButton.visible = false;
					noButton.visible = false;

					m_gameState = GameFinish;
				}

				if (UpdateButton(UIButton::MESSAGE_NO))
				{
					soundManager->PlaySE(noButton.se_id);

					m_windowMode = WindowMode::Close;
				}
			}
		}
		else if (m_windowMode == WindowMode::Close)
		{
			if (UpdateMessageWindow(0.4f, L"", WindowMode::Close, &m_explanationUI[(int)ExplanationType::Exp]))
			{
				m_windowMode = WindowMode::None;

				m_returnTitleState = ReturnTitleState::None;

				yesButton.visible = false;
				noButton.visible = false;
			}
		}

		break;
	}
	}


	//========================================
	// --- チュートリアル振り返りボタン ---
	//========================================
	auto& enemyLeaderButton = m_buttonData[(int)UIButton::TUTORIAL_ENEMY_LEADER];
	auto& expButton = m_buttonData[(int)UIButton::TUTORIAL_EXP];
	auto& levelUpButton = m_buttonData[(int)UIButton::TUTORIAL_LEVEL_UP];
	auto& skillsButton = m_buttonData[(int)UIButton::TUTORIAL_SKILLS];
	auto& playerOperationButton = m_buttonData[(int)UIButton::TUTORIAL_PLAYER_OPERATION];
	auto& playerDamageButton = m_buttonData[(int)UIButton::TUTORIAL_PLAYER_DAMAGE];

	auto& backButton = m_buttonData[(int)UIButton::TUTORIAL_BACK];

	switch (m_tutorialReviewState)
	{
	case TutorialReviewState::None:
	{
		// 振り返りボタンの表示(タイトルボタンを押していないとき)
		if (m_returnTitleState == ReturnTitleState::None)
		{
			tutorialReviewButton.visible = true;
			if (UpdateButton(UIButton::TUTORIAL_REVIEW))
			{
				soundManager->PlaySE(tutorialReviewButton.se_id);

				
				tutorialReviewButton.visible = false;

				// 振り返り用のボタンを表示
				SetTutorialReviewButtonVisible(true);

				//閉じるボタンも表示
				backButton.visible = true;

				m_tutorialReviewState = TutorialReviewState::Select;
			}

		}
		else
		{
			tutorialReviewButton.visible = false;
		}


		break;
	}

	case TutorialReviewState::Select:
	{
		tutorialReviewButton.visible = false;

		SetTutorialReviewButtonVisible(true);

		// 説明ボタンを対応した説明のチュートリアルをセットにする
		const std::pair<UIButton, ExplanationType> explanationButtons[] =
		{
			{ UIButton::TUTORIAL_ENEMY_LEADER,	 ExplanationType::EnemyLeader },
			{ UIButton::TUTORIAL_EXP,			 ExplanationType::Exp },
			{ UIButton::TUTORIAL_LEVEL_UP,		 ExplanationType::LevelUp },
			{ UIButton::TUTORIAL_SKILLS,		 ExplanationType::Skills },
			{ UIButton::TUTORIAL_PLAYER_OPERATION, ExplanationType::PlayerOperation },
			{ UIButton::TUTORIAL_PLAYER_DAMAGE,	 ExplanationType::PlayerDamage }
		};

		//全てのチュートリアル振り返りボタンにあったもののみ押せるようにする
		//ゲーム中に体験したチュートリアルのみを閲覧可能にする
		for (const auto& explanationButton : explanationButtons)
		{
			UIButton button = explanationButton.first;
			ExplanationType type = explanationButton.second;

			if (m_explanationUI[(int)type].isOne &&
				UpdateButton(button))
			{
				ChangeTutorialState(type, true);
				SetTutorialReviewButtonVisible(false);
				m_tutorialReviewState = TutorialReviewState::Explanation;
				break;
			}
		}

		// 戻るボタンのみ状況に対応
		if (UpdateButton(UIButton::TUTORIAL_BACK))
		{
			soundManager->PlaySE(
				m_buttonData[(int)UIButton::TUTORIAL_BACK].se_id);

			SetTutorialReviewButtonVisible(false);

			tutorialReviewButton.visible = true;
			m_tutorialReviewState = TutorialReviewState::None;
		}


		break;
	}

	case TutorialReviewState::Explanation:
	{
		SetTutorialReviewButtonVisible(false);

		break;
	}



	}


}


void SceneMain::UpdateBossPause()
{

}


//==================================================
// --- チュートリアル処理 ---
//==================================================
void SceneMain::UpdateTutorial(float deltaTime)
{
	//if (!m_isTutorial)return;
	//ボタンを画面で押すかキーボードで押す（キーボードは仮）
	if (vnKeyboard::trg(DIK_RETURN))
	{
		m_isTutorial = false;
	}


	switch (m_state_tutorial)
	{
	case TutorialState::None:
	{

	}
	break;

	//===================================================
	// --- 説明UIが出る ---
	//===================================================
	//説明を出して閉じるまでの処理
	//UpdateExplanation(ExplanationType::チュートリアルのタイプ（例：リーダー撃破など）);

	case TutorialState::ExplainEnemyLeader:
	{
		UpdateExplanation(ExplanationType::EnemyLeader);

	}
	break;

	case TutorialState::ExplainExp:
	{
		UpdateExplanation(ExplanationType::Exp);
	}
	break;

	case TutorialState::ExplainLevelUp:
	{
		UpdateExplanation(ExplanationType::LevelUp);

	}
	break;

	case TutorialState::ExplainSkills:
	{
		UpdateExplanation(ExplanationType::Skills);
	}
	break;

	case TutorialState::ExplainPlayerOperation:
	{
		UpdateExplanation(ExplanationType::PlayerOperation);
	}
	break;


	case TutorialState::ExplainPlayerDamage:
	{
		UpdateExplanation(ExplanationType::PlayerDamage);
	}
	break;



	case TutorialState::Finish:
	{

	}
	break;
	}

}



//----------------------------------------

// --- ゲーム中 ---

//----------------------------------------
void SceneMain::UpdatePlay(float deltaTime)
{
	// プレイヤーの更新（移動、エフェクト、落下チェック、HP減少など）
	UpdatePlayer(deltaTime);
	UpdateBlocksCollision();
	// 敵の出現管理（WaveManagerの状態を確認してスポーン）
	SpawnEnemies(deltaTime);

	// 敵全体の更新（移動、敵同士の衝突、プレイヤーとの衝突・撃破）
	UpdateEnemies(deltaTime);

	// コンボと回復の更新（コンボのタイマー処理、コンボの画像表示）
	UpdateCombo(deltaTime);

	// グローバルな更新（フェンスの拡大、カメラ追従、タイマー加算）
	UpdateGlobalSystems(deltaTime);


	// レベルアップチェック//毎フレーム呼ばれるのを防ぐ
	if (m_pExpManager && m_pExpManager->GetLevelUpStock() > 0)
	{
		m_gameState = LevelUp;
		soundManager->PlaySE(SE_LEVELUP);

		// 3つの選択肢を表示
		for (int i = 0; i < 3; i++)
		{
			// UI用データの取得（本来はGetterを作るのが理想）
			// ここでは仮に直接参照するか、公開されたデータを使います
			const ExperienceManager::UpgradeUIData* pChoices = m_pExpManager->GetUIDisplayChoices();
			if (!pChoices) break;

			//auto& data = m_pExpManager->GetUIDisplayChoices()[i];
			ExperienceManager::UpgradeUIData data = pChoices[i];
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



//==================================================
// --- プレイヤー挙動・衝突判定・回復処理 ---
//==================================================
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
	OnCollider(m_pNewPlayer->GetModel(), pGround, 1.0f, m_pNewPlayer->GetRigidbody());
	if (m_pNewPlayer->GetModel()->getPositionY() < -30.0f) {
		//プレイヤーが一定以上落下したら
		m_pNewPlayer->GetModel()->setPosition(0.0f, 1.0f, 0.0f);
	}

	InFence(m_pNewPlayer->GetModel());

	if (waveManager->GetState() == WaveManager::WaveState::InProgress)
	{
		//ゲーム中自動でHPが減る(最終WAVEのみ減らない)
		if (!waveManager->GetFinalWave())
		{
			//m_pNewPlayer->Damage((1.0f * (waveManager->GetCurrentWave() * 0.8) * deltaTime));
		}
		// --- HPが０になったらGameOver ---
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
			// 基礎係数を 0.5 -> 0.2 に落とす
			float baseEffect = (float)m_comboCount * 0.2f;

			// 減衰を (currentWave + 1) ではなく、2乗に近い形にする
			// Wave1: 1.0 / 1.5 = 0.66
			// Wave5: 1.0 / 5.5 = 0.18
			float waveMitigation = 1.0f / ((float)waveManager->GetCurrentWave() * 0.25f + 0.75f);

			float totalRecovery = baseEffect * waveMitigation;

			// 1回の回復に「上限」を作る
			// これを入れないと、100コンボ超えの時に一気にHPが全快してしまいます
			float maxHealCap = 3.0f; // Waveごとのダメージ数秒分に抑える
			if (totalRecovery > maxHealCap) {
				totalRecovery = maxHealCap;
			}

			//pPlayerTest->addHP(totalRecovery);
			m_pNewPlayer->AddHP(totalRecovery/2,m_isTutorial);
			m_killCounter = 0;
		}
	}

#pragma endregion



#pragma region チュートリアル主にスキル説明
	if (!m_isTutorial)return;
	//スキルを獲得したら説明
	if (m_pNewPlayer->GetIsHaveAreaAtkSkill() || m_pNewPlayer->GetIsHavePullSkill())
	{
		ChangeTutorialState(ExplanationType::Skills,false);
	}
	//HPが減ったらダメージの説明を入れる
	if (m_pNewPlayer->GetCurrentHp() != m_pNewPlayer->GetMaxHp())
	{
		ChangeTutorialState(ExplanationType::PlayerDamage,false);
	}
#pragma endregion



}


//==================================================
// --- 敵の出現管理 ---
//==================================================
void SceneMain::SpawnEnemies(float deltaTime)
{
	// === 出現 ===
	waveManager->Update(deltaTime);
	//int totalCount = (int)enemyPool->GetEnemies().size();
	m_activeCount = enemyPool->GetActiveCount();
	bool isBossWave = (waveManager->GetCurrentWave() == waveManager->GetMaxWave());

	//最終WAVEの時は近接耐性分だけ特攻状態の敵が出てくる
	int maxLimit = isBossWave ? ((int)enemyPool->GetBossGroupData().get()->meleeFear+1)*finalChargeEnemyNum: waveManager->GetMaxSpawnLimit();

	if (m_activeCount < maxLimit && waveManager->GetState() == WaveManager::WaveState::InProgress)
	{
		m_spawnNum = maxLimit - m_activeCount;

		for (int i = 0; i < m_spawnNum; i++)
		{
			// ウェーブの残り出現枠がある時だけスポーン
			if (isBossWave||waveManager->CanSpawn())
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
				enemyPool->Spawn(pos, waveManager->GetCurrentWave(), waveManager->GetMaxWave());
				//enemyPool->Spawn(pos);
				if (!isBossWave) {
					waveManager->OnEnemySpawned(); // 通常Waveのみカウントを進める
				}

			}
		}
	}
}

//==================================================
// --- 敵の移動・衝突・プレイヤーとの判定 ---
//==================================================
void SceneMain::UpdateEnemies(float deltaTime)
{
	// プレイヤー座標を全敵に渡す
	enemyPool->SetPlayerPosAll(*m_pNewPlayer);

	// 更新
	enemyPool->Update(deltaTime);
	// 移動反映＋地面衝突
	auto& enemies = enemyPool->GetEnemies();
	// --- WAVEをクリアしたら敵を消す ---
	if (waveManager->GetWaveTimer() <= waveManager->GetWaveTimeLimit())
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
		float dist = XMVectorGetX(XMVector3LengthSq(toPlayerVec));
		// 敵に「今は吸い込み中か？」を判断させる
		enemy->CheckPullTrigger(m_pNewPlayer->IsPulling(), m_pNewPlayer->GetPullRadius(), dist);



		// --- 移動 ---
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
			
			//=========================================
			// --- 敵同士の当たり判定 ---
			//=========================================
			bool aIsL = a->GetIsLeader();
			bool bIsL = b->GetIsLeader();
			bool sameGroup = (a->GetGroupID() == b->GetGroupID());

			// リーダー同士なら（群れが違っても）当たる
			// 同じ群れの配下同士（リーダーじゃない者同士）なら当たる
			if ((aIsL && bIsL) || (sameGroup && !aIsL && !bIsL))
			{
				colliderCtoC(a, b);
				colliderCtoC(b, a);
			}

		}
	}
	//================================
	// --- 敵と味方の当たり判定 ---
	//================================
	for (size_t i = 0; i < enemyPool->GetEnemies().size(); ++i)
	{
		NewEnemyClass* enemy = enemyPool->GetEnemies()[i];

		if (!enemy->GetActive()) continue;

		if (enemy->GetState() != NewEnemyClass::eState::KnockBack)
		{
			auto dir = colliderStoS(enemy, m_pNewPlayer);
			//死因を保存（初期値は近接攻撃）
			NewEnemyClass::DamageSource source = NewEnemyClass::DamageSource::Melee;

			if (dir != None)
			{
				// --- 倒したとき ---
				
				//================================================
				// --- チュートリアルなら説明を出す ---
				//================================================
				if (!enemy->GetIsLeader())
				{
					if (m_state_tutorial == TutorialState::None)
					{
						ChangeTutorialState(ExplanationType::Exp, false);
					}
				}
				else
				{
					ChangeTutorialState(ExplanationType::EnemyLeader, false);
				}



			    soundManager->PlaySE(SE_ENEMY_DEAD);

				pEmitter->setPosition(enemy->GetModel()->getPosition());
				pEmitter->setPositionY(enemy->GetModel()->getPositionY() + 2.0f);
				int index = rand() % (sizeof(vnEmitter::colors) / sizeof(vnEmitter::colors[0]));
				pEmitter->SetColor(vnEmitter::colors[index]);
				pEmitter->setEmit(true, 0.3f);
				AddCombo(enemy);

				// 敵が特攻状態の時に範囲攻撃以外で倒すとダメージを受けるようにする
				if (enemy->GetState() == NewEnemyClass::eState::Charge/*|| enemy->GetIsCharge()*/)
				{
					if (!m_pNewPlayer->IsAreaAttack())
					{
						m_pNewPlayer->Damage(defualtDamage,m_isTutorial);

					}
				}

				//-------------------------------------------------------
				// ボス関係
				//-------------------------------------------------------
				if (enemy->GetIsBoss())
				{
					if (!m_pNewPlayer->IsAreaAttack() || !m_pNewPlayer->IsPulling())
					{
						//ボスに当たるとダメージを受ける
						//float damage = enemy->GetGroupData()->meleeFear;
						//m_pNewPlayer->Damage(damage);
						//m_pNewPlayer->Jump();
					}
				}

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
				
				//waveManager->OnEnemyKilled();
				bool isBossWave = (waveManager->GetCurrentWave() == waveManager->GetMaxWave());
				//最終WAVEのみボスを倒したときにのみカウントを増やす
				if (isBossWave)
				{
					if (enemy->GetIsBoss())
					{
						waveManager->OnEnemyKilled(); 
					}
				}
				else
				{
					// 通常WAVEの場合：ザコを倒したら普通にカウントを進める（既存のロジック）
					waveManager->OnEnemyKilled();
				}
			}
			else
			{
				//enemy->SetIsHitPlayer(false);
				//--当たった時に枠外に飛ばせる
				InFence(enemy->GetModel());
			}

			//==================================================
			// 弾と当たった時
			//==================================================
			auto dirEtoB = colliderStoS(enemy, m_pBullet);

			if (dirEtoB != None)
			{
				// --- 倒したとき ---
				soundManager->PlaySE(SE_ENEMY_DEAD);

				pEmitter->setPosition(enemy->GetModel()->getPosition());
				pEmitter->setPositionY(enemy->GetModel()->getPositionY() + 2.0f);
				int index = rand() % (sizeof(vnEmitter::colors) / sizeof(vnEmitter::colors[0]));
				pEmitter->SetColor(vnEmitter::colors[index]);
				pEmitter->setEmit(true, 0.3f);
				AddCombo(enemy);

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
}


//==================================================
// --- コンボ計算(UI) ---
//==================================================
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
			pComboSprites[i][j]->setPos(offScreen, offScreen);
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
			float startX = combo_num_x;
			float spacing = (128.0f * 0.5f) * m_comboScale / 1.5;
			float posX = startX + (i * spacing);

			// スプライトを取得して設定
			vnSprite* s = pComboSprites[i][digits[i]];
			s->setPos(posX, combo_num_y);
			s->setScale(0.5f * m_comboScale);

			// 【追加】透明度を適用
			s->setAlpha(currentAlpha);
		}

		// Combo!! の表示
		pComboWord->setPos(combo_img_x, combo_img_y);
		pComboWord->setScale(m_comboScale);

		// Combo!!も一緒にフェード
		pComboWord->setAlpha(currentAlpha);

	}
	else {
		// コンボが0なら、Combo!!も画面外に隠す
		pComboWord->setPos(offScreen, offScreen);
	}

	// --- WAVEクリアしたらコンボTimerを最大にしとく ---
	if (waveManager->GetState() != WaveManager::WaveState::InProgress)
	{
		m_comboTimer = m_currentComboLimit;
	}



#pragma endregion
}


//==================================================
// --- コンボ加算(経験値獲得) ---
//==================================================
void SceneMain::AddCombo(NewEnemyClass* enemy)
{
	// コンボ加算！
	m_comboCount++;
	// Waveごとに短くなっている「現在の限界時間」をセット
	m_comboTimer = m_currentComboLimit;
	m_comboScale = 1.5f; // 一瞬で1.5倍の大きさに跳ねさせる！(コンボの文字)
	m_killCounter++; // 回復用カウンターを増やす

	//全ての項目が最大でなければ経験値を獲得しレベルアップする
	if (!m_pExpManager->AllIsMaxLv())
	{
		if (m_comboCount % 200 == 0)
		{
			//==================================
			// コンボ200ごとにレベルアップ
			//==================================
			m_pExpManager->GainLevel(1);

			Common::StartCameraShake(3.5f, 3.5f, 1.0f);
		}
		//経験値を獲得
		if (m_pExpManager)
		{
			float expAmount = 0.5f * (1.0f + (waveManager->GetCurrentWave() * 0.2f));
			//リーダーの時と、その他がパニック状態の時にもらえる経験値が増える
			if (enemy->GetIsLeader())
			{
				expAmount *= 3.0f;
			}
			else if (!enemy->GetIsLeader() && enemy->GetState() == NewEnemyClass::eState::Panic)
			{
				expAmount *= 1.5f;

			}
			//チュートリアル中のみ獲得経験値を増やす
			if (m_isTutorial)expAmount *= 2.0f;

			m_pExpManager->GainExp(expAmount);
		}

	}

}


//==================================================
// --- フェンス・タイマー・カメラ ---
//==================================================
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

//==================================================
//WAVE更新処理
//==================================================
void SceneMain::UpdateWaveTransition()
{
	// --- WAVEの状態の切り替え(WAVEクリア→次のWAVEとか) ---
	if ((waveManager->GetState() == WaveManager::WaveState::ClearWait) && waveManager->GetCurrentWave() < waveManager->GetMaxWave())
	{
		if (isWaveClear == false) // WAVEクリア時
		{
			soundManager->PlaySE(SE_WAVE_CLEAR);

			//クリアしたと同時にプレイヤーのスキルのクールタイムをリセットする
			m_pNewPlayer->ResetSkillCoolTime();

		}
		isWaveClear = true;

		if (vnKeyboard::on(DIK_RETURN) || vnMouse::onR())
		{
			soundManager->PlaySE(SE_ENTER);
			//WAVEを更新
			waveManager->GoNextWave();
			
			//木の幅を更新
			SetWAVETree();


			isWaveClear = false;
			m_pBlockManager->RespawnBlocks(waveManager->GetCurrentWave(), FenceRadius,waveManager->GetFinalWave());
			
			//最終ステージ（ボス登場）
			if (waveManager->GetFinalWave())
			{
				enemyPool->SetBossData();
				soundManager->PlaySE(SE_BOSS_ENEMY);
			}

		}
	}
	if (waveManager->GetState() == WaveManager::WaveState::ClearWait && waveManager->GetCurrentWave() == waveManager->GetMaxWave())
	{
		if (!isGameFinish)
		{
			m_gameState = GameClear;

			CleanUpScene();

		}
	}

}

//==================================================
// --- 背景の木の位置の更新 ---
//==================================================
void SceneMain::SetWAVETree()
{
#if ENABLE_TREE_DELETE
	for (int line = 0; line < 2; line++)
	{
		for (int i = 0; i < TREE_NUM; i++)
		{
    		int index = line * TREE_NUM + i;

			//木が生成されていなければスキップ
			if (pTree[index] == nullptr)continue;

			//等間隔のラジアンを計算（i番目の角度）
			float radian = (2.0f * 3.14159f / (float)TREE_NUM) * (float)i;

			if (line == 1)
			{
				float halfGap = (2.0f * 3.14159f / (float)TREE_NUM / 2.0f);
				radian += halfGap;
			}

			//半径の決定（最新のFenceRadiusをもとに計算）
			float lineOffset = (line == 0) ? 0.0f : 7.0f;
			float totalRadius = FenceRadius + treeRadius + lineOffset;

			//円周上の新しい座標を計算
			float x = sinf(radian) * totalRadius;
			float z = cosf(radian) * totalRadius;

			//位置を更新
			pTree[index]->setPosition(x, 0.0f, z);


		}
	}
#else
	for (int i = 0; i < TREE_NUM; i++)
	{
		int index = i;

		//木が生成されていなければスキップ
		if (pTree[index] == nullptr)continue;

		//等間隔のラジアンを計算（i番目の角度）
		float radian = (2.0f * 3.14159f / (float)TREE_NUM) * (float)i;


		//半径の決定（最新のFenceRadiusをもとに計算）
		float totalRadius = FenceRadius + treeRadius;

		//円周上の新しい座標を計算
		float x = sinf(radian) * totalRadius;
		float z = cosf(radian) * totalRadius;

		//位置を更新
		pTree[index]->setPosition(x, 0.0f, z);


	}

#endif

}


//==================================================
// --- レベルアップ画面の更新 ---
//==================================================
void SceneMain::UpdateLevelUp()
{
	if (m_state_tutorial != TutorialState::None)
	{
		return;
	}


	//敵を全て消す
	enemyPool->HideAllActiveEnemies();

	// 共通の更新（演出など）
	m_pNewPlayer->UpdateLevelUp();
	OnCollider(m_pNewPlayer->GetModel(), pGround, 1.0f, m_pNewPlayer->GetRigidbody());

	
	m_pUpgradeUI->UpdateUI();




	// --- 1. 入力判定（UIがまだ消えていない時だけ受け付ける） ---
	if (!m_pUpgradeUI->GetIsClosingUI()) // UIが消去演出中でなければ
	{
		int selectedIndex = -1;
		//if (vnKeyboard::trg(DIK_1)) selectedIndex = 0;
		//if (vnKeyboard::trg(DIK_2)) selectedIndex = 1;
		//if (vnKeyboard::trg(DIK_3)) selectedIndex = 2;

		//選択肢の画像をマウスカーソルを持って来てクリック
		for (int i = 0; i < 3; i++)
		{
			bool isMax = m_pExpManager->IsMaxUpgrade(i);
			if (UpdateUpgradeButton(
				m_pUpgradeUI->GetDisplayFrameImg(i)->getPosX(),
				m_pUpgradeUI->GetDisplayFrameImg(i)->getPosY(),
				m_pUpgradeUI->GetDisplayFrameImg(i),
				m_pUpgradeUI->GetDisplayBackGroundImg(i),
				m_pUpgradeUI->GetDisplayMainImg(i),
				m_isOnSelectButton[i],
				m_SelectButtonScale[i],
				isMax
			))
			{


				selectedIndex = i;
			}

		}

		if (selectedIndex != -1)
		{
			m_pExpManager->ApplyUpgrade(selectedIndex,m_isTutorial);
			soundManager->PlaySE(SE_ENTER);
			m_pUpgradeUI->SetUiPhase(UpgradeSelectionUI::UIPhase::Closing);
			m_pUpgradeUI->HideUI(); // ここでアニメーション開始（IsHidingがtrueになる想定）
		}

	}
	//チュートリアルの説明を挟む
	ChangeTutorialState(ExplanationType::LevelUp, false);


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
			m_pUpgradeUI->ResetPhase();

		}
		else
		{
			m_pUpgradeUI->ResetPhase();

			const ExperienceManager::UpgradeUIData* pChoices =
				m_pExpManager->GetUIDisplayChoices();

			if (pChoices)
			{
				for (int i = 0; i < 3; i++)
				{
					ExperienceManager::UpgradeUIData data = pChoices[i];
					int index = m_pExpManager->GetChoiceIndex();

					m_pUpgradeUI->SettingUI(data, i, index);
				}
			}
		}
	}

}

//==================================================
//ボタン当たり判定
//==================================================
bool SceneMain::OnButton(float x, float y)
{
	int mx = vnMouse::getX();
	int my = vnMouse::getY();
	
		
	if (mx >= x - freamImgW / 2 && mx <= x + freamImgW / 2 &&
		my >= y - freamImgH / 2 && my <= y + freamImgH / 2) // 中心座標からの判定
	{
		return true;
	}
	else
	{
		return false;
	}
}
//==================================================
//ボタン処理（ボタン押したときにtrue）
//==================================================
bool SceneMain::UpdateUpgradeButton(
	float x,
	float y,
	vnSprite* pFrame,
	vnSprite* pBg,
	vnSprite* pMain,
	bool& isOnButton,
	float& buttonScale,
	bool isMax)
{
	if (isMax)
	{
		isOnButton = false;
		buttonScale += (1.0f - buttonScale) * 0.2f;

		pFrame->setScale(buttonScale);
		pBg->setScale(buttonScale);
		pMain->setScale(buttonScale);

		return false;
	}
	if (OnButton(x, y))
	{
		if (!isOnButton)
		{
			soundManager->PlaySE(SE_TITLE_CURSOR);
		}
		isOnButton = true;
		buttonScale += (1.2f - buttonScale) * 0.2f;

		//pFrame->setColor(V_GAME_COLOR_BLACK);

		if (vnMouse::trgL())
		{
			return true; // クリックされた
		}
	}
	else
	{
		isOnButton = false;
		buttonScale += (1.0f - buttonScale) * 0.2f;
		//pFrame->setColor(V_GAME_COLOR_YELLOW);
	}

	// 3つのスプライト全てに同じスケールを適用
	pFrame->setScale(buttonScale);
	pBg->setScale(buttonScale);
	pMain->setScale(buttonScale);


	return false;
}



//==================================================
// --- ゲームオーバー ---
//==================================================
void SceneMain::UpdateGameOver()
{
	soundManager->StopSE(SE_GRILL);
	soundManager->PlayBGM(BGM_GAMEOVER);
	
	//=====================================================
	// プレイヤーのモデルを隠し、画像を表示する
	//=====================================================
	for (int i = 0; i < m_pNewPlayer->GetModel()->getPartsNum(); i++)
	{
		m_pNewPlayer->GetModel()->getParts(i)->setRenderEnable(false);
	}
	m_pNewPlayer->GetUpKaraModel()->setRenderEnable(false);

	float x, y;
	if (vnFont::CalculateScreenPosition(*m_pNewPlayer->GetModel()->getPosition(), &x, &y))
	{
		m_pGameOverPlayer->setPos(x, y);
	}

	//=====================================================

	if (vnKeyboard::trg(DIK_RETURN) || vnMouse::trgR())
	{
		soundManager->PlaySE(SE_ENTER);

		isGameFinish = true;
		m_gameState = GameFinish;
	}

}



//==================================================
// --- ゲームクリア ---
//==================================================
void SceneMain::UpdateGameClear()
{
	soundManager->StopSE(SE_GRILL);
	soundManager->PlayBGM(BGM_GAMECLEAR);
	//レベルアップ時と同様にジャンプさせる
	m_pNewPlayer->UpdateLevelUp();

	m_levelUpCameraTargetTheta =
		m_pNewPlayer->GetModel()->getRotationY()
		+ XM_PI
		+ XM_PIDIV2;
	Common::UpdateCameraLevelUp(
		m_pNewPlayer->GetModel()->getPosition(),
		m_levelUpCameraTargetTheta,
		vnScene::getDeltaTime(),
		m_phi,
		m_radius,
		m_theta);


	OnCollider(m_pNewPlayer->GetModel(), pGround, 1.0f, m_pNewPlayer->GetRigidbody());

	InFence(m_pNewPlayer->GetModel());


	if (vnKeyboard::trg(DIK_RETURN)||vnMouse::trgR())
	{
		soundManager->PlaySE(SE_ENTER);

		isGameFinish = true;
		m_gameState = GameFinish;
	}

}

//==================================================
//--敵の消去など共通の処理
//==================================================
void SceneMain::CleanUpScene()
{
	pDustEmitter->setEmit(false, 0);

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
		pComboWord->setPos(offScreen, offScreen);
		for (int i = 0; i < 3; i++) {
			// スプライトを取得して設定
			vnSprite* s = pComboSprites[i][digits[i]];
			s->setPos(offScreen, offScreen);
		}

	}
}

//==================================================
// --- ブロックとの当たり判定 ---
//==================================================
void SceneMain::UpdateBlocksCollision()
{
	// --- 地形との当たり判定（ブロック） ---
	// プレイヤー vs ブロック
	//for (auto* block : m_pBlockManager->GetAllActiveBlocks()) {

	//	// 1. 衝突判定と押し戻しを1回ずつ実行し、当たったかどうかのフラグを取る
	//	// (プレイヤーからブロック、ブロックからプレイヤーの双方の押し戻し結果を記録)
	//	bool isHit1 = colliderCtoC(m_pNewPlayer, block);
	//	bool isHit2 = colliderCtoC(block, m_pNewPlayer);

	//	// 2. どちらか一方ででも衝突が検知されていたら「当たっている」とみなす
	//	bool isColliding = isHit1 || isHit2;

	//	// マグマのブロックに触れているならダメージ
	//	if (block->GetIsMagma() && isColliding)
	//	{
	//		soundManager->PlaySEing(SE_ENEMY_CHARGE);
	//		m_pNewPlayer->Damage(defualtDamage * 0.05f);
	//	}
	//	else
	//	{
	//		soundManager->StopSE(SE_ENEMY_CHARGE);
	//	}
	//}

	bool isTouchingMagma = false;

	for (auto* block : m_pBlockManager->GetAllActiveBlocks()) {

		// --- 音・ダメージ用の「少し広げた」判定チェック ---
		bool isNearMagma = false;
		if (block->GetIsMagma()) {
			// 1. 判定を少しだけ広げるためのマージン（サイズを1%〜2%大きくするイメージ）
			// 0.02f の値はゲームのスケールに合わせて調整してください（1cm〜2cm相当）
			XMVECTOR margin = XMVectorSet(0.02f, 0.02f, 0.02f, 0.0f);
			XMVECTOR range = XMVectorAdd(
				XMVectorAdd(m_pNewPlayer->GetCollision().GetSize() * 0.5f, margin),
				block->GetCollision().GetSize() * 0.5f
			);

			float rx = XMVectorGetX(range);
			float ry = XMVectorGetY(range);
			float rz = XMVectorGetZ(range);

			XMVECTOR center1 = XMVectorAdd(*m_pNewPlayer->GetModel()->getPosition(), m_pNewPlayer->GetCollision().GetCenter());
			XMVECTOR center2 = XMVectorAdd(*block->GetModel()->getPosition(), block->GetCollision().GetCenter());
			XMVECTOR dif = XMVectorAbs(center1 - center2);

			// 2. 押し戻す前の「隣接しているか」の純粋なフラグを取る
			if (XMVectorGetX(dif) < rx && XMVectorGetY(dif) < ry && XMVectorGetZ(dif) < rz) {
				isNearMagma = true;
			}
		}

		// --- 通常の衝突判定と押し戻し ---
		bool isHit1 = colliderCtoC(m_pNewPlayer, block);
		bool isHit2 = colliderCtoC(block, m_pNewPlayer);
		bool isColliding = isHit1 || isHit2;

		// 通常の衝突、マグマ接触
		if (block->GetIsMagma() && (isColliding || isNearMagma))
		{
			m_pNewPlayer->Damage(defualtDamage * 0.05f,m_isTutorial);
			isTouchingMagma = true;
		}
	}

	// 全てのブロックをチェックし終わった後に、音の管理をする
	if (isTouchingMagma)
	{
		soundManager->PlaySEing(SE_GRILL);
	}
	else
	{
		soundManager->StopSE(SE_GRILL);
	}


	// 敵 vs ブロック
	for (auto* enemy : enemyPool->GetEnemies()) {
		if (!enemy->GetActive()) continue;
		if (enemy->IsAttracted()) {
			continue;
		}
		for (auto* block : m_pBlockManager->GetAllActiveBlocks()) {
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

	for (auto* block : m_pBlockManager->GetAllActiveBlocks()) {
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

	DebugDraw();
}

void SceneMain::DebugDraw() 
{

}



#pragma region 物理演算（地面との当たり判定など）


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
//=====================================================================
// 弾の反射用
//=====================================================================
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
//==================================================
// --- 当たり判定（地面とキャラクター）
//==================================================
void SceneMain::OnCollider(vnCharacter* pCharacter, vnModel* pGround, float footOffset, RigidbodyComponent& rigidBody)
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
//==================================================
// --- 当たり判定（キャラクターとキャラクター）
//==================================================
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
//==================================================
// 球体同士の判定と押し戻し
//==================================================
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

		float overlap = (sumRadii - dist) * 1.1f;
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

#pragma endregion
