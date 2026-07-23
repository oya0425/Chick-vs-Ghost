#pragma once

class BlockManager
{

public:
	static int GetMaxBlocksNum() { return m_maxActiveBlocks; }
	static int GetMaxMagmaBlocksNum() { return m_maxActiveMagmaBlocks; }

	// 通常ブロック用リストへの追加
	void AddBlock(TerrainBlock* block) 
	{
		m_blocks.push_back(block); 
		m_allBlocks.push_back(block); // 全体リストにも登録
	}

	// マグマブロック用リストへの追加（追加用関数）
	void AddMagmaBlock(TerrainBlock* block)
	{
		m_magmaBlocks.push_back(block); 
		m_allBlocks.push_back(block); // 全体リストにも登録
	}

	// WAVE開始時に位置をシャッフル（引数に最終WAVE判定用のフラグを追加）
	void RespawnBlocks(int waveCount, float fenceRadius, bool isFinalWave = false)
	{
		// 1. 今回の配置対象にするメイン配列と、非表示にする退避配列を決定する
		// 最終WAVE（ボス戦）はマグマのブロックにする

		std::vector<TerrainBlock*>& targetBlocks = isFinalWave ? m_magmaBlocks : m_blocks;
		std::vector<TerrainBlock*>& escapeBlocks = isFinalWave ? m_blocks : m_magmaBlocks;

		// 2. 出現させる数を決定
		if (isFinalWave)
		{
			m_currentActiveBlocks = m_maxActiveMagmaBlocks;
		}
		else
		{
			// 通常ステージはWAVEごとに増やす
			m_currentActiveBlocks = waveCount * m_countUpNum;
		}

		// 安全対策：プールしている最大数を超えないようにクリップ
		if (m_currentActiveBlocks > (int)targetBlocks.size())
		{
			m_currentActiveBlocks = (int)targetBlocks.size();
		}

		// --- 3. メイン対象のブロックをドーナツ状に配置 ---
		for (int i = 0; i < targetBlocks.size(); i++)
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
				targetBlocks[i]->GetModel()->setScale(randomScale, randomScale * 1.5f, randomScale);
				targetBlocks[i]->GetCollision().SetSize(XMVectorSet(randomScale, randomScale * 1.5f, randomScale, 0.0f));

				targetBlocks[i]->GetModel()->setPosition(&pos);
				targetBlocks[i]->GetModel()->setRenderEnable(true); // 表示を戻す
				for (int j = 0; j < targetBlocks[i]->GetModel()->getPartsNum(); j++) {
					targetBlocks[i]->GetModel()->getParts(j)->setRenderEnable(true);
				}
			}
			else
			{
				// 使わないブロックは範囲外で非表示にする
				targetBlocks[i]->GetModel()->setPosition(0, -1000, 0);
				targetBlocks[i]->GetModel()->setRenderEnable(false);
				for (int j = 0; j < targetBlocks[i]->GetModel()->getPartsNum(); j++)
				{
					targetBlocks[i]->GetModel()->getParts(j)->setRenderEnable(false);
				}
			}
		}

		// --- 4. 今回使わない側のプールをすべて強制的に非表示（y = -1000）にする ---
		for (int i = 0; i < escapeBlocks.size(); i++)
		{
			escapeBlocks[i]->GetModel()->setPosition(0, -1000, 0);
			escapeBlocks[i]->GetModel()->setRenderEnable(false);
			for (int j = 0; j < escapeBlocks[i]->GetModel()->getPartsNum(); j++)
			{
				escapeBlocks[i]->GetModel()->getParts(j)->setRenderEnable(false);
			}
		}
	}

	//std::vector<TerrainBlock*>& GetAllBlocks() { return m_blocks; }
	//std::vector<TerrainBlock*>& GetMagmaBlocks() { return m_magmaBlocks; }

	// 今現在、画面上に表示されているブロックの配列を丸ごと返す
	std::vector<TerrainBlock*>& GetAllActiveBlocks()
	{
		// 1. 通常ブロックの先頭が存在し、かつ表示中なら通常ブロックの配列を返す
		if (!m_blocks.empty() && m_blocks[0]->GetModel() && m_blocks[0]->GetModel()->getRenderEnable())
		{
			return m_blocks;
		}

		// 2. マグマブロックの先頭が存在し、かつ表示中ならマグマブロックの配列を返す
		if (!m_magmaBlocks.empty() && m_magmaBlocks[0]->GetModel() && m_magmaBlocks[0]->GetModel()->getRenderEnable())
		{
			return m_magmaBlocks;
		}

		// 3. どちらも表示されていない（WAVE間のインターバルなど）場合は通常ブロック（空 or 非表示）を安全に返す
		return m_blocks;
	}
	std::vector<TerrainBlock*>& GetAllBlocks() { return m_allBlocks; }

private:
	std::vector<TerrainBlock*>m_blocks;			//生成したブロックのリスト
	std::vector<TerrainBlock*>m_magmaBlocks;	//生成したブロックのリスト

	// 通常とマグマの両方をまとめて管理する用のベクター
	std::vector<TerrainBlock*> m_allBlocks;

	int m_currentActiveBlocks = 0;				//現在のWAVEで出す数
	static const int m_maxActiveBlocks	= 100;			//最大数
	static const int m_maxActiveMagmaBlocks	= 75;			//最大数
	int m_countUpNum = 20;
};