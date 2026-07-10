#include "../../framework.h"
#include "../../framework/vn_environment.h"

#define FILE_PATH_MAX	(256)

WCHAR seFile_Title[][FILE_PATH_MAX] =
{
    L"data/sound/decision1.wav",
    L"data/sound/maou_bgm_piano04.wav",
    L"data/sound/maou_se_battle03.wav",	 //STARTボタン押したときの音
    L"data/sound/maou_se_system10.wav",	 //STARTボタンにカーソルを合わせたときの音(お気に入り)

};
//タイトル画面の背景（ルールとか）
WCHAR ui_Title[][FILE_PATH_MAX] =
{
    L"data/image/タイトル画面 - コピー.png",
    //L"data/image/タイトル画面.bmp",
    L"data/image/説明1.png",
    L"data/image/説明2.png",
    L"data/image/説明3.png",
    L"data/image/説明4.png",


};


namespace
{
    //スタートボタン位置
    constexpr float startButton_x = 650;
    constexpr float startButton_y = 500;

    //ルールボタン
    constexpr float ruleButton_x = 650;
    constexpr float ruleButton_y = 600;
    //スタートボタン位置
    constexpr float leftButton_x = 200;
    constexpr float leftButton_y = 600;
    //スタートボタン位置
    constexpr float rightButton_x = 1100;
    constexpr float rightButton_y = 600;


    //全ボタンサイズ
    constexpr float button_w = 250;
    constexpr float button_h = 80;


}

