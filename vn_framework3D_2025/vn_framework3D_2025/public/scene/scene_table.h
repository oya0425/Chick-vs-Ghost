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
	SkinTest,
	KeyTest,
	PickTest,
	EffectTest,
	GroundTest,
	Field_Test,
	ModelTest,
	SpriteTest,
	JoystickTest,
	SeTest,
	FontTest,
	SceneMax,
};

//シーンクラス定義ファイル
#include "vn_scene.h"
#include "scene_boot.h"
#include"scene_filed_test .h"
#include"scene_model_test.h"
#include "scene_sprite_test.h"
#include "scene_joystick_test.h"
#include "scene_se_test.h"
#include "scene_font_test.h"
#include"scene_ground_test.h"

#include"scene_title.h"
#include"scene_pick_test.h"
#include"scene_effect_test.h"

#include"scene_skin_test.h"
#include"skinned_simple_cube.h"
#include"scene_KeyFrame_test.h"

#include"scene_main.h"

//シーン切り替え関数(予約)
void switchScene(eSceneTable scene);
//現在のシーン取得(実処理)
void switchScene();
