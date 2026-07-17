//--------------------------------------------------------------//
//	"vn_resourceLoader.cpp"										//
//		外部ファイル読み込み管理クラス							//
//													2026/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "vn_environment.h"


#if !vnRESOURCE_LOADER_TEX_ONLY
extern HWND hWnd;
#endif

//静的変数
vnResourceLoader::cResourceLoader vnResourceLoader::resourceLoader[vnRESOURCE_LOADER_MAX];

//初期化
bool vnResourceLoader::initialize(void)
{
	for (int i = 0; i < vnRESOURCE_LOADER_MAX; i++)
	{
		resourceLoader[i].init();
	}
	return true;
}

//終了
//void vnResourceLoader::terminate(void)
//{
//	for (int i = 0; i < vnRESOURCE_LOADER_MAX; i++)
//	{
//#if !vnRESOURCE_LOADER_TEX_ONLY
//		SAFE_RELEASE(resourceLoader[i].tex.texbuff);
//#else
//		SAFE_RELEASE(resourceLoader[i].texbuff);
//#endif
//		resourceLoader[i].init();
//	}
//
//}
void vnResourceLoader::terminate(void)
{
	for (int i = 0; i < vnRESOURCE_LOADER_MAX; i++)
	{
#if !vnRESOURCE_LOADER_TEX_ONLY
		// テクスチャポインタが存在する場合のみ安全に Release
		if (resourceLoader[i].tex.texbuff != NULL)
		{
			resourceLoader[i].tex.texbuff->Release();
		}

		// モデルデータの解放漏れ（もしあれば）を安全に Release
		if (resourceLoader[i].vnm.vnm != NULL)
		{
			delete[](BYTE*)resourceLoader[i].vnm.vnm;
		}
		if (resourceLoader[i].vnm.vbuff != NULL)
		{
			resourceLoader[i].vnm.vbuff->Release();
		}
		if (resourceLoader[i].vnm.ibuff != NULL)
		{
			resourceLoader[i].vnm.ibuff->Release();
		}
#else
		if (resourceLoader[i].texbuff != NULL)
		{
			resourceLoader[i].texbuff->Release();
		}
#endif
		// スロットの中身を完全にクリア
		resourceLoader[i].init();
	}
}

//テクスチャの読み込み
HRESULT vnResourceLoader::load(const WCHAR* path, ID3D12Resource** ppTexture, DXGI_FORMAT* format)
{
	if (path == NULL || ppTexture == NULL)return E_FAIL;

	int id = -1;	//新規で読み込む時の配列番号

	//既存のテクスチャを検索
	for (int i = 0; i < vnRESOURCE_LOADER_MAX; i++)
	{
		/*
		
		同名のテクスチャがすでに読み込み済みの場合、新たに読み込みをせず、すでに読み込まれたポインタを返す

		テクスチャがまだ読み込まれていない場合は、読み込み予定の配列番号を覚えておく

		*/
		if (wcscmp(path, resourceLoader[i].path) == 0)
		{
			resourceLoader[i].refCount++;
#if !vnRESOURCE_LOADER_TEX_ONLY
			* ppTexture = resourceLoader[i].tex.texbuff;
#else
			* ppTexture = resourceLoader[i].texbuff;
#endif
			return S_OK;
		}
		if (id == -1 && resourceLoader[i].refCount == 0)
		{
			id = i;
			break;
		}


		//if (resourceLoader[i].refCount > 0 && wcscmp(resourceLoader[i].path, path) == 0)
		//{
		//	resourceLoader[i].refCount++;
		//	*ppTexture = resourceLoader[i].tex.texbuff;
		//	*format = resourceLoader[i].tex.format;
		//	return S_OK;
		//}

		//空きを探しておく（新規書き込み用）
		if (id == -1 || resourceLoader[i].refCount == 0)
		{
			id = i;
		}


	}

	if (id == -1)return E_FAIL;

	//テクスチャの新規読み込み

	HRESULT hr = S_OK;
	ID3D12Resource* texbuff = NULL;
	//WICテクスチャのロード
	TexMetadata metadata = {};
	ScratchImage scratchImg = {};

	const WCHAR* ext = wcschr(path, L'.');
	if (ext == NULL)return E_FAIL;

	if (wcscmp(ext, L".tga") == 0 || wcscmp(ext, L".TGA") == 0)
	{	//tga
		hr = LoadFromTGAFile(path, TGA_FLAGS_NONE, &metadata, scratchImg);
	}
	else
	{	//png, jpg, bmp
		hr = LoadFromWICFile(path, WIC_FLAGS_NONE, &metadata, scratchImg);
	}
	if (hr != S_OK)
	{
		*ppTexture = NULL;
		return hr;
	}
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
		NULL,
		IID_PPV_ARGS(&texbuff)
	);
	assert(hr == S_OK);

	hr = texbuff->WriteToSubresource(0,
		NULL,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch
	);
	assert(hr == S_OK);

	//読み込んだテクスチャ情報の確保
	/*

	読み込み予定の配列番号の構造体へ、次、同じテクスチャが読み込まれ場合に使いまわせるように、情報を覚えておく

	*/
	wcsncpy_s(resourceLoader[id].path, path, 256);
	resourceLoader[id].refCount = 1;
