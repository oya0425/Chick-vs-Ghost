//--------------------------------------------------------------//
//	"vn_effect.cpp"												//
//		エフェクト(パーティクル)クラス							//
//													2025/11/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "../framework/vn_environment.h"

//パーティクルの最大数
#define vnPARTICLE_MAX (1024)

// cppファイルの方で中身（実体）を書く
const XMVECTOR vnEmitter::colors[] = {
	V_GAME_COLOR_RED,        V_GAME_COLOR_ORANGE,     V_GAME_COLOR_PINK,
	V_GAME_COLOR_MAGENTA,    V_GAME_COLOR_GREEN,      V_GAME_COLOR_LIME,
	V_GAME_COLOR_TEAL,       V_GAME_COLOR_BLUE,       V_GAME_COLOR_LIGHT_BLUE,
	V_GAME_COLOR_DARK_BLUE,  V_GAME_COLOR_CYAN,       V_GAME_COLOR_BLUEBLACK,
	V_GAME_COLOR_WHITE,      V_GAME_COLOR_LIGHT_GRAY, V_GAME_COLOR_DARK_GRAY,
	V_GAME_COLOR_BLACK,      V_GAME_COLOR_SILVER,     V_GAME_COLOR_PURPLE,
	V_GAME_COLOR_BROWN,      V_GAME_COLOR_YELLOW,     V_GAME_COLOR_CURSOR,
	V_GAME_COLOR_GOLD
};

