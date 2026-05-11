#pragma once

//ポリゴン頂点構造体
//struct stCubeVertex3D
//{
//	float x, y, z;		//座標
//	float nx, ny, nz;	//法線ベクトル
//	float u,v;			//uv
//};

//ポリゴン頂点構造体(スキン情報付属)
struct stSkinVertex3D
{
	float x, y, z;		//座標
	float nx, ny, nz;	//法線ベクトル
	float u, v;			//uv

	//ボーンの情報
	//※配列の数は影響を受ける最大数（BoneInfluence）多くの環境では４
	UINT16	boneID[2];	//どのボーンに影響を受けるのか（Influence）
	float	boneWeight[2];//ボーンの影響度合い
};

//キューブクラス
class cSkinnedCube
{
public:
	//ボーン情報
	struct stBone
	{
		stBone* pParent;	//親
		XMVECTOR	Pos;	//位置
		XMVECTOR	Rot;	//回転
		XMVECTOR	Scl;	//拡大
		XMMATRIX	Local;	//ローカルマトリクス
		XMMATRIX	World;	//ワールドマトリクス
		
		XMMATRIX	iBind;	//バインド逆行列
		XMMATRIX	SkinMtx;//スキニング変換マトリクス

		XMVECTOR	BindPos;	//初期位置
		XMVECTOR	BindRot;	//初期回転
		XMVECTOR	BindScl;	//初期拡大

	};
	stBone *bones;	//ボーン配列

	int BoneNum;		//ボーン数

protected:
	//頂点数
	int VertexNum;
	//頂点データ
	stCubeVertex3D	*vtx;

	//頂点データ(スキン情報付属)
	stSkinVertex3D	*vtxS;

	//コンスタントバッファ構造体
	struct stConstantBuffer
	{
		XMFLOAT4X4	WVP;		//World*View*Proj
		XMFLOAT4X4	World;		//ワールドマトリクス
		XMFLOAT4	LightDir;	//平行光源の(逆)方向
		XMFLOAT4	LightCol;	//平行光源の色
		XMFLOAT4	LightAmb;	//環境光の色
	};

	//頂点バッファ
	ID3D12Resource* vertBuff;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	//テクスチャ
	ID3D12Resource* texbuff;
	//定数バッファ
	ID3D12Resource* constBuff;
	ID3D12DescriptorHeap* basicDescHeap;
	stConstantBuffer* pConstBuffer;

	//移動値
	XMVECTOR	Position;

	//回転値
	XMVECTOR	Rotation;

	//拡大値
	XMVECTOR	Scale;

	//姿勢変形用マトリクス
	XMMATRIX trans;
	XMMATRIX rotate;
	XMMATRIX scale;
	XMMATRIX world;

	//静的共通データ
	static int						initCommon;	//初期化用
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs[];	//頂点要素
	static ID3D12PipelineState* pPipelineState;	//パイプラインステート

public:
	//静的共通データ初期化
	static bool initializeCommon();

	//静的共通データ削除
	static void terminateCommon();

	//コンストラクタ
	cSkinnedCube(float width = 2.0f, float height = 2.0f, int bone = 2);
	//デストラクタ
	~cSkinnedCube();
	
	//処理
	void execute();
	//描画
	void render();

	//移動値の設定
	void setPositionX(float value);
	void setPositionY(float value);
	void setPositionZ(float value);
	void addPositionX(float value);
	void addPositionY(float value);
	void addPositionZ(float value);

	//移動値の取得
	float getPositionX(void);
	float getPositionY(void);
	float getPositionZ(void);


	//回転値の設定
	void setRotationX(float radian);
	void setRotationY(float radian);
	void setRotationZ(float radian);
	void addRotationX(float radian);
	void addRotationY(float radian);
	void addRotationZ(float radian);

	//回転値の取得
	float getRotationX(void);
	float getRotationY(void);
	float getRotationZ(void);


	//拡大値の設定
	void setScaleX(float value);
	void setScaleY(float value);
	void setScaleZ(float value);
	void addScaleX(float value);
	void addScaleY(float value);
	void addScaleZ(float value);

	//拡大値の取得
	float getScaleX(void);
	float getScaleY(void);
	float getScaleZ(void);


	//ワールド行列の取得
	XMMATRIX *getWorld();

};
