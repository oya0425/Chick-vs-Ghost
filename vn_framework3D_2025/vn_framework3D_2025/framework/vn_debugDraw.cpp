//--------------------------------------------------------------//
//	"vn_debugDraw.cpp"											//
//		デバッグ描画											//
//													2025/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "../framework/vn_environment.h"

//頂点要素
D3D12_INPUT_ELEMENT_DESC vnDebugDraw::inputElementDescs[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, D3D11_APPEND_ALIGNED_ELEMENT , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

int						vnDebugDraw::VertexNum = 0;
vnVertex3D_Line			*vnDebugDraw::vtx = NULL;
XMFLOAT4X4				*vnDebugDraw::VP = NULL;

ID3D12Resource* vnDebugDraw::vertBuff = NULL;
D3D12_VERTEX_BUFFER_VIEW vnDebugDraw::vertexBufferView;

ID3D12Resource* vnDebugDraw::constBuff = NULL;
ID3D12DescriptorHeap* vnDebugDraw::basicDescHeap = NULL;

ID3D12PipelineState* vnDebugDraw::pPipelineState;


bool vnDebugDraw::initialize()
{
	VertexNum = 0;

	HRESULT hr = S_OK;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = vnDirect3D::getRootSignature();

	psoDesc.VS.pShaderBytecode = vnShader::getVShader(vnShader::eVertexShader::VS_3D_Line)->getCode();
	psoDesc.VS.BytecodeLength = vnShader::getVShader(vnShader::eVertexShader::VS_3D_Line)->getLength();
	psoDesc.PS.pShaderBytecode = vnShader::getPShader(vnShader::ePixelShader::PS_3D_Line)->getCode();
	psoDesc.PS.BytecodeLength = vnShader::getPShader(vnShader::ePixelShader::PS_3D_Line)->getLength();

	psoDesc.RasterizerState.MultisampleEnable = false;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.DepthClipEnable = true;
	psoDesc.RasterizerState.FrontCounterClockwise = false;
	psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	psoDesc.RasterizerState.AntialiasedLineEnable = false;
	psoDesc.RasterizerState.ForcedSampleCount = 0;
	psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	psoDesc.BlendState.AlphaToCoverageEnable = false;
	psoDesc.BlendState.IndependentBlendEnable = false;
	psoDesc.BlendState.RenderTarget[0].BlendEnable = true;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_DEST_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
	psoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	psoDesc.DepthStencilState.DepthEnable = true;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	psoDesc.DepthStencilState.StencilEnable = false;
	psoDesc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	psoDesc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	psoDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	psoDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	hr = vnDirect3D::getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineState));
	pPipelineState->SetName(L"vnDebugDraw::pPipelineState");

	//頂点バッファ
	const UINT vertexBufferSize = sizeof(vnVertex3D_Line) * vnDebugDraw_VertexMax;

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
	vertBuff->SetName(L"vnDebugDraw::vertBuff");

	//頂点バッファのマップ
	hr = vertBuff->Map(0, NULL, reinterpret_cast<void**>(&vtx));

	vertexBufferView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(vnVertex3D_Line);
	vertexBufferView.SizeInBytes = vertexBufferSize;

	//定数バッファ
	D3D12_HEAP_PROPERTIES constHeapProp = {};
	constHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	constHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	constHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	constHeapProp.CreationNodeMask = 1;
	constHeapProp.VisibleNodeMask = 1;
	D3D12_RESOURCE_DESC constDesc = {};
	constDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	constDesc.Width = ((sizeof(XMFLOAT4X4) + 0xff) & ~0xff);
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
	constBuff->SetName(L"vnDebugDraw::constBuff");

	//定数バッファのマップ
	hr = constBuff->Map(0, NULL, (void**)&VP);


	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NodeMask = 0;
	descHeapDesc.NumDescriptors = 2;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	hr = vnDirect3D::getDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));
	basicDescHeap->SetName(L"vnDebugDraw::basicDescHeap");

	D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();
	//シェーダリソースビューの作成
	vnDirect3D::getDevice()->CreateShaderResourceView(vnDirect3D::getWhiteTexture(),
		vnDirect3D::getWhiteTextueViewDesc(),
		basicHeapHandle
	);
	basicHeapHandle.ptr += vnDirect3D::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;
	//定数バッファビューの作成
	vnDirect3D::getDevice()->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	return true;
}