vnEmitter::vnEmitter(stEmitterDesc*desc)
{
	vnDirect3D::waitForGpu();
	//パーティクルの作成
	pParticle = new vnParticle[vnPARTICLE_MAX];
	for (int i = 0; i < vnPARTICLE_MAX; ++i) {
		pParticle[i].Life = 0.0f;
		pParticle[i].Pos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		pParticle[i].Vel = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		pParticle[i].Col = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		pParticle[i].Size = 0.5f;
		pParticle[i].StartLife = 0.0f;
	}

	// --- 追加変数初期化 ---
	IndexNum = 0;
	vtxIndex = 0;

	emit = true;

	// --- パーティクルを放出する際の設定
	Desc.LifeMin = desc->LifeMin;
	Desc.LifeMax = desc->LifeMax;
	Desc.ColorMin = desc->ColorMin;
	Desc.ColorMax = desc->ColorMax;
	Desc.SizeMin = desc->SizeMin;
	Desc.SizeMax = desc->SizeMax;
	Desc.SpeedMin = desc->SpeedMin;
	Desc.SpeedMax = desc->SpeedMax;


	m_emitColor = desc->ColorMax;

	// ---------------


	//描画情報の初期化
	HRESULT hr = S_OK;

	TexMetadata metadata = {};
	ScratchImage scratchImg = {};

	texbuff = NULL;
	basicDescHeap = NULL;

	//テクスチャの拡張子
	WCHAR texture_file[64] = L"";//data/image/enn.png";//L"data/image/particle/particle001.png";
	if (desc != nullptr) {
		swprintf_s(texture_file,L"%s", desc->Texture);
	}
	const WCHAR* ext = wcsrchr(texture_file, L'.');

	//テクスチャの読み込み
	if (ext != NULL)
	{
		if (wcscmp(ext, L".tga") == 0 || wcscmp(ext, L".TGA") == 0)
		{
			hr = LoadFromTGAFile(texture_file, TGA_FLAGS_NONE, &metadata, scratchImg);
		}
		else
		{
			hr = LoadFromWICFile(texture_file, WIC_FLAGS_NONE, &metadata, scratchImg);
		}
		if (hr == S_OK)
		{
			const Image* img = scratchImg.GetImage(0, 0, 0);

			D3D12_HEAP_PROPERTIES texHeapProp = {};
			texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
			texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
			texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
			texHeapProp.CreationNodeMask = 0;
			texHeapProp.VisibleNodeMask = 0;

			D3D12_RESOURCE_DESC resDesc = {};
			resDesc.Format = metadata.format;
			resDesc.Width = metadata.width;
			resDesc.Height = (UINT)metadata.height;
			resDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
			resDesc.SampleDesc.Count = 1;
			resDesc.SampleDesc.Quality = 0;
			resDesc.MipLevels = (UINT16)metadata.mipLevels;
			resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
			resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			hr = vnDirect3D::getDevice()->CreateCommittedResource(
				&texHeapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				nullptr,
				IID_PPV_ARGS(&texbuff)
			);
			texbuff->SetName(L"vnEffect::texbuff");

			hr = texbuff->WriteToSubresource(0,
				NULL,
				img->pixels,
				(UINT)img->rowPitch,
				(UINT)img->slicePitch
			);
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descHeapDesc.NodeMask = 0;
		descHeapDesc.NumDescriptors = 2;
		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		hr = vnDirect3D::getDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));

		//定数バッファ
		D3D12_HEAP_PROPERTIES constHeapProp = {};
		constHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		constHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		constHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		constHeapProp.CreationNodeMask = 1;
		constHeapProp.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC constDesc = {};
		constDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		constDesc.Width = ((sizeof(stConstantBuffer) + 0xff) & ~0xff);
		constDesc.Height = 1;
		constDesc.DepthOrArraySize = 1;
		constDesc.MipLevels = 1;
		constDesc.Format = DXGI_FORMAT_UNKNOWN;
		constDesc.SampleDesc.Count = 1;
		constDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		constDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		hr = vnDirect3D::getDevice()->CreateCommittedResource(
			&constHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&constDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuff)
		);
		constBuff->SetName(L"vnEffect::constBuff");

		D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();
		//シェーダリソースビューの作成
		if (texbuff != NULL)
		{
			//テクスチャビュー作成
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = metadata.format;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;

			vnDirect3D::getDevice()->CreateShaderResourceView(texbuff,
				&srvDesc,
				basicHeapHandle
			);
		}
		else
		{
			vnDirect3D::getDevice()->CreateShaderResourceView(vnDirect3D::getWhiteTexture(),
				vnDirect3D::getWhiteTextueViewDesc(),
				basicHeapHandle
			);
		}
		basicHeapHandle.ptr += vnDirect3D::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;
		//定数バッファビューの作成
		vnDirect3D::getDevice()->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

		hr = constBuff->Map(0, NULL, (void**)&pConstBuffer);
	}

	{//頂点バッファ
		const int vnum = vnPARTICLE_MAX * 4;

		const UINT vertexBufferSize = sizeof(vnVertex3D) * vnum;

		D3D12_HEAP_PROPERTIES heapprop = {};
		heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC resdesc = {};
		resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resdesc.Width = vertexBufferSize;
		resdesc.Height = 1;
		resdesc.DepthOrArraySize = 1;
		resdesc.MipLevels = 1;
		resdesc.Format = DXGI_FORMAT_UNKNOWN;
		resdesc.SampleDesc.Count = 1;
		resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		hr = vnDirect3D::getDevice()->CreateCommittedResource(
			&heapprop,
			D3D12_HEAP_FLAG_NONE,
			&resdesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			NULL,
			IID_PPV_ARGS(&vertBuff)
		);
		vertBuff->SetName(L"vnEffect::vertBuff");

		hr = vertBuff->Map(0, NULL, reinterpret_cast<void**>(&vtx));

		vertexBufferView.BufferLocation = vertBuff->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(vnVertex3D);
		vertexBufferView.SizeInBytes = vertexBufferSize;
	}

	{//インデックスバッファ
		const int inum = vnPARTICLE_MAX* 6;
		const UINT indexBufferSize = sizeof(WORD) * inum;

		D3D12_HEAP_PROPERTIES heapprop = {};
		heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC resdesc = {};
		resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resdesc.Width = indexBufferSize;
		resdesc.Height = 1;
		resdesc.DepthOrArraySize = 1;
		resdesc.MipLevels = 1;
		resdesc.Format = DXGI_FORMAT_UNKNOWN;
		resdesc.SampleDesc.Count = 1;
		resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		hr = vnDirect3D::getDevice()->CreateCommittedResource(
			&heapprop,
			D3D12_HEAP_FLAG_NONE,
			&resdesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&pIndexBuffer)
		);
		pIndexBuffer->SetName(L"vnEffect::pIndexBuffer");

		//インデックスデータのコピー
		pIndexBuffer->Map(0, nullptr, (void**)&idx);

		//インデックスバッファビューを作成
		indexBufferView.BufferLocation = pIndexBuffer->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R16_UINT;
		indexBufferView.SizeInBytes = indexBufferSize;

		//インデックスデータの初期化
		memset(idx, 0, sizeof(WORD) * inum);
	}

	//頂点配列の初期化
	for (int i = 0; i < vnPARTICLE_MAX; ++i){
		for (int j = 0; j < 4; ++j) {
			vtx[i * 4 + j].x = 0.0f;
			vtx[i * 4 + j].y = 0.0f;
			vtx[i * 4 + j].z = 0.0f;
			vtx[i * 4 + j].nx = 0.0f;
			vtx[i * 4 + j].ny = 0.0f;
			vtx[i * 4 + j].nz = 0.0f;
			vtx[i * 4 + j].r = 1.0f;
			vtx[i * 4 + j].g = 1.0f;
			vtx[i * 4 + j].b = 1.0f;
			vtx[i * 4 + j].a = 1.0f;

		}
		vtx[i * 4 + 0].u = 0.0f;		vtx[i * 4 + 0].v = 0.0f;
		vtx[i * 4 + 1].u = 1.0f;		vtx[i * 4 + 1].v = 0.0f;
		vtx[i * 4 + 2].u = 0.0f;		vtx[i * 4 + 2].v = 1.0f;
		vtx[i * 4 + 3].u = 1.0f;		vtx[i * 4 + 3].v = 1.0f;
	}
	//インデックスデータの初期化
	memset(idx, 0, sizeof(WORD)* vnPARTICLE_MAX * 6);

	//マテリアル関連
	Diffuse = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	Ambient = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	Specular = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	setTransparent(true);

	setLighting(false);
}

