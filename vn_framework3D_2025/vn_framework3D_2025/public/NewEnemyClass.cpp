#include"../framework.h"
#include"../framework/vn_environment.h"

#include <random>    // random_device, mt19937 用
#include <algorithm> // shuffle 用
namespace
{
    // --- モデル・スケール関連 ---
    constexpr float ghostSize = 2.5f;

    // --- 状態遷移の確率 (0.0f ~ 1.0f) ---
    constexpr float chargeProbability = 0.3f; // 特攻になる確率
    constexpr float wallJumpChance = 0.3f; // 壁でジャンプする確率

    // --- ノックバック演出 (回転分岐) ---
    constexpr float rotationThreshold1 = 0.33f;
    constexpr float rotationThreshold2 = 0.66f;

    // --- 物理・移動パラメータ ---
    constexpr float wallJumpPower = 18.0f;  //ジャンプ力 
    constexpr float fallLimitY = -10.0f;    //リスポーンする保険の高さ

    // --- 特攻時の速度アップ ---
    constexpr float chargeMultiplierOffset = 2.0f;


    // --- ノックバック詳細設定 ---
    constexpr float knockbackDist = 100.0f;     //飛ぶ距離
    constexpr float knockbackDuration = 2.0f;   //飛ぶ時間
    constexpr float knockbackRandomMin = 1.8f;  //吹っ飛ぶ高さの最低値
    constexpr float knockbackRandomMax = 5.2f;  //吹っ飛ぶ高さの最高値
    constexpr float knockbackRotationX = 0.5f;  //X：回転する時の値
    constexpr float knockbackRotationY = 0.3f;  //Y：回転する時の値
    constexpr float knockbackRotationZ = 0.3f;  //Z：回転する時の値

    // --- 吸引(Attracted)設定 ---
    constexpr float attractMinDist     = -0.5f;     //吸収時の距離（0より小さくして目の前で止まるのをなるべくなくす）

    // --- リーダー・群れ設定 ---
    constexpr float leaderScaleMultiplier   = 2.0f;     // リーダーの大きさの倍率
    constexpr float leaderSpeedBoost        = 4.0f;     // リーダー専用速度
    constexpr float leaderSenseRadius       = 10.0f;    // プレイヤーを感知して逃げ始める距離
    constexpr float leaderStopRetreatRadius = 20.0f;    // プレイヤーから十分に離れて逃げやめる距離
    constexpr float leaderRepelRadius       = 20.0f;    // リーダー同士が重ならないように反発する距離（排他範囲）

    constexpr float followerStopDistBase    = 5.0f;    // リーダーに対して停止する基本距離
    constexpr float followerSpeedBoost      = 1.5f;    // 一般個体の速度倍率
    constexpr float followerSearchRadius    = 100.0f;  // リーダーを探し出す索敵範囲
    constexpr int   stopDistRandomRange     = 50;      // 停止距離にバラつきを出すための乱数範囲（0～4.9f）

    // --- ボス設定（基本大きさのみ(その他はリーダーと同じ)）---
    constexpr float bossScaleMultiplier     = 4.0f;


    // --- 速度調整 ---
    constexpr float speedAdjustmentRate     = 0.5f;    // 外部（WaveManager等）から指定される速度に対する補正係数
}

NewEnemyClass::NewEnemyClass()
    : CharacterBase(),
      m_isUnlocked(false),
      m_type(EnemyType::NONE)
{
    m_kbData = {};


}

NewEnemyClass::~NewEnemyClass()
{

}


// static変数の実体化
const NewEnemyClass::EnemyData NewEnemyClass::MasterTable[] = {
    { EnemyType::GHOST,    L"data/model/Ghost2/",       L"Ghost.bone",    300/*150/*200*/, ghostSize, {ghostSize, ghostSize, ghostSize} },
    //{ EnemyType::MUSHROOM, L"data/model/MushroomMon/", L"MushroomMon.bone", 0 , 1.5f, {1.0f, 1.0f,1.0f}},
    //{ EnemyType::MUSHROOM, L"data/model/MushroomMonster/", L"MushroomMonster.bone", 150 , 5.0f, {1.0f, 1.0f, 1.0f}},
};


std::vector<NewEnemyClass::GroupColorData> NewEnemyClass::m_availableColors;
//色の配列の初期化
std::vector<NewEnemyClass::GroupColorData> NewEnemyClass::g_LeaderColorPalette =
{
    { 1,  V_GAME_COLOR_RED,       L"赤" },
    { 2,  V_GAME_COLOR_GREEN,     L"緑" },
    { 3,  V_GAME_COLOR_BLUE,      L"青" },
    { 4,  V_GAME_COLOR_YELLOW,    L"黄" },
    { 5,  V_GAME_COLOR_OLIVE,     L"オリーブ" },
    { 6,  V_GAME_COLOR_CYAN,      L"シアン" },
    { 7,  V_GAME_COLOR_ORANGE,    L"オレンジ" },
    { 8,  V_GAME_COLOR_PURPLE,    L"紫" },
    { 9,  V_GAME_COLOR_BROWN,     L"茶色" },
    { 10, V_GAME_COLOR_PINK,      L"ピンク" },
    { 11, V_GAME_COLOR_LIME,      L"ライム" },
    { 12, V_GAME_COLOR_NAVY,      L"紺" },
    { 13, V_GAME_COLOR_TEAL,      L"ティール" },
    { 14, V_GAME_COLOR_GOLD,      L"金色" },
    { 15, V_GAME_COLOR_MINT,      L"ミント" },
    { 16, V_GAME_COLOR_DARK_GRAY, L"ダークグレー" },
    { 17, V_GAME_COLOR_DEEP_BLACK,L"ブラック" },
    { 18, V_GAME_COLOR_EMERALD,   L"エメラルド" },
    { 19, V_GAME_COLOR_CORAL,     L"コーラル" },
    { 20, V_GAME_COLOR_WINE,      L"ワインレッド" } 
};


