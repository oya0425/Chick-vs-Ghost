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
	L"SkinTest",
	L"KeyTest",
	L"PickTest",
	L"EffectTest",
	L"GroundTest",
	L"FieldTest",
	L"Model Test",
	L"Sprite Test",
	L"Joystick Test",
	L"SE Test",
	L"Font Test",
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
	case SkinTest:		pScene = new SceneSkinTest();		break;
	case KeyTest:		pScene = new SceneKeyFrameTest();	break;
	case PickTest:		pScene = new ScenePickTest();		break;
	case EffectTest:	pScene = new SceneEffectTest();		break;
	case GroundTest:	pScene = new SceneGroundTest();		break;
	case Field_Test:	pScene = new SceneFieldTest();		break;
	case ModelTest:		pScene = new SceneModelTest();		break;
	case SpriteTest:	pScene = new SceneSpriteTest();		break;
	case JoystickTest:	pScene = new SceneJoystickTest();	break;
	case SeTest:		pScene = new SceneSeTest();			break;
	case FontTest:		pScene = new SceneFontTest();		break;
	}
	
	vnMainFrame::initializeScene(pScene);
	
	currentScene = reserveScene;
}
