#include"../framework.h"
#include"../framework/vn_environment.h"
vnMotionData* motion_idle_Ghost;
vnMotionData* motion_run_Ghost;
namespace
{
    const float escapeStartDist = 15.0f;    //逃走開始距離
    const float escapeStopDist  = 10.0f;    //逃走終了距離
    const float motionSpeed = 2.0f;

     
    constexpr  float avoidStartRatio = 0.7f; // 半径の80%から回避開始
    constexpr  float avoidRangeRatio = 0.3f; // 残り20%の幅で重みを変化させる
    /*
    プレイヤーから逃げたい力に対する倍率。これが 1.0 だと力が拮抗して壁に張り付きますが、
    3.0 にすることで、何が何でもプレイヤーを振り切って内側へ飛び出す
    */
    constexpr  float repulsionStrength = 3.0f; 

}

EnemyGhost::EnemyGhost()
    :NewEnemyClass()
{
    m_type = EnemyType::GHOST;
	m_boostSpeedMultiplier = 2.0f;
    motion_idle_Ghost = loadMotionFile(L"data/model/Ghost/motion/ghost_idle.mot");
    motion_run_Ghost = loadMotionFile(L"data/model/Ghost/motion/ghost_run.mot");
    

}
EnemyGhost::~EnemyGhost()
{

}

void EnemyGhost::OnIdel(float deltaTime, float distance, const XMVECTOR& toPlayer)
{
    GetModel()->setMotion(motion_idle_Ghost);
    GetModel()->execute(motionSpeed, false, false);
    if (!XMVector3Equal(GetMoveDir(), XMVectorZero()))
    {
        float rotY = atan2f(XMVectorGetX(GetMoveDir()), XMVectorGetZ(GetMoveDir()));
        GetModel()->setRotationY(rotY);
    }
    
    // --- 状態遷移 ---
    if (GetIsLeader())
    {
        // 1. プレイヤーが来たら逃げる（最優先）
        //範囲攻撃の範囲を取って学習していく（逃げ始める距離を上げる）
        float dynamicEscapeRadius = m_leaderEscapeRadius + GetGroupData()->rangeFear;

        // 判定距離を書き換えてチェック
        if (InPlayerArea(dynamicEscapeRadius, m_leaderRetreatStopRadius))
        {
            if (GetRigidbody().GetIsGround())
            {

                SetState(eState::Run);
                m_runMessage.SetState(eShowUISelect::Text1);
                m_patrolWaitTimer = 0.5f;
                return;
            }
        }
        // 2. プレイヤーが遠いなら徘徊の準備
        // Idle用の待機タイマーを減らす（この変数をIdle用に新設するか、Patrol用を流用）
        m_patrolWaitTimer -= deltaTime;

        if (m_patrolWaitTimer <= 0)
        {
            // 一定時間ボーッとしたら徘徊開始！
            SetState(eState::Patrol);
            if (GetGroupData()->isLeaderEscaping)
            {
                GetGroupData()->isLeaderEscaping = false;
            }

            // OnPatrolに入った瞬間目的地を決めるため、フラグを立てておく
            m_isReachingTarget = true;
        }
    }
    else
    {
         //リーダーを探してついていく
         LeaderSet(m_searchLeaderRadius);
    }
}