vnEmitter::~vnEmitter()
{
	// ★重要：GPUがこのエフェクトの描画（basicDescHeapの参照）を
		// 完全に終えるまで、CPUをここで待機させる
	vnDirect3D::waitForGpu();

	delete[] pParticle;
	pParticle = NULL;
	SAFE_RELEASE(basicDescHeap);
	SAFE_RELEASE(texbuff);
	SAFE_RELEASE(constBuff);
	SAFE_RELEASE(vertBuff);
	SAFE_RELEASE(pIndexBuffer);
}

void vnEmitter::setEmit(bool flag, float duration) {
	emit = flag;
	emitTimer = duration; // 秒数をセット（例: 1.0f）
}


void vnEmitter::execute()
{
	// --- 追加：タイマー処理 ---
	if (emitTimer > 0.0f) {
		emitTimer -= 1.0f / 60.0f; // 1フレーム(1/60秒)ずつ減らす
		if (emitTimer <= 0.0f) {
			emit = false; // 0以下になったら停止
		}
	}

	XMVECTOR world;
	getWorldPosition(&world);

	//パーティクルの放出
	for (int i = 0; i < vnPARTICLE_MAX&&emit==true; ++i) {
		
		if (pParticle[i].Life > 0.0f)continue;
		//放出可能なパーティクルに初期設定
		pParticle[i].Life = 60.0f;
		pParticle[i].Life = (float)(rand() % 30) + 30.0f;	//30~60
		pParticle[i].StartLife = pParticle[i].Life;
		pParticle[i].Pos = world;
		pParticle[i].Vel = XMVectorSet(
			(float)(rand() % 2000) / 1000.0f-1.0f,	//-1~+1		
			(float)(rand() % 1000) / 1000.0f/*-0.0f*/,	//0~+1
			(float)(rand() % 2000) / 1000.0f-1.0f,	//-1~+1
			0.0f
			);
		//pParticle[i].Vel *= 0.1f;
		float speedDiff = Desc.SpeedMax - Desc.SpeedMin;
		float speed = Desc.SpeedMin + (float)(rand() % 1001) / 1000.0f * speedDiff;

		// 3. 速度ベクトルを確定
		pParticle[i].Vel *= speed*0.8f;
		//pParticle[i].Col = XMVectorSet(
		//	(float)(rand() % 1000) / 1000.0f,	//-1~+1		
		//	(float)(rand() % 1000) / 1000.0f,	//0~+1
		//	(float)(rand() % 1000) / 1000.0f,	//-1~+1
		//	1.0f
		//);

		//XMVECTOR base = colors[rand() % 5];

		//float r = XMVectorGetX(base) + ((rand() % 200 - 100) / 1000.0f);
		//float g = XMVectorGetY(base) + ((rand() % 200 - 100) / 1000.0f);
		//float b = XMVectorGetZ(base) + ((rand() % 200 - 100) / 1000.0f);

		//pParticle[i].Col = XMVectorSet(r, g, b, 1);
		//pParticle[i].Col = Desc.ColorMax;
		pParticle[i].Col = m_emitColor;
		//pParticle[i].Size = rand()%(Desc.SizeMax-Desc.SizeMin);
		float sizeRange = Desc.SizeMax - Desc.SizeMin;
		if (sizeRange <= 0.0f) {
			pParticle[i].Size = Desc.SizeMax;
		}
		else {
			pParticle[i].Size = Desc.SizeMin + ((float)(rand() % 1000) / 1000.0f) * sizeRange;
		}

		break;
	}

	//パーティクルの動作
	IndexNum = 0;
	vtxIndex = 0;
	for (int i = 0; i < vnPARTICLE_MAX/*/1000*/; ++i) {
		if (pParticle[i].Life <= 0.0f)continue;

		//Lifeの減少に応じた割合
		float overLifeTime = pParticle[i].Life / pParticle[i].StartLife;

		pParticle[i].Life -= 1.0f;
		pParticle[i].Pos += pParticle[i].Vel;

		//vnDebugDraw::Line(&pParticle[i].Pos, &Position, GAME_COLOR_RED);

		//カメラのビューマトリクスを取得
		XMMATRIX mBillboard = *vnCamera::getView();

		//移動成分をなくす
		mBillboard.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		//逆行列を計算（※アフィン変換のみの為、転置行列と同じ）
		mBillboard = XMMatrixTranspose(mBillboard);

		XMVECTOR v[4];
		float w = pParticle[i].Size* overLifeTime;
		float h = pParticle[i].Size* overLifeTime;
		//表示する座標　左上[0] 右上[1] 左下[2] 右下[3]
		v[0] = XMVectorSet(-w, h, 0.0f, 0.0f);
		v[1] = XMVectorSet(w, h, 0.0f, 0.0f);
		v[2] = XMVectorSet(-w, -h, 0.0f, 0.0f);
		v[3] = XMVectorSet(w, -h, 0.0f, 0.0f);
		for (int j = 0; j < 4; ++j) {
			v[j] = XMVector3TransformNormal(v[j], mBillboard);
			v[j] += pParticle[i].Pos;
		}


		//vnDebugDraw::Line(&v[0], &v[1]);
		//vnDebugDraw::Line(&v[1], &v[3]);
		//vnDebugDraw::Line(&v[3], &v[2]);
		//vnDebugDraw::Line(&v[2], &v[0]);


			

		//描画用の頂点データに設定
		for (int j = 0; j < 4; j++) {
			vtx[vtxIndex * 4 + j].x = XMVectorGetX(v[j]);
			vtx[vtxIndex * 4 + j].y = XMVectorGetY(v[j]);
			vtx[vtxIndex * 4 + j].z = XMVectorGetZ(v[j]);
			vtx[vtxIndex * 4 + j].r = XMVectorGetX(pParticle[i].Col);
			vtx[vtxIndex * 4 + j].g = XMVectorGetY(pParticle[i].Col);
			vtx[vtxIndex * 4 + j].b = XMVectorGetZ(pParticle[i].Col);
			vtx[vtxIndex * 4 + j].a = XMVectorGetW(pParticle[i].Col)*overLifeTime;

		}

		//vtx[vtxIndex*4+0].x = XMVectorGetX(v[0]);
		//vtx[vtxIndex*4+0].y = XMVectorGetY(v[0]);
		//vtx[vtxIndex*4+0].z = XMVectorGetZ(v[0]);
		//vtx[vtxIndex*4+1].x = XMVectorGetX(v[1]);
		//vtx[vtxIndex*4+1].y = XMVectorGetY(v[1]);
		//vtx[vtxIndex*4+1].z = XMVectorGetZ(v[1]);
		//vtx[vtxIndex*4+2].x = XMVectorGetX(v[2]);
		//vtx[vtxIndex*4+2].y = XMVectorGetY(v[2]);
		//vtx[vtxIndex*4+2].z = XMVectorGetZ(v[2]);
		//vtx[vtxIndex*4+3].x = XMVectorGetX(v[3]);
		//vtx[vtxIndex*4+3].y = XMVectorGetY(v[3]);
		//vtx[vtxIndex*4+3].z = XMVectorGetZ(v[3]);

		//インデックスデータの設定
		//idx[0] = 0;	idx[1] = 1;	idx[2] = 2;
		//idx[3] = 1;	idx[4] = 3;	idx[5] = 2;

		idx[IndexNum++] = vtxIndex * 4 + 0;
		idx[IndexNum++] = vtxIndex * 4 + 1;
		idx[IndexNum++] = vtxIndex * 4 + 2;

		idx[IndexNum++] = vtxIndex * 4 + 1;
		idx[IndexNum++] = vtxIndex * 4 + 3;
		idx[IndexNum++] = vtxIndex * 4 + 2;

		vtxIndex++;


	}


}
void vnEmitter::setVertexPosition()
{
}