NewEnemyClass::GroupColorData NewEnemyClass::GetRandomGroupData()
{
    //リストが空になったら補充
    if (m_availableColors.empty())
    {
        m_availableColors = g_LeaderColorPalette;
        //シャッフルして毎回違う順番にする
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(m_availableColors.begin(), m_availableColors.end(), g);
    }
    //末尾から取得して削除
    GroupColorData res = m_availableColors.back();
    m_availableColors.pop_back();
    
    return res;
}


// データの個数を計算
const int NewEnemyClass::MasterTableCount = sizeof(NewEnemyClass::MasterTable) / sizeof(NewEnemyClass::EnemyData);


// --- 敵の種類によってクラスを返す ---
NewEnemyClass* NewEnemyClass::CreateEnemyByType(EnemyType type)
{
    switch (type) {
    case EnemyType::GHOST:    return new EnemyGhost();
    case EnemyType::MUSHROOM: return new EnemyMushroomMon();
    default:                  return nullptr;
    }
}

void NewEnemyClass::Spawn(const XMVECTOR& pos)
{
    //if (isActive)return;
    GetModel()->setRenderEnable(true);
    
    for (int i = 0; i < GetModel()->getPartsNum(); i++)
    {
        GetModel()->getParts(i)->setRenderEnable(true);
    }

    GetModel()->setPosition(&pos);
    GetModel()->setRotation(0, 0, 0);
    m_kbData.active = false;
    m_isActive = true;
    auto& rb = GetRigidbody();
    rb.SetVerticalVelocity(0.0f);
    rb.SetIsGround(false);
    rb.SetIsUseGravity(true);
    m_state = eState::Idel; // 待機状態に戻す
    m_isCharge = false;


    // --- 調整用変数初期化(不具合防止用) ---
    m_isPullChecked = false;    //引き寄せで全員が同じ判定にならないようにする

    if (GetIsLeader())
    {
        GroupData* data = GetGroupData();
        //範囲に入って逃げるを解除
        data->isLeaderEscaping = false;
        data->isLeaderAlive = true;
        m_onceStartUI = false;
        CheckEvolutionOnSpawn();
        data->mode= eGroupMode::Normal;
        data->oldMode= eGroupMode::Normal;

    }
    else
    {
        m_panicRecoveryStartTime = 0.5f;
    }

    SetMark(m_pChargeMark, false);
    SetMark(m_pPanicMark, false);



}

void NewEnemyClass::DeSpawn()
{
    m_isActive = false;
    GetModel()->setPosition(0, -100, 0);

    m_kbData.active = false;
    GetModel()->setRenderEnable(false);
    for (int i = 0; i < GetModel()->getPartsNum(); i++)
    {
        GetModel()->getParts(i)->setRenderEnable(false);
    }
    m_state = eState::Idel;
    m_panicDirTimer = 0;
    m_panicRecoveryTime = 0;

    if (GetIsLeader())
    {
        GroupData* data = GetGroupData();
        data->isLeaderAlive = false;
    }
    //m_pPanicMark->setPosition(0,-500,0);
    //m_pChargeMark->setPosition(0,-500,0);

    SetMark(m_pChargeMark, false);
    SetMark(m_pPanicMark, false);




}

//頭の上にマークを表示する
void NewEnemyClass::SetMark(vnSprite* sprite, bool isVisible)
{
    float x, y;
    XMVECTOR pos = XMVectorSet(
        GetModel()->getPositionX() + 0.5f,
        GetModel()->getPositionY() + 2.0f * (GetModel()->getScaleY() / 2),
        GetModel()->getPositionZ(),
        1.0f
    );

    if (vnFont::CalculateScreenPosition(pos, &x, &y))
    {
        sprite->setPos(x, y);
    }
    sprite->setRenderEnable(isVisible);
}

void NewEnemyClass::ReStartEnemy()
{
    m_pGroupData = nullptr;
}



void NewEnemyClass::Update(float deltaTime)
{
    if (!m_isActive) return;

    // --- 共通計算（ここで1回だけやる） ---
    XMVECTOR enemyPos = *GetModel()->getPosition();
    XMVECTOR toPlayer = *m_pPlayer->GetModel()->getPosition() - enemyPos; // 敵からプレイヤーへの方向
    float distance = XMVectorGetX(XMVector3Length(toPlayer));
    // 【最適化！】ルート計算をしない「距離の2乗」を求める
    //float distanceSq = XMVectorGetX(XMVector3LengthSq(toPlayer));

    EnemyPool::GetInstance().GetGroupData(GetGroupID());

    // マーク更新
    UpdateMark();

    // --- 状態(子クラスに任せる) ---
    UpdateState(deltaTime, distance, toPlayer);

    //プレイヤーと当たったらノックバック状態
    UpdateHitPlayer();

    //XMVECTOR ropecenter = XMVectorAdd(*GetModel()->getPosition(), GetCollision().GetCenter());
    //vnDebugDraw::Sphere(ropecenter, GetEffectiveRadius(), GAME_COLOR_LIME);
    // --- 学習行動 ---
    UpdateEnemyMessage(deltaTime);

    //物理更新
    UpdatePhysics(deltaTime);


    //待機状態以外で出てくるのを防ぐ用
    m_panicRecoveryStartTime -= deltaTime;
}
//============================================================
// 頭上にあるマークの表示切り替え
//============================================================
void NewEnemyClass::UpdateMark()
{
    switch (m_state)
    {
    case Run:
    case Panic:
        SetMark(m_pPanicMark, true);
        SetMark(m_pChargeMark, false);
        break;

    case Charge:
        SetMark(m_pPanicMark, false);
        SetMark(m_pChargeMark, true);
        break;

    default:
        SetMark(m_pPanicMark, false);
        SetMark(m_pChargeMark, false);
        break;
    }
}

