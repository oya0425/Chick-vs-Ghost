struct vertexIn						//ｃ言語から入ってくる頂点情報　の構造体
{
    float4 pos : POSITION0;			//ポジション
    float4 nor : NORMAL0;			//追加した法線の情報　ノーマルの０番上下のやつも同様
	float2 tx0 : TEXCOORD0;
};
 
struct vertexOut				//ライティングの結果を送る
{
    float4 pos : SV_POSITION;
	float2 tx0 : TEXCOORD0;
    float4 col : COLOR0;
};
 
cbuffer ConstantBuffer : register(b0)     //ｃ言語から持って来たい情報を書く　
{
    float4x4	WVP;					//World * View * Proj
	float4x4	World;					//ワールド行列　　　カメラのスクリーンを入れたらしい
    float4		LightDir;				//平行光源の（逆）方向　逆にすると計算が楽
    float4		LightCol;				//平行光源の色
    float4		LightAmb;				//環境光（アンビエント）の色
}
 
vertexOut main(vertexIn IN )//
{
	vertexOut OUT;

	//頂点座標をワールド行列で変換
    OUT.pos = mul(IN.pos, WVP); 
	
	//法線ベクトル
    float3 n = mul(float4(IN.nor.xyz, 0), World).xyz;
		
	//ライティング
    OUT.col.xyz = saturate(dot(normalize(n), LightDir.xyz)) * LightCol.xyz + LightAmb.xyz;
    OUT.col.w = 1;
	

	//テクスチャ座標
	OUT.tx0 = IN.tx0;

	return OUT;
}