void EnemyGhost::OnRun(float deltaTime, float distance, const XMVECTOR& toPlayer)
{
    // 1. モーション再生
    GetModel()->setMotion(motion_run_Ghost);
    GetModel()->execute(motionSpeed, false, true);

    // 2. 逃走方向の計算
    XMVECTOR enemyPos = *GetModel()->getPosition();
    XMVECTOR vInput = XMVectorZero();

    if (distance > 0.1f)    //プレイヤーと一定距離がある場合のみ計算開始
    {
        // 1. 基本の逃走方向の決定
        //プレイヤーから自分への方向を反転、長さ１の純粋な向きにする
        XMVECTOR fleeDir = XMVector3Normalize(-toPlayer);

        //基本の入力値をこの逃走方向に設定する
        vInput = fleeDir;

        // 2. リーダー同士の距離の確保
        //自身の周囲で最も近いリーダーを探す
        NewEnemyClass* closestLeader = EnemyPool::GetInstance().FindClosestLeader(this, m_leaderSeparateRadius);

        //検知を開始する最大距離（外枠）を設定
        float maxDist = m_leaderSeparateRadius;
        //斥力（押し返す力）が最大になる最小距離（内枠）を半径の半分に設定
        float minDist = m_leaderSeparateRadius * 0.5f;

        if (closestLeader)//近くに仲間が見つかった時
        {
            //仲間の現在の座標を更新
            XMVECTOR leaderPos = *closestLeader->GetModel()->getPosition();

            //仲間から自分へと離れる向きのベクトルを計算（自分-相手）
            XMVECTOR diff = enemyPos - leaderPos;

            //仲間との実距離を計算
            float d = XMVectorGetX(XMVector3Length(diff));

            //距離が検知範囲ないで、かつ完全に重なっていない（0.001f）
            if (d < maxDist && d>0.001f)
            {
                float weight = 0.0f;        //斥力の強さを格納する変数

                //危険距離（minDist）より近いなら、斥力を最大（1.0）にする
                if (d <= minDist)
                {
                    weight = 1.0f;
                }
                //そうでなければ、距離に応じて徐々に強くする（線形補間）
                else
                {
                    weight = (maxDist - d) / (maxDist - minDist);
                }

                //仲間から離れる方向を長さ1に正規化
                XMVECTOR separationDir = diff / d;


                //----------------------------
                // --- 挟まり回避ロジック ---
                //----------------------------
                //プレイヤーから逃げたい方向と仲間から離れたい方向が対立（逆向き）かチェック
                float dot = XMVectorGetX(XMVector3Dot(fleeDir, separationDir));

                //内積が-0.7未満＝逃げ道に仲間がいて、挟まれて動けなくなる可能性がある場合
                if (dot < -0.7)
                {
                    //外積を使い、仲間から離れる方向に対して垂直（真横）のベクトルを算出する
                    XMVECTOR sideDir = XMVector3Cross(separationDir, XMVectorSet(0, 1, 0, 0));

                    //横にスライドして逃げる力を重みに合わせて加算する
                    vInput += sideDir * weight * 2.0f;
                }

                //通常の仲間から離れる斥力を、重みに合わせて面の入力に加算する
                vInput += separationDir * weight * 1.5f;


            }
        }

        // 3. 壁（フェンス）の回避
        if (GetIsLeader()) //自身がリーダー個体である場合
        {
            float R = GetFenceRadius(); //現在のステージの壁の半径を獲得
            float currentDist = XMVectorGetX(XMVector3Length(enemyPos));    //ステージの中心からの距離を計算


            //自身の位置が壁の回避開始ライン（80%）を超えてる場合
            if (currentDist > (R * avoidStartRatio))
            {
                //ステージの中心へ向かう方向を計算（-enemyPos）して正規化
                XMVECTOR toCenter = XMVector3Normalize(-enemyPos);

                //壁に近づくほど強くなる（0.0～1.0）重みを計算
                float weight = (currentDist - (R * avoidStartRatio)) / (R * avoidRangeRatio);
                if (weight > 1.0f)weight = 1.0f;    //重みが１を超えないようにクランプ

                //Lerp（線形補間）を使い、現在の移動方向と中心へ戻る方向を混ぜ合わせる（壁際では中心方向を優先）
                vInput = XMVectorLerp(vInput, toCenter, weight * 0.8f);


            }
        }

    }
    // --- 滑らかな方向転換 ---
    //入力（進みたい方向）の長さが十分にある場合
    if (XMVectorGetX(XMVector3LengthSq(vInput)) > 0.001f)
    {
        //入力ベクトルをながさ１にそろえる
        vInput = XMVector3Normalize(vInput);

        //前回の移動方向から（m_lastMoveDir）から今回の目標方向へ、時間をかけてゆっくり近づける
        m_lastMoveDir = XMVectorLerp(m_lastMoveDir, vInput, 10.0f * deltaTime);

        //保管した結果を正規化して、ガタつきのない滑らかな向きにする
        m_lastMoveDir = XMVector3Normalize(m_lastMoveDir);

    }
    else
    {
        //逃走の必要がない（入力が０）なら、直近の移動方向もリセットする
        m_lastMoveDir = XMVectorZero();
    }



    // 3. 移動・回転の適用（補完済みのベクトルを使用）
    ApplyMovement(deltaTime, m_lastMoveDir);

    // 5. 状態遷移
    if (!InPlayerArea(m_leaderEscapeRadius+(5.0f*GetGroupData()->rangeFear), m_leaderRetreatStopRadius))
    {
        if (GetRigidbody().GetIsGround())
        {
            GetRigidbody().SetBaseVelocity(XMVectorZero());
            SetState(eState::Idel);
            m_lastMoveDir = XMVectorZero(); // 次回の動き出しのためにリセット
        }
    }
}


