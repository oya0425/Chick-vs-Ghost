#include"../framework.h"
#include"../framework/vn_environment.h"

SkillButtonUI::SkillButtonUI
(vnSprite* icon, vnSprite* shadow,float maxCoolTime):
	m_currentTimer(0)
{
	m_pIcon = icon;
	m_pShadow = shadow;

	m_maxCoolTime = maxCoolTime;
}

void SkillButtonUI::Update(float currentCoolTimer)
{
	
	
	float raito = currentCoolTimer / m_maxCoolTime;
	//クールタイムが0の時
	//1.アイコンを表示する
	if (currentCoolTimer <= 0.0f)
	{
		//
		m_pIcon->setAlpha(255);	//アイコンをくっきり出す
		m_pShadow->setAlpha(0);	//影を完全に消す
	}
	else
	{
		//クールタイムが0以上の時
		//1.アイコンの透明度を下げる
		//影の濃さ：最初は濃く（180）、終わるにつれて薄く
		int shadowAlpha = static_cast<int>(180 * raito);

		m_pShadow->setAlpha(shadowAlpha);
		//2.影の透明度を最大にして暗くする
		int iconAlpha = 100 + static_cast<int>((255 - 100) * (1.0f - raito));
		m_pIcon->setAlpha(iconAlpha);
		//3.理想は扇形でクールタイムを表現する（今は時間経過で影の透明度を減らして、アイコンの透明度を増やす）

	}
}