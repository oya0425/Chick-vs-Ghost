#include"../framework.h"
#include"../framework/vn_environment.h"
#include <random>    // random_device, mt19937 用
#include <algorithm> // shuffle 用
namespace
{
    // --- モデル・スケール関連 ---
    constexpr float ghostSize = 2.5f;

    // --- 状態遷移の確率 (0.0f ~ 1.0f) ---
    constexpr float chargeProbability = 0.2f; // 特攻になる確率
    constexpr float wallJumpChance = 0.3f; // 壁でジャンプする確率

    // --- ノックバック演出 (回転分岐) ---
    constexpr float rotationThreshold1 = 0.33f;
    constexpr float rotationThreshold2 = 0.66f;

    // --- 物理・移動パラメータ ---
    constexpr float wallJumpPower = 18.0f;  //ジャンプ力 
    constexpr float fallLimitY = -10.0f;    //リスポーンする保険の高さ

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
    constexpr float leaderSpeedBoost        = 2.0f;     // リーダー専用速度
    constexpr float leaderSenseRadius       = 20.0f;    // プレイヤーを感知して逃げ始める距離
    constexpr float leaderStopRetreatRadius = 60.0f;    // プレイヤーから十分に離れて逃げやめる距離
    constexpr float leaderRepelRadius       = 20.0f;    // リーダー同士が重ならないように反発する距離（排他範囲）

    constexpr float followerStopDistBase    = 5.0f;    // リーダーに対して停止する基本距離
    constexpr float followerSpeedBoost      = 1.5f;    // 一般個体の速度倍率
    constexpr float followerSearchRadius    = 100.0f;  // リーダーを探し出す索敵範囲
    constexpr int   stopDistRandomRange     = 50;      // 停止距離にバラつきを出すための乱数範囲（0～4.9f）

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
    { EnemyType::GHOST,    L"data/model/Ghost2/",       L"Ghost.bone",    200/*200*/, ghostSize, {ghostSize, ghostSize, ghostSize} },
    { EnemyType::MUSHROOM, L"data/model/MushroomMon/", L"MushroomMon.bone", 0 , 1.5f, {1.0f, 1.0f,1.0f}},
    //{ EnemyType::MUSHROOM, L"data/model/MushroomMonster/", L"MushroomMonster.bone", 150 , 5.0f, {1.0f, 1.0f, 1.0f}},
};