//状態の更新
void NewEnemyClass::UpdateState(
    float deltaTime,
    float distance,
    const XMVECTOR& toPlayer)
{
    switch (m_state)
    {
    case Idel:
        if (GetIsLeader() && !m_onceStartUI)
        {
            m_isSpwanStart = true;
        }
        OnIdel(deltaTime, distance, toPlayer);
        break;

    case Run:
        OnRun(deltaTime, distance, toPlayer);
        break;

    case KnockBack:
        UpdateKnockback(deltaTime);
        break;

    case Dead:
        OnDead();
        break;

    case Attracted:
        UpdateAttracted(deltaTime, toPlayer, distance);
        break;

    case Follow:
        OnFollow(deltaTime);
        break;

    case Panic:
        OnPanic(deltaTime);
        break;

    case Charge:
        OnCharge(deltaTime, toPlayer);
        break;

    case Patrol:
        OnPatrol(deltaTime, distance);
        break;
    }
}

// プレイヤーと当たった時にノックバック状態にする
void NewEnemyClass::UpdateHitPlayer()
{
    // --- playerと当たった時
    if (m_isHitPlayer && m_state != eState::KnockBack)
    {

        GetRigidbody().SetBaseVelocity(XMVectorZero());
        XMVECTOR dir = *GetModel()->getPosition() - *m_pPlayer->GetModel()->getPosition();
        m_randomKnockNum = knockbackRandomMin + (rand() / (float)RAND_MAX) * (knockbackRandomMax - knockbackRandomMin);

        StartKnockback(dir, knockbackDist, knockbackDuration, m_baseKnockSpeed * m_randomKnockNum); // durationは秒で

        if (GetIsLeader())
        {
            GroupData* data = GetGroupData();
            float finalChargeProbability = chargeProbability + data->meleeFear;
            //リーダーがいなくなった時、パニック状態か特攻状態に変化する
            if (GetRandomFloat() < finalChargeProbability)
            {
                data->mode = eGroupMode::Charge;

            }
            else
            {

                data->mode = eGroupMode::Panic;
            }
        }

        m_state = eState::KnockBack;
    }
}


//物理更新
void NewEnemyClass::UpdatePhysics(float deltaTime)
{
    //障害物が目の前に来たらジャンプ
    Jump();
    //物理更新
    GetRigidbody().Update(deltaTime);
    //伸び縮みのアニメーション
    UpdateSquashAndStretch(deltaTime);

    // --- 保険 --- 一定範囲（高さ（下））を超えたら戻す
    if (GetModel()->getPositionY() < fallLimitY) {
        //一定以上落下したら
        DeSpawn();
    }
}

void NewEnemyClass::ChangeSpeed(float speed)
{

    m_waveBoostSpeedMultiplier = speedAdjustmentRate * speed;
}

// ---------------------------------------------------------------
// 引き寄せ攻撃されたときの処理
// ---------------------------------------------------------------

void NewEnemyClass::UpdateAttracted(float deltaTime, XMVECTOR toPlayer, float distance)
{

    if (distance > attractMinDist)
    {
        XMVECTOR dir = XMVector3Normalize(toPlayer);
        XMVECTOR nextPos = *GetModel()->getPosition() + (dir * m_pullSpeed * deltaTime);

        GetModel()->setPosition(&nextPos);
        //GetRigidbody().setPosition(%nextPos); // Rigidbodyの中身も動かしておく
        GetRigidbody().SetBaseVelocity(XMVectorZero()); // 変な慣性を消す
    }

}
// ---------------------------------------------------------------------------------
//   引き寄せ状態に入ったかどうか
// ---------------------------------------------------------------------------------

void NewEnemyClass::CheckPullTrigger(bool isPlayerPulling, float pullRadius, float distance)
{
    // 状態チェック：死亡、ノックバック、または既に引き寄せ中なら早期リターン
    if (m_state == Dead || m_state == KnockBack || m_state == Attracted) return;

    // 引き寄せ範囲外、またはプレイヤーが引いていない場合
    if (!isPlayerPulling || distance > pullRadius)
    {
        m_isPullChecked = false; // 判定フラグをリセット
        return;
    }

    // --- ここから「引き寄せ範囲内」かつ「まだ引き寄せられていない」時の処理 ---

    // 既にこの回の引き寄せで判定済みなら何もしない
    if (m_isPullChecked) return;
    m_isPullChecked = true;

    // 1. 学習データの獲得（リーダーか自分自身から取得）
    GroupData* pData = nullptr;
    if (GetIsLeader())
    {
        pData = GetGroupData();
    }
    else if (m_pMyLeader)
    {
        pData = m_pMyLeader->GetGroupData();
    }

    // 2. データの存在チェックと確率判定
    if (pData)
    {
        float resistance = pData->pullResistance;    //テスト用で0.8低いとほぼ誤差
        if (resistance > 0)
        {
            resistance += m_individualPullResist;
        }
        if (GetRandomFloat() < resistance)
        {
            // 1. 前フレームから状態が変わった瞬間を検知（エッジトリガー）

            if (m_isPullChecked)
            {
                // 状態が変わった時だけタイマーをリセット！
                m_aiDebugText.Reset(m_defaultTextTime);
            }

            if (resistance < 1.0f)
            {
                // 抵抗成功：状態を変えずに終了
                m_pullAtkMessage.SetState(eShowUISelect::Text1);

                return;
            }
            else
            {
                // 抵抗成功：状態を変えずに終了
                m_pullAtkMessage.SetState(eShowUISelect::Text2);
                return;
            }
        }
    }
    else
    {
        m_pullAtkMessage.SetState(eShowUISelect::Text3);
    }

    // 3. 判定失敗、またはデータなしの場合は引き寄せ状態へ
    m_state = Attracted;
    m_pullAtkMessage.SetState(eShowUISelect::Text3);

}

// ---------------------------------------------------------------








// --------------------------------------------------------
//  ノックバック処理
// --------------------------------------------------------
void NewEnemyClass::StartKnockback(
    const XMVECTOR& dir,
    float dist,
    float time,
    float height)
{
    m_kbData.active = true;
    m_kbData.timer = 0.0f;
    m_kbData.duration = time;
    m_kbData.startPos = *GetModel()->getPosition();   // 現在位置
    m_kbData.direction = XMVector3Normalize(dir);
    m_kbData.distance = dist;
    m_kbData.height = height;

    float startY = XMVectorGetY(*GetModel()->getPosition());
    m_targetHeight = startY + height;

    m_isRising = true;
    m_knockbackRotationPatternRnd = GetRandomFloat(); // 0.0 ～ 1.0

    //スケールを戻す
    GetModel()->setScale(&m_defaultScale);
}


