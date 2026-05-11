#pragma once
#include<memory>	//unique_ptr
class SkillButtonUI
{
public:
	SkillButtonUI(
		vnSprite* icon,
		vnSprite* shadow,
		float maxCoolTime);
	
	~SkillButtonUI() = default;

	// プレイヤーのタイマーをそのまま渡して更新する
	void Update(float currentTimer);
	
private:
	vnSprite *m_pIcon;	//スキル画像
	vnSprite *m_pShadow;//クールタイム中の暗い影

	float m_maxCoolTime = 0;	//最大クールタイム
	float m_currentTimer = 0;	//クールタイムの現在の残り時間



};