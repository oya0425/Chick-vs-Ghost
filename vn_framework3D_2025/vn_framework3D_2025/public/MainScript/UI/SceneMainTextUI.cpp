#include"../../../framework.h"
#include"../../../framework/vn_environment.h"
#include"../../../public/MainScript/Scene/scene_main.h"

//文字のみを影付きで表示する
void SceneMainTextUI::PrintShadow(
    float x,
    float y,
    unsigned int color,
    unsigned int shadowColor,
    const wchar_t* text)
{
    const float shadowOffset = 3.0f;

    // 影
    vnFont::print(
        x + shadowOffset,
        y + shadowOffset,
        shadowColor,
        text);

    // 本体
    vnFont::print(
        x,
        y,
        color,
        text);
}

//文字のみを影付きで表示する
void SceneMainTextUI::PrintShadowValue_int(
    float x,
    float y,
    unsigned int color,
    unsigned int shadowColor, 
    const wchar_t* text,
    int value)
{
    const float shadowOffset = 3.0f;

    // 影
    vnFont::print(
        x + shadowOffset,
        y + shadowOffset,
        shadowColor,
        text,
        value);

    // 本体
    vnFont::print(
        x,
        y,
        color,
        text,
        value);
}


//================================================================
// --- 待機中の文字の表示 ---
//================================================================
void SceneMainTextUI::RenderIdleText(
    float fontOffset,
    float text_RIGHT_CLICK_x,
    IDWriteTextFormat* pFormat)
{
    const float shadowOffset = 3.0f;

    // スタート案内
    blinkCounter++;

    const float alpha =
        (sinf(blinkCounter * 0.1f) + 1.0f) * 0.5f;

    const unsigned int blinkColor =((unsigned int)(alpha * 255) << 24) |(0x00FFFFFF & GAME_COLOR_WHITE);

    const unsigned int shadowAlpha =((unsigned int)(alpha * 255) << 24);

    vnFont::setFontSize(pFormat, 40);

    vnFont::print(
        text_RIGHT_CLICK_x + shadowOffset,
        600.0f + shadowOffset + fontOffset,
        shadowAlpha,
        L"[RIGHT CLICK] TO START");

    vnFont::print(
        text_RIGHT_CLICK_x,
        600.0f + fontOffset,
        blinkColor,
        L"[RIGHT CLICK] TO START");


    // 操作説明
    const float operationX = 50.0f;
    const float operationY = 50.0f + fontOffset;

    vnFont::setFontSize(pFormat, 50);

    PrintShadow(
        operationX,
        operationY,
        GAME_COLOR_LIME,
        GAME_COLOR_BLACK,
        L"【操作説明】");

    vnFont::setFontSize(pFormat, 30);

    PrintShadow(
        operationX,
        operationY + 70.0f,
        GAME_COLOR_WHITE,
        GAME_COLOR_BLACK,
        L"移動      : W, A, S, D");

    PrintShadow(
        operationX,
        operationY + 120.0f,
        GAME_COLOR_WHITE, 
        GAME_COLOR_BLACK,
        L"ジャンプ    : MOUSE");

    PrintShadow(
        operationX,
        operationY + 170.0f,
        GAME_COLOR_WHITE,
        GAME_COLOR_BLACK,
        L"ポーズ      : TAB");


    // ルール説明
    const float ruleX = 820.0f;
    const float ruleY = 50.0f + fontOffset;

    vnFont::setFontSize(pFormat, 50);

    PrintShadow(
        ruleX,
        ruleY,
        GAME_COLOR_YELLOW,
        GAME_COLOR_BLACK,
        L"【ルール】");

    vnFont::setFontSize(pFormat, 30);

    PrintShadow(
        ruleX,
        ruleY + 60.0f,
        GAME_COLOR_WHITE,
        GAME_COLOR_BLACK,
        L"操作などわからなくなったら");

    PrintShadow(
        ruleX,
        ruleY + 110.0f,
        GAME_COLOR_WHITE,
        GAME_COLOR_BLACK,
        L"ポーズ画面で「振り返り」を押し");

    PrintShadow(
        ruleX,
        ruleY + 160.0f,
        GAME_COLOR_WHITE,
        GAME_COLOR_BLACK,
        L"「基本説明」で基本を確認しよう");

    PrintShadow(
        ruleX,
        ruleY + 260.0f,
        GAME_COLOR_RED,
        GAME_COLOR_BLACK,
        L"※HPが0になるとゲームオーバー");
}

