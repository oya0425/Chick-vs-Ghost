#include "../framework.h"
#include "../framework/vn_environment.h"
#include "EnemyPool.h"

EnemyPool::EnemyPool()
{
    m_groupDatas.reserve(100);
    //_enemies.reserve(30);
}

EnemyPool::~EnemyPool()
{
    //_enemies.clear();
}

// --- 敵を追加 ---
void EnemyPool::AddEnemy(NewEnemyClass* enemy)
{
     enemy->SetActive(false);
    _enemies.push_back(enemy);
}

// --- 全敵更新 ---
void EnemyPool::Update(float deltaTime)
{

    for (auto e : _enemies)
    {
        if (e->GetActive())
        {
            e->Update(deltaTime);
        }
    }
}

// --- スポーン ---
void EnemyPool::Spawn(const XMVECTOR& position/*,int globalLimit*/)
{
    // タイマーを減らす
    m_spawnTimer -= vnScene::getDeltaTime();

    // まだ0.1秒経っていないなら、何もせず帰る
    if (m_spawnTimer > 0.0f)
    {
        return;
    }

    NewEnemyClass* enemy = GetInactiveEnemy();
    if (enemy)
    {
        if (enemy->GetState() == NewEnemyClass::eState::Panic)return;
        enemy->Spawn(position);
        m_spawnTimer = SPAWN_INTERVAL;
        // SetActiveはSpwan内でやるなら不要
    }
    else {
        // 念のため描画を消す
        for (auto e : GetEnemies())
        {
            if (!e->GetActive())
            {
                e->GetModel()->setRenderEnable(false);
                for (int i = 0; i < e->GetModel()->getPartsNum(); i++)
                {
                    e->GetModel()->getParts(i)->setRenderEnable(false);
                }
            }
        }
    }
}

// --- 非アクティブ取得 ---
NewEnemyClass* EnemyPool::GetInactiveEnemy()
{
    for (auto e : _enemies)
    {
        if (!e->GetActive()&&e->IsUnlocked())
        {
            return e;
        }
    }
    return nullptr;
}

// --- アクティブ数取得 ---
int EnemyPool::GetActiveCount() const
{
    int count = 0;
    for (auto e : _enemies)
    {
        if (e->GetActive())
        {
            count++;
        }
    }
    return count;
}

// --- プレイヤーの位置をセット ---
void EnemyPool::SetPlayerPosAll(CharacterBase& player)
{
    for (auto enemy : _enemies)
    {
        if (!enemy->GetActive()) continue;

        enemy->SetPlayerPos(&player);
    }
}

// --- 出現する敵のロックを解除 ---
void EnemyPool::UnlockEnemyType(NewEnemyClass::EnemyType type)
{
    for (auto e : _enemies)
    {
        if (e->GetType() == type)
        {
            e->SetUnlock(true); // 指定されたタグの敵をすべて解放！
        }
    }
}


// --- リーダーを探して一体セット ---
NewEnemyClass* EnemyPool::FindClosestLeader(NewEnemyClass* requester, float radius)
{
    NewEnemyClass* closest = nullptr;
    float minDistSq = radius * radius;//距離の2乗で比較（高速化）

    for (auto* enemy : _enemies)
    {
        //アクティブでない、自分自身、リーダーではない、死んでいる場合は無視
        if (!enemy->GetActive() ||
            enemy == requester ||
            !enemy->GetIsLeader())continue;
        if (enemy->GetState() == NewEnemyClass::eState::Dead)continue;

        XMVECTOR diff = *enemy->GetModel()->getPosition() - *requester->GetModel()->getPosition();
        float dSq = XMVectorGetX(XMVector3LengthSq(diff));

        if (dSq < minDistSq) {
            minDistSq = dSq;    //近い距離を更新
            closest = enemy;
        }
    }
    return closest;
}

// --- アクティブな敵をすべて非表示（非アクティブ）にする ---
void EnemyPool::HideAllActiveEnemies()
{
    for (auto e : _enemies)
    {
        // アクティブな敵だけを対象にする
        if (e->GetActive())
        {
            // 1. モデル全体の描画をオフ
            if (e->GetModel())
            {
                e->GetModel()->setRenderEnable(false);

                // 2. 各パーツの描画をオフ（ループ変数 j に注意）
                int partsCount = e->GetModel()->getPartsNum();
                for (int j = 0; j < partsCount; j++)
                {
                    // getParts(j) を使用して正しくアクセス
                    e->GetModel()->getParts(j)->setRenderEnable(false);
                }
            }

            // 3. 敵の状態自体を非アクティブにする
            // これをしないと、Update() が走り続けてしまいます
            //e->SetActive(false);
        }
    }
}

void EnemyPool::ShowAllEnemies()
{
    for (auto e : _enemies)
    {
        // アクティブな敵だけを対象にする
        if (e->GetActive())
        {
            // 1. モデル全体の描画をオフ
            if (e->GetModel())
            {
                e->GetModel()->setRenderEnable(true);

                // 2. 各パーツの描画をオフ（ループ変数 j に注意）
                int partsCount = e->GetModel()->getPartsNum();
                for (int j = 0; j < partsCount; j++)
                {
                    // getParts(j) を使用して正しくアクセス
                    e->GetModel()->getParts(j)->setRenderEnable(true);
                }
            }

            // 3. 敵の状態自体を非アクティブにする
            // これをしないと、Update() が走り続けてしまいます
            //e->SetActive(true);
        }
    }
}