#pragma once
#include<vector>
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
	void RespawnBlocks(int waveCount, float fenceRadius, bool isFinalWave);


	// 今現在、画面上に表示されているブロックの配列を丸ごと返す
	std::vector<TerrainBlock*>& GetAllActiveBlocks();
	// 全てのブロックを返す
	std::vector<TerrainBlock*>& GetAllBlocks() { return m_allBlocks; }

	// 表示されているのブロックの表示非表示
	void AllBlocksShowHide(bool isShow, bool isFinalWave);


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