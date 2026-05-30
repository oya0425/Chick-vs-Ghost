#include "../framework.h"
#include "../framework/vn_environment.h"
#include "EnemyPool.h"

EnemyPool::EnemyPool()
{
    m_groupDatas.clear();
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

void EnemyPool::Spawn(const XMVECTOR& position, int currentWave, int maxWave)
{
    // タイマーを減らす
    m_spawnTimer -= vnScene::getDeltaTime();

    // まだ0.1秒経っていないなら、何もせず帰る
    if (m_spawnTimer > 0.0f)
    {
        return;
    }

    if (currentWave < maxWave)
    {
        NewEnemyClass* enemy = GetInactiveEnemy();

        if (enemy&&!enemy->GetIsBoss())
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
    else if (currentWave == maxWave)
    {
        NewEnemyClass* enemy = GetInactiveBoss();
        if (enemy)
        {
            enemy->Spawn(position);
            m_spawnTimer = SPAWN_INTERVAL;

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
// --- ボス専用の非アクティブ取得 ---
NewEnemyClass* EnemyPool::GetInactiveBoss()
{
    for (auto e : _enemies)
    {
        // 通常の条件に「かつ、ボスであること」をプラスする
        if ((!e->GetActive() && e->IsUnlocked()) &&
            e->GetState() == NewEnemyClass::eState::Idel &&
            e->GetIsBoss()) // ★ここ！
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

// --- ボスの学習データを入れる ---
void EnemyPool::SetBossData()
{
    if (m_groupDatas.empty())return;
    //最初に０にしておく
    m_bossGroupData->meleeFear = 0.0f;
    m_bossGroupData->rangeFear = 0.0f;

    float pullData = 0;
    for (auto& data : m_groupDatas)
    {
        m_bossGroupData->meleeFear += data->meleeFear / 2;
        m_bossGroupData->rangeFear += data->rangeFear / 2;
        if (pullData <= data->pullResistance)
        {
            pullData = data->pullResistance;
        }
    }
    if (m_bossGroupData->meleeFear < 5.0f)
    {
        m_bossGroupData->meleeFear *= 1.2f;
    }
    m_bossGroupData->pullResistance = pullData;

}


// ------------------------------------------------------------------------
// --- 画面表示用 ---
//------++-----------------------------------------------------------------

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
    float x = (float)vnMainFrame::screenWidth - 300;
    float y = (float)vnMainFrame::screenHeight - 175;
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

    //群の色を取ってくる
    // --- 1. data->id から描画用の unsigned int カラーマクロを特定する ---
    unsigned int displayGroupColor = GAME_COLOR_WHITE; // 見つからなかった時のデフォルトは白

    // パレットから一致するIDのカラーマクロ（GAME_COLOR_xxx）を割り当てる
    switch (data->id) {
    case 1:  displayGroupColor = GAME_COLOR_RED;         break;
    case 2:  displayGroupColor = GAME_COLOR_GREEN;       break;
    case 3:  displayGroupColor = GAME_COLOR_BLUE;        break;
    case 4:  displayGroupColor = GAME_COLOR_YELLOW;      break;
    case 5:  displayGroupColor = GAME_COLOR_OLIVE;       break; 
    case 6:  displayGroupColor = GAME_COLOR_CYAN;        break;
    case 7:  displayGroupColor = GAME_COLOR_ORANGE;      break;
    case 8:  displayGroupColor = GAME_COLOR_PURPLE;      break;
    case 9:  displayGroupColor = GAME_COLOR_BROWN;       break;
    case 10: displayGroupColor = GAME_COLOR_PINK;        break;
    case 11: displayGroupColor = GAME_COLOR_LIME;        break;
    case 12: displayGroupColor = GAME_COLOR_DARK_BLUE;   break; 
    case 13: displayGroupColor = GAME_COLOR_TEAL;        break;
    case 14: displayGroupColor = GAME_COLOR_GOLD;        break;
    case 15: displayGroupColor = GAME_COLOR_NEON_GREEN;  break; 
    case 16: displayGroupColor = GAME_COLOR_DARK_GRAY;   break;
    case 17: displayGroupColor = GAME_COLOR_BLACK;       break;
    case 18: displayGroupColor = GAME_COLOR_GREEN;       break; 
    case 19: displayGroupColor = GAME_COLOR_PINK;        break; 
    case 20: displayGroupColor = GAME_COLOR_RED;         break; 
    default: displayGroupColor = GAME_COLOR_WHITE;       break;
    }


    //表示ロジック
    //タイトル
    // 2. 画面右下の表示位置を計算
    float x = (float)vnMainFrame::screenWidth - 1080;
    float y = (float)vnMainFrame::screenHeight - 512*1.1f;
    float lineYPitch = 35.0f; // 行間
    float lineXPitch = 210.0f;
    vnFont::setFontSize(31, 25);


    // タイトルの表示
    vnFont::print(x, y, GAME_COLOR_ICE_BLUE, L"～敵の群れ情報表示～");

    vnFont::print(x, y + lineYPitch * 1, GAME_COLOR_WHITE, L"番号");
    vnFont::print(x + lineXPitch, y + lineYPitch * 1, GAME_COLOR_WHITE, L"：%d番", m_debugGroupIndex);

    //vnFont::print(x, y + lineYPitch * 2, GAME_COLOR_WHITE, L"番号：%d", data->id);
    //色
    vnFont::print(x, y + lineYPitch * 2, GAME_COLOR_WHITE, L"色");
    vnFont::print(x + lineXPitch, y + lineYPitch * 2, displayGroupColor, L"： %s", data->colorName);
    
    //学習状況
    //項目
    vnFont::print(x, y + lineYPitch * 3, GAME_COLOR_ICE_BLUE, L"～成長値～");
    vnFont::print(x, y + lineYPitch * 4, GAME_COLOR_AMBER, L"近接耐性");
    vnFont::print(x, y + lineYPitch * 5, GAME_COLOR_ELECTRIC_PURPLE, L"範囲攻撃耐性");
    vnFont::print(x, y + lineYPitch * 6, GAME_COLOR_ELECTRIC_CYAN, L"引き寄せ攻撃耐性");
  
    //数値
    vnFont::print(x+ lineXPitch, y + lineYPitch * 4, GAME_COLOR_SUNGLOW, L"：% .2f", data->meleeFear);
    vnFont::print(x+ lineXPitch, y + lineYPitch * 5, GAME_COLOR_NEON_MAGENTA, L"：%.2f", data->rangeFear);
    vnFont::print(x+ lineXPitch, y + lineYPitch * 6, GAME_COLOR_AQUA_GREEN, L"：%.0f%%", data->pullResistance * 100);


    vnFont::print(x, y + lineYPitch * 8, GAME_COLOR_ICE_BLUE, L"～説明～");
    //強化の項目
    vnFont::print(x, y + lineYPitch * 9, GAME_COLOR_AMBER,             L"近接耐性");
    vnFont::print(x, y + lineYPitch * 10, GAME_COLOR_ELECTRIC_PURPLE,         L"範囲攻撃耐性");
    vnFont::print(x, y + lineYPitch * 11, GAME_COLOR_ELECTRIC_CYAN,     L"引き寄せ攻撃耐性");

    //項目の説明
    vnFont::print(x + lineXPitch, y + lineYPitch * 9, GAME_COLOR_SUNGLOW,         L"：基本速度に加算");
    vnFont::print(x + lineXPitch, y + lineYPitch * 10, GAME_COLOR_NEON_MAGENTA, L"：逃げ始める基本範囲に加算(範囲攻撃可能時のみ)");
    vnFont::print(x + lineXPitch, y + lineYPitch * 11, GAME_COLOR_AQUA_GREEN, L"：無効確率に加算");

    vnFont::print(x + lineXPitch * 3.0f, y, GAME_COLOR_YELLOW, L"Tab    ：戻る");
    vnFont::print(x + lineXPitch * 3.0f, y + lineYPitch, GAME_COLOR_YELLOW, L"← / →：群番号切り替え");



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
                    GAME_COLOR_RED
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


void EnemyPool::DebugBossPause()
{
    // ポーズ中の更新処理内
    DrawBossDebugInfo();

}

void EnemyPool::DrawBossDebugInfo()
{
    if (!m_bossGroupData)return;
    auto& data = m_bossGroupData;

    //表示ロジック
    //タイトル
    // 2. 画面右下の表示位置を計算
    float x = (float)vnMainFrame::screenWidth - 1080;
    float y = (float)vnMainFrame::screenHeight - 512;
    float lineYPitch = 35.0f; // 行間
    float lineXPitch = 210.0f;


    vnFont::setFontSize(38, 25);

    //vnFont::print(x - 50, y - linePitch * 1, GAME_COLOR_YELLOW, L" Tab．戻る:　左右Key. 番号切り替え");

    // タイトルの表示
    vnFont::print(x, y, GAME_COLOR_CYAN, L"～ボス情報表示～");

    //vnFont::print(x, y + lineYPitch * 1, GAME_COLOR_WHITE, L"番号：%d番", m_debugGroupIndex);

    ////vnFont::print(x, y + lineYPitch * 2, GAME_COLOR_WHITE, L"番号：%d", data->id);
    ////色
    //vnFont::print(x, y + lineYPitch * 2, GAME_COLOR_WHITE, L"色");
    //vnFont::print(x + lineXPitch, y + lineYPitch * 2, displayGroupColor, L"： %s", data->colorName);
    
    //学習状況
    //項目
    vnFont::print(x, y + lineYPitch * 1, GAME_COLOR_ICE_BLUE, L"～成長値～");
    vnFont::print(x, y + lineYPitch * 2, GAME_COLOR_AMBER, L"近接耐性");
    vnFont::print(x, y + lineYPitch * 3, GAME_COLOR_ELECTRIC_PURPLE, L"範囲攻撃耐性");
    vnFont::print(x, y + lineYPitch * 4, GAME_COLOR_ELECTRIC_CYAN, L"引き寄せ攻撃耐性");
  
    //数値
    vnFont::print(x+ lineXPitch, y + lineYPitch * 5, GAME_COLOR_SUNGLOW, L"：% .2f", data->meleeFear);
    vnFont::print(x+ lineXPitch, y + lineYPitch * 6, GAME_COLOR_NEON_MAGENTA, L"：%.2f", data->rangeFear);
    vnFont::print(x+ lineXPitch, y + lineYPitch * 7, GAME_COLOR_AQUA_GREEN, L"：%.0f%%", data->pullResistance * 100);


    vnFont::print(x, y + lineYPitch * 8, GAME_COLOR_ICE_BLUE, L"～説明～");
    //強化の項目
    vnFont::print(x, y + lineYPitch * 9, GAME_COLOR_AMBER,             L"近接耐性");
    vnFont::print(x, y + lineYPitch * 10, GAME_COLOR_ELECTRIC_PURPLE,         L"範囲攻撃耐性");
    vnFont::print(x, y + lineYPitch * 11, GAME_COLOR_ELECTRIC_CYAN,     L"引き寄せ攻撃耐性");

    //項目の説明
    vnFont::print(x + lineXPitch, y + lineYPitch * 9, GAME_COLOR_SUNGLOW,         L"：基本速度に加算");
    vnFont::print(x + lineXPitch, y + lineYPitch * 10, GAME_COLOR_NEON_MAGENTA, L"：逃げ始める基本範囲に加算(範囲攻撃可能時のみ)");
    vnFont::print(x + lineXPitch, y + lineYPitch * 11, GAME_COLOR_AQUA_GREEN, L"：無効確率に加算");

    vnFont::print(x + lineXPitch * 3.0f, y, GAME_COLOR_YELLOW, L"Tab    ：戻る");
    vnFont::print(x + lineXPitch * 3.0f, y + lineYPitch, GAME_COLOR_YELLOW, L"← / →：群番号切り替え");


}