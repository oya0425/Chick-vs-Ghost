#include "../framework.h"
#include "../framework/vn_environment.h"
#include "EnemyPool.h"

namespace
{
    constexpr float ENEMY_GRAPH_CENTER_X = 700.0f;
    constexpr float ENEMY_GRAPH_MAX_W = 300.0f; // baseBarWidth

}

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

void EnemyPool::ReleaseQuestionUI(vnScene* scene)
{
    if (!scene) return;

    // 1. 近接耐性 (m_meleeQus) の解放
    if (m_meleeQus.pQuestionIcon) {
        scene->deleteObject(m_meleeQus.pQuestionIcon);
        m_meleeQus.pQuestionIcon = nullptr;
    }
    if (m_meleeQus.pBalloonBg) {
        scene->deleteObject(m_meleeQus.pBalloonBg);
        m_meleeQus.pBalloonBg = nullptr;
    }

    // 2. 遠距離耐性 (m_rangeQus) の解放
    if (m_rangeQus.pQuestionIcon) {
        scene->deleteObject(m_rangeQus.pQuestionIcon);
        m_rangeQus.pQuestionIcon = nullptr;
    }
    if (m_rangeQus.pBalloonBg) {
        scene->deleteObject(m_rangeQus.pBalloonBg);
        m_rangeQus.pBalloonBg = nullptr;
    }

    // 3. 引き寄せ耐性 (m_pullQus) の解放
    if (m_pullQus.pQuestionIcon) {
        scene->deleteObject(m_pullQus.pQuestionIcon);
        m_pullQus.pQuestionIcon = nullptr;
    }
    if (m_pullQus.pBalloonBg) {
        scene->deleteObject(m_pullQus.pBalloonBg);
        m_pullQus.pBalloonBg = nullptr;
    }
}
//=============================
// ボタンとかUIの状態のリセット
//=============================
void EnemyPool::ResetQuestionUI()
{
    // 1. 共通のスケールを0（最小）に戻す
    m_balloonScale = 0.0f;

    // 2. 各項目のホバー・表示フラグをすべて強制OFF
    m_meleeQus.isHovered = false;
    m_meleeQus.isShowExplain = false;
    m_meleeQus.pBalloonBg->setScale(0.0f);
    m_meleeQus.pBalloonBg->setRenderEnable(false);
    m_meleeQus.pQuestionIcon->setColor(V_GAME_COLOR_WHITE); // 色も白に戻す

    m_rangeQus.isHovered = false;
    m_rangeQus.isShowExplain = false;
    m_rangeQus.pBalloonBg->setScale(0.0f);
    m_rangeQus.pBalloonBg->setRenderEnable(false);
    m_rangeQus.pQuestionIcon->setColor(V_GAME_COLOR_WHITE);

    m_pullQus.isHovered = false;
    m_pullQus.isShowExplain = false;
    m_pullQus.pBalloonBg->setScale(0.0f);
    m_pullQus.pBalloonBg->setRenderEnable(false);
    m_pullQus.pQuestionIcon->setColor(V_GAME_COLOR_WHITE);
}

// マウスが？マークの上に乗っているかを判定する関数
bool EnemyPool::IsMouseOver(const UIQuestionExplain& ui, int mx, int my)
{
    // 構造体に保存されている中心座標（qX, qY）とサイズ（qW, qH）を使う
    float bx = ui.qX;
    float by = ui.qY;
    float bw = ui.qW;
    float bh = ui.qH;

    // 最初の中心座標からの判定式
    if (mx >= bx - bw / 2.0f && mx <= bx + bw / 2.0f &&
        my >= by - bh / 2.0f && my <= by + bh / 2.0f)
    {
        return true;  // 重なっている
    }

    return false; // 重なっていない
}

//？マーク、吹き出しの位置設定
void EnemyPool::SetQuestionUIPos(UIQuestionExplain& ui, float qX, float qY)
{
    // 1. ？マークの位置を設定＆保存（これは項目ごとに変わる）
    ui.pQuestionIcon->setPos(qX, qY);
    ui.qX = qX;
    ui.qY = qY;

    // 2. 吹き出しの固定位置を設定（全項目で同じ場所に表示されるように固定値を指定）
    // ※画面の右上の開いているスペースなどに合わせて、数値を調整してください。
    float fixedBalloonX = 400.0f;  // 例：画面の右側の固定位置
    float fixedBalloonY = 600.0f;  // 例：画面の上側の固定位置
    ui.pBalloonBg->setPos(fixedBalloonX, fixedBalloonY);

    // 3. 説明テキストの位置も、その固定された吹き出しの中に合わせる
    ui.textX = fixedBalloonX - 120.0f;  // 吹き出しの中心からの微調整
    ui.textY = fixedBalloonY - 15.0f;
}

