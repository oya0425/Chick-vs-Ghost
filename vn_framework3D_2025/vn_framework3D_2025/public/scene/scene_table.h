//--------------------------------------------------------------//
//	"scene_table.h"												//
//		シーンテーブル											//
//													2025/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

//シーンの種類
enum eSceneTable
{
	Boot,
	TITEL,
	Main,
	SceneMax,
};

//シーンクラス定義ファイル
#include "vn_scene.h"
#include "scene_boot.h"
#include"scene_title.h"

#include"scene_main.h"

//シーン切り替え関数(予約)
void switchScene(eSceneTable scene);
//現在のシーン取得(実処理)
void switchScene();
