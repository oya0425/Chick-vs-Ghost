#include "../framework.h"
#include "../framework/vn_environment.h"
#include"../public/SoundManager.h"
// --- BGM ---
#define FILE_PATH_MAX	(256)


WCHAR bgmFile_Main[][FILE_PATH_MAX] =
{
	L"data/sound/灼熱のユーロビート.wav",    //ゲームプレイ中のサウンド
	L"data/sound/maou_bgm_orchestra26.wav",	 //ゲームオーバーになった時に鳴らすBGM
	L"data/sound/maou_bgm_acoustic40.wav",	 //ゲームクリアになった時に鳴らすBGM
	L"data/sound/maou_bgm_piano04.wav",	     //タイトル画面のBGM

};
WCHAR seFile_Main[][FILE_PATH_MAX] =
{
	L"data/sound/maou_se_battle_gun02.wav",		 //敵を倒したときになるSE
	L"data/sound/ボス登場.wav",					 //リーダーを倒したときになるSE
	L"data/sound/システム決定音_8.wav",			 //Enter押したときになるSE
	L"data/sound/キラキラ効果音.wav",			 //WAVEクリア時に鳴らすSE
	L"data/sound/キラキラした音.wav",		     //レベルアップ時に鳴らすSE
	L"data/sound/AreaAttack.wav",				 //範囲攻撃時に鳴らすSE
	L"data/sound/PullAttack.wav",				 //引き寄せ攻撃時に鳴らすSE
	L"data/sound/スキル回復.wav",				 //スキル回復時に鳴らすSE
	L"data/sound/特攻状態変化.wav",				 //特攻状態時に鳴らすSE
	L"data/sound/パニック状態変化.wav",		     //パニック時に鳴らすSE
	L"data/sound/Jump.wav",					     //ジャンプ時に鳴らすSE
	L"data/sound/目玉焼きを焼く.wav",			 //溶岩に当たってる時に鳴らすSE

	//タイトル画面で使用
	L"data/sound/maou_se_battle03.wav",		//STARTボタン押したときの音
	L"data/sound/maou_se_system10.wav",		//STARTボタンにカーソルを合わせたときの音(お気に入り)
	L"data/sound/PageChange.wav",	//進むボタン等の音

};


SoundManager::SoundManager()
{
	currentBGM = -1;
	//BGMをセッティング
	int BGMfileNum = sizeof(bgmFile_Main) / (sizeof(WCHAR) * FILE_PATH_MAX);

	for (int i = 0; i < BGMfileNum; i++)
	{
		pBGM.push_back(std::make_unique<vnSound>(bgmFile_Main[i]));
	}
	
	//効果音をセッティング
	int SEfileNum = sizeof(seFile_Main) / (sizeof(WCHAR) * FILE_PATH_MAX);

	for (int i = 0; i < SEfileNum; i++)
	{
		pSE.push_back(std::make_unique<vnSound>(seFile_Main[i]));
	}
	//=====================================
	// 音量調整
	//=====================================
	pSE[SE_AREA_ATTACK]->setVolume(2);
	pSE[SE_PULL_ATTACK]->setVolume(2);
	pSE[SE_SKILL_HEAL]->setVolume(2);
	pSE[SE_ENEMY_CHARGE]->setVolume(2);
	pSE[SE_ENEMY_PANIC]->setVolume(2);
	pSE[SE_JUMP]->setVolume(1.5f);
	pSE[SE_GRILL]->setVolume(20.5f);

	pSE[SE_TITLE_CHANGEPAGE]->setVolume(2);

}

void SoundManager::PlayBGM(BGM_ID id)
{
	if (id == currentBGM&&pBGM[id]->isPlaying())
	{
		return;
	}
	//鳴っているBGMを止める
	if (currentBGM != -1 && pBGM[currentBGM])
	{
		pBGM[currentBGM]->stop();
	}
	//新しいBGMを鳴らす
	if (pBGM[id])
	{
		pBGM[id]->play(true);
		
		currentBGM = id;
	}

}
void SoundManager::StopBGM(BGM_ID id)
{
	if (id == currentBGM)
	{
		return;
	}
	else
	{
		if (currentBGM != -1)
		{
			pBGM[currentBGM]->stop();

		}
	}

	if (!pBGM[id]->isPlaying())
	{
		pBGM[id]->play(true);
		currentBGM = id;
	}


}

void SoundManager::PlaySE(SE_ID id)
{
	//if (pSE[id]->isPlaying())return;
	pSE[id]->play();
}

void SoundManager::PlaySEing(SE_ID id)
{
	if (pSE[id]->isPlaying())return;
	pSE[id]->play();
}
void SoundManager::StopSE(SE_ID id)
{
	if (!pSE[id]->isPlaying())return;
	pSE[id]->stop();
}





