//--------------------------------------------------------------//
//	"WaveManager.h"												//
//		WAVEクラス												//
//													2025/02/14	//
//														Oya  	//
//--------------------------------------------------------------//
#pragma once
#include"../public/RigidbodyComponent.h"
class WaveManager
{
public:
    WaveManager();
    ~WaveManager();


    enum class WaveState
    {
        InProgress, //ゲーム中（戦闘中）
        ClearWait,  //WAVEクリア画面
        Finished    //全WAVEクリア画面
    };

    void Init();
    void Update(float deltaTime);

    void OnEnemyKilled();      // 撃破通知
    void OnEnemySpawned();     // 出現通知

    bool IsWaveClear() const;

    bool CanSpawn() const;  //スポーンできるか?

    void GoNextWave();  //WAVEを次へ進める

    WaveState GetState() { return m_state; }

    // --- 表示する必要がある変数を返す ---
    int GetKillCount()const { return m_killedCount; };    //現在の撃破数
    int GetCurrentWave() const;                         //現在のWAVE数
    int GetKillTargetCount()const { return m_killTarget; }//必要撃破数


    int GetKillBossCountTarget()const { return m_killBossCountTarget - m_killedCount; } //ボスの残り撃破数

    int GetNextKillTargetCount()const { return m_nextKillTarget; }

    //--WAVE切り替え用
    bool IsWaitingForNext() const
    {
        return m_state == WaveState::ClearWait;
    }

    bool IsFinished() const
    {
        return m_state == WaveState::Finished;
    }

    int GetMaxWave() { return m_maxWave; }
    bool GetFinalWave() { return m_currentWave == m_maxWave; }

    //ボスWAVEゲームオーバー
    bool GetIsBossWAVEGAMEOver() { return (m_currentWave == m_maxWave && m_waveTimer >= m_waveTimeLimit); }
    

    const wchar_t* GetStateString() const
    {
        switch (m_state)
        {
        case WaveState::InProgress:
            return L"InProgress";

        case WaveState::ClearWait:
            return L"ClearWait";

        case WaveState::Finished:
            return L"Finished";

        default:
            return L"Unknown";
        }
    }


    // --- 出現する敵を解除(使用可能にする) ---
    void UnlockEnemyType(NewEnemyClass::EnemyType type);


    // --- 出現する最大数 ---
    int GetMaxSpawnLimit()const { return m_spawnLimit; }

    // --- 時間を返す ---
    float GetWaveTimer() const { return m_waveTimer; }
    float GetWaveTimeLimit() const { return m_waveTimeLimit; }

private:
    void SetupWave();          // 次WAVE準備


private:
    int m_currentWave;

    int m_killTarget;        // 必要撃破数
    int m_killedCount;       // 現在撃破数

    int m_killBossCountTarget = 5;

    float m_waveTimer = 0;            //WAVE中の時間
    float m_waveTimeLimit=20;         //WAVEクリアまでの時間
    const float m_waveTimeBase = 1;  //基準の時間
    const float m_addTime = 10;       //増える時間   

    int m_nextKillTarget;    // 次の必要キル数

    int m_totalKillCount;    // 全体の撃破数

    int m_spawnLimit;        // 同時出現上限
    int m_aliveCount;        // 現在生存数
    int m_configMaxSimultaneous = 1000;   //最初の敵の最大数

    float m_respawnInterval; // 湧き間隔
    float m_respawnTimer;    // 湧きタイマー

    WaveState m_state;
    int m_maxWave = 5;
};