void vnDebugDraw::terminate()
{
	SAFE_RELEASE(basicDescHeap);
	SAFE_RELEASE(constBuff);
	SAFE_RELEASE(vertBuff);
	SAFE_RELEASE(pPipelineState);
}

void vnDebugDraw::render()
{
	if(VertexNum==0)return;
	
	//コンスタントバッファ
	XMStoreFloat4x4(VP, XMMatrixTranspose(*vnCamera::getScreen()));

	vnDirect3D::getCommandList()->SetPipelineState(pPipelineState);

	vnDirect3D::getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	vnDirect3D::getCommandList()->SetDescriptorHeaps(1, &basicDescHeap);
	vnDirect3D::getCommandList()->SetGraphicsRootDescriptorTable(0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());
	vnDirect3D::getCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	vnDirect3D::getCommandList()->DrawInstanced(VertexNum, 1, 0, 0);

	VertexNum = 0;
}

void vnDebugDraw::Line(const XMVECTOR *start, const XMVECTOR *end, DWORD color)
{
	if(VertexNum>=vnDebugDraw_VertexMax-2)return;
	
	vtx[VertexNum].x = XMVectorGetX(*start);	vtx[VertexNum].y = XMVectorGetY(*start);	vtx[VertexNum].z = XMVectorGetZ(*start);	vtx[VertexNum].color = color;	VertexNum++;
	vtx[VertexNum].x = XMVectorGetX(*end);		vtx[VertexNum].y = XMVectorGetY(*end);		vtx[VertexNum].z = XMVectorGetZ(*end);		vtx[VertexNum].color = color;	VertexNum++;
}

void vnDebugDraw::Line(float start_x, float start_y, float start_z, float end_x, float end_y, float end_z, DWORD color)
{
	if (VertexNum >= vnDebugDraw_VertexMax - 2)return;

	vtx[VertexNum].x = start_x;		vtx[VertexNum].y = start_y;		vtx[VertexNum].z = start_z;		vtx[VertexNum].color = color;	VertexNum++;
	vtx[VertexNum].x = end_x;		vtx[VertexNum].y = end_y;		vtx[VertexNum].z = end_z;		vtx[VertexNum].color = color;	VertexNum++;
}

void vnDebugDraw::Grid(int num, float interval, DWORD color)
{
	//上の段が原点　下の段がどこまで線を引くかの値
    //X軸
	for (int i = -num/2; i <= num/2; i++) {

		vtx[VertexNum].x = -num / 2 * interval;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = i*interval;	vtx[VertexNum].color = 0xffffffff;	VertexNum++;
		vtx[VertexNum].x = num / 2 * interval;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = i * interval;	vtx[VertexNum].color = 0xffffffff;	VertexNum++;

		////Y軸
		//vtx[VertexNum].x = 0.0f;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = 0.0f;	vtx[VertexNum].color = 0xffffffff;	VertexNum++;
		//vtx[VertexNum].x = 0.0f;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = 0.0f;	vtx[VertexNum].color = 0xffffffff;	VertexNum++;
		//Z軸
		vtx[VertexNum].x = i * interval;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = -num/2*interval;	vtx[VertexNum].color = 0xffffffff;	VertexNum++;
		vtx[VertexNum].x = i * interval;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = num/2*interval;	vtx[VertexNum].color = 0xffffffff;	VertexNum++;

	}
	//X軸
	//for (int i = -num; i <= num; i++) {

	//	vtx[VertexNum].x = -num* interval;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = i * interval;	vtx[VertexNum].color = 0xffffffff;	VertexNum++;
	//	vtx[VertexNum].x = num* interval;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = i * interval;	vtx[VertexNum].color = 0xffffffff;	VertexNum++;

	//	////Y軸
	//	//vtx[VertexNum].x = 0.0f;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = 0.0f;	vtx[VertexNum].color = 0xffffffff;	VertexNum++;
	//	//vtx[VertexNum].x = 0.0f;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = 0.0f;	vtx[VertexNum].color = 0xffffffff;	VertexNum++;
	//	//Z軸
	//	vtx[VertexNum].x = i * interval;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = -num * interval;	vtx[VertexNum].color = 0xffffffff;	VertexNum++;
	//	vtx[VertexNum].x = i * interval;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = num * interval;	vtx[VertexNum].color = 0xffffffff;	VertexNum++;

	//}

}

