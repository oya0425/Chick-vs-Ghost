//--------------------------------------------------------------//
//	"scene_table.cpp"											//
//		シーンテーブル											//
//													2025/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../../framework.h"
#include "../../framework/vn_environment.h"

//起動時のシーン
#if _DEBUG
eSceneTable initialScene = eSceneTable::Boot;
#else
eSceneTable initialScene = eSceneTable::TITEL;
#endif


//現在のシーン
eSceneTable currentScene = (eSceneTable)-1;
//切り替え予約のシーン
eSceneTable reserveScene = initialScene;


//各シーンの名前	//起動時に出てくる名前
WCHAR SceneName[(int)eSceneTable::SceneMax][32] =
{
	L"Boot",
	L"TITEL",
	L"Main",
};

//ステージ切り替え関数(予約)
void switchScene(eSceneTable scene)
{
	reserveScene = scene;
}

//現在のステージ取得(実処理)
void switchScene()
{
	if(currentScene==reserveScene)
	{
		return;
	}
	
	vnMainFrame::terminateScene();
	
	vnScene *pScene=NULL;
	switch(reserveScene)
	{
	case Boot:			pScene = new SceneBoot();			break;

	case Main:			pScene = new SceneMain();			break;
	case TITEL:			pScene = new SceneTitle();			break;
	}
	
	vnMainFrame::initializeScene(pScene);
	
	currentScene = reserveScene;
}
