//--------------------------------------------------------------//
//	"common.cpp"												//
//		汎用用関数												//
//													2025/11/05	//
//														Oya  	//
//--------------------------------------------------------------//
#include"../framework.h"
#include"../framework/vn_environment.h"

using namespace Common;
//--------------------------------------
// マウスによるカメラ操作（引数なし）
//--------------------------------------
//void Common::UpdateCameraByMouse(float& theta, float& phi, float& radius, const XMVECTOR* pPlayerPos)
//{
//    // 左クリックドラッグで回転
//    if (vnMouse::onL()) {
//        int dx = vnMouse::getDX();
//        int dy = vnMouse::getDY();
//
//        theta += dx * 0.005f;
//        phi -= dy * 0.005f;
//
//        // 上下角の制限
//        const float limit = XM_PIDIV2 - 0.1f;
//        if (phi > limit) phi = limit;
//        if (phi < -limit) phi = -limit;
//    }
//
//    // ホイールでズーム
//    int wheel = vnMouse::getR();
//    if (wheel != 0) {
//        radius -= wheel * 0.001f*10;
//        if (radius < 1.0f) radius = 1.0f;
//        if (radius > 100.0f) radius = 100.0f;
//    }
//
//    // --- カメラ位置の更新 ---
//    float camX = radius * cosf(phi) * cosf(theta);
//    float camY = radius * sinf(phi);
//    float camZ = radius * cosf(phi) * sinf(theta);
//
//    // プレイヤーの位置を中心にする
//    XMVECTOR camPos = XMVectorSet(camX, camY, camZ, 0.0f);
//    XMVECTOR camTarget = *pPlayerPos;   // 注視点をプレイヤーへ
//    camPos = XMVectorAdd(camPos, *pPlayerPos);
//
//    vnCamera::setPosition(&camPos);
//    vnCamera::setTarget(&camTarget);
//}


void Common::UpdateCameraByMouse(
    float& theta,
    float& phi,
    float& radius,
    const XMVECTOR* pTargetPos
)
{
    // --- マウス移動量 ---
    int dx = vnMouse::getDX();
    int dy = vnMouse::getDY();

    const float sensitivity = 0.0025f;

    theta += dx * sensitivity;
    phi -= dy * sensitivity;

    // ピッチ制限
    const float limit = XM_PIDIV2 - 0.1f;
    if (phi > limit)  phi = limit;
    //if (phi < -limit) phi = -limit;
    if (phi < 0.2) phi = 0.2f;

    // ズーム
    int wheel = vnMouse::getR();
    if (wheel != 0)
    {
        radius -= wheel * 0.01f;
        //radius = max(1.0f, min(radius, 100.0f));
        radius = max(10.0f, min(radius, 40.0f));
    } 
    

    // --- カメラ座標 ---
    float camX = radius * cosf(phi) * cosf(theta);
    float camY = radius * sinf(phi);
    float camZ = radius * cosf(phi) * sinf(theta);

    XMVECTOR camPos = XMVectorSet(camX, camY, camZ, 0.0f);
    camPos = XMVectorAdd(camPos, *pTargetPos);

    vnCamera::setPosition(&camPos);
    vnCamera::setTarget(pTargetPos);
}

void Common::UpdateFlexibleCamera(const XMVECTOR* pPlayerPos, float phi, float radius, float theta,float fenceRadius)
{
    if (!pPlayerPos)return;
#pragma region カメラに回転がある（奥行きがよく見えてしまう）

    {
        //// 1.プレイヤーの座標をコピーして制限をかける
        //XMVECTOR limitedPos = *pPlayerPos;

        //float x = XMVectorGetX(limitedPos);
        //float z = XMVectorGetZ(limitedPos);

        //// 2. XとZをそれぞれ正しく制限
        //if (fenceRadius != 0)
        //{
        //    float h = fenceRadius / 3;
        //    x = MyClamp(x, -h, h);
        //    z = MyClamp(z, -h, h);
        //}

        //// 3. 制限した値を戻す
        //limitedPos = XMVectorSetX(limitedPos, x);
        //limitedPos = XMVectorSetZ(limitedPos, z);

        //float camX = radius * cosf(phi) * cosf(theta);
        //float camY = radius * sinf(phi);
        //float camZ = radius * cosf(phi) * sinf(theta);

        //XMVECTOR camOffset = XMVectorSet(camX, camY, camZ, 0.0f);
        ////XMVECTOR finalCamPos = XMVectorAdd(camOffset, *pPlayerPos);
        //XMVECTOR finalCamPos = XMVectorAdd(camOffset, limitedPos);

        ////シェイク処理を追加
        //if (g_shakeTimer > 0.0f)
        //{
        //    //float damp = g_shakeTimer / g_shakeMaxDuration;
        //    float damp = (g_shakeTimer / g_shakeMaxDuration) * (g_shakeTimer / g_shakeMaxDuration);

        //    //乱数で揺らす
        //    float offsetX = ((rand() % 2000 - 1000) / 1000.0f) * g_shakeIntensityX * damp;
        //    float offsetY = ((rand() % 2000 - 1000) / 1000.0f) * g_shakeIntensityY * damp;
        //    XMVECTOR shakeVec = XMVectorSet(offsetX, offsetY, 0.0f, 0.0f);

        //    finalCamPos = XMVectorAdd(finalCamPos, shakeVec);

        //    //タイマーを減らす
        //    g_shakeTimer -= vnScene::getDeltaTime();;
        //}


        //vnCamera::setPosition(&finalCamPos);
        //vnCamera::setTarget(pPlayerPos);

    }

#pragma endregion


#pragma region カメラに回転がない
    // 1.理想のオフセットを作成
    {
        float camX = radius * cosf(phi) * cosf(theta);
        float camY = radius * sinf(phi);
        float camZ = radius * cosf(phi) * sinf(theta);
        XMVECTOR camOffset = XMVectorSet(camX, camY, camZ, 0.0f);

        // 2.注視点を作る
        XMVECTOR finalTargetPos = *pPlayerPos;

        // 3.XZの座標の制限
        if (fenceRadius > 0.1)
        {
            //Yをプレイヤーのジャンプで動かないようにする
            //finalTargetPos = XMVectorSetY(finalTargetPos, 0.0f);

            //映す範囲の設定
            float limit = fenceRadius * 0.25f;
            float tx = MyClamp(XMVectorGetX(finalTargetPos), -limit, limit);
            float tz = MyClamp(XMVectorGetZ(finalTargetPos), -limit, limit);

            //制限したXZと、固定したYをセット
            finalTargetPos = XMVectorSet(tx, 0.0f, tz, 0.0f);
        }
        //最終的なカメラ位置を計算
        XMVECTOR finalCamPos = XMVectorAdd(camOffset, finalTargetPos);
        //シェイク処理を追加
        if (g_shakeTimer > 0.0f)
        {
            //float damp = g_shakeTimer / g_shakeMaxDuration;
            float damp = (g_shakeTimer / g_shakeMaxDuration) * (g_shakeTimer / g_shakeMaxDuration);

            //乱数で揺らす
            float offsetX = ((rand() % 2000 - 1000) / 1000.0f) * g_shakeIntensityX * damp;
            float offsetY = ((rand() % 2000 - 1000) / 1000.0f) * g_shakeIntensityY * damp;
            XMVECTOR shakeVec = XMVectorSet(offsetX, offsetY, 0.0f, 0.0f);

            finalCamPos = XMVectorAdd(finalCamPos, shakeVec);

            //タイマーを減らす
            g_shakeTimer -= vnScene::getDeltaTime();;
        }

        vnCamera::setPosition(&finalCamPos);
        vnCamera::setTarget(&finalTargetPos);

    }
#pragma endregion



}