#if !vnRESOURCE_LOADER_TEX_ONLY
	resourceLoader[id].tex.texbuff = texbuff;
	resourceLoader[id].tex.format = metadata.format;
#else
	resourceLoader[id].texbuff = texbuff;
	resourceLoader[id].format = metadata.format;
#endif

	//呼び出し元へ返却
	/*
	
	新規に読み込んだテクスチャの情報(ポインタ)を呼び出し元へ返す

	*/
#if !vnRESOURCE_LOADER_TEX_ONLY
	* ppTexture = resourceLoader[id].tex.texbuff;
	*format = resourceLoader[id].tex.format;
#else
	* ppTexture = resourceLoader[id].texbuff;
	*format = resourceLoader[id].format;
#endif




	return hr;
}


//テクスチャの解放
void vnResourceLoader::release(ID3D12Resource* p)
{
	if (p == NULL)return;

	/*

	①解放させるテクスチャを配列の中から見つける

	②参照数を1減らす

	③-1 : 参照数が残っている場合は、そのテクスチャは別のオブジェクトがまだ使っている状態なので、何もしない

	③-2 : 参照数がゼロになった場合、そのテクスチャを使っているオブジェクトが存在しないので、テクスチャ自体も削除(Release)する
		   ※cTexLoaderの該当スロットは再利用できるように、中身をクリアしておく

	*/
	for (int i = 0; i < vnRESOURCE_LOADER_MAX; i++)
	{
#if !vnRESOURCE_LOADER_TEX_ONLY
		if (p == resourceLoader[i].tex.texbuff)
#else
		if (p == resourceLoader[i].texbuff)
#endif
		{
			resourceLoader[i].refCount--;
			if (resourceLoader[i].refCount == 0)
			{
#if !vnRESOURCE_LOADER_TEX_ONLY
				SAFE_RELEASE(resourceLoader[i].tex.texbuff);
#else
				SAFE_RELEASE(resourceLoader[i].texbuff);
#endif
				resourceLoader[i].init();
			}
			break;
		}
	}
}

#if !vnRESOURCE_LOADER_TEX_ONLY