void vnDebugDraw::Axis(float length)
{
	//上の段が原点　下の段がどこまで線を引くかの値
	//X軸
	vtx[VertexNum].x = 0.0f;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = 0.0f;	vtx[VertexNum].color = 0xff0000ff;	VertexNum++;
	vtx[VertexNum].x = length;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = 0.0f;	vtx[VertexNum].color = 0xff0000ff;	VertexNum++;
	//Y軸
	vtx[VertexNum].x = 0.0f;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = 0.0f;	vtx[VertexNum].color = 0xff00ff00;	VertexNum++;
	vtx[VertexNum].x = 0.0f;	vtx[VertexNum].y = length;	vtx[VertexNum].z = 0.0f;	vtx[VertexNum].color = 0xff00ff00;	VertexNum++;
	//Z軸
	vtx[VertexNum].x = 0.0f;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = 0.0f;	vtx[VertexNum].color = 0xffff0000;	VertexNum++;
	vtx[VertexNum].x = 0.0f;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = length;	vtx[VertexNum].color = 0xffff0000;	VertexNum++;
}

void vnDebugDraw::Box(const XMVECTOR& center,const XMVECTOR& size, DWORD color)
{
	XMVECTOR half = XMVectorScale(size, 0.5f);

	float cx = XMVectorGetX(center);
	float cy = XMVectorGetY(center);
	float cz = XMVectorGetZ(center);

	float hx = XMVectorGetX(half);
	float hy = XMVectorGetY(half);
	float hz = XMVectorGetZ(half);

	// 8頂点
	XMVECTOR v[8] = {
		XMVectorSet(cx-hx, cy-hy, cz-hz, 0), // 0: 左下奥
		XMVectorSet(cx+hx, cy-hy, cz-hz, 0), // 1: 右下奥
		XMVectorSet(cx-hx, cy+hy, cz-hz, 0), // 2: 左上奥
		XMVectorSet(cx+hx, cy+hy, cz-hz, 0), // 3: 右上奥
		XMVectorSet(cx-hx, cy-hy, cz+hz, 0), // 4: 左下手前
		XMVectorSet(cx+hx, cy-hy, cz+hz, 0), // 5: 右下手前
		XMVectorSet(cx-hx, cy+hy, cz+hz, 0), // 6: 左上手前
		XMVectorSet(cx+hx, cy+hy, cz+hz, 0)  // 7: 右上手前
	};

	// 奥面
	Line(&v[0], &v[1], color);
	Line(&v[1], &v[3], color);
	Line(&v[3], &v[2], color);
	Line(&v[2], &v[0], color);

	// 手前面
	Line(&v[4], &v[5], color);
	Line(&v[5], &v[7], color);
	Line(&v[7], &v[6], color);
	Line(&v[6], &v[4], color);

	// 縦の辺
	Line(&v[0], &v[4], color);
	Line(&v[1], &v[5], color);
	Line(&v[2], &v[6], color);
	Line(&v[3], &v[7], color);
}