bool SceneTitle::initialize()
{
    //---フォント--------
    //使用できるフォントの数を取得

    FontNum = vnFont::getFontNum();

    //作成したフォント用の情報を確保しておく変数を必要な数作成
    textFormat_score = new IDWriteTextFormat * [FontNum];

    for (int i = 0; i < FontNum; i++)
    {
        //フォント名とサイズを指定してフォントを作成(フォント名は直接指定することも可能)
        textFormat_score[i] = vnFont::create(vnFont::getFontName(i), 100);
    }

    // --- BGM ---
    m_soundManager = std::make_unique<SoundManager>();


    //================================================
    // 背景設定
    //================================================
    int uifileNum = sizeof(ui_Title) / (sizeof(WCHAR) * FILE_PATH_MAX);

    for (int i = 0; i < uifileNum; i++)
    {
        vnSprite* pSprite = new vnSprite(
            SCREEN_WIDTH / 2,
            SCREEN_HEIGHT / 2,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            ui_Title[i]
        );

        pSprite->setRenderEnable(false);
        pSprite->setAlpha(0.8f);
        pSprite->setColor(V_GAME_COLOR_WHITE);

        m_pBackGround.push_back(pSprite);
        registerObject(pSprite);
    }

    m_pBackGround[TITLE_MAIN]->setRenderEnable(true);

    //pBackGround = new vnSprite(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH , SCREEN_HEIGHT ,L"data/image/TitleBG.png");
    //pBackGround = new vnSprite(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH , SCREEN_HEIGHT ,L"data/image/タイトル画面.png");
    //pBackGround = new vnSprite(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH , SCREEN_HEIGHT ,L"data/image/説明１.png");
    //pBackGround = new vnSprite(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH , SCREEN_HEIGHT ,L"data/image/説明２.png");
    //pBackGround = new vnSprite(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH , SCREEN_HEIGHT ,L"data/image/説明３.png");
    //pBackGround = new vnSprite(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH , SCREEN_HEIGHT ,L"data/image/説明４.png");

    //スタートボタン
    //pStartButton = new vnSprite(startButton_x, startButton_y, button_w, button_h, L"data/image/選択ボタン形.png");
    pStartButton = new vnSprite(startButton_x, startButton_y, button_w, button_h, L"data/image/無題.png");
    registerObject(pStartButton);
    //ルールボタン
    pRuleButton = new vnSprite(ruleButton_x, ruleButton_y, button_w, button_h, L"data/image/選択ボタン形.png");
    pRuleButton = new vnSprite(ruleButton_x, ruleButton_y, button_w, button_h, L"data/image/無題.png");
    registerObject(pRuleButton);
    //左ボタン（戻るボタン）
    pLeftButton = new vnSprite(leftButton_x, leftButton_y, button_w, button_h, L"data/image/選択ボタン形.png");
    registerObject(pLeftButton);
    //右ボタン（進むボタン）
    pRightButton = new vnSprite(rightButton_x, rightButton_y, button_w, button_h, L"data/image/選択ボタン形.png");
    registerObject(pRightButton);

    pStartButton->setRenderEnable(false);
    pRuleButton->setRenderEnable(false);
    pLeftButton->setRenderEnable(false);
    pRightButton->setRenderEnable(false);

    //pBackGround->setAlpha(0.8f);

    m_soundManager->PlayBGM(BGM_TITLE);

    isStarting = false;
    titleRotation = 0.0f;
    isOnStartButton  =false;
    isOnRuleButton   =false;
    isOnLeftButton   =false;
    isOnRightButton  =false;



    return true;
}
void SceneTitle::execute()
{
    m_soundManager->PlayBGM(BGM_TITLE);

    int mx = vnMouse::getX(); 
    int my = vnMouse::getY();

    switch (m_titleState)
    {
    case SceneTitle::TitleState::MAIN:
        pStartButton->setRenderEnable(true);
        pRuleButton->setRenderEnable(true);
        pRightButton->setRenderEnable(false);
        pLeftButton->setRenderEnable(false);

        if (!isStarting)
        {
            if (UpdateButton(
                startButton_x,
                startButton_y,
                pStartButton,
                isOnStartButton,
                startButtonScale))
            {
                isStarting = true;
                m_soundManager->PlaySE(SE_TITLE_START);
            }
        }
        //=============================
        // スタートボタンを押したら
        //=============================
        else {
            // --- 演出開始：回転しながら巨大化 ---

            //ルールボタンは非表示
            pRuleButton->setRenderEnable(false);

                    // 回転速度を徐々に上げる
            titleRotation += 10.0f;

            // スケールを指数関数的に増やす (1.1倍し続ける)
            startButtonScale *= 1.05f;

            // 適用
            pStartButton->setScale(startButtonScale);
            titleRotation += (startButtonScale * 2.0f);
            pStartButton->rot = titleRotation; // 直接代入


            // 画面を完全に覆うサイズになったら遷移
            if (startButtonScale > 70.0f)
            {
                switchScene(eSceneTable::Main);
            }
        }
        //-----------------------------

        //ルールボタン押す
        if (UpdateButton(
            ruleButton_x,
            ruleButton_y,
            pRuleButton,
            isOnRuleButton,
            ruleButtonScale))
        {

            m_soundManager->PlaySE(SE_TITLE_CHANGEPAGE);
            m_titleState = TitleState::RULE1;
            ChangeBackGround(TITLE_RULE1);
        }




        break;
    case SceneTitle::TitleState::RULE1:
        //Start,ルールボタンを隠して、進むボタンのみ表示
        pStartButton->setRenderEnable(false);
        pRuleButton->setRenderEnable(false);
        pLeftButton->setRenderEnable(false);
        pRightButton->setRenderEnable(false);

        //戻るボタン
        if (UpdateButton(
            leftButton_x,
            leftButton_y,
            pLeftButton,
            isOnLeftButton,
            leftButtonScale))
        {

            m_soundManager->PlaySE(SE_TITLE_CHANGEPAGE);
            m_titleState = TitleState::MAIN;
            ChangeBackGround(TITLE_MAIN);
        }

        //進むボタン
        if (UpdateButton(
            rightButton_x,
            rightButton_y,
            pRightButton,
            isOnRightButton,
            rightButtonScale))
        {

            m_soundManager->PlaySE(SE_TITLE_CHANGEPAGE);
            m_titleState = TitleState::RULE2;
            ChangeBackGround(TITLE_RULE2);
        }



        break;
    case SceneTitle::TitleState::RULE2:
        //戻るボタンも表示する
        //pLeftButton->setRenderEnable(true);
        
        //戻るボタン
        if (UpdateButton(
            leftButton_x,
            leftButton_y,
            pLeftButton,
            isOnLeftButton,
            leftButtonScale))
        {

            m_soundManager->PlaySE(SE_TITLE_CHANGEPAGE);
            m_titleState = TitleState::RULE1;
            ChangeBackGround(TITLE_RULE1);
        }


        //進むボタン（右ボタン）
        if (UpdateButton(
            rightButton_x,
            rightButton_y,
            pRightButton,
            isOnRightButton,
            rightButtonScale))
        {

            m_soundManager->PlaySE(SE_TITLE_CHANGEPAGE);
            m_titleState = TitleState::RULE3;
            ChangeBackGround(TITLE_RULE3);
        }

        break;
    case SceneTitle::TitleState::RULE3:
        //戻るボタン
        if (UpdateButton(
            leftButton_x,
            leftButton_y,
            pLeftButton,
            isOnLeftButton,
            leftButtonScale))
        {

            m_soundManager->PlaySE(SE_TITLE_CHANGEPAGE);
            m_titleState = TitleState::RULE2;
            ChangeBackGround(TITLE_RULE2);
        }


        //進むボタン（右ボタン）
        if (UpdateButton(
            rightButton_x,
            rightButton_y,
            pRightButton,
            isOnRightButton,
            rightButtonScale))
        {

            m_soundManager->PlaySE(SE_TITLE_CHANGEPAGE);
            m_titleState = TitleState::RULE4;
            ChangeBackGround(TITLE_RULE4);
        }



        break;
    case SceneTitle::TitleState::RULE4:
        //戻るボタン
        if (UpdateButton(
            leftButton_x,
            leftButton_y,
            pLeftButton,
            isOnLeftButton,
            leftButtonScale))
        {

            m_soundManager->PlaySE(SE_TITLE_CHANGEPAGE);
            m_titleState = TitleState::RULE3;
            ChangeBackGround(TITLE_RULE3);
        }


        //進むボタン（右ボタン）
        if (UpdateButton(
            rightButton_x,
            rightButton_y,
            pRightButton,
            isOnRightButton,
            rightButtonScale))
        {

            m_soundManager->PlaySE(SE_TITLE_CHANGEPAGE);
            m_titleState = TitleState::MAIN;
            ChangeBackGround(TITLE_MAIN);
        }
        break;
    }

}

