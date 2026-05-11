//--------------------------------------------------------------//
//	"vn_billboard.h"											//
//		ビルボードポリゴン										//
//													2025/08/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

class vnBillboard : public vnObject
{
protected:
	//基本座標
	XMVECTOR BasePos[4];

	//頂点データ
	vnVertex3D	*vtx;

	//頂点バッファ
	ID3D12Resource* vertBuff;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

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

public:
	//コンストラクタ
	vnBillboard() {}
	vnBillboard(float width, float height, const WCHAR* texture_file, float left_u = 0.0f, float right_u = 1.0f, float top_v = 0.0f, float bottom_v = 1.0f);

	//デストラクタ
	virtual ~vnBillboard();

	//処理
	virtual void execute();

	//描画
	virtual void render();

	//拡散色の設定
	void setDiffuse(float r, float g, float b, float a);

	void setVertexPositionAsUI(float screenX, float screenY, float width, float height);;
};