#pragma once
#include"../object/bear.h"
#define AABB_MAX (64)
//
#define FENCE_NUM (32)			//フェンスモデルの数
#define FENCE_RADIUS (30)	//フェンスを配置する円周の半径
class cObj : public vnModel
{
public:
	XMVECTOR center;	//AABBの中心(vnModel::Positionからの相対値
	XMVECTOR size;		//AABBのサイズ


	cObj(const WCHAR* folder, const WCHAR* file);
};

class SceneFieldTest : public vnScene
{
private:
	// --- 後期追加 ---
	vnModel* pCrown;

	vnEmitter* pEmitter;
	vnEmitter* pEmitterNPC;

	vnEmitter* pEmitterBrid;

	//vnModel* pBrid;
	vnModel* pKaraUp;

	// ----------------
	std::vector<vnEmitter*> emitters;


	//cObj* pPlayer;
	//cBear* pPlayer;
	vnCharacter* pPlayer;
	//OBB* pPlayer;

	vnModel* pShadow;


	
	vnModel* pFence[FENCE_NUM];

	//cObj* pGround;

	vnModel* pSky;
	//vnModel* pNPC;
	//cBear* pNPC;
	vnCharacter* pNPC;
	vnModel* pDebugFan;

	//プレイヤーアクション関連(ジャンプ)
	bool air;
	XMVECTOR	jumpForce;
	XMVECTOR	velocity;
	XMVECTOR	gravity;

	//プレイヤー初期位置
	XMVECTOR	startPos;

	//2D表示
	vnBillboard* pBillboard;
	vnBillboard* pIcon;

	//衝突方向
	enum eDirection
	{
		None,
		X_Pos,
		X_Neg,
		Y_Pos,
		Y_Neg,
		Z_Pos,
		Z_Neg,
	};

	//プレイヤーアクション関連(移動)
	float		speed;
	bool input;


	// --- 0202追加 ---
	PlayerClass* pPlayerTest;
	//直線の情報
	XMVECTOR LinePos;	//直線上の１点（始点、位置ベクトル）
	XMVECTOR LineDir;	//直線の方向（方向ベクトル、正規化済み）
    
	vnModel* pGround;

	vnCharacter* pBoxUnity;
	bool isDead;

	EnemyClass* pEnemyTest;


	//極座標
	float radius;	//半径
	float theta;	//角度（平面角/経度）
	float phi;		//角度（仰角/緯度）φ（ファイ）
	float radiusCam;	//半径
	float thetaCam;	//角度（平面角/経度）
	float phiCam;		//角度（仰角/緯度）φ（ファイ）

	float radiusRe;
	bool reverse;	//反転
	bool onePos;	//一人称変換

	//bool isJump;	//ジャンプ中かどうか
	//float velocityY = 0.0f;
	//float gravity = 0.025f;
	//float jumpPower = 0.5f; // 上向きなのでマイナス
	bool isatari = false;
	XMVECTOR fii;

	//プレイヤーの移動(入力処理)
	bool movePlayer(vnObject* p, bool canMove);

	//カメラの操作(入力処理)
	bool operateCamera(bool update);

	//AABB同士の衝突の検出・解消
	eDirection colliderAtoB(cObj* p1, cObj* p2);

	//AABB同士の衝突の検出・解消
	eDirection colliderCtoC(vnCharacter* p1, vnCharacter* p2);


	eDirection colliderAtoB_new(vnCharacter* p1, cObj* p2);

	void renderEmitters();


	//AABBオブジェクトの作成
	cObj* createObj(float posX, float posY, float posZ, float sizeX, float sizeY, float sizeZ);

	void OnCollider(vnCharacter* pCharacter, vnModel* pGround, float footOffset, RigidbodyComponent &rigidBody);


public:
	//初期化
	bool initialize();
	//終了
	void terminate();

	//処理
	void execute();
	//描画
	void render();

	//void InFence(vnModel* pObject);
	void InFence(vnCharacter* pObject);

	//オブジェクトを回転させる
	void MoveCircle(cObj* obj, float radius, float centerY,float centerX, float speed);

};
