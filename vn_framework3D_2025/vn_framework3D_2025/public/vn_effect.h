//--------------------------------------------------------------//
//	"vn_effect.h"												//
//		エフェクト(パーティクル)クラス							//
//													2025/11/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

//パーティクル
class vnParticle
{
public:
	float	 Life = 0.0f;	//寿命（フレーム）
	float	 StartLife;	//寿命
	XMVECTOR Pos = XMVectorZero();	//位置	
	XMVECTOR Vel = XMVectorZero();	//速度
	XMVECTOR Col;					//色
	float Size;	//サイズ

//public:
//	// --- セット ---
//	void SetLife(float life) { life = Life; }
//	void SetPos();


};

class vnEmitter : public vnObject
{
private:
	//放出するかのフラグ
	bool emit;

	float emitTimer = 0.0f; // 追加：放出残り時間

	//
	vnParticle* pParticle;

	//描画されるインデックス数
	int IndexNum;

	int vtxIndex;

	//頂点データ
	vnVertex3D* vtx;



	//インデックスデータ
	WORD* idx;

	//頂点バッファ
	ID3D12Resource* vertBuff;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	//インデックスバッファ
	ID3D12Resource* pIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	//テクスチャ
	ID3D12Resource* texbuff;
	//定数バッファ
	ID3D12Resource* constBuff;
	ID3D12DescriptorHeap* basicDescHeap;
	stConstantBuffer* pConstBuffer;


	//マテリアル関連
	XMVECTOR	Diffuse;	//拡散色
	XMVECTOR	Ambient;	//環境色
	XMVECTOR	Specular;	//鏡面反射色

	//頂点データへの各種情報の設定
	virtual void setVertexPosition();

	XMVECTOR m_emitColor;

public:
	//パーティクルを放出する際の設定
	struct stEmitterDesc {
		WCHAR Texture[64] = L"";

		float LifeMin = 30.0f;
		float LifeMax = 60.0f;
		XMVECTOR ColorMin = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR ColorMax = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		float SizeMin = 300.0f;
		float SizeMax = 600.0f;
		float SpeedMin = 0.1f;
		float SpeedMax = 0.2f;
		//stEmitterDesc() {
		//	//初期化　）例
		//	LifeMin = 30.0f;
		//}
	};
	static const XMVECTOR colors[22];
	static const int NumColors = sizeof(colors) / sizeof(colors[0]);

	void SetColor(XMVECTOR col);
	 
	void setEmit(bool flag, float duration = 0.0f); // 引数を追加

	stEmitterDesc Desc;

	vnEmitter(stEmitterDesc* desc);
	virtual ~vnEmitter();

	virtual void execute();

	virtual void render();


	void setEmit(bool flag);
	bool isEmit();
};