//終了
void SceneTitle::terminate()
{
    //---フォント---
//描画フォントをデフォルトに戻す
    vnFont::setTextFormat();

    for (int i = 0; i < FontNum; i++)
    {
        //作成したフォントを削除する
        SAFE_RELEASE(textFormat_score[i]);
    }
    //作成したフォント用の情報を確保しておく変数を削除
    delete[] textFormat_score;

    int uifileNum = sizeof(ui_Title) / (sizeof(WCHAR) * FILE_PATH_MAX);

    for (int i = 0; i < uifileNum; i++)
    {
        deleteObject(m_pBackGround[i]);
    }
    deleteObject(pStartButton);
    deleteObject(pRuleButton);
    deleteObject(pLeftButton);
    deleteObject(pRightButton);
}

//描画
void SceneTitle::render()
{
    switch (m_titleState)
    {
    case SceneTitle::TitleState::MAIN:
    {
        float off = 5.0f; // 影のズレ幅
        unsigned int shadowCol = 0xFF000000;

        blinkCounter++;
        // クリア文字を上下に「ふわふわ」させる
        float offsetY = sinf(blinkCounter * 0.08f) * 15.0f;
        if (!isStarting)
        {
            vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 90));
            vnFont::print(250 + off, 200 + offsetY + off, shadowCol, L"『Chick vs Ghost』");
            vnFont::print(250, 200 + offsetY, GAME_COLOR_GOLD, L"『Chick vs Ghost』");

            ChangeButtonTextSize(ruleButton_x, ruleButton_y, ruleButtonScale, isOnRuleButton, L"RULES");

        }

        ChangeButtonTextSize(startButton_x, startButton_y, startButtonScale, isOnStartButton, L"START");
    }
        break;
    case SceneTitle::TitleState::RULE1:
        ChangeButtonTextSize(rightButton_x-10, rightButton_y, rightButtonScale, isOnRightButton, L"進む");
        ChangeButtonTextSize(leftButton_x - 10, leftButton_y, leftButtonScale, isOnLeftButton, L"タイトル");

        break;
    case SceneTitle::TitleState::RULE2:
        ChangeButtonTextSize(leftButton_x-10, leftButton_y, leftButtonScale, isOnLeftButton, L"戻る");
        ChangeButtonTextSize(rightButton_x-10, rightButton_y, rightButtonScale, isOnRightButton, L"進む");

        break;
    case SceneTitle::TitleState::RULE3:
        ChangeButtonTextSize(leftButton_x-10, leftButton_y, leftButtonScale, isOnLeftButton, L"戻る");
        ChangeButtonTextSize(rightButton_x-10, rightButton_y, rightButtonScale, isOnRightButton, L"進む");

        break;
    case SceneTitle::TitleState::RULE4:
        ChangeButtonTextSize(leftButton_x-10, leftButton_y, leftButtonScale, isOnLeftButton, L"戻る");
        ChangeButtonTextSize(rightButton_x-30, rightButton_y, rightButtonScale, isOnRightButton, L"タイトル");

        break;
    }

    vnScene::render();
}