void Common::UpdateCameraLevelUp(
    const XMVECTOR* pPlayerPos, //プレイヤーの現在地
    float targetTheta,          //演出開始時に固定した目標角度
    float deltaTime,            //フレーム間の経過時間
    float& currentPhi,          //現在のカメラの上下角
    float& currentRadius,       //現在のカメラの距離
    float& currentTheta         //現在のカメラの回転角
)
{
    // 1.目標値の設定
    const float targetPhi = 0.5f;       //最終的にこの高さに合わせる
    const float targetRadius = 10.0f;    //最終的にプレイヤーから５ｍの距離まで近づく
    float speed = 5.0f * deltaTime;     //補間速度（１秒間にどれくらい近づくか）

    // 2.数値の補間
    //Lerpを使って、現在の「高さ」と「距離」を目標値へ向かって一歩近づける
    currentPhi = MyLerp(currentPhi, targetPhi, speed);
    currentRadius = MyLerp(currentRadius, targetRadius, speed);

    // 3.角度の計算と修正
    //モデルの回転方向とカメラの計算方向（時計回り/反時計回り）を合わせるため符号を反転
    targetTheta = -targetTheta;

    //現在の角度から目標角度までの「差分」を計算
    float deltaTheta = targetTheta - currentTheta;

    //最短距離補間：350度から10度に動くとき、逆回転せず最短の２０度分だけ回るように調整
    while (deltaTheta > XM_PI) deltaTheta -= 2.0f * XM_PI;
    while (deltaTheta < -XM_PI) deltaTheta += 2.0f * XM_PI;

    //計算した差分にスピードを掛けて、現在の角度を目標へ近づける
    currentTheta += deltaTheta * speed;

    // --- 4. 注視点のY座標を固定する ---
        // プレイヤーの座標をコピー
    XMVECTOR fixedTargetPos = *pPlayerPos;

    // Y座標だけ「地面の高さ（またはお好みの高さ）」に固定する
    // プレイヤーがピョンピョン跳ねても、カメラのターゲットは地面に残る
    float groundLevel = 2.0f; // 地面の高さに合わせて調整してください
    fixedTargetPos = XMVectorSetY(fixedTargetPos, groundLevel);

    ////プレイヤーを画面の端に寄せるためのオフセット計算
    ////offsetAmountがプラスならプレイヤーは画面の左側、マイナスなら右側に寄せる
    //float offsetAmount = 3.0f;

    ////カメラの横方向ベクトルを計算(Thetaを利用)
    ////currentThetaはカメラの回転角なので、その方向に合わせてオフセットを振る
    //float offsetX = cosf(currentTheta) * offsetAmount;
    //float offsetZ = -sinf(currentTheta) * offsetAmount;

    ////注視点をプレイヤーの位置から横にずらす
    //fixedTargetPos = XMVectorAdd(fixedTargetPos, XMVectorSet(offsetX, 0, offsetZ, 0));

    // 5. 最終的な適用 (書き換えた fixedTargetPos を渡す)
    UpdateFlexibleCamera(&fixedTargetPos, currentPhi, currentRadius, currentTheta,0);

}


//カメラシェイク開始関数
void Common::StartCameraShake(float x, float y, float duration)
{
    g_shakeIntensityX = x;
    g_shakeIntensityY = y;
    g_shakeMaxDuration = duration;
    g_shakeTimer = duration;
}