void NewEnemyClass::UpdateKnockback(float deltaTime)
{
    if (!m_kbData.active)
    {
        m_state = Idel; 
        return;
    }

    // --- 回転のバリエーション ---
    if (m_knockbackRotationPatternRnd < rotationThreshold1)
    {
        // パターン1：横にきりもみ回転
        GetModel()->addRotationY(knockbackRotationY);
        //GetModel()->addRotationZ(knockbackRotationZ);
    }
    else if (m_knockbackRotationPatternRnd < rotationThreshold2)
    {
        // パターン2：縦にバク転気味に回転
        GetModel()->addRotationX(knockbackRotationX);
        //GetModel()->addRotationY(knockbackRotationY);
    }
    else
    {
        // パターン3：全軸でぐちゃぐちゃに回転（今の設定に近い）
        //GetModel()->addRotationX(knockbackRotationX);
        //GetModel()->addRotationY(knockbackRotationY);
        GetModel()->addRotationZ(knockbackRotationZ);
    }

    GetModel()->execute(2, true, true);
    m_isHitPlayer = false;

    // --- 上に飛ぶ ---
    // 現在Y取得
    float currentY = XMVectorGetY(*GetModel()->getPosition());
    // 上昇中
    if (m_isRising)
    {
        if (currentY >= m_targetHeight)
        {
            m_isRising = false;
            GetRigidbody().SetVerticalVelocity(-m_baseKnockSpeed/2 * m_randomKnockNum); // 落下速度（固定）
        }
        else {
            GetRigidbody().SetVerticalVelocity(m_baseKnockSpeed * m_randomKnockNum); // 上昇速度（固定）
        }
    }


    // --- 横に飛ぶ処理 ---
    // 時間進行 
    m_kbData.timer += deltaTime;
    float t = m_kbData.timer / m_kbData.duration/2;
    if (t > 1.0f) t = 1.0f;

    // 1 → 0 に落ちる係数（減速）
    float decel = 1.0f - t;

    // 初速（距離 ÷ 時間）
    float speed = m_kbData.distance / m_kbData.duration;

    // 速度として与える（dtは絶対に掛けない）
    XMVECTOR velocity =
        m_kbData.direction * speed * decel;

    GetRigidbody().AddExternalVelocity(velocity);


    // =========================
    // 終了
    // =========================
    if (m_kbData.timer >= m_kbData.duration)
    {
        m_kbData.active = false;
        m_kbData.timer = 0.0f;
        m_isEscaping = false;
        GetModel()->setRotationX(0);
        DeSpawn();
        m_state = Dead;

    }

}

// ---------------------------------------------------------


//InPlayerArea(a,b)a以内に入ったら逃走開始、bより遠くへ行ったら止まる
bool NewEnemyClass::InPlayerArea(float escapeStartDist,float escapeStopDist)//逃げ始める距離、やめる距離
{
    if (!m_pPlayer || !GetModel()) return false;

    XMVECTOR enemyPos = *GetModel()->getPosition();
    XMVECTOR diff = enemyPos - *m_pPlayer->GetModel()->getPosition();

    float distance =
        XMVectorGetX(XMVector3Length(diff));

    //--１.逃走中
    if (m_isEscaping)
    {
        if (distance > escapeStopDist)
        {
            m_isEscaping = false;
        }
    }
    //--２.待機中
    else
    {
        if (distance < escapeStartDist)
        {

            m_isEscaping = true;
        }
    }

    return m_isEscaping;
}


// 回転・速度・移動
void NewEnemyClass::ApplyMovement(float deltaTime, const XMVECTOR moveDir)
{

    //向き
    if (!XMVector3Equal(moveDir, XMVectorZero()))
    {
        float rotY = atan2f(XMVectorGetX(moveDir), XMVectorGetZ(moveDir));
        GetModel()->setRotationY(rotY);

    }

    // 速度計算
    // 1. 基礎速度の計算（WAVE補正など）
    float finalSpeed = m_baseMoveSpeed * m_boostSpeedMultiplier * m_waveBoostSpeedMultiplier;

    // 2. 所属グループの学習データ(Fear)を取得
    // リーダーなら自分、部下なら m_pMyLeader からデータを取る
    GroupData* data = GetIsLeader() ? GetGroupData() : (m_pMyLeader ? m_pMyLeader->GetGroupData() : nullptr);

#pragma region 範囲攻撃耐性が逃げ始める範囲
    //// 学習データがない（単独個体など）場合の安全策
    //float meleeFear = data ? data->meleeFear : 0.0f;
    //float rangeFear = data ? data->rangeFear : 0.0f;

    //// 3. 役割（リーダー/部下）に応じたベース倍率の決定
    //float roleMultiplier = GetIsLeader() ? m_leaderSpeedMultiplier : m_otherSpeedMultiplier;

    //// 学習データ(近接への恐怖)をベース倍率に加算
    //roleMultiplier += meleeFear;

    //// 4. 状況に応じた追加ブースト（パニック等）の計算
    //float situationBoost = 1.0f;
    //if (data /*&& rangeFear > 0.0f && GetPlayer()->IsAreaAttack()*/)
    //{
    //    if (data->isLeaderEscaping)
    //    {
    //        // リーダー逃走中かつ範囲攻撃中なら大幅加速
    //        situationBoost = GetIsLeader() ? 3.0f : 3.0f;
    //    }
    //    else if (GetIsLeader())
    //    {
    //        // リーダーが逃げていないが範囲攻撃中のリーダー専用処理
    //        roleMultiplier = m_defalutLeaderSpeedMultiplier + meleeFear;
    //    }
    //}

#pragma endregion

#pragma region 範囲攻撃耐性が基礎速度アップ
    // 学習データをもとに足す基礎速度を取る
    float rangeFear = data ? data->rangeFear : 0.0f;

    //群のそれぞれに応じたベース倍率の決定
    float roleMultiplier = GetIsLeader() ? m_leaderSpeedMultiplier : m_otherSpeedMultiplier;

    // 2. 範囲攻撃の学習データ（基礎速度アップ）をそのまま加算
    roleMultiplier += rangeFear;

    float chargeMultiplier = GetState() == eState::Charge ? chargeMultiplierOffset : 0;

    roleMultiplier += chargeMultiplier;
#pragma endregion


    // 5. 最終速度の適用
    finalSpeed *= (roleMultiplier);
    //finalSpeed *= (roleMultiplier * situationBoost);
    //速度適応
    GetRigidbody().SetBaseVelocity(moveDir * finalSpeed);

}

