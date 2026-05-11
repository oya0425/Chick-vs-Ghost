#include "../framework.h"
#include "../framework/vn_environment.h"

namespace
{
	constexpr int canSelectCount = 3;	//選択肢の数
}

static const ExperienceManager::UpgradeUIData MASTER_DATA[] =
{
	//「速度」「攻撃範囲」
	//「%アップ」「回増加」などの単位
	//10,5などの数値(仮入れ後で代入して書き換える)配列
	//アイコン画像の番号
	//スキルのレベル
	//スキルの最大レベル(一応保険としての最大)

	{ L"移動速度",     L"%アップ",    { 10.0f, 15.0f, 10.0f, 15.0f, 10.0f },   0,  1,  10 },
	{ L"攻撃範囲",     L"%アップ",    { 10.0f, 15.0f, 20.0f, 10.0f, 15.0f },   1,  0,  50 },
	{ L"引き寄せ範囲", L"%アップ",    { 10.0f, 15.0f, 20.0f, 10.0f, 15.0f },   2,  0,  50 },
	{ L"反射回数",     L"回増加",     {  1.0f,  2.0f,  3.0f,  1.0f,  2.0f },   3,  1,  50 },
	{ L"弾の速度",     L"%アップ",    { 10.0f, 15.0f, 10.0f, 15.0f, 10.0f },   4,  1,  50 },

};



ExperienceManager::ExperienceManager()
{

	//辞書からコピーして初期化
	for (int i = 0; i < (int)UpgradeType::MaxCount; i++)
	{
		m_upgradeMaster[i] = MASTER_DATA[i];
	}
	
	// その他初期化
	m_currentLevel = 1;
	m_currentExp = 0;
	m_neededExp = 20.0f;
	m_maxLevel = 999;
	for (int i = 0; i < canSelectCount; i++)
	{
		m_uiDisplayChoices[i] = {};
		m_currentSelectedTypes[i] = UpgradeType::MaxCount;
	}

}
void ExperienceManager::GainExp(float exp)
{
	if (!m_player)return;

	m_currentExp += exp;
	while (m_currentExp >= m_neededExp)
	{
		//1.余った経験値を次に持ち越す
		m_currentExp -= m_neededExp;

		//2.レベルアップ
		m_currentLevel++;
		m_levelUpStock++;
		//3.次の必要経験値を計算
		m_neededExp = m_neededExp * 1.2f;


		//最大レベルに達していたらループを抜ける
		if (m_currentLevel >= m_maxLevel)
		{
			m_currentExp = 0;
			break;
		}
	}
	//4.通知を送る
	if (m_levelUpStock&& OnLevelUp)
	{
		GenerateLevelUpOptions();
		OnLevelUp();
	}

}

void ExperienceManager::GenerateLevelUpOptions()
{
	//1.候補リストを作成（最大レベルに達していないものだけを入れる）
	std::vector<UpgradeType>candidates;
	for (int i = 0; i < (int)UpgradeType::MaxCount; i++)
	{
		if (m_upgradeMaster[i].currentLv < m_upgradeMaster[i].maxLv)
		{
			candidates.push_back((UpgradeType)i);
		}
	}

	//2.シャッフルして先頭から３つ選ぶ（またはランダムに抽出）
	for (int i = 0; i < canSelectCount; i++)
	{
		if (candidates.empty())break;//選ぶものがなかったら終了

		int index = rand() % candidates.size();//ランダム決定

		//選ばれたやつ
		UpgradeType selected = candidates[index];

		// -- 内部変数を更新 ---
		m_currentSelectedTypes[i] = selected;

		// --- UI表示用データを更新 ---
		m_uiDisplayChoices[i] = m_upgradeMaster[(int)selected];

		// --- Lv0(未習得)ならテキストを差し替える
		if (m_uiDisplayChoices[i].currentLv == 0)
		{
			m_uiDisplayChoices[i].suffix = L"Lvを習得！";

			// 習得時は数値をレベルとして出す
			m_uiDisplayChoices[i].value[m_choiceIndex] = 1.0f;//表示する時に（int）にして使用
		}

		// --- 選んだものは候補から消す（重複防止）
		candidates.erase(candidates.begin() + index);

	}

}

void ExperienceManager::ApplyUpgrade(int choiceIndex)
{
	//範囲外チェック
	if (choiceIndex < 0 || choiceIndex >= canSelectCount)return;

	//何のタイプが選ばれたか取得
	UpgradeType selectedType = m_currentSelectedTypes[choiceIndex];
	if (selectedType == UpgradeType::MaxCount)return;

	//1.マスターデータレベルを上げる
	m_upgradeMaster[(int)selectedType].currentLv++;

	//2.プレイヤーのステータスに反映(この前にあげる値を変更する)
	m_choiceIndex = rand() % 5;
	float boost = MASTER_DATA[(int)selectedType].value[m_choiceIndex];

	switch (selectedType)
	{
	case ExperienceManager::UpgradeType::MoveSpeed:
		//移動速度
		m_speedBoost += boost;	//10%だったら10.0f
		//プレイヤーの速度を（初期速度*（1.0+全体ブースト））
		m_player->SetSpeedMultiplier(1.0f + (m_speedBoost / 100.0f));
		break;
	case ExperienceManager::UpgradeType::AttackRange:
		//攻撃範囲の強化
		m_rangeBoost += boost;  
		m_player->SetRangeMultiplier(1.0f + (m_rangeBoost / 100.0f));
		// 2. もしこれが「最初のレベル（Lv1）」なら、スキルを有効化する
		if (m_upgradeMaster[(int)selectedType].currentLv == 1)
		{
			m_player->UnlockAreaAttackSkill(true);
		}
		break;
	case ExperienceManager::UpgradeType::MagnetRange:
		m_magnetBoost += boost;
		m_player->SetPullMultiplier(1.0f + (m_magnetBoost / 100.0f));
		// 引き寄せスキルの習得
		if (m_upgradeMaster[(int)selectedType].currentLv == 1)
		{
			m_player->UnlockPullAttackSkill(true);
		}
		break;
	case ExperienceManager::UpgradeType::ReflectCount:
		m_reflectCount += (int)boost;
		m_player->SetBounceMultiplier(m_reflectCount);
		break;
	case ExperienceManager::UpgradeType::BulletSpeed:
		m_bulletSpeedBoost += boost;
		m_player->SetBulletSpeedMultiplier(1.0f+(m_bulletSpeedBoost/100.0f));
		break;
	}


	//3.レベルアップ可能のストックを減らす
	if (m_levelUpStock > 0)
	{
		m_levelUpStock--;
	}

	//4.まだレベルアップストックがあるなら、次の抽選をする
	if (m_levelUpStock > 0)
	{
		GenerateLevelUpOptions();
		// 連続レベルアップさせるなら、もう一度 OnLevelUp を通知して
		// UIを出し直す（または開いたまま中身を更新する）必要があります
		if (OnLevelUp)
		{
			OnLevelUp();
		}

	}
	else
	{
		//全てのレベルアップ処理が終了
		if (OnUpgradeApplied)
		{
			OnUpgradeApplied();
		}
	}
}
int ExperienceManager::GetLevelUpStock()
{
	return m_levelUpStock;
}

void ExperienceManager::SetPlayer(NewPlayerClass* player)
{
	m_player = player;
}