void EnemyGhost::OnDead()
{
    if (GetRigidbody().GetIsGround())
    {
        DeSpawn();
    }

}

void EnemyGhost::OnFollow(float deltaTime)
{

    //リーダーが決めたモードを群れで共有
    auto mode = m_pMyLeader->GetGroupMode();
    if (mode == eGroupMode::Panic)
    {
        m_panicDirTimer = 0.0f;
        m_panicRecoveryTime = 5.0f; //パニック状態から復帰する時間の設定
        GetModel()->SetAllPartsDiffuse(m_defaultOtherColor, 1.0f);   //パニック状態になると色が戻る
        SetGroupID(-1);
        m_pMyLeader = nullptr;
        SetState(eState::Panic);    //リーダーがいなくなるとパニック開始
        return;

    }
    else if (mode == eGroupMode::Charge)
    {
        //特攻状態では色はそのまま残しておく
        //１割の確率で特攻（10%以下）
        SetState(eState::Charge);
        GetModel()->SetAllPartsDiffuse(V_GAME_COLOR_WHITE, 0.1f);
        m_isCharge = true;
        m_pMyLeader = nullptr;
        return;

    }
    //2.モーション再生
    GetModel()->setMotion(motion_run_Ghost);
    GetModel()->execute(motionSpeed, false, false);

    //3.リーダーへの方向と距離を計算
    XMVECTOR leaderPos = *m_pMyLeader->GetModel()->getPosition();
    XMVECTOR myPos = *GetModel()->getPosition();
    XMVECTOR toLeader = leaderPos - myPos;

    //Yを無視して平面距離で計算
    toLeader = XMVectorSetY(toLeader, 0);
    float dist = XMVectorGetX(XMVector3Length(toLeader));

    //4.移動処理
    //自分の停止距離（m_myStopDist）より遠ければ近づく
    if (dist > m_baseFollowDist+m_myStopDist)
    {
        XMVECTOR vDir = XMVector3Normalize(toLeader);
        ApplyMovement(deltaTime, vDir);

    }
    else
    {
        //停止距離内なら、その場で止まる
        GetRigidbody().SetBaseVelocity(XMVectorZero());
        //身体をリーダーに向かせる
        XMVECTOR vDir = XMVector3Normalize(toLeader);
        float rotY = atan2f(XMVectorGetX(vDir), XMVectorGetZ(vDir));
        GetModel()->setRotationY(rotY);
    }


}
void EnemyGhost::OnPanic(float deltaTime)
{ 
    if (m_panicRecoveryStartTime > 0&&GetState()!=eState::Idel)
    {
       SetState(eState::Idel);
      return;
    }

    m_panicDirTimer -= deltaTime;
    m_panicRecoveryTime -= deltaTime;

    //EnemyAIDebug::ShowStateOnce(*GetModel()->getPosition(), m_aiDebugText, deltaTime, L"リーダー！！", GAME_COLOR_BLUE);

    if (m_panicDirTimer <= 0.0f)
    {
        m_panicMessage.SetState(eShowUISelect::Text1);

        //移動する方向をランダムで取る
        m_panicDir = GetRandomDirection();

        //方向転換までの時間をランダムに設定GetRandomRange(a,b)a~bの間
       m_panicDirTimer = GetRandomRange(1.0f, 3.0f);
    }
    //決まった方向に移動
    ApplyMovement(deltaTime, m_panicDir);

    //パニック状態から再びリーダーを探す
    //見つかったらついて行く
    if (m_panicRecoveryTime <= 0)
    {
        m_panicMessage.SetState(eShowUISelect::Text2);
        LeaderSet(m_panicSearchRadius);
        
    }
    
    
}
void EnemyGhost::OnCharge(float deltaTime, const XMVECTOR& toPlayer)
{
    if (m_panicRecoveryStartTime > 0 && GetState() != eState::Idel)
    {
        SetState(eState::Idel);
        return;
    }

    XMVECTOR moveDir = XMVector3Normalize(toPlayer);
    ApplyMovement(deltaTime, moveDir * m_chargeSpeedMultiplier);
}


