#pragma once
class TerrainBlock :public CharacterBase
{
public:
	TerrainBlock():CharacterBase(){}

	void Update(float deltaTime)override{}
	void SetIsMagma(bool magma) { isMagma = magma; }
	bool GetIsMagma() { return isMagma; }
private:
	bool isMagma = false;

};