//================================================================
// --- ゲーム中の文字の表示 ---
//================================================================
void SceneMainTextUI::RenderPlayText(
    WaveManager* waveManager,
    bool isTutorial,
    bool isEndless,
    float fontOffset,
    float text_RIGHT_CLICK_x,
    float baseX,
    float baseY,
    bool& showBossText,
    float& bossTextTimer,
    IDWriteTextFormat* pFormat)
{
    const float screenWidth =
        (float)vnMainFrame::screenWidth;

    const float shadowOffset = 3.0f;
    const unsigned int shadowColor = GAME_COLOR_BLACK;

    //==================================================
    // タイム表示
    //==================================================

    vnFont::setFontSize(pFormat, 40);

    const int remainTime =static_cast<int>(waveManager->GetWaveTimer());

    //残り3秒で赤に色を変える
    const unsigned int timeColor =(remainTime <= 3)? GAME_COLOR_RED: GAME_COLOR_WHITE;

    //無限の時間を表示
    const bool showInfinity =waveManager->GetFinalWave() ||isTutorial ||isEndless;

    if (showInfinity)
    {
        PrintShadow(screenWidth / 2 - 30.0f,
            95.0f + fontOffset,
            GAME_COLOR_WHITE,
            shadowColor,
            L" ∞");
    }
    else
    {
        PrintShadowValue_int(screenWidth / 2 - 40.0f,
            95.0f + fontOffset,
            timeColor,
            shadowColor,
            L"%02ds",
            remainTime);
    }
    //==================================================
    // 撃破数
    //==================================================
    vnFont::setFontSize(pFormat, 25);
    int killCount = waveManager->GetTotalKillCount();

    PrintShadowValue_int(
        330.0f,
        100.0f + fontOffset,
        GAME_COLOR_BLACK,
        GAME_COLOR_WHITE,
        L"撃破数：%d体",
        killCount);

    //==================================================
    // WAVE進捗
    //==================================================

    vnFont::print(
        screenWidth / 2 - 25.0f + shadowOffset,
        10.0f + shadowOffset + fontOffset,
        shadowColor,
        L"%d/%d",
        waveManager->GetCurrentWave(),
        waveManager->GetMaxWave());

    vnFont::print(
        screenWidth / 2 - 25.0f,
        10.0f + fontOffset,
        GAME_COLOR_WHITE,
        L"%d/%d",
        waveManager->GetCurrentWave(),
        waveManager->GetMaxWave());

    PrintShadow(screenWidth / 2 - 70.0f,
        35.0f + fontOffset,
        GAME_COLOR_SILVER,
        shadowColor,
        L"WAVE進捗");

    //==================================================
    // WAVE CLEAR
    //==================================================

    if (waveManager->IsWaitingForNext() &&
        waveManager->GetCurrentWave() < waveManager->GetMaxWave())
    {
        vnFont::setFontSize(pFormat, 80);

        PrintShadowValue_int(320.0f,
            220.0f + fontOffset,
            GAME_COLOR_WHITE,
            shadowColor,
            L"WAVE %d CLEAR!!",
            waveManager->GetCurrentWave());

        vnFont::setFontSize(pFormat, 40);

        PrintShadow(430.0f,
            340.0f + fontOffset,
            GAME_COLOR_LIME,
            shadowColor,
            L"NEXT : フィールド拡大");

        // 点滅
        blinkCounter++;

        const float alpha =
            (sinf(blinkCounter * 0.1f) + 1.0f) * 0.5f;
        // 透明度を0～255に変換
        const unsigned int alphaValue =(unsigned int)(alpha * 255);
        // 黒色 + 透明度
        const unsigned int blinkShadow =(alphaValue << 24);
        // シアン色 + 透明度
        const unsigned int blinkColor =(alphaValue << 24) |(GAME_COLOR_CYAN & 0x00FFFFFF);

        PrintShadow(text_RIGHT_CLICK_x,
            470.0f + fontOffset,
            blinkColor,
            blinkShadow,
            L"[RIGHT CLICK] NEXT WAVE");
    }
    //==================================================
    // 操作説明
    //==================================================
    vnFont::setFontSize(pFormat, 25);
    PrintShadow(baseX - 25.0f,
        baseY + 10.0f,
        GAME_COLOR_YELLOW,
        shadowColor,
        L"移動：");

    PrintShadow(baseX - 25.0f,
        baseY - 35.0f,
        GAME_COLOR_YELLOW,
        shadowColor,
        L"ポーズ：");
    //==================================================
    // ボス出現演出
    //==================================================

    if (showBossText)
    {
        float dt = vnScene::getDeltaTime();

        bossTextTimer -= dt;

        if (bossTextTimer <= 0.0f)
        {
            showBossText = false;
        }

        float bossButtonScale = 1.5f;

        if (bossTextTimer > 1.5f)
        {
            float progressTime = 2.0f - bossTextTimer;

            float rate = progressTime / 0.5f;

            bossButtonScale = 0.2f + (1.5f - 0.2f) * rate;
        }

        float currentFontSize = 50.0f * bossButtonScale;

        vnFont::setFontSize(pFormat, (int)currentFontSize);

        float actualTextWidth = currentFontSize * 3.1f;

        float actualTextHeight =currentFontSize * 0.5f;

        float tx =(vnMainFrame::screenWidth / 2.0f)- actualTextWidth;

        float ty =(vnMainFrame::screenHeight / 2.0f)- 50- actualTextHeight;

        float off = 4.0f;

        PrintShadow(
            tx,
            ty + fontOffset,
            GAME_COLOR_RED,
            shadowColor,
            L"～ボス出現～");

    }


    //==================================================
    // ボス撃破数
    //==================================================

    if (waveManager->GetFinalWave())
    {
        int remainBoss =
            waveManager->GetKillBossCountTarget();

        if (remainBoss > 0)
        {
            float off = 4.0f;

            vnFont::setFontSize(pFormat, 30);

            PrintShadow(1130.0f,
                10.0f + fontOffset,
                GAME_COLOR_BLACK,
                GAME_COLOR_WHITE,
                L"ボス");
        }
    }
}