// --------------------------------------------------------------
//   パトロール中処理
// --------------------------------------------------------------
void EnemyGhost::OnPatrol(float deltaTime, float distance)
{
    GetModel()->setMotion(motion_run_Ghost);
    GetModel()->execute(motionSpeed, false, false);
    //メッセージ表示
    m_patrolMessage.SetState(eShowUISelect::Text1);
    
    //メインの移動
    MoveAlongPath(deltaTime, distance);

    // プレイヤー検知
    CheckSurroundings(distance);

}
//メインの移動
void EnemyGhost::MoveAlongPath(float deltaTime, float distance)
{

    XMVECTOR enemyPos = *GetModel()->getPosition();
    XMVECTOR toTarget = m_patrolTargetPos - enemyPos;
    // Y軸（高さ）の差を無視して距離判定（これ重要！）
    toTarget = XMVectorSetY(toTarget, 0);
    float distToTarget = XMVectorGetX(XMVector3Length(toTarget));

    // --- A. 到着済み、または待機中の処理 ---
    if (m_isReachingTarget)
    {
        m_patrolWaitTimer -= deltaTime;
        GetRigidbody().SetBaseVelocity(XMVectorZero()); // 止まる

        if (m_patrolWaitTimer <= 0)
        {
            // 新しい目的地を決定
            /*
            1. (rand() / (float)RAND_MAX) の部分これは
            「0.0 から 1.0 の間の数を作る」 というプログラミングの定番テクニックです。
            rand()：0 から 32767（環境による）の整数をランダムに返す。
            RAND_MAX：rand() が出す最大値（32767など）。
            これらを割り算すると：
            必ず 0.0（0/32767）から 1.0（32767/32767）の間の小数になります。
            例：0.5 なら「半分」、
                0.1 なら「ちょっと」という意味になります。
            2. float angle = ... * XM_2PI;
                の部分これは 「360度、全方向のどこか」 を決めています。
                XM_2PI：数学の $2\pi$ です。
                角度（ラジアン）で言うと 360度。
                0.0〜1.0 の数 × 360度 ＝ 「0度から360度のどこか」 がランダムに決まります。
                これで、敵がどっちの方向に歩き出すかが決まるわけです。
            3. float range = 5.0f + ... * 5.0f;
                の部分これは 「移動する距離（半径）」 を決めています。
                基本の 5.0f（最低でも5mは行く）＋ (0.0〜1.0) × 5.0f（おまけで 0m〜5m 足す）
                結果：5.0m 〜 10.0m の間のどこかになります。
                毎回同じ距離だと機械的なので、これで歩く距離にバラつきを出しています。
            4. XMVectorSet(cosf(angle) * range, 0.0f, sinf(angle) * range, 0.0f)最後に、
                決めた 角度（向き） と 距離 を、ゲーム内の X座標とZ座標 に変換しています。
                cosf(angle) * range ＝ X座標sinf(angle) * range ＝ Z座標
                Y座標は 0（地面を歩くから）
            */
            //float angle = (rand() / (float)RAND_MAX) * XM_2PI;
            //float range = 5.0f + (rand() / (float)RAND_MAX) * 5.0f;
            //m_patrolTargetPos = enemyPos + XMVectorSet(cosf(angle) * range, 0.0f, sinf(angle) * range, 0.0f);

            //m_isReachingTarget = false; // 「移動中」に切り替え
            // タイマーは「次に到着した時の待ち時間」として使う

            // 1. 今の向きをベースにする
            float currentAngle;
            if (XMVectorGetX(XMVector3LengthSq(m_lastMoveDir)) < 0.01f) {
                currentAngle = (rand() / (float)RAND_MAX) * XM_2PI;
            }
            else {
                currentAngle = atan2f(XMVectorGetZ(m_lastMoveDir), XMVectorGetX(m_lastMoveDir));
            }

            // 2. 「八の字」を防ぐため、曲がる角度を「±30度〜45度」くらいに絞る
            // これで「直進性」が生まれます
            float randomOffset = ((rand() / (float)RAND_MAX) - 0.5f) * (XM_PI / 3.0f); // ±60度くらい
            float angle = currentAngle + randomOffset;

            // 3. 距離を「しっかり」出す
            // 10m〜20m くらい先を目的地に「投げる」イメージ
            float range = 10.0f + (rand() / (float)RAND_MAX) * 10.0f;

            // 4. 目的地を決定
            XMVECTOR offset = XMVectorSet(cosf(angle) * range, 0.0f, sinf(angle) * range, 0.0f);
            m_patrolTargetPos = enemyPos + offset;

            // 5. 【重要】目的地が「フェンス（壁）」の外なら、内側に戻す
            // GetInFenceが座標を補正してくれるならそれを利用する
            // (もし座標を補正する関数なら、m_patrolTargetPosを引数に入れる)
            m_patrolTargetPos = GetInFence(m_patrolTargetPos, *GetModel()->getPosition());

            m_isReachingTarget = false;

        }
    }
    // --- B. 移動中の処理 ---
    else
    {
        if (distToTarget < 1.5f) // 少し余裕を持って到着判定
        {
            m_isReachingTarget = true;
            m_patrolWaitTimer = 1.0f + (rand() / (float)RAND_MAX) * 1.0f; // 1~3秒のランダム待機
        }
        else
        {
            // 1. 目的地に向かう基本のベクトル
            XMVECTOR vInput = XMVector3Normalize(toTarget);

            // --------------------------------------------------------
            // 2. 【追加】リーダー同士の距離確保（分離ロジック）
            // --------------------------------------------------------
            // 逃走の時と同じく、一番近いリーダーを探す
            NewEnemyClass* closestLeader = EnemyPool::GetInstance().FindClosestLeader(this, m_leaderSeparateRadius);

            if (closestLeader)
            {
                XMVECTOR leaderPos = *closestLeader->GetModel()->getPosition();
                XMVECTOR diff = enemyPos - leaderPos; // 相手から自分へのベクトル
                diff = XMVectorSetY(diff, 0);         // 高さ無視
                float d = XMVectorGetX(XMVector3Length(diff));

                // 範囲内に仲間がいたら斥力を計算
                if (d < m_leaderSeparateRadius && d > 0.001f)
                {
                    float maxDist = m_leaderSeparateRadius * 1.5f;
                    float minDist = m_leaderSeparateRadius * 0.5f;
                    float weight = 0.0f;

                    // 斥力の強さ（近いほど強い）
                    if (d <= minDist) weight = 1.0f;
                    else weight = (maxDist - d) / (maxDist - minDist);

                    XMVECTOR separationDir = XMVector3Normalize(diff);

                    // 挟まり回避（今進みたい方向 vInput と 避けたい方向 separationDir を比較）
                    float dot = XMVectorGetX(XMVector3Dot(vInput, separationDir));
                    if (dot < -0.7f)
                    {
                        // 垂直に逃げる力を加える
                        XMVECTOR sideDir = XMVector3Cross(separationDir, XMVectorSet(0, 1, 0, 0));
                        vInput += sideDir * weight * 1.0f; // 徘徊なので少し控えめ(1.0f)
                    }

                    // 仲間に背を向ける力を合成（徘徊なので 0.8f 程度でマイルドに）
                    vInput += separationDir * weight * 0.8f;

                    // 合成した結果を再度正規化
                    vInput = XMVector3Normalize(vInput);
                }
            }
            // --------------------------------------------------------

            // フェンス判定（最終的な移動方向をチェック）
            vInput = GetInFence(vInput, enemyPos);

            // 方向転換と移動
            if (XMVectorGetX(XMVector3LengthSq(vInput)) > 0.01f)
            {
                // Lerpで滑らかに回転
                m_lastMoveDir = XMVectorLerp(m_lastMoveDir, vInput, 3.0f * deltaTime);
                m_lastMoveDir = XMVector3Normalize(m_lastMoveDir);
            }

            ApplyMovement(deltaTime, m_lastMoveDir);
        }
    }


}

