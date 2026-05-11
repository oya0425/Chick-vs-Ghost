#pragma once
//class cObj : public vnModel
//{
//public:
//	XMVECTOR center;	//AABBの中心(vnModel::Positionからの相対値
//	XMVECTOR size;		//AABBのサイズ
//
//
//	cObj(const WCHAR* folder, const WCHAR* file);
//};

class SceneGroundTest : public vnScene
{
private:
	// --- 後期追加 ---
	vnModel* pCrown;

	// --- 
	//直線の情報
	XMVECTOR LinePos;	//直線上の１点（始点、位置ベクトル）
	XMVECTOR LineDir;	//直線の方向（方向ベクトル、正規化済み）

	//平面の情報（　＾ω＾）・・・( ,,`･ω･´)ﾝﾝﾝ？
	//平面の方程式 ax + by + cz = d
	XMVECTOR PlaneNormal;	//平面の法線（a,b,c）
	float PlaneDistance;	//平面と原点との距離(d)


	// ----------------



	vnModel* pPlayer;
	//OBB* pPlayer;

	vnModel* pShadow;


	

	vnModel* pGround;

	vnModel* pSky;

	//プレイヤーアクション関連(ジャンプ)
	bool air;
	XMVECTOR	jumpForce;


	XMVECTOR	velocity;
	XMVECTOR	gravity;

	//プレイヤー初期位置
	XMVECTOR	startPos;

	//2D表示

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


	//極座標
	float radius;	//半径
	float theta;	//角度（平面角/経度）
	float phi;		//角度（仰角/緯度）φ（ファイ）
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
	//eDirection colliderAtoB(cObj* p1, cObj* p2);



	//AABBオブジェクトの作成
	//cObj* createObj(float posX, float posY, float posZ, float sizeX, float sizeY, float sizeZ);



public:
	//初期化
	bool initialize();
	//終了
	void terminate();

	//処理
	void execute();
	//描画
	void render();



};
