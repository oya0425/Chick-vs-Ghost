#include"../framework.h"
#include"../framework/vn_environment.h"
vnMotionData* motion_idle_Mush;
vnMotionData* motion_run_Mush;

namespace
{
    const float escapeStartDist = 55.0f;    //逃走開始距離
    const float escapeStopDist  = 10.0f;    //逃走終了距離
    const float motionSpeed = 2.0f;

}

EnemyMushroomMon::EnemyMushroomMon()
    :NewEnemyClass()
{
    m_type = EnemyType::MUSHROOM;
	m_boostSpeedMultiplier = 1.0f;
    motion_idle_Mush = loadMotionFile(L"data/model/Ghost/motion/ghost_idle.mot");
    motion_run_Mush = loadMotionFile(L"data/model/MushroomMon/motion/Run.mot");


}
EnemyMushroomMon::~EnemyMushroomMon()
{

}

void EnemyMushroomMon::OnIdel(float deltaTime, float distance, const XMVECTOR& toPlayer)
{
    GetModel()->setMotion(motion_idle_Mush);
    GetModel()->execute(motionSpeed, false, false);
    if (!XMVector3Equal(GetMoveDir(), XMVectorZero()))
    {
        float rotY = atan2f(XMVectorGetX(GetMoveDir()), XMVectorGetZ(GetMoveDir()));
        GetModel()->setRotationY(rotY);
    }

    // --- 状態遷移 ---
    if (InPlayerArea(escapeStartDist, escapeStopDist))
    {
        if (GetRigidbody().GetIsGround())
        {
            SetState(eState::Run);
        }
    }
}

void EnemyMushroomMon::OnRun(float deltaTime, float distance, const XMVECTOR& toPlayer)
{
    CharacterBase* target = GetPlayer();
    if (!target) return;

    // --- 1.タイマー更新と状態切り替え ---
    m_timer += deltaTime;
    if (m_timer > 10.0f)
    {
        m_isMoving = !m_isMoving;
        m_timer = 0.0f;

        //--状態変化の処理
        if (m_isMoving)
        {
            //動いてるときは青
            GetModel()->SetAllPartsDiffuse(V_GAME_COLOR_BLUE,1.0f);
        }
        else
        {
            //止まっているときは赤
            GetModel()->SetAllPartsDiffuse(V_GAME_COLOR_RED, 1.0f);
            GetRigidbody().SetBaseVelocity(XMVectorZero());
        }
    }

    // --- 2.移動処理 ---
    if (m_isMoving)
    {
        // 1. モーション再生
        GetModel()->setMotion(motion_run_Mush);
        GetModel()->execute(motionSpeed, false, false);

        // 2. 逃走方向の計算 (toPlayerは enemyPos - playerPos である前提)
        XMVECTOR vInput = XMVectorZero();
        if (distance > 0.1f)
        {
            vInput = XMVector3Normalize(toPlayer); // プレイヤーに向かう方向
        }
        ApplyMovement(deltaTime, vInput);

        // 3. 向き（回転）の更新
        //if (!XMVector3Equal(vInput, XMVectorZero()))
        //{
        //    float rotY = atan2f(XMVectorGetX(vInput), XMVectorGetZ(vInput));
        //    GetModel()->setRotationY(rotY);
        //}
        // 4. 移動速度の適用 (Rigidbodyに速度を渡す)
        // m_baseMoveSpeed と m_boostSpeedMultiplier を掛け合わせる
        //GetRigidbody().SetBaseVelocity(vInput * (m_baseMoveSpeed * m_boostSpeedMultiplier*m_waveBoostSpeedMultiplier));


    }
    else
    {
        // 止まっている時の処理
        GetModel()->setMotion(motion_idle_Mush); // 待機モーションがあれば
        GetModel()->execute(motionSpeed, false, false);
        GetRigidbody().SetBaseVelocity(XMVectorZero());
    }
    // 5. 状態遷移：プレイヤーが停止距離（escapeStopDist）より離れたら待機へ
    if (!InPlayerArea(escapeStartDist, escapeStopDist))
    {
        if (GetRigidbody().GetIsGround())
        {
            GetRigidbody().SetBaseVelocity(XMVectorZero());
            SetState(eState::Idel);
        }
    }
}
void EnemyMushroomMon::OnDead()
{
    if (GetRigidbody().GetIsGround())
    {
        DeSpawn();
    }

}
void EnemyMushroomMon::OnFollow(float deltaTime)
{

}
void EnemyMushroomMon::OnPanic(float deltaTime)
{

}
void EnemyMushroomMon::OnCharge(float deltaTime, const XMVECTOR& toPlayer)
{

}
void EnemyMushroomMon::OnPatrol(float deltaTime,float distance)
{

}