//ボタン当たり判定
bool SceneTitle::OnButton(float x, float y)
{
    int mx = vnMouse::getX();
    int my = vnMouse::getY();

    if (mx >= x - button_w / 2 && mx <= x + button_w / 2 &&
        my >= y - button_h / 2 && my <= y + button_h / 2) // 中心座標からの判定
    {
        return true;
    }
    else
    {
        return false;
    }
}

//ボタン処理（ボタン押したときにtrue）
bool SceneTitle::UpdateButton(
    float x,
    float y,
    vnSprite* pButton,
    bool& isOnButton,
    float& buttonScale)
{
    if (OnButton(x, y))
    {
        if (!isOnButton)
        {
            m_soundManager->PlaySE(SE_TITLE_CURSOR);
        }

        isOnButton = true;

        buttonScale += (1.2f - buttonScale) * 0.2f;
        pButton->setColor(V_GAME_COLOR_BLACK);

        if (vnMouse::trgL())
        {
            return true;
        }
    }
    else
    {
        isOnButton = false;

        buttonScale += (1.0f - buttonScale) * 0.2f;
        pButton->setColor(V_GAME_COLOR_YELLOW);
    }

    pButton->setScale(buttonScale);

    return false;
}

//=====================
// 背景の変更
//=====================
void SceneTitle::ChangeBackGround(TitleUI ui)
{
    m_pBackGround[m_currentUI]->setRenderEnable(false);
    m_pBackGround[ui]->setRenderEnable(true);

    m_currentUI = ui;
}


//=========================================================
// 文字の大きさ変更（ボタンに合わせた大きさにする）
//=========================================================
void SceneTitle::ChangeButtonTextSize(float x,float y,float fontScale, bool isOnButton, const WCHAR* text)
{
    float off = 4.0f; // 影のズレ幅
    unsigned int shadowCol = GAME_COLOR_WHITE;

    //文字数の取得
    int len = wcslen(text);

    // 現在のフォントサイズ
    float currentFontSize = 45.0f * fontScale;
    vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), (int)currentFontSize));

    // --- 座標の補正計算 ---
    // 1文字あたりの幅をざっくりフォントサイズの半分と仮定して、
    // 「START」の5文字分が中心に来るように調整します。
    //float textWidth = currentFontSize * 1.6f;
    //float textHeight = currentFontSize * 0.5f; // 高さも調整
    
    float textWidth = currentFontSize * 0.65f * (len);
    float textHeight = currentFontSize * 0.5f;

    float tx = x - textWidth*0.5f;
    float ty = y - textHeight;

    // ボタンの中心(650, 500)から引く
    //float tx = x - textWidth;
    //float ty = y - textHeight;

    // 影と本体を描画
    vnFont::print(tx + off, ty + off*0.3, shadowCol, text);
    if (isOnButton)
    {
        vnFont::print(tx, ty, GAME_COLOR_RED, text);
    }
    else {
        vnFont::print(tx, ty, GAME_COLOR_BLACK, text);
    }



}