void vnEmitter::render()
{
//#if 0
	if (IndexNum == 0) return;

	// ★ここでもう一度、ヒープが生きているか、GPUが前回の描画を終えているか確認
	if (!basicDescHeap) return;

	if (IndexNum == 0) return;
	if (!basicDescHeap || !constBuff || !pParticle) return;
	calculateLocalMatrix();
	calculateWorldMatrix();

	//ビューポート変換マトリクスの計算
	XMMATRIX WVP = XMMatrixMultiply(XMMatrixIdentity(), *vnCamera::getScreen());

	//頂点座標の設定
	setVertexPosition();

	//コンスタントバッファに情報を設定
	XMStoreFloat4x4(&pConstBuffer->WVP, XMMatrixTranspose(WVP));
	XMStoreFloat4x4(&pConstBuffer->World, XMMatrixTranspose(World));
	XMStoreFloat4(&pConstBuffer->LightDir, *vnLight::getILightDir());
	XMStoreFloat4(&pConstBuffer->LightCol, *vnLight::getLightColor());
	XMStoreFloat4(&pConstBuffer->LightAmb, *vnLight::getAmbient());
	XMStoreFloat4(&pConstBuffer->CameraPos, *vnCamera::getPosition());
	XMStoreFloat4(&pConstBuffer->Diffuse, Diffuse);
	XMStoreFloat4(&pConstBuffer->Ambient, Ambient);
	XMStoreFloat4(&pConstBuffer->Specular, Specular);

	vnDirect3D::getCommandList()->SetPipelineState(pPipelineState[renderFlag]);

	vnDirect3D::getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	vnDirect3D::getCommandList()->SetDescriptorHeaps(1, &basicDescHeap);
	vnDirect3D::getCommandList()->SetGraphicsRootDescriptorTable(0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());
	vnDirect3D::getCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	vnDirect3D::getCommandList()->IASetIndexBuffer(&indexBufferView);

	vnDirect3D::getCommandList()->DrawIndexedInstanced(IndexNum, 1, 0, 0, 0);	//セットしてきたやつを描画する(引数(IndexNum:四角形を書きたい,))


	//#endif
}

void vnEmitter::setEmit(bool flag) {
	emit = flag;
}
bool vnEmitter::isEmit() {
	return emit;
}

void vnEmitter::SetColor(XMVECTOR col)
{
	m_emitColor = col;
}