//範囲攻撃可能かの判定
void EnemyGhost::CheckSurroundings(float distance)
{
    bool canE = GetPlayer()->CanAreaAttack();
    if (canE)
    {
        float dist = distance;
        float limit = m_leaderEscapeRadius + (5.0f + GetGroupData()->rangeFear);
        if (dist < limit)
        {
            StartEscapeTransition(true);
            m_areaAtkMessage.SetState(eShowUISelect::Text1);
        }
        else
        {
            m_areaAtkMessage.SetState(eShowUISelect::Text2);
        }
    }
    else
    {
        float dist = distance;
        float limit = m_leaderEscapeRadius;
        if (dist < limit)StartEscapeTransition(false);
    }

}

void EnemyGhost::StartEscapeTransition(bool can)
{
    GetGroupData()->isLeaderEscaping = can;
    if(GetGroupData()->isLeaderEscaping == false)
    {
        m_runMessage.SetState(eShowUISelect::Text1);
    }
    SetState(eState::Run);

    m_isReachingTarget = true;
}

// -------------------------------------------------------------------------




// --- 汎用 ---

//移動時に壁を擦り続けないようにする
XMVECTOR EnemyGhost::GetInFence(XMVECTOR vInput, XMVECTOR myPos)
{
    float R = GetFenceRadius(); //現在のステージの壁の半径を獲得
    float currentDist = XMVectorGetX(XMVector3Length(myPos));    //ステージの中心からの距離を計算


    //自身の位置が壁の回避開始ライン（80%）を超えてる場合
    if (currentDist > (R * avoidStartRatio))
    {
        //ステージの中心へ向かう方向を計算（-enemyPos）して正規化
        XMVECTOR toCenter = XMVector3Normalize(-myPos);

        //壁に近づくほど強くなる（0.0～1.0）重みを計算
        float weight = (currentDist - (R * avoidStartRatio)) / (R * avoidRangeRatio);
        if (weight > 1.0f)weight = 1.0f;    //重みが１を超えないようにクランプ

        //Lerp（線形補間）を使い、現在の移動方向と中心へ戻る方向を混ぜ合わせる（壁際では中心方向を優先）
        return XMVectorLerp(vInput, toCenter, weight * 0.8f);

    }
    return vInput;

}

