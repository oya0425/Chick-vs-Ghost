//--------------------------------------------------------------//
//	"vn_billboard.cpp"											//
//		ビルボードポリゴン										//
//													2025/08/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "../framework/vn_environment.h"

vnBillboard::vnBillboard(float width, float height, const WCHAR* texture_file, float left_u, float right_u, float top_v, float bottom_v) : vnObject()
{
	HRESULT hr = S_OK;

	float w = width * 0.5f;
	float h = height * 0.5f;
	//表示する座標　左上[0] 右上[1] 左下[2] 右下[3]
	BasePos[0] = XMVectorSet(-w, h, 0.0f, 0.0f);
	BasePos[1] = XMVectorSet(w, h, 0.0f, 0.0f);
	BasePos[2] = XMVectorSet(-w, -h, 0.0f, 0.0f);
	BasePos[3] = XMVectorSet(w, -h, 0.0f, 0.0f);

	TexMetadata metadata = {};
	ScratchImage scratchImg = {};
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

	texbuff = NULL; 
	basicDescHeap = NULL;

	//テクスチャの拡張子
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
			texbuff->SetName(L"vnBillboard::texbuff");

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
		if (hr != S_OK)
		{
			assert(hr == S_OK);
		}
		constBuff->SetName(L"vnBillboard::constBuff");

		D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();
		//シェーダリソースビューの作成
		if (texbuff != NULL)
		{
			//テクスチャビュー作成
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = format;// metadata.format;
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

	//頂点バッファ
	const int vnum = 4;

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
	if (hr != S_OK)
	{
		assert(hr == S_OK);
	}
	vertBuff->SetName(L"vnBillboard::vertBuff");

	hr = vertBuff->Map(0, NULL, reinterpret_cast<void**>(&vtx));

	vertexBufferView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(vnVertex3D);
	vertexBufferView.SizeInBytes = vertexBufferSize;

	//頂点メモリの初期化
	memset(vtx, 0, sizeof(vnVertex3D) * vnum);

	//頂点座標を設定
	setVertexPosition();

	//頂点カラーを設定	//色の変更　今は白
	vtx[0].r = 1.0f;	vtx[0].g = 1.0f;	vtx[0].b = 1.0f;	vtx[0].a = 1.0f;
	vtx[1].r = 1.0f;	vtx[1].g = 1.0f;	vtx[1].b = 1.0f;	vtx[1].a = 1.0f;
	vtx[2].r = 1.0f;	vtx[2].g = 1.0f;	vtx[2].b = 1.0f;	vtx[2].a = 1.0f;
	vtx[3].r = 1.0f;	vtx[3].g = 1.0f;	vtx[3].b = 1.0f;	vtx[3].a = 1.0f;

	//uvを設定
	vtx[0].u = left_u;	vtx[0].v = top_v;
	vtx[1].u = right_u;	vtx[1].v = top_v;
	vtx[2].u = left_u;	vtx[2].v = bottom_v;
	vtx[3].u = right_u;	vtx[3].v = bottom_v;

	//マテリアル関連
	Diffuse = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	Ambient = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	Specular = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	setTransparent(true);

	setLighting(false);
	setZWrite(false);

}

vnBillboard::~vnBillboard()
{
	SAFE_RELEASE(basicDescHeap);
	SAFE_RELEASE(texbuff);
	SAFE_RELEASE(constBuff);
	SAFE_RELEASE(vertBuff);
}

void vnBillboard::execute()
{
}

void vnBillboard::render()
{
	calculateLocalMatrix();
	calculateWorldMatrix();

	XMMATRIX W = XMMatrixIdentity();
	XMMATRIX WVP = *vnCamera::getScreen();

	//頂点座標の設定
	setVertexPosition();

#if 1
	//コンスタントバッファに情報を設定
	XMStoreFloat4x4(&pConstBuffer->WVP, XMMatrixTranspose(WVP));
	XMStoreFloat4x4(&pConstBuffer->World, XMMatrixTranspose(W));
	XMStoreFloat4(&pConstBuffer->LightDir, *vnLight::getILightDir());
	XMStoreFloat4(&pConstBuffer->LightCol, *vnLight::getLightColor());
	XMStoreFloat4(&pConstBuffer->LightAmb, *vnLight::getAmbient());
	XMStoreFloat4(&pConstBuffer->CameraPos, *vnCamera::getPosition());
	XMStoreFloat4(&pConstBuffer->Diffuse, Diffuse);
	XMStoreFloat4(&pConstBuffer->Ambient, Ambient);
	XMStoreFloat4(&pConstBuffer->Specular, Specular);
	
	vnDirect3D::getCommandList()->SetPipelineState(pPipelineState[renderFlag]);
	vnDirect3D::getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	vnDirect3D::getCommandList()->SetDescriptorHeaps(1, &basicDescHeap);
	vnDirect3D::getCommandList()->SetGraphicsRootDescriptorTable(0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());
	vnDirect3D::getCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	vnDirect3D::getCommandList()->DrawInstanced(4, 1, 0, 0);
#endif
}

//頂点座標の設定
void vnBillboard::setVertexPosition()
{
	//カメラのビューマトリクスを取得
	XMMATRIX mBillboard = *vnCamera::getView();

	//移動成分をなくす
	mBillboard.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	//逆行列を計算（※アフィン変換のみの為、転置行列と同じ）
	mBillboard = XMMatrixTranspose(mBillboard);

	//ワールド座標
	XMVECTOR w;
	getWorldPosition(&w);
	//w = World.r[3]

	XMVECTOR v[4];
	for (int i = 0; i < 4; i++) {
		v[i] = XMVector3TransformNormal(BasePos[i], mBillboard) + w;	//
		vtx[i].x = XMVectorGetX(v[i]);
		vtx[i].y = XMVectorGetY(v[i]);
		vtx[i].z = XMVectorGetZ(v[i]);
		
	}

	vnDebugDraw::Line(&v[0], &v[1], 0xff0000ff);	//AARRBBGG
	vnDebugDraw::Line(&v[1], &v[3], 0xff0000ff);
	vnDebugDraw::Line(&v[3], &v[2], 0xff0000ff);
	vnDebugDraw::Line(&v[2], &v[0], 0xff0000ff);

}

void vnBillboard::setDiffuse(float r, float g, float b, float a)
{
	Diffuse = XMVectorSet(r, g, b, a);
}