// --- ジャンプ処理（移動中にブロックが目の前にある時に発動）---
void NewEnemyClass::Jump()
{
    // 壁に当たっている 且つ まだジャンプしていないなら
    if (GetWallHit() && !m_isJump)
    {
        m_isJump = true;
        GetRigidbody().SetIsGround(false);
        GetRigidbody().AddVerticalVelocity(wallJumpPower); // 18.0fは結構高いので調整してください

        // フラグをリセットしておかないと、空中で何度もジャンプしようとする可能性がある
        SetWallHit(false);
    }
    //地面にいたらジャンプ可能にする（重力無効）
    if (GetRigidbody().GetIsGround())
    {
        if (m_isJump)
        {
            m_isJump = false;
            GetRigidbody().SetVerticalVelocity(0.0f);
            GetRigidbody().SetIsUseGravity(false);
        }
    }
    else
    {
        GetRigidbody().SetIsUseGravity(true);
    }
}

// ------------------------------------------------------
//   プレイヤーの範囲攻撃の範囲外に出る(使用していない)
// ------------------------------------------------------
void NewEnemyClass::EscapeAreaAttack()
{
    if (!GetIsLeader()) return;
    GroupData* pData = GetGroupData();
    if (!pData) return;
    
    // プレイヤーが攻撃中かどうかに関わらず、
    // 「プレイヤーの周囲 R メートル」を常に危険地帯とみなす
    // 学習が進む（rangeFearが上がる）ほど、その半径が広がる
    
    float escapeStartDist = m_pPlayer->GetAreaAttackRadius() + pData->rangeFear; // 基礎半径 + 警戒心
    float escapeStopDist = escapeStartDist + 2.0f;
    
    // 常に判定を回しておく
    pData->isLeaderEscaping = InPlayerArea(escapeStartDist, escapeStopDist);

    if (pData->isLeaderEscaping)
    {
        m_leaderSpeedMultiplier *= 2.0f;

    }
    else
    {
        m_leaderSpeedMultiplier = m_defalutLeaderSpeedMultiplier;
    }
}



// ------------------------------------------------------
//  伸び縮みするアニメーション
// ------------------------------------------------------
void NewEnemyClass::UpdateSquashAndStretch(float deltaTime)
{
    //時間を進める
    //m_livingTime += deltaTime*2;
    //if (m_state != eState::KnockBack)
    //{
    //    //sin(累計時間*速さ)*強さ
    //    float wave = sinf(m_livingTime * m_animSpeed);

    //    GetModel()->setScaleY(XMVectorGetY(m_defaultScale)+ (wave * m_animMagnitude));
    //}

    m_livingTime += (deltaTime*0.8f);
    if (m_state != eState::KnockBack)
    {
        float wave = sinf(m_livingTime * m_animSpeed);

        //伸縮の強さ（ここを大きくすればするほど、極端にビヨンビヨンします）
        float stretchAmount = m_animMagnitude * 3.0f;

        // デフォルトのスケールを取得
        float defaultX = XMVectorGetX(m_defaultScale);
        float defaultY = XMVectorGetY(m_defaultScale);
        float defaultZ = XMVectorGetZ(m_defaultScale);

        // Yが伸びるとき（+）、XとZは縮む（-）
        float newScaleY = defaultY + (wave * stretchAmount);
        float newScaleX = defaultX - (wave * stretchAmount * 0.5f); // 横の縮み具合は少し抑えめに調整
        float newScaleZ = defaultZ - (wave * stretchAmount * 0.5f);

        // モデルに反映（※もし setScale(x, y, z) のような一括設定関数があればそれを使うとスマートです）
        GetModel()->setScale(newScaleX, newScaleY, newScaleZ);

    }
}

// -------------------------------------------------------
//  敵の設定
// -------------------------------------------------------

// --- リーダー設定 ---
void NewEnemyClass::SettingLeader(GroupData* groupData)
{
    // 群データ保持
    m_pGroupData = groupData;

    //リーダーにする
    m_isLeader = true;

    //最初の色を保存しておく
    m_defaultLeaderColor = GetModel()->GetAllPartsDiffuse();

    // 10.0f 〜 11.0f の間で、0.01刻みの細かい個体差を出す場合
    m_animSpeed = 10.0f + (rand() % 101) / 100.0f;

    //リーダー同士の距離設定
    m_leaderSeparateRadius = leaderRepelRadius;

    //プレイヤーを感知する距離
    m_leaderEscapeRadius = leaderSenseRadius;

    //プレイヤーからの止まる距離
    m_leaderRetreatStopRadius = leaderStopRetreatRadius;

    //少し速度をあげる
    m_leaderSpeedMultiplier = leaderSpeedBoost;
    m_defalutLeaderSpeedMultiplier = m_leaderSpeedMultiplier;

    //少しモデルのサイズと当たり判定を大きくする
    XMVECTOR leaderSize = *GetModel()->getScale();
    leaderSize = leaderSize * leaderScaleMultiplier;
    GetModel()->setScale(&leaderSize);

    XMVECTOR leaderSizeCol = GetCollision().GetSize();
    leaderSizeCol = leaderSizeCol * leaderScaleMultiplier;
    GetCollision().SetSize(leaderSizeCol);

    //最初の大きさを保存しておく
    m_defaultScale = *GetModel()->getScale();

    // 中心点は高さ(Y)の半分に設定
    float centerY = XMVectorGetY(GetCollision().GetSize()) / 2.0f;
    GetCollision().SetCenter(XMVectorSet(0, centerY, 0, 0));


    //色を変える
    //GetModel()->SetAllPartsDiffuse(V_GAME_COLOR_RED, 1.0f);
    GetModel()->SetAllPartsDiffuse(groupData->color, 1.0f);
    SetGroupID(groupData->id);

}

