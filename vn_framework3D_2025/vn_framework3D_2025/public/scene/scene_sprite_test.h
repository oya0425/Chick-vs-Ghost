#pragma once

class SceneSpriteTest : public vnScene
{
private:
	vnSprite* pSprite;

	//操作の種類
	enum eOperation
	{
		PositionX,
		PositionY,
		ScaleX,
		ScaleY,
		Rotate,
		OperationMax,
	};

	int	Cursor;

public:
	bool initialize();	//初期化(WM_CREATE,Start)
	void terminate();	//終了(WM_DESTROY,Destroy)
	
	void execute();	//処理（毎フレーム　60fps）(WM_PAINT/TIMER,Updata)
	void render();	//描画（毎フレーム　60fps）(WM_PAINT,Draw)
};