//マウスを持ってきたときに吹き出しを表示
void EnemyPool::UpdateQuestionHover(UIQuestionExplain& ui, int mx, int my)
{
    // マウスが？マークの上にあるかチェック
    ui.isHovered = IsMouseOver(ui, mx, my);

    if (ui.isHovered)
    {
        // マウスが乗っているときは黄色にする
        ui.pQuestionIcon->setColor(V_GAME_COLOR_YELLOW);
    }
    else
    {
        // マウスが離れたら元の色に戻す
        ui.pQuestionIcon->setColor(V_GAME_COLOR_WHITE);
    }

    // ホバー状態と説明表示フラグを連動
    ui.isShowExplain = ui.isHovered;

    // trueなら吹き出し画像を描画する、falseなら描画を消す
    ui.pBalloonBg->setRenderEnable(ui.isShowExplain);
}

//？マークと吹き出しUpdate
void EnemyPool::UpdateAndRenderQuestionUI(float baseX, float baseY, float lineYPitch,
    UIQuestionExplain& melee, UIQuestionExplain& range, UIQuestionExplain& pull)
{
    // マウスの位置を取る
    int mx = vnMouse::getX();
    int my = vnMouse::getY();

    // 1. ？マークの基準のX座標
    float qusX = baseX - 20.0f;

    // 2. 各UIの位置を「固定位置」の吹き出し連動込みで設定
    // ※一番上の倍率は、先ほど調整した3.5fなどに合わせています
    SetQuestionUIPos(melee, qusX, baseY + lineYPitch * 4.3f);
    SetQuestionUIPos(range, qusX, baseY + lineYPitch * 5.3f);
    SetQuestionUIPos(pull, qusX, baseY + lineYPitch * 6.3f);

    // 3. マウスが乗っているか判定し、ホバー状態（色変更など）を更新
    UpdateQuestionHover(melee, mx, my);
    UpdateQuestionHover(range, mx, my);
    UpdateQuestionHover(pull, mx, my);

    // 4. 「どれか1つでも」乗っているか判定してスケールを計算
    bool anyHovered = melee.isHovered || range.isHovered || pull.isHovered;
    if (anyHovered) {
        m_balloonScale += (1.0f - m_balloonScale) * 0.2f;
    }
    else {
        m_balloonScale += (0.0f - m_balloonScale) * 0.2f;
    }

    // 5. 3つの吹き出しスプライトすべてに共通のスケールを適用
    melee.pBalloonBg->setScale(m_balloonScale);
    range.pBalloonBg->setScale(m_balloonScale);
    pull.pBalloonBg->setScale(m_balloonScale);

    // 6. スケールに合わせて文字表示フラグを更新
    melee.isShowExplain = melee.isHovered && (m_balloonScale > 0.5f);
    range.isShowExplain = range.isHovered && (m_balloonScale > 0.5f);
    pull.isShowExplain = pull.isHovered && (m_balloonScale > 0.5f);

    // 7. フラグがONの時だけ、説明テキストを吹き出しの中に印字する
    if (melee.isShowExplain) {
        vnFont::print(melee.textX, melee.textY, melee.textColor, melee.explainText);
    }
    if (range.isShowExplain) {
        vnFont::print(range.textX, range.textY, range.textColor, range.explainText);
    }
    if (pull.isShowExplain) {
        vnFont::print(pull.textX, pull.textY, pull.textColor, pull.explainText);
    }
}


//============================
// バーのセッティング --------
//============================ 
void EnemyPool::SetMeleeBar(const UIBar& bar)
{
    m_meleeBar.pBackBlack = bar.pBackBlack;
    m_meleeBar.pBack = bar.pBack;
    m_meleeBar.pFront = bar.pFront;

}

void EnemyPool::SetRangeBar(const UIBar& bar)
{
    m_rangeBar.pBackBlack = bar.pBackBlack;
    m_rangeBar.pBack = bar.pBack;
    m_rangeBar.pFront = bar.pFront;

}

void EnemyPool::SetPullBar(const UIBar& bar)
{
    m_pullBar.pBackBlack = bar.pBackBlack;
    m_pullBar.pBack = bar.pBack;
    m_pullBar.pFront = bar.pFront;
}

