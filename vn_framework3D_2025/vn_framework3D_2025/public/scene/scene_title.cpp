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
    fileNum = sizeof(seFile_Title) / (sizeof(WCHAR) * FILE_PATH_MAX);

    pSound = new vnSound * [fileNum];
    for (int i = 0; i < fileNum; i++)
    {
        pSound[i] = new vnSound(seFile_Title[i]);
    }


    pBackGround = new vnSprite(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH , SCREEN_HEIGHT ,L"data/image/TitleBG.png");
    registerObject(pBackGround);
    pBackGround->setAlpha(0.8f);

    pButton = new vnSprite(650, 500, 250, 80, L"data/image/選択ボタン形.png");
    registerObject(pButton);
    //pBackGround->setAlpha(0.8f);

    pSound[1]->play();
    pSound[1]->setVolume(1.0f);

    isStarting = false;
    isOnButton = false;
    titleRotation = 0.0f;



    return true;
}
void SceneTitle::execute()
{

    if (!isStarting) {
        int mx = vnMouse::getX();
        int my = vnMouse::getY();

        float bx = 650.0f;
        float by = 500.0f;
        float bw = 250.0f;
        float bh = 80.0f;

        // --- 当たり判定 ---
        if (mx >= bx - bw / 2 && mx <= bx + bw / 2 &&
            my >= by - bh / 2 && my <= by + bh / 2) // 中心座標からの判定
        {
            if (isOnButton == false) // まだ「乗っている状態」に切り替わる前なら
            {
                pSound[3]->play();   // 鳴らす
            }

            isOnButton = true;

            // 目標サイズを1.2にする
            buttonScale += (1.2f - buttonScale) * 0.2f;
            // 別の光スプライトを表示させるか、ボタンの色を赤っぽくする
            pButton->setColor(V_GAME_COLOR_BLACK);
            if (vnMouse::trgL()) {
                isStarting = true; // 演出開始！
                pSound[2]->play();   // 鳴らす

                //switchScene(eSceneTable::Main);
            }
        }
        else
        {
            isOnButton = false;
            // 目標サイズを1.0にする
            buttonScale += (1.0f - buttonScale) * 0.2f;
            pButton->setColor(V_GAME_COLOR_YELLOW);

        }
    }
    else {
        // --- 演出開始：回転しながら巨大化 ---

                // 回転速度を徐々に上げる
        titleRotation += 10.0f;

        // スケールを指数関数的に増やす (1.1倍し続ける)
        buttonScale *= 1.05f;

        // 適用
        pButton->setScale(buttonScale);
        titleRotation += (buttonScale * 2.0f);
        pButton->rot = titleRotation; // 直接代入


        // 画面を完全に覆うサイズになったら遷移
        if (buttonScale > 50.0f)
        {
            switchScene(eSceneTable::Main);
        }
    }

    // 更新した値を適用
    pButton->setScale(buttonScale);


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

    if (pSound != NULL)
    {
        for (int i = 0; i < fileNum; i++)
        {
            if (pSound[i] == NULL)continue;
            pSound[i]->stop();
            delete pSound[i];
            pSound[i] = NULL;
        }
        delete[] pSound;
        pSound = NULL;
    }


    deleteObject(pBackGround);
    deleteObject(pButton);
}

//描画
void SceneTitle::render()
{
    float off = 3.0f; // 影のズレ幅
    unsigned int shadowCol = 0xFF000000;

    blinkCounter++;
    // クリア文字を上下に「ふわふわ」させる
    float offsetY = sinf(blinkCounter * 0.08f) * 15.0f;
    if (!isStarting)
    {
        vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), 90));
        vnFont::print(100 + off, 200 + offsetY + off, shadowCol, L"『ひよこ無双：BOX壊滅』");
        vnFont::print(100, 200 + offsetY, GAME_COLOR_GOLD, L"『ひよこ無双：BOX壊滅』");

    }
    // 現在のフォントサイズ
    float currentFontSize = 50.0f * buttonScale;
    vnFont::setTextFormat(vnFont::create(vnFont::getFontName(38), (int)currentFontSize));

    // --- 座標の補正計算 ---
    // 1文字あたりの幅をざっくりフォントサイズの半分と仮定して、
    // 「START」の5文字分が中心に来るように調整します。
    float textWidth = currentFontSize * 1.6f; 
    float textHeight = currentFontSize * 0.5f; // 高さも調整

    // ボタンの中心(650, 500)から引く
    float tx = 650.0f - textWidth;
    float ty = 500.0f - textHeight;

    // 影と本体を描画
    vnFont::print(tx + off, ty + off, shadowCol, L"START");
    if (isOnButton)
    {
        vnFont::print(tx, ty, GAME_COLOR_GOLD, L"START");
    }
    else {
        vnFont::print(tx, ty, GAME_COLOR_WHITE, L"START");
    }


    vnScene::render();
}
