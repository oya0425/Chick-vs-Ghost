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

void EnemyPool::ReStartEnemyGroupData()
{
    m_groupDatas.clear();
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
    DebugSetting();

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
        if ((!e->GetActive()&&e->IsUnlocked())&&e->GetState()==NewEnemyClass::eState::Idel)
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


// --- 学習データをグループのIDをもとに返す ---
NewEnemyClass::GroupData* EnemyPool::GetGroupData(int id)
{
    //IDがマイナスなら何もしない
    if (id < 0)return nullptr;

    //指定されたIDを持つデータを検索して返す
    for (auto& data : m_groupDatas)
    {
        if (data->id == id)
        {
            return data.get();//生ポインタを返して操作できるようにする
        }
    }
    //見つからなかった場合
    return nullptr; 

}


void EnemyPool::DebugSetting()
{
    //if (vnKeyboard::trg(DIK_1))m_displayMode = eDisplayMode::LeaderOnly;
    //if (vnKeyboard::trg(DIK_2))m_displayMode = eDisplayMode::OthersOnly;
    //if (vnKeyboard::trg(DIK_3))m_displayMode = eDisplayMode::AllOff;
    //if (vnKeyboard::trg(DIK_4))m_displayMode = eDisplayMode::AllOn;
    if (vnKeyboard::trg(DIK_1)) ChangeDisplayMode(eDisplayMode::LeaderOnly);
    if (vnKeyboard::trg(DIK_2)) ChangeDisplayMode(eDisplayMode::OthersOnly);
    if (vnKeyboard::trg(DIK_3)) ChangeDisplayMode(eDisplayMode::AllOff);
    if (vnKeyboard::trg(DIK_4)) ChangeDisplayMode(eDisplayMode::AllOn);
    // 2. 画面右下の表示位置を計算
    float x = (float)vnMainFrame::screenWidth - 1260.0f;
    float y = (float)vnMainFrame::screenHeight - 150.0f;
    float linePitch = 25.0f; // 行間

    vnFont::setFontSize(38, 25);

    // タイトルの表示
    vnFont::print(x, y, GAME_COLOR_CYAN, L"～敵のメッセージ表示～");

    // 各項目の色を決定（選択中なら明るい色、それ以外は暗いグレーにする）
    DWORD col1 = (m_displayMode == eDisplayMode::LeaderOnly) ? GAME_COLOR_YELLOW : GAME_COLOR_WHITE;
    DWORD col2 = (m_displayMode == eDisplayMode::OthersOnly) ? GAME_COLOR_YELLOW : GAME_COLOR_WHITE;
    DWORD col3 = (m_displayMode == eDisplayMode::AllOff) ? GAME_COLOR_YELLOW : GAME_COLOR_WHITE;
    DWORD col4 = (m_displayMode == eDisplayMode::AllOn) ? GAME_COLOR_YELLOW : GAME_COLOR_WHITE;

    // リストの描画（選択中には「▶」を付けるとさらに分かりやすい）
    vnFont::print(x, y + linePitch * 1, col1, L"%s １．リーダーのみ", (m_displayMode == eDisplayMode::LeaderOnly ? L"▶" : L"　"));
    vnFont::print(x, y + linePitch * 2, col2, L"%s ２．その他のみ", (m_displayMode == eDisplayMode::OthersOnly ? L"▶" : L"　"));
    vnFont::print(x, y + linePitch * 3, col3, L"%s ３．全てオフ", (m_displayMode == eDisplayMode::AllOff ? L"▶" : L"　"));
    vnFont::print(x, y + linePitch * 4, col4, L"%s ４．全てオン", (m_displayMode == eDisplayMode::AllOn ? L"▶" : L"　"));

    vnFont::print(x, y - linePitch * 2, GAME_COLOR_WHITE, L" Tab．群れ情報");

}
// 状態の定義（関数ポインタやクラス等で管理）
void EnemyPool::ChangeDisplayMode(eDisplayMode nextMode) {
    if (m_displayMode == nextMode) return; // 同じ状態なら何もしない

    // 【Exit】現在の状態を抜ける時の処理
    switch (m_displayMode) {
    case eDisplayMode::LeaderOnly:  break;
    case eDisplayMode::OthersOnly:  break;
    case eDisplayMode::AllOff:      break;
    case eDisplayMode::AllOn:       break;
    }

    m_displayMode = nextMode;

    // 【Enter】新しい状態に入る時の処理
    switch (m_displayMode) {
    case eDisplayMode::LeaderOnly:
        break;
    case eDisplayMode::OthersOnly:
        break;
    case eDisplayMode::AllOff:
        break;
    case eDisplayMode::AllOn:
        break;
    }
}
void EnemyPool::DebugPause()
{
    // ポーズ中の更新処理内
    if (vnKeyboard::trg(DIK_LEFT)) {
        ChangeDebugGroupIndex(-1);
    }
    if (vnKeyboard::trg(DIK_RIGHT)) {
        ChangeDebugGroupIndex(1);
    }

    DrawGroupDebugInfo();
    DrawGroupDebugArrow();

}

void EnemyPool::DrawGroupDebugInfo()
{
    if (m_groupDatas.empty())return;
    auto& data = m_groupDatas[m_debugGroupIndex];

    //表示ロジック
    //タイトル
    // 2. 画面右下の表示位置を計算
    float x = (float)vnMainFrame::screenWidth - 1080;
    float y = (float)vnMainFrame::screenHeight - 512;
    float linePitch = 30.0f; // 行間

    vnFont::setFontSize(38, 25);

    vnFont::print(x-50, y - linePitch * 1, GAME_COLOR_YELLOW, L" Tab．戻る:　左右Key. 番号切り替え");

    // タイトルの表示
    vnFont::print(x, y, GAME_COLOR_CYAN, L"～敵の群れ情報表示～");

    vnFont::print(x, y + linePitch * 1, GAME_COLOR_WHITE, L"番号：%d番", m_debugGroupIndex);

    //vnFont::print(x, y + linePitch * 2, GAME_COLOR_WHITE, L"番号：%d", data->id);
    //色
    vnFont::print(x, y + linePitch * 2, GAME_COLOR_WHITE, L"色： %s", data->colorName);
    
    //学習状況
    vnFont::print(x, y + linePitch * 3, GAME_COLOR_CYAN, L"～成長値～");
    vnFont::print(x, y + linePitch * 4, GAME_COLOR_WHITE, L"近接耐性　　　　：%.2f", data->meleeFear);
    vnFont::print(x, y + linePitch * 5, GAME_COLOR_WHITE, L"範囲攻撃耐性　　：%.2f", data->rangeFear);
    vnFont::print(x, y + linePitch * 6, GAME_COLOR_WHITE, L"引き寄せ攻撃耐性：%.0f%%", data->pullResistance*100);

    vnFont::print(x, y + linePitch * 8, GAME_COLOR_CYAN, L"～説明～");
    vnFont::print(x, y + linePitch * 9, GAME_COLOR_WHITE, L"近接耐性　　　　：基本速度に加算");
    vnFont::print(x, y + linePitch * 10, GAME_COLOR_WHITE, L"範囲攻撃耐性　　：逃げ始める基本範囲に加算(範囲攻撃可能時のみ)");
    vnFont::print(x, y + linePitch * 11, GAME_COLOR_WHITE, L"引き寄せ攻撃耐性：無効確率に加算");

}
void EnemyPool::DrawGroupDebugArrow() {
    if (m_groupDatas.empty()) return;

    // 現在選択中のグループデータ
    auto& targetGroup = m_groupDatas[m_debugGroupIndex];

    // リーダーが生きているかチェック
    if (targetGroup->isLeaderAlive) {
        // 全敵リストの中から、このグループIDを持っていて、かつリーダーである敵を探す
        for (auto& enemy : _enemies) {
            if (enemy->GetGroupID() == targetGroup->id && enemy->GetIsLeader()) {

                // リーダーの座標を取得
                XMVECTOR pos = *enemy->GetModel()->getPosition();

                // 少し高い位置に表示するために座標を調整（例：Yを+2.0f）
                pos.m128_f32[1] += 3.0f;
                pos.m128_f32[0] -= 0.4f;

                // 矢印を表示
                EnemyAIDebug::ShowStateArrow(
                    pos,
                    L"▼",      // 下向き矢印
                    GAME_COLOR_YELLOW
                );

                break; // 見つかったらループ終了
            }
        }
    }
}

void EnemyPool::ChangeDebugGroupIndex(int direction)
{
    //1.データがないなら何もしない
    if (m_groupDatas.empty())
    {
        m_debugGroupIndex = -1;
        return;
    }

    //2.インデックスを増減する
    m_debugGroupIndex += direction;

    //3.ループ処理（端に行ったら反対側へ）
    if (m_debugGroupIndex < 0)
    {
        //左へ行きすぎたら一番最後へ
        m_debugGroupIndex = (int)m_groupDatas.size() - 1;

    }
    else if(m_debugGroupIndex>=(int)m_groupDatas.size())
    {
        m_debugGroupIndex = 0;
    }

}