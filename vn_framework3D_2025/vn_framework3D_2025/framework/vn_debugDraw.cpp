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
	const float angleStep = XM_2PI / segments;

	float cx = XMVectorGetX(center);
	float cy = XMVectorGetY(center);
	float cz = XMVectorGetZ(center);

	for (int i = 0; i < segments; i++)
	{
		float a1 = i * angleStep;
		float a2 = (i + 1) * angleStep;

		float s1 = sinf(a1) * radius;
		float c1 = cosf(a1) * radius;
		float s2 = sinf(a2) * radius;
		float c2 = cosf(a2) * radius;

		// --- XY平面の円 (正面から見た輪郭) ---
		XMVECTOR vXY1 = XMVectorSet(cx + c1, cy + s1, cz, 0);
		XMVECTOR vXY2 = XMVectorSet(cx + c2, cy + s2, cz, 0);
		Line(&vXY1, &vXY2, color);

		// --- XZ平面の円 (真上から見た輪郭) ---
		XMVECTOR vXZ1 = XMVectorSet(cx + c1, cy, cz + s1, 0);
		XMVECTOR vXZ2 = XMVectorSet(cx + c2, cy, cz + s2, 0);
		Line(&vXZ1, &vXZ2, color);

		// --- YZ平面の円 (横から見た輪郭) ---
		XMVECTOR vYZ1 = XMVectorSet(cx, cy + s1, cz + c1, 0);
		XMVECTOR vYZ2 = XMVectorSet(cx, cy + s2, cz + c2, 0);
		Line(&vYZ1, &vYZ2, color);
	}
}