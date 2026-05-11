//--------------------------------------------------------------//
//	"common.h"													//
//		汎用用関数												//
//													2025/11/05	//
//														Oya  	//
//--------------------------------------------------------------//
#pragma once
//--------------------------------------
// マウスによるカメラ操作（引数なし）
//--------------------------------------
namespace Common {
	void UpdateCameraByMouse(float& theta, float& phi, float& radius, const XMVECTOR* pPlayerPos);
    void UpdateFlexibleCamera(
        const XMVECTOR* pPlayerPos,
        float phi = 0.6f,    // デフォルトの仰角
        float radius = 20.0f, // デフォルトの距離
        float theta = 92.688f, // 基本の向き
        float fenceRadius=0.0f       //ステージの半径
    );


    //レベルアップ時のカメラの動き
    void UpdateCameraLevelUp(
        const XMVECTOR* pPlayerPos,
        float playerRotationY,
        float deltaTime,
        float& currentPhi,
        float& currentRadius,
        float& currentTheta
    );
    // static関数にして、Common::MyLerp のように呼ぶ
    static float MyLerp(float start, float end, float t) {
        return start + (end - start) * t;
    }
    //カメラシェイク
    static float g_shakeTimer = 0.0f;   //揺れる時間
    static float g_shakeMaxDuration = 0.0f;
    static float g_shakeIntensityX = 0.0f;  //横に揺れる最大値
    static float g_shakeIntensityY = 0.0f;  //縦に揺れる最大値

    void StartCameraShake(float x, float y, float duration);

    template<typename T>
    T MyClamp(T value, T min, T max)
    {
        if (value < min)return min;
        if (value > max)return max;
        return value;
    }
}
