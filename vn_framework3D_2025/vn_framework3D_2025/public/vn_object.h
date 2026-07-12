//--------------------------------------------------------------//
//	"vn_object.h"												//
//		オブジェクト基底クラス									//
//													2025/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

#include "../framework/vn_modelData.h"

//ポリゴン頂点構造体
struct vnVertex3D
{
	float x,y,z;		//座標
	float nx,ny,nz;		//法線
	float r,g,b,a;		//カラー
	float u,v;			//UV
};

class vnObject
{
protected:
	//移動値
	XMVECTOR	Position;
	
	//回転値
	XMVECTOR	Rotation;
	
	//拡大値
	XMVECTOR	Scale;

	//変換行列
	XMMATRIX	Local;
	XMMATRIX	World;

	//階層構造の親
	vnObject* pParent;

	//実行状態の管理
	bool executeEnable;

	//描画状態の管理
	bool renderEnable;

	//ライティング有り
	bool lighting;
	
	//半透明
	bool transparent;

	////深度書き込みの有効/無効
	bool zWrite;
public:
	//レンダーフラグ(パイプラインのパターン)
	enum eRenderFlag
	{
		//(2進数)
		Transparent = 0x00000001,	//半透明
		Lighting = 0x00000002,	//ライティング
		ZWrite = 0x00000004,	//深度書き込み
		Addition=0x00000008,	//加算合成
		DoubleSoded= 0x0000010,//両面描画

		//3ビット全部立った状態が７（２進数）
		MaxNum = 0x00000020,	//組み合わせの最大数
		//MaxNum=(Transparent|Lighting|ZWrite)+1,


	};
protected:

	UINT32 renderFlag;	//オブジェクト固有の描画のパターン(32ビットだから32個以上はこれでは作れない)


	//静的共通データ
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs[];	//頂点要素


	static ID3D12PipelineState* pPipelineState[eRenderFlag::MaxNum];		//パイプラインステート



	//static ID3D12PipelineState		*pPipelineState;		//パイプラインステート
	//static ID3D12PipelineState      *pPipelineState_NoLight;//パイプラインステート(ライティングなし)

	////半透明バージョン
	//static ID3D12PipelineState* pPipelineState_Alpha;		//パイプラインステート
	//static ID3D12PipelineState* pPipelineState_Alpha_NoLight;//パイプラインステート(ライティングなし)

	////深度書き込み無効系
	//static ID3D12PipelineState* pPipelineState_ZOff;		
	//static ID3D12PipelineState* pPipelineState_NL_ZOff;
	//static ID3D12PipelineState* pPipelineState_Alpha_ZOff;
	//static ID3D12PipelineState* pPipelineState_Alpha_NL_ZOff;
	

	//コンスタントバッファ構造体
	struct stConstantBuffer
	{
		XMFLOAT4X4	WVP;		//World*View*Proj
		XMFLOAT4X4	World;		//World
		XMFLOAT4	LightDir;	//平行光源の(逆)方向
		XMFLOAT4	LightCol;	//平行光源の色
		XMFLOAT4	LightAmb;	//環境光の色

		XMFLOAT4    CameraPos;	//カメラのワールド座標
		XMFLOAT4	Diffuse;	//マテリアルの拡散色
		XMFLOAT4	Ambient;	//マテリアルの環境色
		XMFLOAT4	Specular;	//マテリアルの鏡面反射光

	};

	//ローカルマトリクスを計算
	void calculateLocalMatrix();

	//ワールドマトリクスを計算
	void calculateWorldMatrix();


public:


	//静的共通データ初期化
	static bool initializeCommon();

	//静的共通データ削除
	static void terminateCommon();

	//コンストラクタ
	vnObject();

	//デストラクタ
	virtual ~vnObject();
	
	//システム関数
	virtual void execute();
	virtual void render();

	//移動値の設定
	void setPosition(float x, float y, float z);
	void setPosition(const XMVECTOR *v);
	void setPositionX(float value);
	void setPositionY(float value);
	void setPositionZ(float value);
	void addPosition(float x, float y, float z);
	void addPosition(const XMVECTOR *v);
	void addPositionX(float value);
	void addPositionY(float value);
	void addPositionZ(float value);

	//移動値の取得
	XMVECTOR *getPosition();
	float getPositionX(void);
	float getPositionY(void);
	float getPositionZ(void);


	//回転値の設定
	void setRotation(float x, float y, float z);
	void setRotation(const XMVECTOR *v);
	void setRotationX(float radian);
	void setRotationY(float radian);
	void setRotationZ(float radian);
	void addRotation(float x, float y, float z);
	void addRotation(const XMVECTOR *v);
	void addRotationX(float radian);
	void addRotationY(float radian);
	void addRotationZ(float radian);

	//回転値の取得
	XMVECTOR *getRotation();
	float getRotationX(void);
	float getRotationY(void);
	float getRotationZ(void);
	
	
	//拡大値の設定
	void setScale(float x, float y, float z);
	void setScale(const XMVECTOR *v);
	void setScaleX(float value);
	void setScaleY(float value);
	void setScaleZ(float value);
	void addScale(float x, float y, float z);
	void addScale(const XMVECTOR *v);
	void addScaleX(float value);
	void addScaleY(float value);
	void addScaleZ(float value);

	//拡大値の取得
	XMVECTOR *getScale();
	float getScaleX(void);
	float getScaleY(void);
	float getScaleZ(void);


	//ワールド行列の取得
	XMMATRIX *getWorld();

	//ワールド座標の取得
	void getWorldPosition(XMVECTOR *v);
	float getWorldPositionX();
	float getWorldPositionY();
	float getWorldPositionZ();

	//ワールド行列の設定
	void setWorld(XMMATRIX *m);


	//実行状態の設定
	void setExecuteEnable(bool flag);

	//描画状態の設定
	void setRenderEnable(bool flag);
	bool getRenderEnable();

	//実行状態の取得
	bool isExecuteEnable();

	//描画状態の取得
	bool isRenderEnable();

	//ライティングありの設定
	void setLighting(bool flag);

	//ライティング無しの設定
	bool getLighting(void);


	//ライティングありの設定
	void setTransparent(bool flag);

	//ライティング無しの設定
	bool getTransparent(void);

	//深度書き込みの有効/無効の設定
	void setZWrite(bool flag);

	//深度書き込みの有効/無効の取得
	bool getZWrite();

	//加算合成の有効/無効の設定
	void setAddtion(bool flag);
	//両面描画の有効/無効の設定
	void setDoubleSoded(bool flag);


	//階層構造の設定
	bool getTransParent(void);

	//階層構造の設定
	void setParent(vnObject* p);

	//階層構造の取得
	vnObject* getParent(void);

	void removeParent();


	//レンダーフラグの設定
	void setRenderFlag(eRenderFlag flag, bool value);

	//レンダーフラグの取得
	bool getRenderFlag(eRenderFlag flag);

};