// バーの更新
void EnemyPool::UpdateUIBarHelper(UIBar ui, float currentVal, float maxVal, float leftEdge, float maxW, float posY, XMVECTOR color)
{
    if (!ui.pBack) return; // 念のためヌルチェック
    if (!ui.pBackBlack) return; // 念のためヌルチェック
    if (!ui.pFront) return; // 念のためヌルチェック

    // 1. 比率の計算とクランプ
    float ratio = (maxVal > 0.0f) ? (currentVal / maxVal) : 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    if (ratio < 0.0f) ratio = 0.0f;

    // 2. スケールと座標の適用（左端固定の計算式）(frontの設定)
    float pixelOffset = -10.0f;
    ui.pFront->setScaleX(ratio);
    float posX = leftEdge + (maxW * ratio * 0.5f)+ pixelOffset;
    ui.pFront->setPos(posX, posY);
    ui.pFront->setColor(color);

    // 3.その他のバー設定（Yのみ移動）
    float centerPosX = leftEdge + (maxW * 0.5f) + pixelOffset;
    ui.pBack->setPos(centerPosX, posY);
    ui.pBackBlack->setPos(centerPosX, posY);

}


// --- 全バー表示・非表示 ---
void EnemyPool::ShowHideUI(bool isShow)
{
    //バーの表示非表示
    m_meleeBar.pBackBlack->setRenderEnable(isShow);
    m_meleeBar.pBack     ->setRenderEnable(isShow);
    m_meleeBar.pFront    ->setRenderEnable(isShow);

    m_rangeBar.pBackBlack->setRenderEnable(isShow);
    m_rangeBar.pBack->setRenderEnable(isShow);
    m_rangeBar.pFront->setRenderEnable(isShow);
   
    m_pullBar.pBackBlack->setRenderEnable(isShow);
    m_pullBar.pBack->setRenderEnable(isShow);
    m_pullBar.pFront->setRenderEnable(isShow);

    //ボタンの表示非表示
    m_ImageTab->setRenderEnable(isShow);
    if (!m_isFinalWave)
    {
        m_ImageA->setRenderEnable(isShow);
        m_ImageD->setRenderEnable(isShow);
        m_ImageSlash->setRenderEnable(isShow);

    }
    //お化けの画像の表示非表示
    m_ImageGhost->setRenderEnable(isShow);

    //？と吹き出しの表示非表示
    m_meleeQus.pQuestionIcon->setRenderEnable(isShow);
    m_rangeQus.pQuestionIcon->setRenderEnable(isShow);
    m_pullQus.pQuestionIcon->setRenderEnable(isShow);


}