void vnDebugDraw::Sphere(const XMVECTOR& center, float radius, DWORD color)
{
	const int segments = 16; // 円を何分割するか（多いほど滑らか）

	//１つの円をどう書くか（これは180分表示（上半分だけ表示））
	const float angleStep = XM_2PI / segments;

	float cx = XMVectorGetX(center);
	float cy = XMVectorGetY(center);
	float cz = XMVectorGetZ(center);

	//for (int i = 0; i < segments; i++)
	//{
	//	//隣同士点をつないで線を作る
	//	float a1 = i * angleStep;
	//	float a2 = (i + 1) * angleStep;

	//	float s1 = sinf(a1) * radius;
	//	float c1 = cosf(a1) * radius;
	//	float s2 = sinf(a2) * radius;
	//	float c2 = cosf(a2) * radius;

	//	// --- XY平面の円 (正面から見た輪郭) ---
	//	XMVECTOR vXY1 = XMVectorSet(cx + c1, cy + s1, cz, 0);
	//	XMVECTOR vXY2 = XMVectorSet(cx + c2, cy + s2, cz, 0);
	//	Line(&vXY1, &vXY2, color);

	//	// --- XZ平面の円 (真上から見た輪郭) ---
	//	XMVECTOR vXZ1 = XMVectorSet(cx + c1, cy, cz + s1, 0);
	//	XMVECTOR vXZ2 = XMVectorSet(cx + c2, cy, cz + s2, 0);
	//	Line(&vXZ1, &vXZ2, color);

	//	// --- YZ平面の円 (横から見た輪郭) ---
	//	XMVECTOR vYZ1 = XMVectorSet(cx, cy + s1, cz + c1, 0);
	//	XMVECTOR vYZ2 = XMVectorSet(cx, cy + s2, cz + c2, 0);
	//	Line(&vYZ1, &vYZ2, color);

	const int numCircles = 150; // 【ここが重要！】半球を何本（何枚）の円で敷き詰めるか。増やすほど密度が上がって完全な半球になる
	const float circleStep = XM_PI / numCircles; // 180度（半周）を本数で割って、1本あたりの回転角度を決める

	//円を描くように点（segments）を配置してそれをつないで線にする
	//まず何本の線を用意する→
	// 線を全て置けるように１８０度を分割する、１８０度なのは半径としての線ではなく直径としての線だから
	//  →点の位置を作っていく

	for (int j = 0; j < numCircles; j++)
	{
		float rotAngle = j * circleStep; // 円の、上から見た回転角度
		float rotCos = cosf(rotAngle);   // X用の配分
		float rotSin = sinf(rotAngle);   // Z用の配分

		// 1本の円を細かく線で繋いで描く
		for (int i = 0; i < segments; i++)
		{
			float a1 = i * angleStep;
			float a2 = (i + 1) * angleStep;

			float s1 = sinf(a1) * radius;
			float c1 = cosf(a1) * radius;
			float s2 = sinf(a2) * radius;
			float c2 = cosf(a2) * radius;

			// 高さが地面より上（cy以上）のときだけ線を描くことで「半球」にする
			//if (cy + s1 >= cy && cy + s2 >= cy)
			//{
			//	// XとZに、その角度ごとの配分（rotCos, rotSin）を掛け算する！
			//	XMVECTOR v1 = XMVectorSet(cx + c1 * rotCos, cy + s1, cz + c1 * rotSin, 0);
			//	XMVECTOR v2 = XMVectorSet(cx + c2 * rotCos, cy + s2, cz + c2 * rotSin, 0);
			//	Line(&v1, &v2, color);
			//}
			// XとZに、その角度ごとの配分（rotCos, rotSin）を掛け算する！
			XMVECTOR v1 = XMVectorSet(cx + c1 * rotCos, cy + s1, cz + c1 * rotSin, 0);
			XMVECTOR v2 = XMVectorSet(cx + c2 * rotCos, cy + s2, cz + c2 * rotSin, 0);
			Line(&v1, &v2, color);

		}
	}

	//// 最後に、地面の「真ん丸の輪っか（底面）」を1本だけ描いてビシッと引き締める
	//for (int i = 0; i < segments; i++)
	//{
	//	float a1 = i * angleStep; float a2 = (i + 1) * angleStep;
	//	float s1 = sinf(a1) * radius; float c1 = cosf(a1) * radius;
	//	float s2 = sinf(a2) * radius; float c2 = cosf(a2) * radius;
	//	XMVECTOR vXZ1 = XMVectorSet(cx + c1, cy, cz + s1, 0);
	//	XMVECTOR vXZ2 = XMVectorSet(cx + c2, cy, cz + s2, 0);
	//	Line(&vXZ1, &vXZ2, color);
	//}
}


// エフェクトの見た目を切り替えるマクロ
#define USE_RING_COUNT 5	//使用するリングの数（３，５）
// 0 = 4角形でシンプルに水平回転するモード
// 1 = 16角形で縦横クロスして球体に見せるモード
#define SUCTION_EFFECT_MODE 0

