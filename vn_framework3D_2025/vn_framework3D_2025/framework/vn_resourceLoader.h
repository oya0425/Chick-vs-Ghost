//--------------------------------------------------------------//
//	"vn_resourceLoader.h"										//
//		外部ファイル読み込み管理クラス							//
//													2026/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

//リソースの最大数
#define vnRESOURCE_LOADER_MAX	(256)

class vnResourceLoader
{
private:
	//リソース１件の情報を保持するクラス
	class cResourceLoader
	{
	public:
		//ファイルのパス
		WCHAR	path[256];

		//参照数
		int refCount;

		//読み込まれたテクスチャ
		ID3D12Resource* texbuff;
		DXGI_FORMAT format;

		cResourceLoader()
		{
			init();
		}

		void init()
		{
			memset(path, 0, sizeof(path));
			refCount = 0;
			texbuff = NULL;
			format = DXGI_FORMAT_UNKNOWN;
		}
	};
	static cResourceLoader	resourceLoader[vnRESOURCE_LOADER_MAX];

public:
	//初期化
	static bool initialize(void);
	//終了
	static void terminate(void);

	//テクスチャの読み込み
	static HRESULT load(const WCHAR *path, ID3D12Resource **ppTexture, DXGI_FORMAT *format);

	//テクスチャの解放
	static void release(ID3D12Resource *p);
};
