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
std::vector<std::wstring> ui_Title =
{
    L"data/image/タイトル画面 - コピー.png", // 0
    L"data/image/Rule1.png",               // 1
    L"data/image/Rule2.png",               // 2
    L"data/image/Rule3.png",               // 3
    L"data/image/Rule4.png",               // 4
    L"data/image/Rule5.png",               // 5
};

//チュートリアルをするかどうか
bool g_isTutorial = false;
bool g_isEndless = false;

namespace
{
    //スタートボタン位置
    constexpr float startButton_x = 650;
    constexpr float startButton_y = 550;

    //チュートリアルスタートボタン位置
    constexpr float tutorialStartButton_x = 650;
    constexpr float tutorialStartButton_y = 450;

    //エンドレススタートボタン位置
    constexpr float endlessStartButton_x = 650;
    constexpr float endlessStartButton_y = 350;

    //ルールボタン
    constexpr float ruleButton_x = 650;
    constexpr float ruleButton_y = 650;

    //左のボタン（戻るなど）位置
    constexpr float leftButton_x = 200;
    constexpr float leftButton_y = 600;

    //右のボタン（進むなど）位置
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
    for (const auto& file : ui_Title)
    {
        vnSprite* pSprite = new vnSprite(
            SCREEN_WIDTH / 2,
            SCREEN_HEIGHT / 2,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            file.c_str());

        pSprite->setRenderEnable(false);
        pSprite->setAlpha(0.8f);
        pSprite->setColor(V_GAME_COLOR_WHITE);

        m_pBackGround.push_back(pSprite);
        registerObject(pSprite);
    }
    //タイトル画面を表示する
    ChangeBackGround(0);


    //スタートボタン
    //pStartButton = new vnSprite(startButton_x, startButton_y, button_w, button_h, L"data/image/選択ボタン形.png");
    pStartButton = new vnSprite(startButton_x, startButton_y, button_w, button_h, L"data/image/無題.png");
    registerObject(pStartButton);

    //チュートリアルスタートボタン
    pTutorialStartButton = new vnSprite(tutorialStartButton_x, tutorialStartButton_y, button_w, button_h, L"data/image/無題.png");
    registerObject(pTutorialStartButton);

    //エンドレススタートボタン
    pEndlessStartButton = new vnSprite(endlessStartButton_x, endlessStartButton_y, button_w, button_h, L"data/image/無題.png");
    registerObject(pEndlessStartButton);

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
        pTutorialStartButton->setRenderEnable(true);
        pEndlessStartButton->setRenderEnable(true);

        pRightButton->setRenderEnable(false);
        pLeftButton->setRenderEnable(false);

        if (!isStarting)
        {
            if (Common::UpdateButton(
                startButton_x,
                startButton_y,
                button_w,
                button_h,
                pStartButton,
                isOnStartButton,
                startButtonScale,
                m_soundManager.get()))
            {
                isStarting = true;
                g_isTutorial = false;
                g_isEndless = false;
                m_soundManager->PlaySE(SE_TITLE_START);
                startType = StartType::Normal;
            }
            if (Common::UpdateButton(
                tutorialStartButton_x,
                tutorialStartButton_y,
                button_w,
                button_h,
                pTutorialStartButton,
                isOnTutorialStartButton,
                tutorialStartButtonScale,
                m_soundManager.get()))
            {
                isStarting = true;
                g_isTutorial = true;
                g_isEndless = false;
                m_soundManager->PlaySE(SE_TITLE_START);
                startType = StartType::Tutorial;
            }

            if (Common::UpdateButton(
                endlessStartButton_x,
                endlessStartButton_y,
                button_w,
                button_h,
                pEndlessStartButton,
                isOnEndlessStartButton,
                endlessStartButtonScale,
                m_soundManager.get()))
            {
                isStarting = true;
                g_isTutorial = false;
                g_isEndless = true;
                m_soundManager->PlaySE(SE_TITLE_START);
                startType = StartType::Endless;
            }

        }
        //=============================
        // スタートボタンを押したら
        //=============================
        else {

            // --- 演出開始：回転しながら巨大化 ---
            if (startType == StartType::Normal)
            {
                OnStartButton(StartType::Normal,
                    startButtonScale,
                    pStartButton);
            }
            else if (startType == StartType::Tutorial)
            {
                OnStartButton(StartType::Tutorial,
                    tutorialStartButtonScale,
                    pTutorialStartButton);
            }
            else if (startType == StartType::Endless)
            {
                OnStartButton(StartType::Endless,
                    endlessStartButtonScale,
                    pEndlessStartButton);
            }
        }
        //-----------------------------

        if (!isStarting)
        {
            //ルールボタン押す
            if (Common::UpdateButton(
                ruleButton_x,
                ruleButton_y,
                button_w,
                button_h,
                pRuleButton,
                isOnRuleButton,
                ruleButtonScale,
                m_soundManager.get()))
            {

                m_soundManager->PlaySE(SE_TITLE_CHANGEPAGE);
                m_titleState = TitleState::RULE;
                m_rulePage = 0; //0はタイトル画面　１はルール説明画面最初
                ChangeBackGround(m_rulePage + 1);

            }
        }




        break;

    case TitleState::RULE:
        pStartButton->setRenderEnable(false);
        pTutorialStartButton->setRenderEnable(false);
        pEndlessStartButton->setRenderEnable(false);
        pRuleButton->setRenderEnable(false);

