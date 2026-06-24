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


}

void SoundManager::PlayBGM(BGM_ID id)
{
	if (id == currentBGM)
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
	pSE[id]->play();
}





