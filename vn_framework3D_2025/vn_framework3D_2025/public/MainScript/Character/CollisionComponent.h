#pragma once
class CollisionComponent
{
public:
	//サイズと中心を設定
	CollisionComponent(const XMVECTOR& size, const XMVECTOR& center) 
							:m_size(size), m_center(center) {};

	XMVECTOR GetSize() const { return m_size; }
	XMVECTOR GetCenter() const { return m_center; }


	void SetSize(const XMVECTOR& size) { m_size = size; }
	void SetCenter(const XMVECTOR& center) { m_center = center; }


private:
	XMVECTOR m_size;	//判定の大きさ
	XMVECTOR m_center;	//モデルの中心

	
};