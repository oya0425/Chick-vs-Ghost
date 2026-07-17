//--------------------------------------------------------------//
//	"vn_resourceLoader.h"										//
//		外部ファイル読み込み管理クラス							//
//													2026/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once
#include "../framework/vn_modelData.h"

//リソースの最大数
#define vnRESOURCE_LOADER_MAX	(256)
#define vnRESOURCE_LOADER_TEX_ONLY	(0)

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
#if vnRESOURCE_LOADER_TEX_ONLY
		//読み込まれたテクスチャ
		ID3D12Resource* texbuff;
		DXGI_FORMAT format;
#else
		//テクスチャ情報
		struct _tex
		{
			ID3D12Resource* texbuff;
			DXGI_FORMAT format;
		};
		//vnm情報
		struct _vnm
		{
			vnModelData* vnm;
			ID3D12Resource* vbuff;
			ID3D12Resource* ibuff;
		};

		//読み込まれたリソース
		//union
		//{
		//	_tex tex;
		//	_vnm vnm;
		//};
		_tex tex;
		_vnm vnm;
#endif

		cResourceLoader()
		{
			init();
		}

//		void init()
//		{
//			memset(path, 0, sizeof(path));
//			refCount = 0;
//#if vnRESOURCE_LOADER_TEX_ONLY
//			texbuff = NULL;
//			format = DXGI_FORMAT_UNKNOWN;
//#else
//			memset(&vnm, 0, sizeof(_vnm));
//#endif
//
//		}
		void init()
		{
			memset(path, 0, sizeof(path));
			refCount = 0;
#if vnRESOURCE_LOADER_TEX_ONLY
			texbuff = NULL;
			format = DXGI_FORMAT_UNKNOWN;
#else
			//共用体ではないので、両方のメモリを安全かつ確実に個別初期化する
			tex.texbuff = NULL;
			tex.format = DXGI_FORMAT_UNKNOWN;

			vnm.vnm = NULL;
			vnm.vbuff = NULL;
			vnm.ibuff = NULL;
#endif
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

#if !vnRESOURCE_LOADER_TEX_ONLY
	//vnmファイルの読み込み
	static HRESULT load(const WCHAR* path, vnModelData** ppModelData, ID3D12Resource** ppVertexBuffer, ID3D12Resource** ppIndexBuffer);
#endif


	//テクスチャの解放
	static void release(ID3D12Resource *p);


#if !vnRESOURCE_LOADER_TEX_ONLY
	//vnmファイルの解放
	static void release(vnModelData* p);
#endif


};