// --- リーダー以外の敵（通常個体）の設定 ---
void NewEnemyClass::SettingOther()
{
    //リーダーではない
    m_isLeader = false;

    //最初の色を保存しておく
    m_defaultOtherColor = GetModel()->GetAllPartsDiffuse();

    //最初の大きさを保存しておく
    m_defaultScale = *GetModel()->getScale();

    //リーダーとの基本停止距離
    m_baseFollowDist = followerStopDistBase;

    // 10.0f 〜 11.0f の間で、0.01刻みの細かい個体差を出す場合
    m_animSpeed = 20.0f + (rand() % 101) / 100.0f;


    //個体ごとのばらつき距離
    /*
    1. rand() % 5 の場合
    この書き方だと、得られる整数は 0, 1, 2, 3, 4 の 5種類だけ になります。
    これをそのまま使ったり、あるいは 5 を足したりしても、
    結果は 5.0, 6.0, 7.0, 8.0, 9.0 といった具合に、カチカチとした整数値しか出てきません。

    2. rand() % 50 / 10.0f の場合
    50 で割る余りを使うことで、内部的には 0, 1, 2, ... 49 という 50種類 の数値を持っています。
    これを 10 で割ることで、以下のような「小数点第1位」までのバリエーションが生まれます。
    0.1, 0.2, 0.3 ... 4.8, 4.9*/

    m_myStopDist = static_cast<float>(rand() % stopDistRandomRange) / 10;

    // --- 引き寄せ攻撃への抵抗値 ---
    m_individualPullResist = static_cast<float>(rand() % 41) / 100.0f;

    //リーダーを探す範囲
    m_searchLeaderRadius = followerSearchRadius;

    //通常個体の速度設定
    float speedVariance = (90 + (rand() % 21)) / 100.0f;
    m_otherSpeedMultiplier *= speedVariance;
    //m_otherSpeedMultiplier = followerSpeedBoost;

    m_defalutOtherSpeedMultiplier = m_otherSpeedMultiplier;
}

// --- ボスのセッティング ---
void NewEnemyClass::SettingBoss(GroupData* groupData)
{
    m_pGroupData = groupData;
    m_isLeader = true;
    m_isBoss = true;
    //最初の色を保存しておく
    m_defaultLeaderColor = GetModel()->GetAllPartsDiffuse();

    // 10.0f 〜 11.0f の間で、0.01刻みの細かい個体差を出す場合
    m_animSpeed = 10.0f + (rand() % 101) / 100.0f;

    //リーダー同士の距離設定
    m_leaderSeparateRadius = leaderRepelRadius;

    //プレイヤーを感知する距離
    m_leaderEscapeRadius = leaderSenseRadius;

    //プレイヤーからの止まる距離
    m_leaderRetreatStopRadius = leaderStopRetreatRadius;

    //少し速度をあげる
    m_leaderSpeedMultiplier = 5.0f;
    m_defalutLeaderSpeedMultiplier = m_leaderSpeedMultiplier;

    //少しモデルのサイズと当たり判定を大きくする
    XMVECTOR leaderSize = *GetModel()->getScale();
    leaderSize = leaderSize * bossScaleMultiplier;
    GetModel()->setScale(&leaderSize);

    XMVECTOR leaderSizeCol = GetCollision().GetSize();
    leaderSizeCol = leaderSizeCol * bossScaleMultiplier;
    GetCollision().SetSize(leaderSizeCol);

    //最初の大きさを保存しておく
    m_defaultScale = *GetModel()->getScale();

    // 中心点は高さ(Y)の半分に設定
    float centerY = XMVectorGetY(GetCollision().GetSize()) / 2.0f;
    GetCollision().SetCenter(XMVectorSet(0, centerY, 0, 0));


    //色を変える
    //GetModel()->SetAllPartsDiffuse(V_GAME_COLOR_RED, 1.0f);
    GetModel()->SetAllPartsDiffuse(groupData->color, 1.0f);
    SetGroupID(groupData->id);

}


// ======================================================================
//  死因を取る
// ======================================================================
void NewEnemyClass::OnDie(DamageSource source)
{
    if (!GetGroupData() && !GetIsLeader() || !GetGroupData() && GetIsBoss())return;

    GroupData* data = GetGroupData();

    switch (source)
    {
    case DamageSource::Melee:   //近接で死亡(特攻確率アップ（0.0~1.0(100%)）)
        if(data->meleeFear<data->maxMeleeFear)
        data->meleeFear += 0.1f;
        break;
    case DamageSource::AreaAttack://範囲攻撃で(基礎速度アップ)
        if (data->rangeFear < data->maxRangeFear)
        data->rangeFear += 1.5f;
        break;
    case DamageSource::PullAttack://引き寄せ攻撃(引き寄せ無効確率)
        if (data->pullResistance < data->maxPullResistance)
        data->pullResistance += 0.2f;
        break;

    }
}







