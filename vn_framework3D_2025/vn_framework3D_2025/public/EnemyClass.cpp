#include"../framework.h"
#include"../framework/vn_environment.h"
#include"EnemyClass.h"

vnMotionData* motion_BoxUnity_WAIT02;
extern vnMotionData* motion_BoxUnity_RunF;
vnMotionData* motion_BoxUnity_DAMAGED01;
extern vnMotionData* motion_idle_enemy;


EnemyClass::EnemyClass()
{
    srand((unsigned int)time(nullptr));

    pModel = nullptr;
    isActive = false;
    baseMoveSpeed = 12.0f;		    //元の速度
    boostSpeedMultiplier = 2.0f;	//加速値
    maxMoveSpeed = 15.0f;	        //最大速度（wave数×boostSpeedMultiplier）で行こうと思う

    knockSpeed = 5.0f;
    randomKnockNum = 2.0f;

    isEscaping = false;

    isHit = false;

    //knockback.startPos = nullptr;


    isRising = false;
    targetHeight = 0;
    playerPos = nullptr;
    currentWave = 0;
    state = eState::Idel;


    motion_BoxUnity_WAIT02 = loadMotionFile(L"data/model/BoxUnityChan/motion/WAIT02.mot");
    motion_BoxUnity_RunF = loadMotionFile(L"data/model/BoxUnityChan/motion/RUN00_F.mot");
    motion_BoxUnity_DAMAGED01 = loadMotionFile(L"data/model/BoxUnityChan/motion/DAMAGED01.mot");
    motion_idle_enemy = loadMotionFile(L"data/model/Ghost/motion/ghost_run.mot");

    rigidbody.SetIsUseGravity(true);


}

EnemyClass::~EnemyClass()
{
}

void EnemyClass::SetModel(vnCharacter* model)
{
    pModel = model;

    float sizeSetNum = 3;

    pModel->setScale(1* sizeSetNum, 1 * sizeSetNum, 1 * sizeSetNum);
    pModel->size = XMVectorSet(1.0f, 2.0f, 1.0f, 0) * sizeSetNum;
    pModel->center = XMVectorSet(0, 1.0f, 0, 0) * sizeSetNum;
}

void EnemyClass::Spawn(const XMVECTOR& pos)
{
    //if (isActive)return;
    pModel->setRenderEnable(true);

    for (int i = 0; i < pModel->getPartsNum(); i++)
    {
        pModel->getParts(i)->setRenderEnable(true);
    }
    pModel->setPosition(&pos);
    pModel->setRotation(0, 0, 0);
    knockback.active = false;
    isActive = true;
    auto& rb = GetRigidbody();
    rb.SetVerticalVelocity(0.0f);
    rb.SetIsGround(false);
    rb.SetIsUseGravity(true);
    state = eState::Idel; // 待機状態に戻す
}

//初期化
void EnemyClass::DeSpawn()
{
    knockback.active = false;
    isActive = false;
    pModel->setRenderEnable(false);
    for (int i = 0; i < pModel->getPartsNum(); i++)
    {
        pModel->getParts(i)->setRenderEnable(false);
    }
    pModel->setPosition(0, 0, 0);
    state = eState::Idel;

}

