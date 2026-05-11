#pragma once

class BlockManager
{

public:
	static int GetMaxBlocksNum() { return m_maxActiveBlocks; }

	void AddBlock(TerrainBlock* block)
	{
		m_blocks.push_back(block);
	}

	//WAVE開始時に位置をシャッフル
	void RespawnBlocks(int waveCount, float fenceRadius)
	{
		m_currentActiveBlocks = waveCount * m_countUpNum;
		if (m_currentActiveBlocks > m_maxActiveBlocks)m_currentActiveBlocks = m_maxActiveBlocks;
		for (int i = 0;  i < m_blocks.size(); i++)
		{
			if (i < m_currentActiveBlocks)
			{
				float minRadius = 3.0f;
				float maxRadius = fenceRadius * 1.0f; // フェンスの8割まで

				float r1 = (float)rand() / RAND_MAX;
				float r2 = (float)rand() / RAND_MAX;
				float angle = r1 * XM_2PI;
				// ドーナツ型分布で計算
				float dist = sqrtf(r2 * (maxRadius * maxRadius - minRadius * minRadius) + minRadius * minRadius);

				XMVECTOR pos = XMVectorSet(cosf(angle) * dist, 1.0f, sinf(angle) * dist, 0.0f);
				// 1.0f ～ 4.0f の間でランダムな倍率を作成
				float randomScale = 1.0f + ((float)rand() / RAND_MAX) * 3.0f;

				// 見た目の大きさを変更
				m_blocks[i]->GetModel()->setScale(randomScale, randomScale*1.5f, randomScale);
				m_blocks[i]->GetCollision().SetSize(XMVectorSet(randomScale, randomScale*1.5f, randomScale, 0.0f));

				m_blocks[i]->GetModel()->setPosition(&pos);
				//m_blocks[i]->GetModel()->setScale(3.0f, 3.0f, 3.0f);
				//m_blocks[i]->GetCollision().SetSize(XMVectorSet(3,3,3,0));
				m_blocks[i]->GetModel()->setRenderEnable(true); // 表示を戻す
				for (int j = 0; j < m_blocks[i]->GetModel()->getPartsNum(); j++) {
					m_blocks[i]->GetModel()->getParts(j)->setRenderEnable(true);
				}
			}
			else
			{
				//使わないブロックは範囲外で非表示にする
				m_blocks[i]->GetModel()->setPosition(0, -1000, 0);
				m_blocks[i]->GetModel()->setRenderEnable(false);
				for (int j = 0; j < m_blocks[i]->GetModel()->getPartsNum(); j++)
				{
					m_blocks[i]->GetModel()->getParts(j)->setRenderEnable(false);
				}
			}
		}
	}

	std::vector<TerrainBlock*>& GetAllBlocks() { return m_blocks; }


private:
	std::vector<TerrainBlock*>m_blocks;	//生成したブロックのリスト
	int m_currentActiveBlocks = 0;			//現在のWAVEで出す数
	static const int m_maxActiveBlocks	= 100;			//最大数
	int m_countUpNum = 20;
};