// -----------------------------------------------------------------------
//   敵のメッセージ表示
// -----------------------------------------------------------------------
void NewEnemyClass::UpdateEnemyMessage(float deltaTime)
{ 
    //フォントの設定
    vnFont::setFontSize(31, 25);


#pragma region その他の敵の範囲攻撃から逃げるメッセージ
    {
        if (!GetIsLeader() && m_pMyLeader)
        {
            GroupData* data = m_pMyLeader->GetGroupData();
            if (data)
            {
                // 1. 前フレームから状態が変わった瞬間を検知（エッジトリガー）
                bool isChanged = (data->isLeaderEscaping != m_wasLeaderEscaping);

                if (isChanged)
                {
                    // 状態が変わった時だけタイマーをリセット！
                    m_aiDebugText.Reset(m_defaultTextTime);
                }

                // 2. 現在の状態を保存しておく（次のフレームの比較用）
                m_wasLeaderEscaping = data->isLeaderEscaping;

                // --- 表示 ---
                if (data->isLeaderEscaping)
                {
                    m_areaAtkMessage.SetState(eShowUISelect::Text1);
                }
                else
                {

                }
            }
        }

    }

#pragma endregion

    //敵のコード内
    if (m_isSpwanStart)
    {
        m_upgradeTimer -= deltaTime ;
        float floatUp = (0.8f - m_upgradeTimer) * 2.0f; // 1秒かけて 1.0m 上に上がる
        // 表示位置に floatUp を足す
        XMVECTOR displayPos = *GetModel()->getPosition();
        displayPos = XMVectorSetY(displayPos, XMVectorGetY(displayPos) + 1.0f + floatUp);
        // ステップに応じて出す文字を変える
        if (m_upgradeStep < m_upgradeTexts.size())
        {
            EnemyAIDebug::ShowUpgrade(displayPos, m_upgradeTexts[m_upgradeStep].first.c_str(),m_upgradeTexts[m_upgradeStep].second);
        }


        // 1つ目の文字が上がりきったら（タイマーが0になったら）
        if (m_upgradeTimer <= 0.0f)
        {
            m_upgradeStep++;    // 次のステップへ

            // 全てのステップが終わったら終了
            if (m_upgradeStep<m_upgradeTexts.size()) // 全部出し終わったら
            {
                m_upgradeTimer = 3.0f;
            }
            else
            {
                m_isSpwanStart = false;
                m_upgradeStep = 0;
                m_onceStartUI = true;
                // 次回のためにタイマーなどは戻さない（またはリセット）
            }
        }

    }
    m_areaAtkMessage.Update(deltaTime);
    m_pullAtkMessage.Update(deltaTime);
    m_panicMessage.Update(deltaTime);
    m_chargeMessage.Update(deltaTime);
    m_patrolMessage.Update(deltaTime);
    m_followMessage.Update(deltaTime);
    m_runMessage.Update(deltaTime);

    ShowMessage();
}
//スポーン時の最初に何が強化されたか出す
void NewEnemyClass::CheckEvolutionOnSpawn()
{
    if (!GetGroupData())return;
    GroupData* data = GetGroupData();
    m_upgradeTexts.clear(); //リストをきれいにする


    //現在のモードを取得
    auto currentMode = EnemyPool::GetInstance().GetDisplayMode();
    bool shouldShow = false;
    switch (currentMode)
    {
    case EnemyPool::eDisplayMode::AllOff:      shouldShow = false; break;
    case EnemyPool::eDisplayMode::AllOn:       shouldShow = true; break;
    }


    //1.近接耐性が上がっていたらリストに追加
    if (data->meleeFear > data->oldMeleeFear)
    {
        m_upgradeTexts.push_back({L"「近接耐性アップ」", GAME_COLOR_RED});
        data->oldMeleeFear = data->meleeFear;   //データを更新
    }
    //2.範囲攻撃耐性
    if (data->rangeFear > data->oldRangeFear)
    {
        m_upgradeTexts.push_back({ L"「範囲攻撃耐性アップ」", GAME_COLOR_BLUE });
        data->oldRangeFear = data->rangeFear;
    }
    //3.引き寄せ耐性
    if (data->pullResistance > data->oldPullResistance)
    {
        m_upgradeTexts.push_back({ L"「引き寄せ耐性アップ」", GAME_COLOR_GREEN });
        data->oldPullResistance = data->pullResistance;
    }
    if (!shouldShow)return;

    //進化した項目が１つでもあれば、表示する
    if (!m_upgradeTexts.empty())
    {
        m_isSpwanStart = true;
        m_upgradeStep = 0;
        m_upgradeTimer = 3.0f;
    }

}