//--------------------------------------------------------------------------------


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
        if (e->GetPanicMark())
        {
            e->GetPanicMark()->setRenderEnable(false);
        }
        if (e->GetChargeMark())
        {
            e->GetChargeMark()->setRenderEnable(false);
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
        if (e->GetPanicMark())
        {
            e->GetPanicMark()->setRenderEnable(true);
        }
        if (e->GetChargeMark())
        {
            e->GetChargeMark()->setRenderEnable(true);
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

//======================================
// --- ボスの学習データを入れる ---
//======================================
void EnemyPool::SetBossData()
{
    if (m_groupDatas.empty())return;
    //最初に０にしておく
    m_bossGroupData->meleeFear = 0.0f;
    m_bossGroupData->rangeFear = 0.0f;

    //全てのリーダーの学習データを取って来て足す（÷とかでおかしい数値にならないように調整）
    float pullData = 0;
    for (auto& data : m_groupDatas)
    {
        //近接耐性（ボス用に変更（プレイヤーにダメージを与える：通常は特攻状態になる確率アップ））
        m_bossGroupData->meleeFear += data->meleeFear*1.5f;
        m_bossGroupData->rangeFear += data->rangeFear*1.2f;
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
    if (vnKeyboard::trg(DIK_LEFT)||vnKeyboard::trg(DIK_A)) {
        ChangeDebugGroupIndex(-1);
    }
    if (vnKeyboard::trg(DIK_RIGHT)||vnKeyboard::trg(DIK_D)) {
        ChangeDebugGroupIndex(1);
    }

    DrawGroupDebugInfo();
    DrawGroupDebugArrow();

}


//=====================================
// 通常WAVE中に出るデバッグ
//=====================================
void EnemyPool::DrawGroupDebugInfo()
{
    //==================================================
    // 1. 事前チェック & データ取得
    //==================================================
    if (m_groupDatas.empty()) return;
    auto& data = m_groupDatas[m_debugGroupIndex];

    // 入力状態の取得
    bool aPressed = vnKeyboard::on(DIK_A);
    bool dPressed = vnKeyboard::on(DIK_D);

    //==================================================
    // 2. カラーパレットの特定 (data->id から取得)
    //==================================================
    unsigned int displayGroupColor = GAME_COLOR_WHITE;

    switch (data->id) {
    case 1:  displayGroupColor = GAME_COLOR_RED;          break;
    case 2:  displayGroupColor = GAME_COLOR_GREEN;        break;
    case 3:  displayGroupColor = GAME_COLOR_BLUE;         break;
    case 4:  displayGroupColor = GAME_COLOR_YELLOW;       break;
    case 5:  displayGroupColor = GAME_COLOR_OLIVE;        break;
    case 6:  displayGroupColor = GAME_COLOR_CYAN;         break;
    case 7:  displayGroupColor = GAME_COLOR_ORANGE;       break;
    case 8:  displayGroupColor = GAME_COLOR_PURPLE;       break;
    case 9:  displayGroupColor = GAME_COLOR_BROWN;        break;
    case 10: displayGroupColor = GAME_COLOR_PINK;         break;
    case 11: displayGroupColor = GAME_COLOR_LIME;         break;
    case 12: displayGroupColor = GAME_COLOR_DARK_BLUE;    break;
    case 13: displayGroupColor = GAME_COLOR_TEAL;         break;
    case 14: displayGroupColor = GAME_COLOR_GOLD;         break;
    case 15: displayGroupColor = GAME_COLOR_NEON_GREEN;   break;
    case 16: displayGroupColor = GAME_COLOR_DARK_GRAY;    break;
    case 17: displayGroupColor = GAME_COLOR_BLACK;        break;
    case 18: displayGroupColor = GAME_COLOR_GREEN;        break;
    case 19: displayGroupColor = GAME_COLOR_PINK;         break;
    case 20: displayGroupColor = GAME_COLOR_RED;          break;
    default: displayGroupColor = GAME_COLOR_WHITE;        break;
    }

    //==================================================
    // 3. 描画基準座標・レイアウトの計算
    //==================================================
    float x = (float)vnMainFrame::screenWidth - 1080 * 0.95f;
    float y = (float)vnMainFrame::screenHeight - 512 * 1.1f;

    float lineYPitch = 40.0f;  // 行間（縦）
    float lineXPitch = 210.0f; // 列間（横）
    float colonX = x + lineXPitch;

    // フォントサイズ設定
    vnFont::setFontSize(31, 25);

    //==================================================
    // 4. ヘッダー情報の描画 (タイトル / Tab戻る)
    //==================================================
    // タイトル
    vnFont::print(x, y, GAME_COLOR_ICE_BLUE, L"～敵の群れ情報表示～");

    // Tabキー（戻るボタン）
    if (m_ImageTab) {
        float imageX = x + lineXPitch * 3.5f;
        m_ImageTab->setPos(imageX, y + lineYPitch * 1.2f);
        vnFont::print(imageX + 55.0f, y + lineYPitch * 0.8f, GAME_COLOR_YELLOW, L"：戻る");
    }

    //==================================================
    // 5. 群れ選択・基本情報の描画 (A/D切り替え / 色)
    //==================================================
    // 群切り替えテキスト
    vnFont::print(x, y + lineYPitch * 1, GAME_COLOR_WHITE, L"群切り替え");
    vnFont::print(colonX, y + lineYPitch * 1, GAME_COLOR_WHITE, L"：", m_debugGroupIndex);

    // 区切り線（Slash）
    if (m_ImageSlash) {
        m_ImageSlash->setPos(x + lineXPitch * 1.4f, y + lineYPitch * 1.3f);
    }

    // Aボタン
    float buttonAX = colonX + 40.0f;
    if (m_ImageA) {
        m_ImageA->setColor(aPressed ? V_GAME_COLOR_GOLD : V_GAME_COLOR_WHITE);
        m_ImageA->setPos(buttonAX, y + lineYPitch * 1.3f);
    }

    // Dボタン
    float numberX = buttonAX + 50.0f;
    float buttonDX = numberX + 40.0f;
    if (m_ImageD) {
        m_ImageD->setColor(dPressed ? V_GAME_COLOR_GOLD : V_GAME_COLOR_WHITE);
        m_ImageD->setPos(buttonDX, y + lineYPitch * 1.3f);
    }

    // 群れの色情報 & ゴースト画像
    vnFont::print(x, y + lineYPitch * 2, GAME_COLOR_WHITE, L"色");
    vnFont::print(colonX, y + lineYPitch * 2, displayGroupColor, L"： %s", data->colorName);

    if (m_ImageGhost) {
        m_ImageGhost->setPos(x + lineXPitch * 1.3f, y + lineYPitch * 4.2f);
        m_ImageGhost->setColor(data->color);
    }

    //==================================================
    // 6. 成長値（ステータス・棒グラフ）の描画
    //==================================================
    // 項目全体のレイアウトを少し下へオフセット
    float changeY = 100.0f;
    y += changeY;

    // セクションタイトル
    vnFont::print(x, y + lineYPitch * 3, GAME_COLOR_ICE_BLUE, L"～成長値～");

    // ステータス名
    vnFont::print(x, y + lineYPitch * 4, GAME_COLOR_AMBER, L"近接耐性");
    vnFont::print(x, y + lineYPitch * 5, GAME_COLOR_ELECTRIC_PURPLE, L"範囲攻撃耐性");
    vnFont::print(x, y + lineYPitch * 6, GAME_COLOR_ELECTRIC_CYAN, L"引き寄せ攻撃耐性");

    // 現在値
    vnFont::print(colonX, y + lineYPitch * 4, GAME_COLOR_SUNGLOW, L"：% .0f%%", data->meleeFear*100.0f);
    vnFont::print(colonX, y + lineYPitch * 5, GAME_COLOR_NEON_MAGENTA, L"：% .1f", data->rangeFear);
    vnFont::print(colonX, y + lineYPitch * 6, GAME_COLOR_AQUA_GREEN, L"：% .0f%%", data->pullResistance * 100.0f);

    // 最大値
    vnFont::print(x + lineXPitch * 2.8f, y + lineYPitch * 4, GAME_COLOR_SUNGLOW, L"/ % .0f%%", data->maxMeleeFear*100.0f);
    vnFont::print(x + lineXPitch * 2.8f, y + lineYPitch * 5, GAME_COLOR_NEON_MAGENTA, L"/ % .1f", data->maxRangeFear);
    vnFont::print(x + lineXPitch * 2.8f, y + lineYPitch * 6, GAME_COLOR_AQUA_GREEN, L"/ % .0f%%", data->maxPullResistance * 100.0f);

    // 棒グラフ（UIバー）の更新・描画
    float barLeftEdge = ENEMY_GRAPH_CENTER_X - (ENEMY_GRAPH_MAX_W * 0.5f);

    UpdateUIBarHelper(m_meleeBar, data->meleeFear, data->maxMeleeFear,
        barLeftEdge, ENEMY_GRAPH_MAX_W, y + lineYPitch * 4.3f, V_GAME_COLOR_SUNGLOW);

    UpdateUIBarHelper(m_rangeBar, data->rangeFear, data->maxRangeFear,
        barLeftEdge, ENEMY_GRAPH_MAX_W, y + lineYPitch * 5.3f, V_GAME_COLOR_NEON_MAGENTA);

    UpdateUIBarHelper(m_pullBar, data->pullResistance, data->maxPullResistance,
        barLeftEdge, ENEMY_GRAPH_MAX_W, y + lineYPitch * 6.3f, V_GAME_COLOR_AQUA_GREEN);

    //==================================================
    // 7. 特殊UI（？マークと説明吹き出し）の処理
    //==================================================
    UpdateAndRenderQuestionUI(x, y, lineYPitch, m_meleeQus, m_rangeQus, m_pullQus);
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
    m_isFinalWave = true;
}

void EnemyPool::DrawBossDebugInfo()
{
    //==================================================
    // 1. 事前チェック & データ取得
    //==================================================
    if (!m_bossGroupData) return;
    auto& data = m_bossGroupData;

    // 既存の近接説明文を「ボス用」に上書きする
    m_meleeQus.explainText = L"：プレイヤーに\n 与えるダメージに加算";

    //==================================================
    // 2. 描画基準座標・レイアウトの計算
    //==================================================
    // ★ ザコ敵側と完全に同じ計算式・スケール（0.95f など）に統一します
    float x = (float)vnMainFrame::screenWidth - 1080.0f * 0.95f;
    float y = (float)vnMainFrame::screenHeight - 512.0f * 1.1f;

    float lineYPitch = 40.0f; // ザコ敵と同じ 40.0f に統一
    float lineXPitch = 210.0f;
    float colonX = x + lineXPitch;

    vnFont::setFontSize(31, 25);

    //==================================================
    // 3. ヘッダー情報の描画 (タイトル)
    //==================================================
    vnFont::print(x, y, GAME_COLOR_CYAN, L"～ボス情報表示～");

    //==================================================
    // 4. お化け画像
    //==================================================
        // 区切り線（Slash）
    if (m_ImageSlash) {
        m_ImageSlash->setRenderEnable(false);
    }
    // Aボタン
    if (m_ImageA) {
        m_ImageA->setRenderEnable(false);
    }

    // Dボタン
    if (m_ImageD) {
        m_ImageD->setRenderEnable(false);
    }
    if (m_ImageGhost) {
        m_ImageGhost->setRenderEnable(true);
        m_ImageGhost->setScale(2);
        m_ImageGhost->setColor(data->color);
        m_ImageGhost->setPos(x + lineXPitch * 1.5,y+lineYPitch*3);
    }


    //==================================================
    // 5. 成長値（ステータス）の描画
    //==================================================
    // ★【ここが重要！】ザコ敵側と同じ位置からスタートさせるため、
    // ボス側でも成長値のセクションの前に offset（changeY）を足します。
    float changeY = 100.0f;
    y += changeY;

    vnFont::print(x, y + lineYPitch * 3.0f, GAME_COLOR_ICE_BLUE, L"～成長値～");

    vnFont::print(x, y + lineYPitch * 4.0f, GAME_COLOR_AMBER, L"近接耐性");
    vnFont::print(x, y + lineYPitch * 5.0f, GAME_COLOR_ELECTRIC_PURPLE, L"範囲攻撃耐性");
    vnFont::print(x, y + lineYPitch * 6.0f, GAME_COLOR_ELECTRIC_CYAN, L"引き寄せ攻撃耐性");

    vnFont::print(colonX, y + lineYPitch * 4.0f, GAME_COLOR_SUNGLOW, L"：% .1f", data->meleeFear);
    vnFont::print(colonX, y + lineYPitch * 5.0f, GAME_COLOR_NEON_MAGENTA, L"：% .1f", data->rangeFear);
    vnFont::print(colonX, y + lineYPitch * 6.0f, GAME_COLOR_AQUA_GREEN, L"：% .0f%%", data->pullResistance * 100.0f);

    vnFont::print(x + lineXPitch * 2.8f, y + lineYPitch * 4.0f, GAME_COLOR_SUNGLOW, L"/ % .1f", data->maxBossMeleeFear);
    vnFont::print(x + lineXPitch * 2.8f, y + lineYPitch * 5.0f, GAME_COLOR_NEON_MAGENTA, L"/ % .1f", data->maxBossRangeFear);
    vnFont::print(x + lineXPitch * 2.8f, y + lineYPitch * 6.0f, GAME_COLOR_AQUA_GREEN, L"/ % .0f%%", data->maxPullResistance * 100.0f);

    //==================================================
    // 6. 棒グラフ（UIバー）の更新・描画
    //==================================================
    float barLeftEdge = ENEMY_GRAPH_CENTER_X - (ENEMY_GRAPH_MAX_W * 0.5f);

    // 行番号（* 4.3f / 5.3f / 6.3f）もザコ敵側と完全に一致させます
    UpdateUIBarHelper(m_meleeBar, data->meleeFear, data->maxBossMeleeFear,
        barLeftEdge, ENEMY_GRAPH_MAX_W, y + lineYPitch * 4.3f, V_GAME_COLOR_SUNGLOW);

    UpdateUIBarHelper(m_rangeBar, data->rangeFear, data->maxBossRangeFear,
        barLeftEdge, ENEMY_GRAPH_MAX_W, y + lineYPitch * 5.3f, V_GAME_COLOR_NEON_MAGENTA);

    UpdateUIBarHelper(m_pullBar, data->pullResistance, data->maxPullResistance,
        barLeftEdge, ENEMY_GRAPH_MAX_W, y + lineYPitch * 6.3f, V_GAME_COLOR_AQUA_GREEN);

    //==================================================
    // 7. 特殊UI（？マークと説明吹き出し）の処理
    //==================================================
    UpdateAndRenderQuestionUI(x, y, lineYPitch, m_meleeQus, m_rangeQus, m_pullQus);
}