        // 戻るボタン
        if (Common::UpdateButton(
            leftButton_x,
            leftButton_y,
            button_w,
            button_h,
            pLeftButton,
            isOnLeftButton,
            leftButtonScale,
            m_soundManager.get()))
        {
            m_soundManager->PlaySE(SE_TITLE_CHANGEPAGE);

            if (m_rulePage == 0)
            {
                m_titleState = TitleState::MAIN;
                ChangeBackGround(0);
            }
            else
            {
                --m_rulePage;
                ChangeBackGround(m_rulePage + 1);
            }
        }
        // 進むボタン
        if (Common::UpdateButton(
            rightButton_x,
            rightButton_y,
            button_w,
            button_h,
            pRightButton,
            isOnRightButton,
            rightButtonScale,
            m_soundManager.get()))
        {
            m_soundManager->PlaySE(SE_TITLE_CHANGEPAGE);

            ++m_rulePage;

            if (m_rulePage >= ui_Title.size() - 1)
            {
                m_titleState = TitleState::MAIN;
                ChangeBackGround(0);
            }
            else
            {
                ChangeBackGround(m_rulePage + 1);
            }
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
    deleteObject(pTutorialStartButton);
    deleteObject(pEndlessStartButton);
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


            vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 40));

            Common::ChangeButtonTextSize(ruleButton_x-15, ruleButton_y, ruleButtonScale, isOnRuleButton, L"ルール");

        }

        //===================================================
        // --- ボタンの文字 ---
        //===================================================
        // 最初は常に表示
        if (!isStarting)
        {
            Common::ChangeButtonTextSize(
                startButton_x-15, startButton_y,
                startButtonScale, isOnStartButton,
                L"スタート");

            Common::ChangeButtonTextSize(
                tutorialStartButton_x-40, tutorialStartButton_y,
                tutorialStartButtonScale, isOnTutorialStartButton,
                L"チュートリアル");

            Common::ChangeButtonTextSize(
                endlessStartButton_x-25, endlessStartButton_y,
                endlessStartButtonScale, isOnEndlessStartButton,
                L"エンドレス");
        }
        else
        {
            //switch (startType)
            //{
            //case StartType::Normal:
            //    Common::ChangeButtonTextSize(
            //        startButton_x, startButton_y,
            //        startButtonScale, isOnStartButton,
            //        L"スタート");
            //    break;

            //case StartType::Tutorial:
            //    Common::ChangeButtonTextSize(
            //        tutorialStartButton_x, tutorialStartButton_y,
            //        tutorialStartButtonScale, isOnTutorialStartButton,
            //        L"チュートリアル");
            //    break;

            //case StartType::Endless:
            //    Common::ChangeButtonTextSize(
            //        endlessStartButton_x, endlessStartButton_y,
            //        endlessStartButtonScale, isOnEndlessStartButton,
            //        L"エンドレス");
            //    break;
            //}
        }
    }
       break;

    case SceneTitle::TitleState::RULE:

        // 左ボタン
        if (m_rulePage == 0)
        {
            Common::ChangeButtonTextSize(
                leftButton_x - 10,
                leftButton_y,
                leftButtonScale,
                isOnLeftButton,
                L"タイトル");
        }
        else
        {
            Common::ChangeButtonTextSize(
                leftButton_x - 10,
                leftButton_y,
                leftButtonScale,
                isOnLeftButton,
                L"戻る");
        }

        // 右ボタン
        if (m_rulePage == ui_Title.size() - 2)
        {
            // 最後の説明ページ
            Common::ChangeButtonTextSize(
                rightButton_x - 30,
                rightButton_y,
                rightButtonScale,
                isOnRightButton,
                L"タイトル");
        }
        else
        {
            Common::ChangeButtonTextSize(
                rightButton_x - 10,
                rightButton_y,
                rightButtonScale,
                isOnRightButton,
                L"進む");
        }

        break;

    }

    vnScene::render();
}


//=====================
// 背景の変更
//=====================
//void SceneTitle::ChangeBackGround(TitleUI ui)
//{
//    m_pBackGround[m_currentUI]->setRenderEnable(false);
//    m_pBackGround[ui]->setRenderEnable(true);
//
//    m_currentUI = ui;
//}

void SceneTitle::ChangeBackGround(int index)
{
    m_pBackGround[m_currentUI]->setRenderEnable(false);
    m_pBackGround[index]->setRenderEnable(true);

    m_currentUI = index;
}



void SceneTitle::OnStartButton(StartType type, float& buttonScale, vnSprite* pButton)
{
    // --- 演出開始：回転しながら巨大化 ---

    //ルールボタンは非表示
    pRuleButton->setRenderEnable(false);

    // 押していないボタンを消す
    if (type != StartType::Normal)
        pStartButton->setRenderEnable(false);

    if (type != StartType::Tutorial)
        pTutorialStartButton->setRenderEnable(false);

    if (type != StartType::Endless)
        pEndlessStartButton->setRenderEnable(false);

    // 回転速度を徐々に上げる
    titleRotation += 10.0f;

    // スケールを指数関数的に増やす (1.1倍し続ける)
    buttonScale *= 1.07f;

    // 適用
    pButton->setScale(buttonScale);
    titleRotation += buttonScale * 2.0f;
    pButton->rot = titleRotation;

    // 画面を完全に覆うサイズになったら遷移
    if (buttonScale > 80.0f)
    {
        switchScene(eSceneTable::Main);
    }

}