void NewEnemyClass::ShowMessage()
{
    //全体のモードを確認し、出すべきでないなら終了

    bool isLeader = GetIsLeader();

    //現在のモードを取得
    auto currentMode = EnemyPool::GetInstance().GetDisplayMode();

    bool shouldShow = false;
    vnFont::setFontSize(31, 22);

    switch (currentMode)
    {
    case EnemyPool::eDisplayMode::AllOff:      shouldShow = false; break;
    case EnemyPool::eDisplayMode::LeaderOnly:  shouldShow = isLeader; break;
    case EnemyPool::eDisplayMode::OthersOnly:  shouldShow = !isLeader; break;
    case EnemyPool::eDisplayMode::AllOn:       shouldShow = true; break;
    }

    if (!shouldShow)return;
    // 1. 表示すべきメッセージ情報を保持する構造体
    struct MsgInfo { std::wstring text; unsigned int color; };
    MsgInfo areaMsg = { L"", 0 };       //範囲攻撃に関するメッセージ
    MsgInfo pullMsg = { L"", 0 };       //引き寄せ攻撃
    MsgInfo panicMsg = { L"", 0 };      //パニック状態
    MsgInfo chargeMsg = { L"", 0 };     //特攻状態
    MsgInfo patrolMsg = { L"", 0 };     //パトロール中
    MsgInfo followMsg = { L"", 0 };     //リーダー追跡中
    MsgInfo runMsg = { L"", 0 };        //リーダー逃走中



    // 2. 範囲攻撃メッセージのテキスト決定
    switch (m_areaAtkMessage.uiState) {
    case eShowUISelect::Text1:
        areaMsg = { isLeader ? L"射程範囲内" : L"逃",GAME_COLOR_NEON_MAGENTA };
        break;
    case eShowUISelect::Text2:
        areaMsg = { L"射程範囲外", GAME_COLOR_ELECTRIC_PURPLE };
        break;
    }

    // 3. 引き寄せ攻撃メッセージのテキスト決定
    switch (m_pullAtkMessage.uiState) {
    case eShowUISelect::Text1:
        pullMsg = { L"無効化",GAME_COLOR_AQUA_GREEN };
        break;
    case eShowUISelect::Text2:
        pullMsg = { L"完全抵抗",GAME_COLOR_AQUA_GREEN };
        break;
    case eShowUISelect::Text3:
        pullMsg = { L"成功",GAME_COLOR_ELECTRIC_CYAN };
        break;
    }

    // 4.パニック状態
    switch (m_panicMessage.uiState)
    {
    case eShowUISelect::Text1:
        panicMsg = { L"リーダー！", isLeader ? GAME_COLOR_YELLOW : GAME_COLOR_CYAN };
        break;
    case eShowUISelect::Text2:
        panicMsg = { L"リーダー探し中",isLeader ? GAME_COLOR_YELLOW : GAME_COLOR_CYAN };
        break;
    case eShowUISelect::Text3:
        panicMsg = { L"リーダー発見！",isLeader ? GAME_COLOR_YELLOW : GAME_COLOR_CYAN };
    }


    // 5.特攻状態
    switch (m_chargeMessage.uiState)
    {
    case eShowUISelect::Text1:
        chargeMsg = { L"特攻", isLeader ? GAME_COLOR_YELLOW : GAME_COLOR_CYAN };
        break;
    case eShowUISelect::Text2:
        chargeMsg = { L"",isLeader ? GAME_COLOR_YELLOW : GAME_COLOR_CYAN };
        break;
    case eShowUISelect::Text3:
        chargeMsg = { L"",isLeader ? GAME_COLOR_YELLOW : GAME_COLOR_CYAN };
    }

    // 6.
    switch (m_patrolMessage.uiState)
    {
    case eShowUISelect::Text1:
        patrolMsg = { L"パトロール中～",GAME_COLOR_DARK_YELLOW };
        break;
    case eShowUISelect::Text2:
        patrolMsg = { L"",isLeader ? GAME_COLOR_YELLOW : GAME_COLOR_CYAN };
        break;
    case eShowUISelect::Text3:
        patrolMsg = { L"",isLeader ? GAME_COLOR_YELLOW : GAME_COLOR_CYAN };
    }

    // 7.
    switch (m_followMessage.uiState)
    {
    case eShowUISelect::Text1:
        followMsg = { L"追跡中",GAME_COLOR_ORANGE };
        break;
    case eShowUISelect::Text2:
        followMsg = { L"", GAME_COLOR_YELLOW };
        break;
    case eShowUISelect::Text3:
        followMsg = { L"",isLeader ? GAME_COLOR_YELLOW : GAME_COLOR_CYAN };
    }

    switch (m_runMessage.uiState)
    {
    case eShowUISelect::Text1:
        runMsg = { L"プレイヤーから\n　逃走中", GAME_COLOR_ORANGE };
        break;
    case eShowUISelect::Text2:
        runMsg = { L"",isLeader ? GAME_COLOR_YELLOW : GAME_COLOR_CYAN };
        break;
    case eShowUISelect::Text3:
        runMsg = { L"",isLeader ? GAME_COLOR_YELLOW : GAME_COLOR_CYAN };
    }

    //最後にまとめて表示（Noneじゃなければ出す）
    if (!areaMsg.text.empty())
    {
        EnemyAIDebug::ShowStateOnce(*GetModel()->getPosition(), m_aiDebugText, m_areaAtkMessage.remainingTime, areaMsg.text.c_str(), areaMsg.color);
    }
    if (!pullMsg.text.empty())
    {
        EnemyAIDebug::ShowStateOnce(*GetModel()->getPosition(), m_aiDebugText, m_pullAtkMessage.remainingTime, pullMsg.text.c_str(), pullMsg.color);
    }

    if (!panicMsg.text.empty())
    {
        EnemyAIDebug::ShowStateOnce(*GetModel()->getPosition(), m_aiDebugText, m_panicMessage.remainingTime, panicMsg.text.c_str(), panicMsg.color);
    }
    if (!chargeMsg.text.empty())
    {
        EnemyAIDebug::ShowStateOnce(*GetModel()->getPosition(), m_aiDebugText, m_chargeMessage.remainingTime, chargeMsg.text.c_str(), chargeMsg.color);
    }
    if (!patrolMsg.text.empty())
    {
        EnemyAIDebug::ShowStateOnce(*GetModel()->getPosition(), m_aiDebugText, m_patrolMessage.remainingTime, patrolMsg.text.c_str(), patrolMsg.color);
    }
    if (!followMsg.text.empty())
    {
        EnemyAIDebug::ShowStateOnce(*GetModel()->getPosition(), m_aiDebugText, m_followMessage.remainingTime, followMsg.text.c_str(), followMsg.color);
    }
    if (!runMsg.text.empty())
    {
        EnemyAIDebug::ShowStateOnce(*GetModel()->getPosition(), m_aiDebugText, m_runMessage.remainingTime, runMsg.text.c_str(), runMsg.color);
    }

        
}



//0.0～1.0の値を返す
float NewEnemyClass::GetRandomFloat()
{
    return static_cast<float>((rand()) / static_cast<float>(RAND_MAX));
}
float NewEnemyClass::GetRandomRange(float min, float max)
{
    return min + (max - min) * GetRandomFloat();
}


XMVECTOR NewEnemyClass::GetRandomDirection()
{
    //-1.0～1.0の間でXZを抽選
    float x = GetRandomRange(-1.0f, 1.0f);
    float z = GetRandomRange(-1.0f, 1.0f);
    
    XMVECTOR dir = XMVectorSet(x, 0.0f, z, 0.0f);

    //全く動かないを防ぐために
    if (XMVector3Equal(dir, XMVectorZero()))
    {
        return XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);//真横向き
    }

    return XMVector3Normalize(dir);//向きだけにして返す
}