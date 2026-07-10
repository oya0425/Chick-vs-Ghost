//--------------------------------------------------------------//
//	"vn_resourceLoader.cpp"										//
//		外部ファイル読み込み管理クラス							//
//													2026/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "vn_environment.h"

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
void vnResourceLoader::terminate(void)
{
	for (int i = 0; i < vnRESOURCE_LOADER_MAX; i++)
	{
		SAFE_RELEASE(resourceLoader[i].texbuff);
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
		if (resourceLoader[i].refCount > 0 && wcscmp(resourceLoader[i].path, path) == 0)
		{
			resourceLoader[i].refCount++;
			*ppTexture = resourceLoader[i].texbuff;
			*format = resourceLoader[i].format;
			return S_OK;
		}

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
	resourceLoader[id].texbuff = texbuff;
	resourceLoader[id].format = metadata.format;

	//呼び出し元へ返却
	*ppTexture = resourceLoader[id].texbuff;
	*format = resourceLoader[id].format;

	/*
	
	新規に読み込んだテクスチャの情報(ポインタ)を呼び出し元へ返す

	*/


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
	if (p == NULL)return;

	// ①解放させるテクスチャを配列の中から見つける
	for (int i = 0; i < vnRESOURCE_LOADER_MAX; i++)
	{
		if (resourceLoader[i].refCount > 0 && resourceLoader[i].texbuff == p)
		{
			// ②参照数を1減らす
			resourceLoader[i].refCount--;

			// ③-1 : 参照数が残っている場合は何もしない
			if (resourceLoader[i].refCount > 0)
			{
				return; // 発見して処理したので終了
			}

			// ③-2 : 参照数がゼロになった場合、テクスチャ自体も削除(Release)する
			SAFE_RELEASE(resourceLoader[i].texbuff);

			// cTexLoaderの該当スロットは再利用できるように、中身をクリアしておく
			// (initializeやterminateで呼ばれているinit関数を使い回す)
			resourceLoader[i].init();

			return; // 解放処理が完了したので終了
		}
	}
}