//vnmファイルの読み込み
HRESULT vnResourceLoader::load(const WCHAR* path, vnModelData** ppModelData, ID3D12Resource** ppVertexBuffer, ID3D12Resource** ppIndexBuffer)
{
	if (path == NULL || ppModelData == NULL)return E_FAIL;

	int id = -1;	//新規で読み込む時の配列番号

	//既存のファイルを検索
	for (int i = 0; i < vnRESOURCE_LOADER_MAX; i++)
	{
		if (wcscmp(path, resourceLoader[i].path) == 0)
		{
			resourceLoader[i].refCount++;
			*ppModelData = resourceLoader[i].vnm.vnm;
			*ppVertexBuffer = resourceLoader[i].vnm.vbuff;
			*ppIndexBuffer = resourceLoader[i].vnm.ibuff;
			return S_OK;
		}
		if (id == -1 && resourceLoader[i].refCount == 0)
		{
			id = i;
			break;
		}
	}

	if (id == -1)return E_FAIL;	//配列に空きがなかった場合(新規で読み込み不可)

	//ファイルの新規読み込み

	HRESULT hr = S_OK;
	FILE* fp = NULL;
	long size = 0;

	if ((_wfopen_s(&fp, path, L"rb")) != 0)
	{
		WCHAR text[256];
		swprintf_s(text, L"cannot open file \"%s\"", path);
		vnFont::output(text);
		MessageBox(hWnd, text, L"vnModel Construct Error", MB_OK);
		assert(false);
		return E_FAIL;
	}
	if (!fp)return E_FAIL;

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	*ppModelData = (vnModelData*)new BYTE[size];
	fread(*ppModelData, size, 1, fp);
	fclose(fp);

	//バージョンチェック
	if ((*ppModelData)->Version < vnMODEL_DATA_LATEST_VER)
	{
		WCHAR text[256];
		swprintf_s(text, L"this file version is obsolete : \"%s\"", path);
		vnFont::output(text);
		MessageBox(hWnd, text, L"vnModel Construct Error", MB_OK);
		assert(false);
		return E_FAIL;
	}

	{	//頂点バッファ
		//const UINT vertexStride = (sizeof(vnVertex3D) > 0) ? sizeof(vnVertex3D) : sizeof(vnVertex3D);
		const UINT vertexStride = sizeof(vnVertex3D);
		const UINT vertexBufferSize = vertexStride * (*ppModelData)->VertexNum;

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
			IID_PPV_ARGS(ppVertexBuffer)
		);
		assert(hr == S_OK);

		//頂点データのコピー
		vnVertex3D* pVertexDataBegin = reinterpret_cast<vnVertex3D*>(reinterpret_cast<__int64>(*ppModelData) + (*ppModelData)->VertexAccess);
		vnVertex3D* pMappedMem = NULL;
		hr = (*ppVertexBuffer)->Map(0, NULL, (void**)&pMappedMem);
		memcpy(pMappedMem, pVertexDataBegin, vertexBufferSize);
		(*ppVertexBuffer)->Unmap(0, nullptr);


	}


	{	//インデックスバッファ
		const UINT indexBufferSize = (((*ppModelData)->VertexNum <= 0xffff) ? sizeof(WORD) : sizeof(DWORD)) * (*ppModelData)->IndexNum;

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
			IID_PPV_ARGS(ppIndexBuffer)
		);
		if (hr != S_OK)
		{
			assert(hr == S_OK);
		}

		//インデックスデータのコピー
		void* pIndexDataBegin = reinterpret_cast<void*>(reinterpret_cast<__int64>(*ppModelData) + (*ppModelData)->IndexAccess);
		void* pMappedMem = NULL;
		(*ppIndexBuffer)->Map(0, nullptr, (void**)&pMappedMem);
		memcpy(pMappedMem, pIndexDataBegin, indexBufferSize);
		(*ppIndexBuffer)->Unmap(0, nullptr);
	}

	wcscpy_s(resourceLoader[id].path, path);
	resourceLoader[id].refCount = 1;
	resourceLoader[id].vnm.vnm = *ppModelData;
	resourceLoader[id].vnm.vbuff = *ppVertexBuffer;
	resourceLoader[id].vnm.ibuff = *ppIndexBuffer;

	return hr;
}


//vnmファイルの解放
void vnResourceLoader::release(vnModelData* p)
{
	if (p == NULL)return;

	for (int i = 0; i < vnRESOURCE_LOADER_MAX; i++)
	{
		if (p == resourceLoader[i].vnm.vnm)
		{
			resourceLoader[i].refCount--;
			if (resourceLoader[i].refCount == 0)
			{
				delete[] resourceLoader[i].vnm.vnm;
				SAFE_RELEASE(resourceLoader[i].vnm.vbuff);
				SAFE_RELEASE(resourceLoader[i].vnm.ibuff);
				resourceLoader[i].init();
			}
			break;
		}
	}
}

#endif