//引き寄せ攻撃のエフェクト代わり
void vnDebugDraw::DrawSuctionEffect(const XMVECTOR& center, float radius, float progress, DWORD color,bool isActive,int ringSelect)
{
	float normalizedTime = progress / 0.5f;

	static float effectTimer = 0;

	//float shrinkFactor = normalizedTime;		//円が小さくなって消える
	float shrinkFactor =  1- normalizedTime;	//円が小さくなってまた大きく戻る

	//現在の状態（ステート）によって、円のサイズ計算を切り替える
	if (!isActive)
	{
		// 非アクティブ（準備中）の時はタイマーをリセットし、最大サイズで維持
		shrinkFactor = 1.0f;
	}
	else
	{
		// 発動中は、経過時間（progress）に合わせて小さくする
		shrinkFactor = 1.0f - normalizedTime;
		effectTimer = 0.0f;

	}

	if (shrinkFactor < 0.0f)shrinkFactor = 0.0f;
	if (shrinkFactor > 1.0f) shrinkFactor = 1.0f;
	//３つのリングの設定
	struct RingConfig
	{
		float yOffset;		//リングの高さ
		float baseRadius;	//リングの大きさ
	};
#if USE_RING_COUNT == 3
	// ⭕ 3本バージョンの設定
	const int ringCount = 3;
	RingConfig rings[3][ringCount] =
	{
		// 円が同じ高さで、どんどん円が小さく
		{{1.0f, radius * 1.2f},
		{1.0f, radius * 0.7f},
		{1.0f, radius * 0.4f}},

		// 円が３つの高さで、どんどん円が小さく
		{{2.0f, radius * 1.2f},
		{1.0f, radius * 0.7f},
		{0.1f, radius * 0.4f}},

		// 円が３つの高さで、円の大きさが同じ
		{{2.0f, radius * 1.2f},
		{1.0f, radius * 1.2f},
		{0.1f, radius * 1.2f}},
	};

#elif USE_RING_COUNT == 5
	// 5本バージョンの設定
	const int ringCount = 5;
	const int ringSS = 5;
	RingConfig rings[ringSS][ringCount] =
	{
		// パターン0：同じ高さでどんどん小さく（5重の同心円）
		{
			//{0.0f, radius * 1.4f},
			//{0.0f, radius * 1.1f},
			//{0.0f, radius * 0.8f},
			//{0.0f, radius * 0.5f},
			//{0.0f, radius * 0.2f}
			{0.0f, radius * 1.40f}, // 差分: 0.51 (特大)
			{0.0f, radius * 0.89f}, // 差分: 0.33
			{0.0f, radius * 0.56f}, // 差分: 0.21
			{0.0f, radius * 0.35f}, // 差分: 0.15 (超密集)
			{0.0f, radius * 0.20f}
		},

		// パターン1：綺麗な逆円錐（5段階で高さと広さを変える）
		{
			{2.0f, radius * 1.1f},
			{1.5f, radius * 0.8f},
			{1.0f, radius * 0.5f},
			{0.5f, radius * 0.3f},
			{0.1f, radius * 0.1f}
			//{  2.0f, radius * 1.4f }, // 1本目：はるか上（広く巻き込む）
			//{  1.0f, radius * 1.1f }, // 2本目：少し上
			//{  0.0f, radius * 0.8f }, // 3本目：【ここが基準0（中心）】
			//{ -1.0f, radius * 0.5f }, // 4本目：下に向かってすぼまる
			//{ -2.0f, radius * 0.2f }  // 5本目：足元の中心（吸い込み口）
		},

		// パターン2：綺麗な円柱（高さだけを等間隔に変える）
		{
			{  1.0f, radius * 1.0f }, // 1本目：頭上
			{  0.5f, radius * 1.0f }, // 2本目：胸のあたり
			{  0.0f, radius * 1.0f }, // 3本目：【ここが基準0（中心）】
			{ -1.0f, radius * 1.0f }, // 4本目：太ももあたり
			{ -2.0f, radius * 1.0f }  // 5本目：足元（地面）
		},
		{
			{  2.2f, radius * 0.2f }, // 1本目：頭上
			{  1.5f, radius * 0.6f }, // 2本目：胸のあたり
			{  0.0f, radius * 1.1f }, // 3本目：【ここが基準0（中心）】
			{ -0.8f, radius * 0.6f }, // 4本目：太ももあたり
			{ -1.2f, radius * 0.2f }  // 5本目：足元（地面）
		},
		{
			{ 2.2f, radius  * 1.2f }, // 1本目：頭上
			{  1.5f, radius * 0.6f }, // 2本目：胸のあたり
			{  0.0f, radius * 0.3f }, // 3本目：【ここが基準0（中心）】
			{ -0.8f, radius * 0.6f }, // 4本目：太ももあたり
			{ -1.2f, radius * 1.2f }  // 5本目：足元（地面）
		}

	};
#endif

	//３つのリングを表示
	//for (int r = 0; r < ringCount; r++)
	//{
	//	//時間経過（shrinkFactor）にあわせて半径をリアルタイムに縮小させる
	//	float currentRadius = rings[ringSelect][r].baseRadius * shrinkFactor/2;

	//	//高さの中心座標を決める
	//	XMVECTOR ringCenter = center + XMVectorSet(0.0f, rings[ringSelect][r].yOffset, 0.0f, 0.0f);

	//	//円を書く
	//	const int segments = 16;
	//	float step = XM_2PI / segments;	//円を分割する

	//	for (int i = 0; i < segments; i++)
	//	{
	//		float angle1 = i * step;
	//		float angle2 = (i + 1) * step;

	//		XMVECTOR start = ringCenter + XMVectorSet(cosf(angle1) * currentRadius,0.0f, sinf(angle1) * currentRadius, 0.0f);
	//		XMVECTOR end = ringCenter + XMVectorSet(cosf(angle2) * currentRadius,0.0f, sinf(angle2) * currentRadius, 0.0f);
	//		vnDebugDraw::Line(&start, &end, color);
	//	}
	//}
	 
	
	// ---  回転用の角度を計算  ---
	// isActiveの時だけタイマーを進める
	if (!isActive)
	{
		effectTimer += vnScene::getDeltaTime() * 1.0f;
	}
	//表示位置（高さ）調整
	int offsetY =2;

#if SUCTION_EFFECT_MODE == 0
	//横で回転
	// ケツの数値を変更で回転の速さ変更
	float rotationAngle = effectTimer * XM_2PI * 0.3f;

	for (int r = 0; r < ringCount; r++)
	{
		float currentRadius = (rings[ringSelect][r].baseRadius*1.2) * shrinkFactor / 2;
		XMVECTOR ringCenter = center + XMVectorSet(0.0f, rings[ringSelect][r].yOffset+ offsetY, 0.0f, 0.0f);

		// 偶数・奇数リングで逆回転
		float currentRotation = (r % 2 == 0) ? rotationAngle : -rotationAngle;

		const int segments = 4;
		float step = XM_2PI / segments;

		for (int i = 0; i < segments; i++)
		{
			float angle1 = (i * step) + currentRotation;
			float angle2 = ((i + 1) * step) + currentRotation;

			XMVECTOR start = ringCenter + XMVectorSet(cosf(angle1) * currentRadius, 0.0f, sinf(angle1) * currentRadius, 0.0f);
			XMVECTOR end = ringCenter + XMVectorSet(cosf(angle2) * currentRadius, 0.0f, sinf(angle2) * currentRadius, 0.0f);
			vnDebugDraw::Line(&start, &end, color);

			XMVECTOR startH = ringCenter + XMVectorSet(cosf(angle1) * currentRadius, 0.0f, sinf(angle1) * currentRadius, 0.0f);
			XMVECTOR endH = ringCenter + XMVectorSet(cosf(angle2) * currentRadius, 0.0f, sinf(angle2) * currentRadius, 0.0f);
			vnDebugDraw::Line(&startH, &endH, color);

		}
		if (r == 0)
		{
			const int segmentsC = 32;	//円のセグメント
			float stepC = XM_2PI / segmentsC;
			for (int i = 0; i < segmentsC; i++)
			{
				float angle1 = (i * stepC) + currentRotation;
				float angle2 = ((i + 1) * stepC) + currentRotation;

				XMVECTOR startH = ringCenter + XMVectorSet(cosf(angle1) * currentRadius, 0.0f, sinf(angle1) * currentRadius, 0.0f);
				XMVECTOR endH = ringCenter + XMVectorSet(cosf(angle2) * currentRadius, 0.0f, sinf(angle2) * currentRadius, 0.0f);
				vnDebugDraw::Line(&startH, &endH, color);

			}
		}

	}


#elif SUCTION_EFFECT_MODE == 1

	//縦横でぐるぐる回る円
	float rotationAngle = effectTimer * XM_2PI * 1.0f;

	for (int r = 0; r < ringCount; r++)
	{
		float currentRadius = rings[ringSelect][r].baseRadius * shrinkFactor / 2;

		// 高さの中心
		XMVECTOR ringCenter = center + XMVectorSet(0.0f, rings[ringSelect][r].yOffset, 0.0f, 0.0f);

		float currentRotation = (r % 2 == 0) ? rotationAngle : -rotationAngle;

		const int segments = 16; // 16のままでも縦に回ればバッチリ見えます！
		float step = XM_2PI / segments;

		for (int i = 0; i < segments; i++)
		{
			float angle1 = (i * step) + currentRotation;
			float angle2 = ((i + 1) * step) + currentRotation;

			// ------ ① いつもの横リング（水平） ------
			XMVECTOR startH = ringCenter + XMVectorSet(cosf(angle1) * currentRadius, 0.0f, sinf(angle1) * currentRadius, 0.0f);
			XMVECTOR endH = ringCenter + XMVectorSet(cosf(angle2) * currentRadius, 0.0f, sinf(angle2) * currentRadius, 0.0f);
			vnDebugDraw::Line(&startH, &endH, color);

			// ------ ② 縦リング：XY平面（正面を向いたフラフープ） ------
			XMVECTOR startV1 = ringCenter + XMVectorSet(cosf(angle1) * currentRadius, sinf(angle1) * currentRadius, 0.0f, 0.0f);
			XMVECTOR endV1 = ringCenter + XMVectorSet(cosf(angle2) * currentRadius, sinf(angle2) * currentRadius, 0.0f, 0.0f);
			vnDebugDraw::Line(&startV1, &endV1, color);

			// ------ ③ 縦リング：ZY平面（横を向いたフラフープ） ------
			XMVECTOR startV2 = ringCenter + XMVectorSet(0.0f, sinf(angle1) * currentRadius, cosf(angle1) * currentRadius, 0.0f);
			XMVECTOR endV2 = ringCenter + XMVectorSet(0.0f, sinf(angle2) * currentRadius, cosf(angle2) * currentRadius, 0.0f);
			vnDebugDraw::Line(&startV2, &endV2, color);
		}
	}
#endif
	
}


