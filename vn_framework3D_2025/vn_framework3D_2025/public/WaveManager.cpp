#include "../framework.h"
#include "../framework/vn_environment.h"
#include "WaveManager.h"

//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
WaveManager::WaveManager()
{
    m_currentWave = 0;

    m_killTarget = 0;
    m_killedCount = 0;
    m_nextKillTarget = 0;
    m_totalKillCount = 0;

    m_spawnLimit = 0;
    m_aliveCount = 0;

    m_maxWave = 2;

    m_respawnInterval = 0.0f;
    m_respawnTimer = 0.0f;

    m_state = WaveState::InProgress;
}

WaveManager::~WaveManager()
{
}

//--------------------------------------------------------------
// 初期化
//--------------------------------------------------------------
void WaveManager::Init()
{
    m_currentWave = 1;
    m_maxWave = 5;
    m_state = WaveState::InProgress;
    SetupWave();
}

//--------------------------------------------------------------
// 更新
//--------------------------------------------------------------
void WaveManager::Update(float deltaTime)
{
    if (m_state == WaveState::InProgress)
    {
        m_waveTimer += deltaTime;
        if (IsWaveClear())
        {
            m_state = WaveState::ClearWait;
        }
    }
}

//--------------------------------------------------------------
// 次のWAVEへ
//--------------------------------------------------------------
void WaveManager::GoNextWave()
{
    if (m_state != WaveState::ClearWait)
        return;

    m_currentWave++;

    if (m_currentWave > m_maxWave)
    {
        m_state = WaveState::Finished;
        return;
    }

    SetupWave();
    m_state = WaveState::InProgress;
}

//--------------------------------------------------------------
// 撃破通知
//--------------------------------------------------------------
void WaveManager::OnEnemyKilled()
{
    m_killedCount++;
    m_totalKillCount++; // 全体のカウントも増やす
    m_aliveCount--;

    if (m_aliveCount < 0)
        m_aliveCount = 0;
}

//--------------------------------------------------------------
// 出現通知
//--------------------------------------------------------------
void WaveManager::OnEnemySpawned()
{
    m_aliveCount++;
}

//--------------------------------------------------------------
// WAVEクリア判定
//--------------------------------------------------------------
bool WaveManager::IsWaveClear() const
{
    //return m_killedCount >= m_killTarget;
    return m_waveTimer >= m_waveTimeLimit;
}

//--------------------------------------------------------------
// 現在WAVE取得
//--------------------------------------------------------------
int WaveManager::GetCurrentWave() const
{
    return m_currentWave;
}

//--------------------------------------------------------------
// WAVE設定
//--------------------------------------------------------------
void WaveManager::SetupWave()
{
    
    m_killedCount = 0;
    m_aliveCount = 0;
    m_waveTimer = 0.0f;

    int baseKill = 100;        // 100

    int startLimit = m_configMaxSimultaneous;
    int stepLimit = 25;
    m_spawnLimit = startLimit + (m_currentWave - 1) * stepLimit;

    // 安全装置：もし在庫（150）を超えそうならカットする
    //if (m_spawnLimit > 150) m_spawnLimit = 150;


    if (m_spawnLimit < 5)   // 下限を決める
        m_spawnLimit = 5;

    //時間制限でWAVEクリアにする
    m_waveTimeLimit = m_waveTimeBase + (m_currentWave - 1) * m_addTime;


    if (m_currentWave != 1)
    {
        m_killTarget = baseKill + m_currentWave * 5;
    }
    else {
        m_killTarget = baseKill;
    }

    m_nextKillTarget = baseKill + (m_currentWave + 1) * 5;
}

//--------------------------------------------------------------
// 湧き処理判定
//--------------------------------------------------------------
bool WaveManager::CanSpawn() const
{
    // 現在の生存数が上限に達していたらダメ
    //if (m_aliveCount >= m_spawnLimit)
    //    return false;

    //// すでにノルマを達成していてもダメ
    //if (m_killedCount >= m_killTarget)
    //    return false;

    //return true;
    //if (m_aliveCount >= m_spawnLimit)
    //    return false;

    // ▼時間制に合わせる
    if (m_waveTimer >= m_waveTimeLimit)
        return false;

    return true;

}