//================================================================
// --- ゲームオーバー時の文字の表示 ---
//================================================================
void SceneMainTextUI::RenderGameOverText(bool isEndless,float maxKillCount,float fontOffset, float text_RIGHT_CLICK_x, IDWriteTextFormat* pFormat)
{
    //==================================================
    // 共通設定
    //==================================================
    const unsigned int shadowColor = GAME_COLOR_BLACK;
    //==================================================
    // GAME OVER
    //==================================================

    // 上下移動（ふわふわ）
    const float offsetY =
        sinf(blinkCounter * 0.08f) * 15.0f;

    vnFont::setFontSize(pFormat, 100);

    PrintShadow(280.0f,
        190.0f + offsetY + fontOffset,
        GAME_COLOR_RED,
        shadowColor,
        L"GAME OVER");

    //==================================================
    // タイトルに戻る案内（点滅）
    //==================================================
    blinkCounter++;
    const float alpha = (sinf(blinkCounter * 0.1f) + 1.0f) * 0.5f;

    const unsigned int alphaValue = (unsigned int)(alpha * 255);

    // 影
    const unsigned int blinkShadow = alphaValue << 24;

    // 本体
    const unsigned int blinkColor = (alphaValue << 24) | (GAME_COLOR_CYAN & 0x00FFFFFF);

    vnFont::setFontSize(pFormat, 40);

    PrintShadow(
        text_RIGHT_CLICK_x,
        450.0f + fontOffset,
        blinkColor,
        blinkShadow,
        L"[RIGHT CLICK]  BACK TITLE");

    //エンドレスモードの時のみ、、最大撃破数を表示する
    if (isEndless)
    {
        PrintShadowValue_int(
            280.0f,
            520.0f + fontOffset,
            GAME_COLOR_WHITE,
            shadowColor,
            L"あなたの最大撃破数 ： %d 体",
            maxKillCount
        );
        PrintShadowValue_int(
            280.0f,
            580.0f + fontOffset,
            GAME_COLOR_AQUA_GREEN,
            shadowColor,
            L"制作者の最大撃破数 ： %d 体",
            92182
        );

    }


}