void vnDebugDraw::DrawSuctionLines(const XMVECTOR& center, float radius, float progress, DWORD color1, DWORD color2, bool isActive)
{
	XMVECTOR safeCenter = XMVectorSetW(center, 1.0f);
	static float effectTimer = 0.0f;

	if (!isActive)
	{
		effectTimer = 0.0f;
		return;
	}

	// 乱数のシードを固定
	srand(0);

	effectTimer += vnScene::getDeltaTime() * 1;

	float normalizedTime = effectTimer / 0.5f;
	if (normalizedTime > 1.0f) normalizedTime = 1.0f;

	// 全体の本数（半分ずつに分ける）
	const int pairCount = 500
		;

	for (int i = 0; i < pairCount; i++)
	{
		XMVECTOR lineStart, lineEnd;

		// --- 1色目の線を計算・描画 ---
		CalculateSuctionLinePoints(safeCenter, radius, normalizedTime, lineStart, lineEnd);
		vnDebugDraw::Line(&lineStart, &lineEnd, color1);

		// --- 2色目の線を計算・描画 ---
		// srandが固定されているので、次のGetRandomFloat()は「別の新しいランダム位置」を指します
		CalculateSuctionLinePoints(safeCenter, radius, normalizedTime, lineStart, lineEnd);
		vnDebugDraw::Line(&lineStart, &lineEnd, color2);
	}
}

