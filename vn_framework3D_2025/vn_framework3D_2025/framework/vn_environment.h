//--------------------------------------------------------------//
//	"vn_environment.h"											//
//		フレームワーク共通環境ヘッダー							//
//													2025/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

//ライブラリ
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "winmm.lib")

#if (_MSC_VER>=1930)

//Visual Studio 2022
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/Bin/Desktop_2022_Win10/x64/Debug/DirectXTex.lib")
#else
#pragma comment(lib, "DirectXTex/Bin/Desktop_2022_Win10/x64/Release/DirectXTex.lib")
#endif

#else

//Visual Studio 2019
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/Bin/Desktop_2019_Win10/x64/Debug/DirectXTex.lib")
#else
#pragma comment(lib, "DirectXTex/Bin/Desktop_2019_Win10/x64/Release/DirectXTex.lib")
#endif

#endif

//共通ヘッダファイルのインクルード
#include <stdio.h>
#include <assert.h>
#include <mmsystem.h>
#include <locale.h>
#include <time.h>

//DirectXヘッダー
#include <d2d1_3.h>
#include <dwrite.h>
#include <d3d11on12.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#define	DIRECTINPUT_VERSION	0x0800
#include <dinput.h>
#include <Xinput.h>
#include <xaudio2.h>

#include <DirectXMath.h>

#include "../DirectXTex/DirectXTex.h"

//ネームスペース
using namespace DirectX;

//一般マクロ
#define SCREEN_WIDTH	(1280)	//クライアント領域の幅(ピクセル)
#define SCREEN_HEIGHT	(720)	//クライアント領域の高さ(ピクセル)

//リリース
#define SAFE_RELEASE(p) {if(p){(p)->Release();(p)=NULL;}}

//フレームワークヘッダー
#include "directX/vn_shader.h"
#include "directX/vn_Direct3D.h"
#include "directX/vn_Direct2D.h"
#include "directX/vn_DirectInput.h"
#include "directX/vn_XInput.h"
#include "directX/vn_XAudio2.h"

#include "vn_mouse.h"
#include "vn_keyboard.h"
#include "vn_joystick.h"
#include "vn_sound.h"

#include "vn_font.h"
#include"vn_camera.h"
#include"vn_light.h"
#include"vn_debugDraw.h"

#include"../public/vn_object.h"

#include"../public/vn_model.h"
#include"../public/vn_billboard.h"
#include"../public/vn_effect.h"
#include"../public/vn_character.h"
#include "../public/vn_sprite.h"
#include"../public/SkillButtonUI.h"
#include "../public/vn_collide.h"
#include"../public/common.h"
#include"../public/RigidbodyComponent.h"
#include"../public/CollisionComponent.h"
#include"../public/CharacterBase.h"
#include"../public/EnemyAIDebug.h"

#include"../public/newPlayerClass.h"
#include"../public/playerClass.h"
#include"../public/Bullet.h"

#include"../public/EnemyClass.h"
#include"../public/NewEnemyClass.h"

#include"../public/EnemyMushroomMon.h"

#include"../public/ExperienceManager.h"
#include"../public/UpgradeSelectionUI.h"

#include"../public/WaveManager.h"

#include"../public/TerrainBlock.h"
#include"../public/BlockManager.h"
#include "../public/scene/vn_scene.h"
#include"../public/EnemyPool.h"
#include"../public/EnemyGhost.h"

#include "../public/scene/scene_table.h"
#include "vn_mainframe.h"

//色

// --- 赤系 ---
#define GAME_COLOR_RED         0xFFFF0000  // 赤
#define GAME_COLOR_ORANGE      0xFFFF8000  // オレンジ
#define GAME_COLOR_PINK        0xFFFFA0C0  // ピンク
#define GAME_COLOR_MAGENTA     0xFFFF00FF  // マゼンタ（ピンク紫）

// --- 緑系 ---
#define GAME_COLOR_GREEN       0xFF00FF00  // 緑
#define GAME_COLOR_LIME        0xFF80FF00  // 黄緑
#define GAME_COLOR_TEAL        0xFF008080  // シアン系暗色（緑寄り）
#define GAME_COLOR_NEON_GREEN  0xFFBFFF00  // ネオン・グリーン（さらに明るい黄緑）

// --- 青系 ---
#define GAME_COLOR_BLUE        0xFF0000FF  // 青
#define GAME_COLOR_LIGHT_BLUE  0xFF80FFFF  // 明るい水色
#define GAME_COLOR_DARK_BLUE   0xFF000080  // 濃い青
#define GAME_COLOR_CYAN        0xFF00FFFF  // シアン（水色）
#define GAME_COLOR_BLUEBLACK   0xFF000D1A  // 深い青みの黒（R=0, G=13, B=26, A=255）