//================================================================
// --- ゲームクリア時の文字の表示 ---
//================================================================
void SceneMainTextUI::RenderGameClearText(float fontOffset,float text_RIGHT_CLICK_x,IDWriteTextFormat* pFormat)
{
    //==================================================
    // 共通設定
    //==================================================
    const float shadowOffset = 3.0f;
    const unsigned int shadowColor = GAME_COLOR_BLACK;

    //==================================================
    // ALL WAVE CLEAR!!
    //==================================================
    blinkCounter++;
    // クリア文字を上下にふわふわさせる
    const float offsetY = sinf(blinkCounter * 0.08f) * 15.0f;

    vnFont::setFontSize(pFormat, 90);

    PrintShadow(150.0f + shadowOffset,
        200.0f + offsetY + shadowOffset + fontOffset,
        GAME_COLOR_GOLD,
        shadowColor,
        L"ALL WAVE CLEAR!!");

    //==================================================
    // THANK YOU FOR PLAYING!
    //==================================================
    vnFont::setFontSize(pFormat, 50);

    PrintShadow(
        260.0f + shadowOffset,
        450.0f + shadowOffset + fontOffset,
        GAME_COLOR_LIME,
        shadowColor,
        L"THANK YOU FOR PLAYING!");

    //==================================================
    // タイトルに戻る案内（点滅）
    //==================================================
    const float alpha = (sinf(blinkCounter * 0.1f) + 1.0f) * 0.5f;
    const unsigned int alphaValue = (unsigned int)(alpha * 255);
    const unsigned int blinkShadow = alphaValue << 24;
    const unsigned int blinkColor = (alphaValue << 24) | (GAME_COLOR_CYAN & 0x00FFFFFF);

    vnFont::setFontSize(pFormat, 40);

    PrintShadow(text_RIGHT_CLICK_x + shadowOffset,
        600.0f + shadowOffset + fontOffset,
        blinkColor,
        blinkShadow,
        L"[RIGHT CLICK]  BACK TITLE");

}




//================================================================
// --- ゲーム中のポーズ中の文字の表示 ---
//================================================================
void SceneMainTextUI::RenderPauseText(float fontOffset,IDWriteTextFormat* pFormat)
{
    //==================================================
    // 共通設定
    //==================================================
    const float shadowOffset = 3.0f;
    const unsigned int shadowColor = GAME_COLOR_BLACK;
    //==================================================
    // ～遊び方～
    //==================================================
    //遊び方ボタンを押した時にのみ表示
    vnFont::setFontSize(pFormat, 50);

    PrintShadow(
        530.0f + shadowOffset,
        100.0f + shadowOffset + fontOffset,
        GAME_COLOR_WHITE,
        shadowColor,
        L"～遊び方～");

}


//================================================================
// --- チュートリアルクリア時の文字の表示 ---
//================================================================
void SceneMainTextUI::RenderTutorialClearText(
    float textScale,
    float position_x,
    float position_y,
    const wchar_t* text,
    float fontOffset,
    IDWriteTextFormat* pFormat)
{
    float off = 3.0f;
    const unsigned int shadowColor = GAME_COLOR_BLACK;

    // 全てのミッションをクリア
        int fontSize = (int)(70.0f * textScale);

        vnFont::setFontSize(pFormat, fontSize);

        size_t len = wcslen(text);

        // 文字の大きさから幅・高さを計算
        float textWidth = fontSize * 0.95f * len;

        float textHeight = fontSize * 0.5f;

        // 文字の中心を基準に座標を計算
        float tx = position_x - textWidth * 0.5f;
        float ty = position_y - textHeight * 0.5f;

        PrintShadow(
            tx + 30.0f,
            ty + fontOffset,
            GAME_COLOR_YELLOW,
            shadowColor,
            text);
}