void vnDebugDraw::CalculateSuctionLinePoints(
	const XMVECTOR& center, float radius, float normalizedTime,
	XMVECTOR& outStart, XMVECTOR& outEnd)
{
	// ループ内で呼ぶため、ここで GetRandomFloat() を使って形状を決める
	float randAngle = GetRandomFloat() * XM_2PI; // 角度
	float randHeight = GetRandomFloat();          // 高さ
	float randLength = GetRandomFloat();          // 長さ

	// 外側の半径と高さのオフセットを計算
	float baseStartFactor = 0.8f;
	float outerRadius = radius * (baseStartFactor + randLength * 0.1f);
	float yOffset = 0.1f + randHeight * 1.9f;

	// スタート地点（外側固定）
	outStart = center + XMVectorSet(
		cosf(randAngle) * outerRadius,
		yOffset,
		sinf(randAngle) * outerRadius,
		0.0f
	);

	// 先端を中心に向かって伸ばす計算
	float targetInnerRadius = outerRadius * 0.3f;
	float currentInnerRadius = outerRadius - (outerRadius - targetInnerRadius) * normalizedTime;

	// ゴール地点（内側へ伸びる先端）
	outEnd = center + XMVectorSet(
		cosf(randAngle) * currentInnerRadius,
		yOffset,
		sinf(randAngle) * currentInnerRadius,
		0.0f
	);
}

float vnDebugDraw::GetRandomFloat()
{
	return static_cast<float>((rand()) / static_cast<float>(RAND_MAX));
}
