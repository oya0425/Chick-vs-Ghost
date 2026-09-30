//--------------------------------------------------------------//
//	"common.cpp"												//
//		汎用用関数												//
//													2025/11/05	//
//														Oya  	//
//--------------------------------------------------------------//
#include"../../framework.h"
#include"../../framework/vn_environment.h"
#include"../MainScript//Character/CharacterBase.h"

using namespace Common;



#pragma region カメラ関係
//======================================================================
// --- マウスに合わせてカメラの向き変更 ---
//======================================================================
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

void Common::UpdateFlexibleCamera(const XMVECTOR* pPlayerPos, float phi, float radius, float theta, float fenceRadius)
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
    // オフセットを作成
    {
        float camX = radius * cosf(phi) * cosf(theta);
        float camY = radius * sinf(phi);
        float camZ = radius * cosf(phi) * sinf(theta);
        XMVECTOR camOffset = XMVectorSet(camX, camY, camZ, 0.0f);

        // 注視点を作る
        XMVECTOR finalTargetPos = *pPlayerPos;

        // XZの座標の制限
        if (fenceRadius > 0.1)
        {
            //Yをプレイヤーのジャンプで動かないようにする
            //finalTargetPos = XMVectorSetY(finalTargetPos, 0.0f);

            //映す範囲の設定
            float limit = fenceRadius * 0.5f;
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

//======================================================================
// --- プレイヤーの前にカメラを持ってくる ---
//======================================================================
void Common::UpdateCameraLevelUp(
    const XMVECTOR* pPlayerPos, //プレイヤーの現在地
    float targetTheta,          //演出開始時に固定した目標角度
    float deltaTime,            //フレーム間の経過時間
    float& currentPhi,          //現在のカメラの上下角
    float& currentRadius,       //現在のカメラの距離
    float& currentTheta         //現在のカメラの回転角
)
{
    //目標値の設定
    const float targetPhi = 0.5f;       //最終的にこの高さに合わせる
    const float targetRadius = 10.0f;    //最終的にプレイヤーから５ｍの距離まで近づく
    float speed = 5.0f * deltaTime;     //補間速度（１秒間にどれくらい近づくか）

    //数値の補間
    //Lerpを使って、現在の「高さ」と「距離」を目標値へ向かって一歩近づける
    currentPhi = MyLerp(currentPhi, targetPhi, speed);
    currentRadius = MyLerp(currentRadius, targetRadius, speed);

    //角度の計算と修正
    //モデルの回転方向とカメラの計算方向（時計回り/反時計回り）を合わせるため符号を反転
    targetTheta = -targetTheta;

    //現在の角度から目標角度までの「差分」を計算
    float deltaTheta = targetTheta - currentTheta;

    //最短距離補間：350度から10度に動くとき、逆回転せず最短の２０度分だけ回るように調整
    while (deltaTheta > XM_PI) deltaTheta -= 2.0f * XM_PI;
    while (deltaTheta < -XM_PI) deltaTheta += 2.0f * XM_PI;

    //計算した差分にスピードを掛けて、現在の角度を目標へ近づける
    currentTheta += deltaTheta * speed;

    //注視点のY座標を固定する
    //プレイヤーの座標をコピー
    XMVECTOR fixedTargetPos = *pPlayerPos;

    //Y座標だけ地面の高さに固定する
    //プレイヤーがピョンピョン跳ねても、カメラのターゲットは地面に残る
    float groundLevel = 2.0f; // 地面の高さに合わせて調整してください
    fixedTargetPos = XMVectorSetY(fixedTargetPos, groundLevel);


    //最終的な適用 (書き換えた fixedTargetPos を渡す)
    UpdateFlexibleCamera(&fixedTargetPos, currentPhi, currentRadius, currentTheta, 0);

}


//カメラシェイク開始関数
void Common::StartCameraShake(float x, float y, float duration)
{
    g_shakeIntensityX = x;
    g_shakeIntensityY = y;
    g_shakeMaxDuration = duration;
    g_shakeTimer = duration;
}

#pragma endregion


#pragma region ボタン関係


//ボタンの上に来たかどうか（画像の位置にマウスが来たかどうか）
bool Common::OnButton(float x, float y, float button_w, float button_h)
{
    int mx = vnMouse::getX();
    int my = vnMouse::getY();

    return (mx >= x - button_w / 2 && mx <= x + button_w / 2 &&
        my >= y - button_h / 2 && my <= y + button_h / 2);
}


//ボタン処理（ボタン押したときにtrue）
bool Common::UpdateButton(
    float x,
    float y,
    float button_w,
    float button_h,
    vnSprite* pButton,
    bool& isOnButton,
    float& buttonScale,
    SoundManager* soundManager)
{
    bool isPressed = false;

    if (OnButton(x, y, button_w, button_h))
    {
        if (!isOnButton)
        {
            soundManager->PlaySE(SE_TITLE_CURSOR);
        }

        isOnButton = true;

        buttonScale += (1.2f - buttonScale) * 0.2f;
        pButton->setColor(V_GAME_COLOR_WHITE);

        if (vnMouse::trgL())
        {
            buttonScale = 1.0f;
            isPressed = true;
        }
    }
    else
    {
        isOnButton = false;

        buttonScale += (1.0f - buttonScale) * 0.2f;
        pButton->setColor(V_GAME_COLOR_BLACK);
    }

    pButton->setScale(buttonScale);

    return isPressed;
}

//======================================================================
// --- ボタンに合わせてテキストも拡大(ボタンの拡大縮小) ---
//======================================================================
void Common::ChangeButtonTextSize(
    float x,
    float y,
    float buttonScale,
    bool isOnButton,
    const WCHAR* text,
    IDWriteTextFormat* pFormat)
{
    size_t len = wcslen(text);

    float currentFontSize = 30.0f * buttonScale;

    vnFont::setFontSize(pFormat, (int)currentFontSize);

    float textWidth = currentFontSize * 0.65f * len;
    float textHeight = currentFontSize * 0.5f;

    float tx = x - textWidth * 0.5f;
    float ty = y - textHeight;

    unsigned int textCol;
    unsigned int outlineCol;

    if (isOnButton)
    {
        // 文字：黒
        // 輪郭：白
        textCol = GAME_COLOR_BLACK;
        outlineCol = GAME_COLOR_LIGHT_GRAY;
    }
    else
    {
        // 文字：白
        // 輪郭：黒
        textCol = GAME_COLOR_WHITE;
        outlineCol = GAME_COLOR_DARK_GRAY;
    }

    //================================================
    // 文字の輪郭
    //================================================
    //文字のサイズの拡大ではずれた
    const float outline = 3.0f;

    // 上
    vnFont::print(tx, ty - outline, outlineCol, text);

    // 下
    vnFont::print(tx, ty + outline, outlineCol, text);

    // 左
    vnFont::print(tx - outline, ty, outlineCol, text);

    // 右
    vnFont::print(tx + outline, ty, outlineCol, text);

    // 左上
    vnFont::print(tx - outline, ty - outline, outlineCol, text);

    // 右上
    vnFont::print(tx + outline, ty - outline, outlineCol, text);

    // 左下
    vnFont::print(tx - outline, ty + outline, outlineCol, text);

    // 右下
    vnFont::print(tx + outline, ty + outline, outlineCol, text);

    //================================================
    // 本来の文字
    //================================================
    vnFont::print(tx, ty, textCol, text);
}

#pragma endregion

#pragma region 当たり判定
//==================================================
// --- 当たり判定（キャラクターとキャラクター）
//==================================================
Common::eDirection Common::colliderCtoC(CharacterBase* p1, CharacterBase* p2)
{
    eDirection ret = eDirection::None;

    if (!p1 || !p2) return ret;

    XMVECTOR range = XMVectorAdd(p1->GetCollision().GetSize() * 0.5f, p2->GetCollision().GetSize() * 0.5f);
    float rx = XMVectorGetX(range);
    float ry = XMVectorGetY(range);
    float rz = XMVectorGetZ(range);

    XMVECTOR center1 = XMVectorAdd(*p1->GetModel()->getPosition(), p1->GetCollision().GetCenter());
    XMVECTOR center2 = XMVectorAdd(*p2->GetModel()->getPosition(), p2->GetCollision().GetCenter());

    XMVECTOR dif = XMVectorAbs(center1 - center2);

    float dx = XMVectorGetX(dif);
    float dy = XMVectorGetY(dif);
    float dz = XMVectorGetZ(dif);

    if (dx < rx && dy < ry && dz < rz)
    {
        float sx = rx - dx;
        float sy = ry - dy;
        float sz = rz - dz;

        if (sx < sy && sx < sz)
        {
            if (XMVectorGetX(center1) < XMVectorGetX(center2))
            {
                p1->GetModel()->addPositionX(-sx);
                ret = X_Neg;
            }
            else
            {
                p1->GetModel()->addPositionX(sx);
                ret = X_Pos;
            }
        }
        else if (sy < sz)
        {
            if (XMVectorGetY(center1) < XMVectorGetY(center2))
            {
                p1->GetModel()->addPositionY(-sy);
                ret = Y_Neg;
            }
            else
            {
                p1->GetModel()->addPositionY(sy);
                p1->GetRigidbody().SetIsGround(true);
                ret = Y_Pos;
            }
        }
        else
        {
            if (XMVectorGetZ(center1) < XMVectorGetZ(center2))
            {
                p1->GetModel()->addPositionZ(-sz);
                ret = Z_Neg;
            }
            else
            {
                p1->GetModel()->addPositionZ(sz);
                ret = Z_Pos;
            }
        }
    }

    return ret;
}


//==================================================
// 球体同士の判定と押し戻し
//==================================================
Common::eDirection Common::colliderStoS(CharacterBase* p1, CharacterBase* p2)
{
    eDirection ret = eDirection::None;
    if (!p1 || !p2) return ret;

    auto& col1 = p1->GetCollision();
    auto& col2 = p2->GetCollision();

    // 半径の取得（size.xを直径として扱う、またはradiusを追加）
    float r1 = p1->GetEffectiveRadius();
    float r2 = p2->GetEffectiveRadius();
    float sumRadii = r1 + r2;

    // 世界座標での中心位置
    XMVECTOR center1 = XMVectorAdd(*p1->GetModel()->getPosition(), col1.GetCenter());
    XMVECTOR center2 = XMVectorAdd(*p2->GetModel()->getPosition(), col2.GetCenter());

    // 距離の計算
    XMVECTOR diff = center2 - center1;
    XMVECTOR distSqVec = XMVector3LengthSq(diff);
    float distSq = XMVectorGetX(distSqVec);

    // 衝突判定
    if (distSq < sumRadii * sumRadii)
    {
        float dist = sqrtf(distSq);
        if (dist < 0.0001f) return ret; // 重なりすぎ防止

        float overlap = (sumRadii - dist) * 1.1f;
        XMVECTOR pushDir = XMVector3Normalize(diff * -1.0f); // p1を押し戻す方向

        // --- 範囲攻撃かどうかの分岐をここに入れる ---
        //if (p1->IsAttacking()) {
        //	// 攻撃中なら敵(p2)を吹っ飛ばす！
        //	XMVECTOR knockbackDir = XMVector3Normalize(diff);
        //	p2->GetRigidbody().AddImpulse(knockbackDir * 25.0f);
        //	p2->ApplyDamage(10);
        //}
        //else 
        {
            // 通常時は位置を補正
            XMVECTOR pushVector = pushDir * overlap;
            p1->GetModel()->addPosition(&pushVector);
        }

        ret = X_Pos; // 戻り値は必要に応じて調整
    }
    return ret;
}


//==================================================
// --- 当たり判定（地面とキャラクター）
//==================================================
void Common::OnCollider(vnCharacter* pCharacter, vnModel* pGround, float footOffset, RigidbodyComponent& rigidBody)
{
    XMVECTOR LinePos = *pCharacter->getPosition();
    //LineDir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

    // --- モデルデータから内部情報を取得 ---
    int vnum = pGround->getVertexNum();	//頂点数を獲得
    int inum = pGround->getIndexNum();	//インデックス数

    //メッシュ単位で走査するため、メッシュデータを取得
    int meshNum = pGround->getMeshNum();
    vnModel_MeshData* pMesh = pGround->getMesh();

    vnVertex3D* vtx = pGround->getVertex();	//頂点配列
    unsigned short* idx = pGround->getIndex();	//インデックス配列
    //ワールドマトリクス
    XMMATRIX world = *pGround->getWorld();

    float highestY = -10000.0f; // 初期値は極端に低く
    int hitMeshID = -1;

    //地面
    vnCollide::stSegment seg;
    float safetyMargin = 0.2f;

    seg.Pos = *pCharacter->getPosition() + XMVectorSet(0, footOffset, 0, 0);
    seg.Dir = XMVectorSet(0, -1, 0, 0);
    seg.Length = footOffset + safetyMargin;

    for (int m = 0; m < meshNum; m++)
    {
        int m_inum = pMesh[m].IndexNum;
        int m_sidx = pMesh[m].StartIndex;

        //for(int i=sidx;i<sidx+inum;i+=3)
        for (int i = 0; i < m_inum; i += 3)
        {
            XMVECTOR v1 = XMVector3TransformCoord(
                XMVectorSet(vtx[idx[m_sidx + i + 0]].x,
                    vtx[idx[m_sidx + i + 0]].y,
                    vtx[idx[m_sidx + i + 0]].z, 0.0f),
                world);

            XMVECTOR v2 = XMVector3TransformCoord(
                XMVectorSet(vtx[idx[m_sidx + i + 1]].x,
                    vtx[idx[m_sidx + i + 1]].y,
                    vtx[idx[m_sidx + i + 1]].z, 0.0f),
                world);

            XMVECTOR v3 = XMVector3TransformCoord(
                XMVectorSet(vtx[idx[m_sidx + i + 2]].x,
                    vtx[idx[m_sidx + i + 2]].y,
                    vtx[idx[m_sidx + i + 2]].z, 0.0f),
                world);

            // ここで vnCollide 用の三角形を作る
            vnCollide::stTriangle tri;
            tri.fromPoints(&v1, &v2, &v3);

            // ここで Segment と当てる
            XMVECTOR hit;
            if (vnCollide::isCollide(&hit, &seg, &tri))
            {
                float y = XMVectorGetY(hit);
                if (y > highestY)
                {
                    highestY = y;
                }
            }
        }
    }

    if (highestY > -10000.0f)
    {
        rigidBody.SetVerticalVelocity(0.0f);
        rigidBody.SetIsGround(true);
        rigidBody.SetIsUseGravity(false);

        pCharacter->setPositionY(highestY + GROUND_OFFSET);
    }
    else {
        rigidBody.SetIsUseGravity(true);
        rigidBody.SetIsGround(false);

    }


}


#pragma endregion