std::vector<NewEnemyClass::GroupColorData> NewEnemyClass::m_availableColors;
//色の配列の初期化
std::vector<NewEnemyClass::GroupColorData> NewEnemyClass::g_LeaderColorPalette =
{
{ 1,  V_GAME_COLOR_RED },          // 01: 赤
    { 2,  V_GAME_COLOR_GREEN },        // 02: 緑
    { 3,  V_GAME_COLOR_BLUE },         // 03: 青
    { 4,  V_GAME_COLOR_YELLOW },       // 04: 黄
    { 5,  V_GAME_COLOR_OLIVE },        // 05: オリーブ (渋い緑/黄) ※マゼンタを廃止
    { 6,  V_GAME_COLOR_CYAN },         // 06: シアン
    { 7,  V_GAME_COLOR_ORANGE },       // 07: オレンジ
    { 8,  V_GAME_COLOR_PURPLE },       // 08: 紫 (紫系はこれ1つ)
    { 9,  V_GAME_COLOR_BROWN },        // 09: 茶色
    { 10, V_GAME_COLOR_PINK },         // 10: ピンク
    { 11, V_GAME_COLOR_LIME },         // 11: ライム
    { 12, V_GAME_COLOR_NAVY },         // 12: 紺 (青系と区別化)
    { 13, V_GAME_COLOR_TEAL },         // 13: ティール (青緑)
    { 14, V_GAME_COLOR_GOLD },         // 14: 金色
    { 15, V_GAME_COLOR_MINT },         // 15: ミント (白に近いが緑寄り) ※白を廃止
    { 16, V_GAME_COLOR_DARK_GRAY },    // 16: ダークグレー
    { 17, V_GAME_COLOR_DEEP_BLACK },    // 17: ほぼ黒
    { 18, V_GAME_COLOR_EMERALD },      // 18: エメラルド
    { 19, V_GAME_COLOR_CORAL },        // 19: コーラル (朱色に近い)
    { 20, V_GAME_COLOR_WINE }          // 20: ワインレッド (深い赤紫) ※スカイブルーを廃止
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

void NewEnemyClass::Update(float deltaTime)
{
    if (!m_isActive) return;

    // --- 共通計算（ここで1回だけやる） ---
    XMVECTOR enemyPos = *GetModel()->getPosition();
    XMVECTOR toPlayer = *m_pPlayer->GetModel()->getPosition() - enemyPos; // 敵からプレイヤーへの方向
    float distance = XMVectorGetX(XMVector3Length(toPlayer));

    //EnemyPool::GetInstance().GetGroupData(GetGroupID());

    // 敵のコード内
    if (m_isSpwanStart)
    {
        m_upgradeTimer -= deltaTime*2;
        float floatUp = (0.8f - m_upgradeTimer) * 2.0f; // 1秒かけて 1.0m 上に上がる

        // 表示位置に floatUp を足す
        XMVECTOR displayPos = *GetModel()->getPosition();
        displayPos = XMVectorSetY(displayPos, XMVectorGetY(displayPos) + 1.0f + floatUp);

        // ステップに応じて出す文字を変える
        if (m_upgradeStep == 0)
        {
            EnemyAIDebug::ShowUpgrade(displayPos, L"近接回避UP！");
        }
        else if (m_upgradeStep == 1)
        {
            EnemyAIDebug::ShowUpgrade(displayPos, L"移動速度UP！");
        }

        // 1つ目の文字が上がりきったら（タイマーが0になったら）
        if (m_upgradeTimer <= 0.0f)
        {
            m_upgradeStep++;    // 次のステップへ
            m_upgradeTimer = 3.0f; // タイマーリセット

            // 全てのステップが終わったら終了
            if (m_upgradeStep >= 2) // 2個出し終わったら
            {
                m_isSpwanStart = false;
                m_upgradeStep = 0;
                // 次回のためにタイマーなどは戻さない（またはリセット）
            }
        }

    }

    // --- 状態(子クラスに任せる) ---
    switch (m_state)
    {
    case Idel:
        if (GetIsLeader())
        {
            m_isSpwanStart = true;
        }

        // 距離と方向を渡してあげる
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
        UpdateAttracted(deltaTime,toPlayer,distance);
        break;
    case Follow:
        OnFollow(deltaTime);
        break;

    case Panic:
        OnPanic(deltaTime);
        break;
    case Charge:
        OnCharge(deltaTime,toPlayer);
        break;

    case Patrol:
        OnPatrol(deltaTime, distance);
        break;
    }


    //XMVECTOR ropecenter = XMVectorAdd(*GetModel()->getPosition(), GetCollision().GetCenter());
    //vnDebugDraw::Sphere(ropecenter, GetEffectiveRadius(), GAME_COLOR_LIME);

    // --- playerと当たった時
    if (m_isHitPlayer && m_state != eState::KnockBack)
    {

        GetRigidbody().SetBaseVelocity(XMVectorZero());
        XMVECTOR dir = *GetModel()->getPosition() - *m_pPlayer->GetModel()->getPosition();
        m_randomKnockNum = knockbackRandomMin + (rand() / (float)RAND_MAX) * (knockbackRandomMax - knockbackRandomMin);

        StartKnockback(dir, knockbackDist, knockbackDuration, m_baseKnockSpeed * m_randomKnockNum); // durationは秒で

        if(GetIsLeader())
        if (GetRandomFloat() < chargeProbability)
        {
            m_currentGroupMode = eGroupMode::Charge;

        }
        else
        {

            m_currentGroupMode = eGroupMode::Panic;
        }

        m_state = eState::KnockBack;
    }

    //物理更新
    //壁（ブロック）が前に来たらジャンプ
    Jump();
    GetRigidbody().Update(deltaTime);
    
    //伸び縮みするアニメーション
    UpdateSquashAndStretch(deltaTime);

    // --- 保険 --- 一定範囲（高さ（下））を超えたら戻す
    if (GetModel()->getPositionY() < fallLimitY) {
        //一定以上落下したら
        DeSpawn();
    }

    //待機状態以外で出てくるのを防ぐ用
    m_panicRecoveryStartTime -= deltaTime;


}

void NewEnemyClass::ChangeSpeed(float speed)
{

    m_waveBoostSpeedMultiplier = speedAdjustmentRate * speed;
}


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

void NewEnemyClass::CheckPullTrigger(bool isPlayerPulling, float pullRadius, float distance)
{
    // 死亡時やノックバック中は無視する
    if (m_state == Dead || m_state == KnockBack) return;

    if (isPlayerPulling && distance <= pullRadius)
    {
        m_state = Attracted;
    }
    else if (m_state == Attracted)
    {
        // 吸引が終わったら通常の移動状態に戻す
        m_state = Run;
    }
}

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


void NewEnemyClass::Spawn(const XMVECTOR& pos)
{
    //if (isActive)return;
    GetModel()->setRenderEnable(true);
    m_panicRecoveryStartTime = 0.5f;
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
    m_currentGroupMode = eGroupMode::Normal;
   
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

}

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
    float finalSpeed = m_baseMoveSpeed * m_boostSpeedMultiplier * m_waveBoostSpeedMultiplier;

    // リーダーならさらに加算
    if (m_isLeader)finalSpeed *= m_leaderSpeedMultiplier;
    else if (!m_isLeader)finalSpeed *= m_otherSpeedMultiplier;

    //速度適応
    GetRigidbody().SetBaseVelocity(moveDir * finalSpeed);

}

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
//  伸び縮みするアニメーション
// ------------------------------------------------------
void NewEnemyClass::UpdateSquashAndStretch(float deltaTime)
{
    //時間を進める
    m_livingTime += deltaTime;
    if (m_state != eState::KnockBack)
    {
        //sin(累計時間*速さ)*強さ
        float wave = sinf(m_livingTime * m_animSpeed);

        GetModel()->setScaleY(XMVectorGetY(m_defaultScale) + (wave * m_animMagnitude));
    }
}





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
    m_animSpeed = 10.0f + (rand() % 101) / 100.0f;


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

    //リーダーを探す範囲
    m_searchLeaderRadius = followerSearchRadius;

    //通常個体の速度設定
    float speedVariance = (90 + (rand() % 21)) / 100.0f;
    m_otherSpeedMultiplier *= speedVariance;
    //m_otherSpeedMultiplier = followerSpeedBoost;
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