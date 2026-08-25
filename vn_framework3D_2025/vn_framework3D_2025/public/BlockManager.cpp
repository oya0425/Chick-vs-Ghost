#include"../framework.h"	
#include"../framework/vn_environment.h"

// WAVE開始時に位置をシャッフル（引数に最終WAVE判定用のフラグを追加）
void BlockManager::RespawnBlocks(int waveCount, float fenceRadius, bool isFinalWave = false)
{
	// 今回の配置対象にするメイン配列と、非表示にする退避配列を決定する
	// 最終WAVE（ボス戦）はマグマのブロックにする

	std::vector<TerrainBlock*>& targetBlocks = isFinalWave ? m_magmaBlocks : m_blocks;
	std::vector<TerrainBlock*>& escapeBlocks = isFinalWave ? m_blocks : m_magmaBlocks;

	// 出現させる数を決定
	if (isFinalWave)
	{
		m_currentActiveBlocks = m_maxActiveMagmaBlocks;
	}
	else
	{
		// 通常ステージはWAVEごとに増やす
		m_currentActiveBlocks = waveCount * m_countUpNum;
	}

	// プールしている最大数を超えないようにクリップ
	if (m_currentActiveBlocks > (int)targetBlocks.size())
	{
		m_currentActiveBlocks = (int)targetBlocks.size();
	}

	// --- メイン対象のブロックをドーナツ状に配置 ---
	for (int i = 0; i < targetBlocks.size(); i++)
	{
		if (i < m_currentActiveBlocks)
		{
			float minRadius = 3.0f;
			float maxRadius = fenceRadius * 1.0f;

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

	// --- 今回使わない側のプールをすべて強制的に非表示にする ---
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



// 今現在、画面上に表示されているブロックの配列を丸ごと返す
std::vector<TerrainBlock*>& BlockManager::GetAllActiveBlocks()
{
	// 通常ブロックの先頭が存在し、かつ表示中なら通常ブロックの配列を返す
	if (!m_blocks.empty() && m_blocks[0]->GetModel() && m_blocks[0]->GetModel()->getRenderEnable())
	{
		return m_blocks;
	}

	// マグマブロックの先頭が存在し、かつ表示中ならマグマブロックの配列を返す
	if (!m_magmaBlocks.empty() && m_magmaBlocks[0]->GetModel() && m_magmaBlocks[0]->GetModel()->getRenderEnable())
	{
		return m_magmaBlocks;
	}

	// どちらも表示されていない（WAVE間のインターバルなど）場合は通常ブロック（空 or 非表示）を安全に返す
	return m_blocks;
}

// 表示されているのブロックの表示非表示
void BlockManager::AllBlocksShowHide(bool isShow,bool isFinalWave)
{
	//ブロックの表示非表示
	//通常Waveでは緑の普通のブロックの表示非表示
	if (!isFinalWave)
	{
		for (auto* block : m_blocks)
		{
			block->GetModel()->setRenderEnable(isShow);

			for (int i = 0; i < block->GetModel()->getPartsNum(); i++)
			{
				block->GetModel()->getParts(i)->setRenderEnable(isShow);
			}
		}

	}
	//最終Waveではマグマのブロックの表示非表示
	else
	{

		for (auto* block : m_magmaBlocks)
		{
			block->GetModel()->setRenderEnable(isShow);

			for (int i = 0; i < block->GetModel()->getPartsNum(); i++)
			{
				block->GetModel()->getParts(i)->setRenderEnable(isShow);
			}
		}
	}
}
