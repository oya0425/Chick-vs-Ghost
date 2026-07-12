#pragma once
#include <vector>
#include <memory>
enum BGM_ID
{
	BGM_GAME = 0,		//ゲームプレイ中のサウンド
	BGM_GAMEOVER,		//ゲームオーバーになった時に鳴らすBGM
	BGM_GAMECLEAR,		//ゲームクリアになった時に鳴らすBGM

	BGM_TITLE,			

	BGM_MAX
};
enum SE_ID
{
	SE_ENEMY_DEAD = 0,	//敵を倒したときになるSE
	SE_BOSS_ENEMY,		//リーダーを倒したときになるSE
	SE_ENTER,			//Enter押したときになるSE
	SE_WAVE_CLEAR,		//WAVEクリア時に鳴らすSE
	SE_LEVELUP,			//レベルアップ時に鳴らすSE
	SE_AREA_ATTACK,		//範囲攻撃時に鳴らすSE
	SE_PULL_ATTACK,		//引き寄せ攻撃時に鳴らすSE
	SE_SKILL_HEAL,		//スキル回復時に鳴らすSE
	SE_ENEMY_CHARGE,	//特攻状態時に鳴らすSE
	SE_ENEMY_PANIC,		//パニック時に鳴らすSE
	SE_JUMP,			//ジャンプ時に鳴らすSE
	SE_GRILL,			//溶岩に当たってるときの音


	//タイトル画面で使用
	SE_TITLE_START,		//スタートボタン押したときの音
	SE_TITLE_CURSOR,	//カーソルを併せたときの音
	SE_TITLE_CHANGEPAGE,//ルール説明の画面の進むボタン等の音
	SE_MAX
};

class SoundManager
{
private:
	//vnSound** pSound;
	int currentBGM;
	std::vector<std::unique_ptr<vnSound>> pSE;
	std::vector<std::unique_ptr<vnSound>> pBGM;

public:
	SoundManager();
	~SoundManager() = default;
	
	void PlayBGM(BGM_ID id);
	void StopBGM(BGM_ID id);

	void PlaySE(SE_ID id);
	void PlaySEing(SE_ID id);//効果音がなっていたら終わるまで鳴らさない

	void StopSE(SE_ID id);	//効果音を消す



};