//リーダーを設定
void EnemyGhost::LeaderSet(float searchRadius)
{
    //リーダーを範囲で探して見つかったら色をリーダーに合わせて、
    // リーダーについていくモードに変える
    
    // リーダー以外：リーダーを探す（既存のFollowロジック）
    if (!m_pMyLeader)
    {
        m_pMyLeader = EnemyPool::GetInstance().FindClosestLeader(this, searchRadius);
        if (m_pMyLeader)
        {
            //リーダーの色を取得
            XMVECTOR leaderColor = m_pMyLeader->GetColor();

            //少し色を足して薄くする
            XMVECTOR offset = XMVectorSet(0.2f, 0.2f, 0.2f, 0.0f);
            XMVECTOR followerColor = XMVectorAdd(leaderColor, offset);

            //1.0を超えないようにクランプ
            followerColor = XMVectorClamp(followerColor, XMVectorZero(), XMVectorSplatOne());

            //群れの番号を適応
            SetGroupID(m_pMyLeader->GetGroupID());

            //自分に色を適用
            GetModel()->SetAllPartsDiffuse(followerColor, 1.0f);
            SetState(eState::Follow);
            m_panicMessage.SetState(eShowUISelect::Text3);
        }
    }
    //すでにリーダーを知ってる場合はそのリーダーについていく
    else
    {
        SetState(eState::Follow);
    }
}