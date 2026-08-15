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
void WaveManager::Init(bool isTutorial,bool isEndless)
{
    m_isTutorial = isTutorial;
    m_isEndless = isEndless;
    m_totalKillCount = 0;
    //チュートリアル時 WAVEを0スタートとする,最大はそのまま
    m_currentWave = m_isTutorial||m_isEndless ? 0 : 1;
    m_maxWave = 5;
    m_state = WaveState::InProgress;

    SetupWave();
}



//--------------------------------------------------------------
// 更新
//--------------------------------------------------------------
void WaveManager::Update(float deltaTime)
{
    //vnFont::print(400, 50, L"maxWave %d currentWave %d State: %s", m_maxWave, m_currentWave, GetStateString());

    if (m_state == WaveState::InProgress)
    {
        //最終WAVEとチュートリアル,エンドレスモードは時間が減らない
        if (!GetFinalWave() && !(m_isTutorial || m_isEndless))
        {
            //m_waveTimer += deltaTime;
            m_waveTimer -= deltaTime;
        }
        if (IsWaveClear())
        {
            m_state = WaveState::ClearWait;
        }

        if (m_isEndless)
        {
            m_endless_time += deltaTime;
            if (m_endless_time >= 10.0f)
            {
                m_endless_time -= 10.0f;

                if (m_spawnLimit < m_configMaxSimultaneous_endless)
                {
                    m_spawnLimit += 50;
                    
                }
                else
                {
                    m_spawnLimit = m_configMaxSimultaneous_endless;
                }
            }
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
void WaveManager::OnEnemyLeaderKilled()
{
    m_killedLeaderCount++;
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
    //チュートリアル時
    if (m_isTutorial)
    {
        return m_isTutorial_Clear;
    }
    else
    {
        //return m_killedCount >= m_killTarget;
        //最終WAVEのみボスを５体倒したらクリア
        if (m_currentWave == m_maxWave)
        {
            return m_killedCount >= 5;
        }
        else
        {
            //return m_waveTimer >= m_waveTimeLimit;
            return m_waveTimer < GetWaveTimeLimit();

        }

    }
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

    if (m_spawnLimit < 5)   // 下限を決める
        m_spawnLimit = 5;

    //チュートリアル中は敵の数を大幅に減らす
    if (m_isTutorial)
    {
        m_spawnLimit = m_spawnLimit_tutorial;
    }
    //エンドレスモードでは少し敵の数を増やす
    if (m_isEndless)
    {
        m_spawnLimit = m_startEnemyNum_endless;
    }

    //時間制限でWAVEクリアにする
    //m_waveTimeLimit = m_waveTimeBase/* + (m_currentWave - 1) * m_addTime*/;

    //始まりの時間を入れる（20秒）
    m_waveTimer = m_waveTimeBase;


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
    //if (m_waveTimer >= m_waveTimeLimit)
    //    return false;
    //if (m_waveTimer <= 0)
    //    return false;
    if (IsWaveClear())
        return false;
    //if (m_currentWave == m_maxWave && m_waveTimer >= m_waveTimeLimit)
    //    return false;

    return true;

}