void EnemyClass::Update(float deltaTime)
{
    if (!pModel) return;
    if (!isActive) return;
    if (!playerPos) return;

#if _DEBUG
    //XMVECTOR ropecenter = XMVectorAdd(*pModel->getPosition(), pModel->center);
    //vnDebugDraw::Box(ropecenter, pModel->size, GAME_COLOR_LIME);
#endif
    XMVECTOR vInput = XMVectorZero();
    XMVECTOR enemyPos = *pModel->getPosition();
    XMVECTOR toPlayer = enemyPos - *playerPos;

    float distance =
        XMVectorGetX(XMVector3Length(toPlayer));

    // --- 逃走距離設定 ---
    const float escapeStartDist = 10.0f; // 近づいたら逃げる
    const float escapeStopDist = 15.0f; // 離れたら止まる

    // 逃走状態（※EnemyClassのメンバにするのが本命）


    switch (state)
    {
    case EnemyClass::Dead:
        //state=Idel;
        if (rigidbody.GetIsGround())
        {
            DeSpawn();

        }
        break;
    case EnemyClass::Idel:
        //pModel->setMotion(motion_BoxUnity_WAIT02);
        //pModel->setMotion(motion_idle_enemy);

        pModel->execute(2, false, true);
        SetMoveDirection(vInput);
        if (!XMVector3Equal(moveDir, XMVectorZero()))
        {
            float rotY = atan2f(XMVectorGetX(moveDir), XMVectorGetZ(moveDir));
            pModel->setRotationY(rotY);
        }

        // --- 状態遷移 ---
        if (!isEscaping && distance < escapeStartDist)
        {
            if (rigidbody.GetIsGround())
            {
                isEscaping = true;
                state = eState::Run;
            }
        }

        break;
    case EnemyClass::Run:
        vInput = XMVectorZero();
        //rigidbody.setBaseVelocity(XMVectorZero());
        if (isEscaping && distance > escapeStopDist)
        {
            if (rigidbody.GetIsGround())
            {
                isEscaping = false;
                state = eState::Idel;
            }
        }
        //pModel->setMotion(motion_BoxUnity_RunF);
        pModel->execute(3, false, true);

        // --- 移動方向 ---
        if (isEscaping && distance > 0.1f)
        {
            vInput = XMVector3Normalize(toPlayer);
        }

        // --- 向きを調整 ---
        SetMoveDirection(vInput);
        if (!XMVector3Equal(moveDir, XMVectorZero()))
        {
            float rotY = atan2f(XMVectorGetX(moveDir), XMVectorGetZ(moveDir));
            pModel->setRotationY(rotY);
        }

        // --- Rigidbody 更新 ---
        rigidbody.SetBaseVelocity(vInput * (baseMoveSpeed * boostSpeedMultiplier));

        break;
    case EnemyClass::KnockBack:
        if (!knockback.active)
        {
            state = Idel; break;
        }


        //pModel->setMotion(motion_BoxUnity_DAMAGED01);
        pModel->addRotationX(0.5f);
        pModel->execute(2, true, true);
        isHit = false;


        // --- 上に飛ぶ ---
        // 現在Y取得
        float currentY = XMVectorGetY(*pModel->getPosition());
        //pModel->addPositionY(1000.02);
        // 上昇中
        if (isRising)
        {
            if (currentY >= targetHeight)
            {
                isRising = false;
                rigidbody.SetVerticalVelocity(-knockSpeed * randomKnockNum); // 落下速度（固定）
            }
            else {
                rigidbody.SetVerticalVelocity(knockSpeed * randomKnockNum); // 上昇速度（固定）
            }
        }


        // --- 横に飛ぶ処理 ---
        // 時間進行 
        knockback.timer += deltaTime;
        float t = knockback.timer / knockback.duration;
        if (t > 1.0f) t = 1.0f;

        // 1 → 0 に落ちる係数（減速）
        float decel = 1.0f - t;

        // 初速（距離 ÷ 時間）
        float speed = knockback.distance / knockback.duration;

        // 速度として与える（dtは絶対に掛けない）
        XMVECTOR velocity =
            knockback.direction * speed * decel;

        rigidbody.AddExternalVelocity(velocity);


        // =========================
        // 終了
        // =========================
        if (knockback.timer >= knockback.duration)
        {
            knockback.active = false;
            knockback.timer = 0.0f;
            isEscaping = false;
            pModel->setRotationX(0);
            state = Dead;
        }

        break;
    }

    // --- playerと当たった時
    if (isHit && state != eState::KnockBack)
    {
        rigidbody.SetBaseVelocity(XMVectorZero());
        XMVECTOR dir = *GetModel()->getPosition() - *playerPos;
        randomKnockNum = 0.8f + (rand() / (float)RAND_MAX) * (5.2f - 0.8f);

        StartKnockback(dir, 100.0f, 2, knockSpeed * randomKnockNum); // durationは秒で
        state = eState::KnockBack;
    }

    rigidbody.Update(deltaTime);

    if (rigidbody.GetIsGround())
    {
        rigidbody.SetIsUseGravity(false);
    }

    // --- 保険 --- 一定範囲（高さ（下））を超えたら戻す
    if (pModel->getPositionY() < -30.0f) {
        //一定以上落下したら
        DeSpawn();
    }


    //vnFont::print(10.0f, 100.0f, rigidbody.GetIsUseGravity() ? L"GetIsUseGravity : ture" : L"GetIsUseGravity:false");


}

void EnemyClass::StartKnockback(
    const XMVECTOR& dir,
    float dist,
    float time,
    float height)
{
    knockback.active = true;
    knockback.timer = 0.0f;
    knockback.duration = time;
    knockback.startPos = *pModel->getPosition();   // 現在位置
    knockback.direction = XMVector3Normalize(dir);
    knockback.distance = dist;
    knockback.height = height;

    float startY = XMVectorGetY(*pModel->getPosition());
    targetHeight = startY + height;

    isRising = true;
}

void EnemyClass::ChangeSpeed(float speed)
{

    boostSpeedMultiplier= 0.5f*speed;


}

void EnemyClass::SetMoveDirection(const XMVECTOR& dir)
{
    moveDir = dir;
}

