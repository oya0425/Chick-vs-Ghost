#pragma once
class ExperienceManager
{
public:
	ExperienceManager();
	~ExperienceManager() = default;
	
	// --- UI表示用の１項目分のデータ(強化ひとつのデータ) ---
	struct UpgradeUIData
	{
		const wchar_t* label;		//「速度」「攻撃範囲」
		const wchar_t* suffix;		//「%アップ」「回増加」などの単位
	
		const wchar_t* explanation;	//項目に対する説明
		
		float value[5];			//10,5などの数値

		int spriteID;			//アイコン画像の番号

		int currentLv;		//スキルのレベル
		int maxLv;				//スキルの最大レベル

	};

	// --- レベルアップできる数をあげる ---
	int GetLevelUpStock();

	// --- プレイヤーをセット ---
	void SetPlayer(NewPlayerClass* player);

	// --- 経験値獲得 ---
	void GainExp(float amount);

	// --- レベルアップ ---
	void GainLevel(int level);

	// --- 選択した結果をプレイヤーに適応する ---
	void ApplyUpgrade(int choiceIndex);


	// --- 選ばれた配列番号（強化する値）---
	int GetChoiceIndex()const { return m_choiceIndex; }

	const UpgradeUIData* GetUIDisplayChoices() const { return m_uiDisplayChoices; }

	// --- 経験値表示用 ---
	float GetCurrentExp()const { return m_currentExp; }
	float GetNeedExp()const { return m_neededExp; }


	/*
	1.最初にプレイヤーを入れる
	2.敵を倒したときに経験値獲得の関数を呼ぶ
	3.レベルアップしたときに強化項目が出る。必要経験値が最大になったことを伝える
	4.WAVEManagerのモードを切り替える、ここで強化項目を出す。（ここ時間終了と同時にレベルアップするかもしれんからmainシーンのモードを変更する方がいいかも）
	5.選択されたものを強化して選択が終わって強化されたことを伝える。
	6.伝わったらモードを戻す
	7.これで一周
	*/

	/*
	1.UIに情報を渡すのに、「テキスト」[変数]「速度」[10]「%アップ」
	2.速度やったら靴の画像を入れる、UnityのScriptableObjectみたいな感じで、
	char型、float型、int型(画像の番号)→メインで画像の配列を作って表示する時にこれを参照して
	対応した画像を出す,をメインで代入する
	
	*/
private:
	// --- ステータス項目の種類 ---
	enum class UpgradeType
	{
		MoveSpeed,		//移動速度
		AttackRange,	//攻撃範囲
		MagnetRange,	//引き寄せ
		//ReflectCount,	//反射回数
		//BulletSpeed,	//弾の速度
		MaxCount,		//項目の総数
	};

	// --- ひとつの選択肢を表すデータ ---
	struct UpgradeChoice
	{
		UpgradeType type;	//どの項目か
		float boostValue;	//5%アップ,10,15,20などの強化幅

	};

	// --- 全項目のマスターデータを保持する配列 ---
	UpgradeUIData m_upgradeMaster[(int)UpgradeType::MaxCount];

	// --- 実際に画面に出す３つの枠 ---
	UpgradeUIData m_uiDisplayChoices[3];

	// 3. 内部的にどのタイプが選ばれているかを判別するための隠し変数（後述）
	UpgradeType m_currentSelectedTypes[3];
	

	// --- 抽選用関数 ---
	//1.まず重複しない３つのUpgradeTypeを選ぶ
	//2.それぞれのTypeに対して、レベルに応じたBoostValueを決める
	void GenerateLevelUpOptions();




	// --- プレイヤーを情報を入れておく ---
	NewPlayerClass* m_player = nullptr;

	// --- 経験値系 ---
	int m_currentLevel = 1;		 //現在のレベル
	int m_maxLevel	   = 999;	 //一応保険の最大レベル
	float m_currentExp = 0;	 //現在の経験値量
	float m_neededExp = 100;	 //必要な経験値量(もとが20)
	int m_levelUpStock = 0;      //多重にレベルアップしたときのいる奴

	int m_choiceIndex = 0;

	/*
	強化ステータスを伸ばして、これをプレイヤーの中にある
	ステータスに掛けるか足すかする感じ
	player.スピード *= (1 + m_speedBoost)  
	*/
	// --- 強化ステータス（現在の伸び幅） ---
	float m_speedBoost		 = 0.0f;		//1.移動速度アップ
	float m_rangeBoost		 = 0.0f;		//2.攻撃範囲アップ
	float m_magnetBoost		 = 0.0f;		//3.引き寄せ範囲
	int m_reflectCount		 = 1;			//4.反射回数（撃つ球の）
	float m_bulletSpeedBoost = 0.0f;		//5.弾の速度



	// --- 通知用 ---	
	/*
	if (OnUpgradeApplied) {
    OnUpgradeApplied();
	}
	最も読みやすく、デバッグ（ブレークポイントを置くなど）もしやすいので、まずはこれで進めるのがベストです。
	On~.Invoke();と同じ

	m_expManager->OnLevelUp = [this]() {
		this->SetGameMode(Mode::UpgradeSelection);
		this->PauseGame(true);
	};
	*/
	std::function<void()>OnLevelUp = []() {};	//Unityのactionとほぼ同じ機能
	std::function<void()>OnUpgradeApplied = []() {};//選択が終了した



};