// --- グレー系 ---
#define GAME_COLOR_WHITE       0xFFFFFFFF  // 白
#define GAME_COLOR_LIGHT_GRAY  0xFFC0C0C0  // 薄いグレー
#define GAME_COLOR_DARK_GRAY   0xFF404040  // 濃いグレー
#define GAME_COLOR_BLACK       0xFF000000  // 黒
#define GAME_COLOR_SILVER      0xFFC0C0C0  // シルバー

// --- その他 / 特殊色 ---
#define GAME_COLOR_PURPLE      0xFF8000FF  // 明るめの紫
#define GAME_COLOR_BROWN       0xFF804000  // 茶色（地面や木など）
#define GAME_COLOR_YELLOW      0xFFFFFF00  // 黄色
#define GAME_COLOR_CURSOR      0xFFFFFF80  // カーソル色
#define GAME_COLOR_GOLD        0xFFFFD700  // ゴールド（金色、アイテム用）

// --- 赤系 ---
#define V_GAME_COLOR_RED        XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f)
#define V_GAME_COLOR_ORANGE     XMVectorSet(1.0f, 0.5f, 0.0f, 1.0f)
#define V_GAME_COLOR_PINK       XMVectorSet(1.0f, 0.627f, 0.753f, 1.0f)
#define V_GAME_COLOR_MAGENTA    XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f)

// --- 緑系 ---
#define V_GAME_COLOR_GREEN      XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f)
#define V_GAME_COLOR_LIME       XMVectorSet(0.502f, 1.0f, 0.0f, 1.0f)
#define V_GAME_COLOR_TEAL       XMVectorSet(0.0f, 0.502f, 0.502f, 1.0f)

// --- 青系 ---
#define V_GAME_COLOR_BLUE       XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f)
#define V_GAME_COLOR_LIGHT_BLUE XMVectorSet(0.502f, 1.0f, 1.0f, 1.0f)
#define V_GAME_COLOR_DARK_BLUE  XMVectorSet(0.0f, 0.0f, 0.502f, 1.0f)
#define V_GAME_COLOR_CYAN       XMVectorSet(0.0f, 1.0f, 1.0f, 1.0f)
#define V_GAME_COLOR_BLUEBLACK  XMVectorSet(0.0f, 0.051f, 0.102f, 1.0f)

// --- グレー系 ---
#define V_GAME_COLOR_WHITE      XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)
#define V_GAME_COLOR_LIGHT_GRAY XMVectorSet(0.753f, 0.753f, 0.753f, 1.0f)
#define V_GAME_COLOR_DARK_GRAY  XMVectorSet(0.251f, 0.251f, 0.251f, 1.0f)
#define V_GAME_COLOR_BLACK      XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)
#define V_GAME_COLOR_SILVER     XMVectorSet(0.753f, 0.753f, 0.753f, 1.0f)

// --- その他 / 特殊色 ---
#define V_GAME_COLOR_PURPLE     XMVectorSet(0.502f, 0.0f, 1.0f, 1.0f)
#define V_GAME_COLOR_BROWN      XMVectorSet(0.502f, 0.251f, 0.0f, 1.0f)
#define V_GAME_COLOR_YELLOW     XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f)
#define V_GAME_COLOR_CURSOR     XMVectorSet(1.0f, 1.0f, 0.502f, 1.0f)
#define V_GAME_COLOR_GOLD       XMVectorSet(1.0f, 0.843f, 0.0f, 1.0f)

// --- 追加・調整色 (より判別しやすい中間色) ---
#define V_GAME_COLOR_DEEP_BLACK  XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f)   // 17: ほぼ黒 (背景・枠線用)
#define V_GAME_COLOR_EMERALD     XMVectorSet(0.4f, 1.0f, 0.7f, 1.0f)   // 18: エメラルド (明るい青緑)
#define V_GAME_COLOR_CORAL       XMVectorSet(1.0f, 0.4f, 0.4f, 1.0f)   // 19: コーラル (温かみのある赤橙)
#define V_GAME_COLOR_SKY_BLUE    XMVectorSet(0.7f, 0.9f, 1.0f, 1.0f)   // 20: スカイブルー (淡い水色)

#define V_GAME_COLOR_OLIVE      XMVectorSet(0.5f, 0.5f, 0.0f, 1.0f)   // 05: オリーブ (マゼンタの代わり)
#define V_GAME_COLOR_NAVY       XMVectorSet(0.0f, 0.0f, 0.3f, 1.0f)   // 12: 濃紺 (DARK_BLUEよりさらに深く)
#define V_GAME_COLOR_MINT       XMVectorSet(0.6f, 1.0f, 0.8f, 1.0f)   // 15: ミント (WHITEの代わり)
#define V_GAME_COLOR_WINE       XMVectorSet(0.5f, 0.0f, 0.2f, 1.0f)   // 20: ワインレッド (SKY_BLUEの代わり)