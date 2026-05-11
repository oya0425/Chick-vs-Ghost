#include "../../framework.h"
#include "../../framework/vn_environment.h"

//初期化関数
bool SceneSkinTest::initialize()
{
	vnCamera::setTarget(0.0f, 2.0f, 0.0f);

	//[カメラの直交座標を極座標に変換
	XMVECTOR dist = *vnCamera::getPosition() - *vnCamera::getTarget();
	float dX = XMVectorGetX(dist);
	float dY = XMVectorGetY(dist);
	float dZ = XMVectorGetZ(dist);

	cam_theta = atan2f(dX, dZ);
	cam_phi = -atan2f(dY, sqrtf(dZ * dZ + dX * dX));
	cam_distance = XMVectorGetX(XMVector3Length(dist));
	init_theta = cam_theta;
	init_phi = cam_phi;
	init_distance = cam_distance;
	//]

	Width = 2.0f;
	Height = 2.0f;
	BoneNum = 2;
	pCube = NULL;


	Cursor = 0;
	Select = 0;

	return true;
}

//終了関数
void SceneSkinTest::terminate()
{
	//オブジェクトの削除
	if(pCube)delete pCube;
}

//処理関数
void SceneSkinTest::execute()
{
	int cusorMax = pCube ? OperationMax : CreationMax;

	float value_t = 0.01f;
	float value_r = XMConvertToRadians(1.0f);
	float value_s = 0.01f;
	float value_c = 0.1f;

	if (vnKeyboard::trg(DIK_UP) && --Cursor < 0)
	{
		Cursor = cusorMax - 1;
	}
	else if (vnKeyboard::trg(DIK_DOWN) && ++Cursor >= cusorMax)
	{
		Cursor = 0;
	}

	if (!pCube)
	{
		if (vnKeyboard::trg(DIK_RIGHT))
		{
			switch (Cursor)
			{
			case CubeBone:		if (++BoneNum > BoneNumMax)BoneNum = BoneNumMin;	break;
			}
		}
		else if (vnKeyboard::on(DIK_RIGHT))
		{
			switch (Cursor)
			{
			case CubeWidth:	Width += value_c;	break;
			case CubeHeight:Height += value_c;	break;
			}
		}
		else if (vnKeyboard::trg(DIK_LEFT))
		{
			switch (Cursor)
			{
			case CubeBone:		if (--BoneNum < BoneNumMin)BoneNum = BoneNumMax;	break;
			}
		}
		else if (vnKeyboard::on(DIK_LEFT))
		{
			switch (Cursor)
			{
			case CubeWidth:	Width -= value_c;	if (Width < CubeSizeMin)Width = CubeSizeMin;	break;
			case CubeHeight:Height -= value_c;	if (Height < CubeSizeMin)Height = CubeSizeMin;	break;
			}
		}
		else if (vnKeyboard::trg(DIK_SPACE))
		{
			pCube = new cSkinnedCube(Width, Height, BoneNum);
			//pCube = new cSkinnedSimpleCube();
			Cursor = Select = 0;

			XMVECTOR trg = XMVectorLerp(pCube->bones[0].World.r[3], pCube->bones[pCube->BoneNum - 1].World.r[3], 0.5f);
			vnCamera::setTarget(&trg);
			operateCamera(true);
		}
		int line = 0;
		vnFont::print(80.0f, (float)(100 + (Cursor) * 16), 0xffffffff, L"→");
		vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"BoneNum : %d", BoneNum);
		vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"Width : %.3f", Width);
		vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"Height : %.3f", Height);
		vnFont::print(100.0f, 100.0f + (float)((++line)++ * 16), L"[SPACE] Create Skinned Cube");
		
		return;
	}

	if(vnKeyboard::trg(DIK_RIGHT))
	{
		switch (Cursor)
		{
		case Bone:		if (++Select >= pCube->BoneNum)Select = 0;	break;
		}
	}
	else if (vnKeyboard::on(DIK_RIGHT))
	{
		if (vnKeyboard::on(DIK_LSHIFT) || vnKeyboard::on(DIK_RSHIFT))
		{
			switch (Cursor)
			{
			case PositionX:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorAdd(pCube->bones[i].Pos, XMVectorSet(value_t, 0.0f, 0.0f, 0.0f));	break;
			case PositionY:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorAdd(pCube->bones[i].Pos, XMVectorSet(0.0f, value_t, 0.0f, 0.0f));	break;
			case PositionZ:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorAdd(pCube->bones[i].Pos, XMVectorSet(0.0f, 0.0f, value_t, 0.0f));	break;
			case RotateX:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorAdd(pCube->bones[i].Rot, XMVectorSet(value_r, 0.0f, 0.0f, 0.0f));	break;
			case RotateY:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorAdd(pCube->bones[i].Rot, XMVectorSet(0.0f, value_r, 0.0f, 0.0f));	break;
			case RotateZ:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorAdd(pCube->bones[i].Rot, XMVectorSet(0.0f, 0.0f, value_r, 0.0f));	break;
			case ScaleX:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorAdd(pCube->bones[i].Scl, XMVectorSet(value_s, 0.0f, 0.0f, 0.0f));	break;
			case ScaleY:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorAdd(pCube->bones[i].Scl, XMVectorSet(0.0f, value_s, 0.0f, 0.0f));	break;
			case ScaleZ:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorAdd(pCube->bones[i].Scl, XMVectorSet(0.0f, 0.0f, value_s, 0.0f));	break;
			}
		}
		else if (vnKeyboard::on(DIK_LCONTROL) || vnKeyboard::on(DIK_RCONTROL))
		{
			switch (Cursor)
			{
			case PositionX:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorAdd(pCube->bones[i].Pos, XMVectorSet(value_t, 0.0f, 0.0f, 0.0f));	break;
			case PositionY:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorAdd(pCube->bones[i].Pos, XMVectorSet(0.0f, value_t, 0.0f, 0.0f));	break;
			case PositionZ:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorAdd(pCube->bones[i].Pos, XMVectorSet(0.0f, 0.0f, value_t, 0.0f));	break;
			case RotateX:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorAdd(pCube->bones[i].Rot, XMVectorSet(value_r, 0.0f, 0.0f, 0.0f));	break;
			case RotateY:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorAdd(pCube->bones[i].Rot, XMVectorSet(0.0f, value_r, 0.0f, 0.0f));	break;
			case RotateZ:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorAdd(pCube->bones[i].Rot, XMVectorSet(0.0f, 0.0f, value_r, 0.0f));	break;
			case ScaleX:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorAdd(pCube->bones[i].Scl, XMVectorSet(value_s, 0.0f, 0.0f, 0.0f));	break;
			case ScaleY:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorAdd(pCube->bones[i].Scl, XMVectorSet(0.0f, value_s, 0.0f, 0.0f));	break;
			case ScaleZ:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorAdd(pCube->bones[i].Scl, XMVectorSet(0.0f, 0.0f, value_s, 0.0f));	break;
			}
		}
		else
		{
			switch (Cursor)
			{
			case PositionX:	pCube->bones[Select].Pos = XMVectorAdd(pCube->bones[Select].Pos, XMVectorSet(value_t, 0.0f, 0.0f, 0.0f));	break;
			case PositionY:	pCube->bones[Select].Pos = XMVectorAdd(pCube->bones[Select].Pos, XMVectorSet(0.0f, value_t, 0.0f, 0.0f));	break;
			case PositionZ:	pCube->bones[Select].Pos = XMVectorAdd(pCube->bones[Select].Pos, XMVectorSet(0.0f, 0.0f, value_t, 0.0f));	break;
			case RotateX:	pCube->bones[Select].Rot = XMVectorAdd(pCube->bones[Select].Rot, XMVectorSet(value_r, 0.0f, 0.0f, 0.0f));	break;
			case RotateY:	pCube->bones[Select].Rot = XMVectorAdd(pCube->bones[Select].Rot, XMVectorSet(0.0f, value_r, 0.0f, 0.0f));	break;
			case RotateZ:	pCube->bones[Select].Rot = XMVectorAdd(pCube->bones[Select].Rot, XMVectorSet(0.0f, 0.0f, value_r, 0.0f));	break;
			case ScaleX:	pCube->bones[Select].Scl = XMVectorAdd(pCube->bones[Select].Scl, XMVectorSet(value_s, 0.0f, 0.0f, 0.0f));	break;
			case ScaleY:	pCube->bones[Select].Scl = XMVectorAdd(pCube->bones[Select].Scl, XMVectorSet(0.0f, value_s, 0.0f, 0.0f));	break;
			case ScaleZ:	pCube->bones[Select].Scl = XMVectorAdd(pCube->bones[Select].Scl, XMVectorSet(0.0f, 0.0f, value_s, 0.0f));	break;
			}
		}
	}
	else if (vnKeyboard::trg(DIK_LEFT))
	{
		switch (Cursor)
		{
		case Bone:		if (--Select < 0)Select = pCube->BoneNum - 1;	break;
		}
	}
	else if (vnKeyboard::on(DIK_LEFT))
	{
		if (vnKeyboard::on(DIK_LSHIFT) || vnKeyboard::on(DIK_RSHIFT))
		{
			switch (Cursor)
			{
			case PositionX:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorSubtract(pCube->bones[i].Pos, XMVectorSet(value_t, 0.0f, 0.0f, 0.0f));	break;
			case PositionY:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorSubtract(pCube->bones[i].Pos, XMVectorSet(0.0f, value_t, 0.0f, 0.0f));	break;
			case PositionZ:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorSubtract(pCube->bones[i].Pos, XMVectorSet(0.0f, 0.0f, value_t, 0.0f));	break;
			case RotateX:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorSubtract(pCube->bones[i].Rot, XMVectorSet(value_r, 0.0f, 0.0f, 0.0f));	break;
			case RotateY:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorSubtract(pCube->bones[i].Rot, XMVectorSet(0.0f, value_r, 0.0f, 0.0f));	break;
			case RotateZ:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorSubtract(pCube->bones[i].Rot, XMVectorSet(0.0f, 0.0f, value_r, 0.0f));	break;
			case ScaleX:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorSubtract(pCube->bones[i].Scl, XMVectorSet(value_s, 0.0f, 0.0f, 0.0f));	break;
			case ScaleY:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorSubtract(pCube->bones[i].Scl, XMVectorSet(0.0f, value_s, 0.0f, 0.0f));	break;
			case ScaleZ:	for (int i = 1; i < pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorSubtract(pCube->bones[i].Scl, XMVectorSet(0.0f, 0.0f, value_s, 0.0f));	break;
			}
		}
		else if (vnKeyboard::on(DIK_LCONTROL) || vnKeyboard::on(DIK_RCONTROL))
		{
			switch (Cursor)
			{
			case PositionX:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorSubtract(pCube->bones[i].Pos, XMVectorSet(value_t, 0.0f, 0.0f, 0.0f));	break;
			case PositionY:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorSubtract(pCube->bones[i].Pos, XMVectorSet(0.0f, value_t, 0.0f, 0.0f));	break;
			case PositionZ:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorSubtract(pCube->bones[i].Pos, XMVectorSet(0.0f, 0.0f, value_t, 0.0f));	break;
			case RotateX:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorSubtract(pCube->bones[i].Rot, XMVectorSet(value_r, 0.0f, 0.0f, 0.0f));	break;
			case RotateY:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorSubtract(pCube->bones[i].Rot, XMVectorSet(0.0f, value_r, 0.0f, 0.0f));	break;
			case RotateZ:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorSubtract(pCube->bones[i].Rot, XMVectorSet(0.0f, 0.0f, value_r, 0.0f));	break;
			case ScaleX:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorSubtract(pCube->bones[i].Scl, XMVectorSet(value_s, 0.0f, 0.0f, 0.0f));	break;
			case ScaleY:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorSubtract(pCube->bones[i].Scl, XMVectorSet(0.0f, value_s, 0.0f, 0.0f));	break;
			case ScaleZ:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorSubtract(pCube->bones[i].Scl, XMVectorSet(0.0f, 0.0f, value_s, 0.0f));	break;
			}
		}
		else
		{
			switch (Cursor)
			{
			case PositionX:	pCube->bones[Select].Pos = XMVectorSubtract(pCube->bones[Select].Pos, XMVectorSet(value_t, 0.0f, 0.0f, 0.0f));	break;
			case PositionY:	pCube->bones[Select].Pos = XMVectorSubtract(pCube->bones[Select].Pos, XMVectorSet(0.0f, value_t, 0.0f, 0.0f));	break;
			case PositionZ:	pCube->bones[Select].Pos = XMVectorSubtract(pCube->bones[Select].Pos, XMVectorSet(0.0f, 0.0f, value_t, 0.0f));	break;
			case RotateX:	pCube->bones[Select].Rot = XMVectorSubtract(pCube->bones[Select].Rot, XMVectorSet(value_r, 0.0f, 0.0f, 0.0f));	break;
			case RotateY:	pCube->bones[Select].Rot = XMVectorSubtract(pCube->bones[Select].Rot, XMVectorSet(0.0f, value_r, 0.0f, 0.0f));	break;
			case RotateZ:	pCube->bones[Select].Rot = XMVectorSubtract(pCube->bones[Select].Rot, XMVectorSet(0.0f, 0.0f, value_r, 0.0f));	break;
			case ScaleX:	pCube->bones[Select].Scl = XMVectorSubtract(pCube->bones[Select].Scl, XMVectorSet(value_s, 0.0f, 0.0f, 0.0f));	break;
			case ScaleY:	pCube->bones[Select].Scl = XMVectorSubtract(pCube->bones[Select].Scl, XMVectorSet(0.0f, value_s, 0.0f, 0.0f));	break;
			case ScaleZ:	pCube->bones[Select].Scl = XMVectorSubtract(pCube->bones[Select].Scl, XMVectorSet(0.0f, 0.0f, value_s, 0.0f));	break;
			}
		}
	}
	else if (vnKeyboard::trg(DIK_SPACE))
	{
		if (vnKeyboard::on(DIK_LSHIFT) || vnKeyboard::on(DIK_RSHIFT))
		{
			switch (Cursor)
			{
			case PositionX:	for(int i=0;i<pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorSetX(pCube->bones[i].Pos, XMVectorGetX(pCube->bones[i].BindPos));	break;
			case PositionY:	for(int i=0;i<pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorSetY(pCube->bones[i].Pos, XMVectorGetY(pCube->bones[i].BindPos));	break;
			case PositionZ:	for(int i=0;i<pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorSetZ(pCube->bones[i].Pos, XMVectorGetZ(pCube->bones[i].BindPos));	break;
			case RotateX:	for(int i=0;i<pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorSetX(pCube->bones[i].Rot, XMVectorGetX(pCube->bones[i].BindRot));	break;
			case RotateY:	for(int i=0;i<pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorSetY(pCube->bones[i].Rot, XMVectorGetY(pCube->bones[i].BindRot));	break;
			case RotateZ:	for(int i=0;i<pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorSetZ(pCube->bones[i].Rot, XMVectorGetZ(pCube->bones[i].BindRot));	break;
			case ScaleX:	for(int i=0;i<pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorSetX(pCube->bones[i].Scl, XMVectorGetX(pCube->bones[i].BindScl));	break;
			case ScaleY:	for(int i=0;i<pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorSetY(pCube->bones[i].Scl, XMVectorGetY(pCube->bones[i].BindScl));	break;
			case ScaleZ:	for(int i=0;i<pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorSetZ(pCube->bones[i].Scl, XMVectorGetZ(pCube->bones[i].BindScl));	break;
			}
		}
		else if (vnKeyboard::on(DIK_LCONTROL) || vnKeyboard::on(DIK_RCONTROL))
		{
			switch (Cursor)
			{
			case PositionX:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorSetX(pCube->bones[i].Pos, XMVectorGetX(pCube->bones[i].BindPos));	break;
			case PositionY:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorSetY(pCube->bones[i].Pos, XMVectorGetY(pCube->bones[i].BindPos));	break;
			case PositionZ:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Pos = XMVectorSetZ(pCube->bones[i].Pos, XMVectorGetZ(pCube->bones[i].BindPos));	break;
			case RotateX:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorSetX(pCube->bones[i].Rot, XMVectorGetX(pCube->bones[i].BindRot));	break;
			case RotateY:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorSetY(pCube->bones[i].Rot, XMVectorGetY(pCube->bones[i].BindRot));	break;
			case RotateZ:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Rot = XMVectorSetZ(pCube->bones[i].Rot, XMVectorGetZ(pCube->bones[i].BindRot));	break;
			case ScaleX:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorSetX(pCube->bones[i].Scl, XMVectorGetX(pCube->bones[i].BindScl));	break;
			case ScaleY:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorSetY(pCube->bones[i].Scl, XMVectorGetY(pCube->bones[i].BindScl));	break;
			case ScaleZ:	for (int i = Select; i < pCube->BoneNum; i++)pCube->bones[i].Scl = XMVectorSetZ(pCube->bones[i].Scl, XMVectorGetZ(pCube->bones[i].BindScl));	break;
			}
		}
		else
		{
			switch (Cursor)
			{
			case PositionX:	pCube->bones[Select].Pos = XMVectorSetX(pCube->bones[Select].Pos, XMVectorGetX(pCube->bones[Select].BindPos));	break;
			case PositionY:	pCube->bones[Select].Pos = XMVectorSetY(pCube->bones[Select].Pos, XMVectorGetY(pCube->bones[Select].BindPos));	break;
			case PositionZ:	pCube->bones[Select].Pos = XMVectorSetZ(pCube->bones[Select].Pos, XMVectorGetZ(pCube->bones[Select].BindPos));	break;
			case RotateX:	pCube->bones[Select].Rot = XMVectorSetX(pCube->bones[Select].Rot, XMVectorGetX(pCube->bones[Select].BindRot));	break;
			case RotateY:	pCube->bones[Select].Rot = XMVectorSetY(pCube->bones[Select].Rot, XMVectorGetY(pCube->bones[Select].BindRot));	break;
			case RotateZ:	pCube->bones[Select].Rot = XMVectorSetZ(pCube->bones[Select].Rot, XMVectorGetZ(pCube->bones[Select].BindRot));	break;
			case ScaleX:	pCube->bones[Select].Scl = XMVectorSetX(pCube->bones[Select].Scl, XMVectorGetX(pCube->bones[Select].BindScl));	break;
			case ScaleY:	pCube->bones[Select].Scl = XMVectorSetY(pCube->bones[Select].Scl, XMVectorGetY(pCube->bones[Select].BindScl));	break;
			case ScaleZ:	pCube->bones[Select].Scl = XMVectorSetZ(pCube->bones[Select].Scl, XMVectorGetZ(pCube->bones[Select].BindScl));	break;
			}
		}
	}
	else if (vnKeyboard::trg(DIK_C))
	{
		cam_theta = init_theta;
		cam_phi = init_phi;
		cam_distance = init_distance;
	}

	int line = 0;
	vnFont::print(80.0f, (float)(100 + (Cursor) * 16), 0xffffffff, L"→");
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"BoneID : %d / %d", Select, pCube->BoneNum);
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"RotationX : %.3f", XMConvertToDegrees(XMVectorGetX(pCube->bones[Select].Rot)));
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"RotationY : %.3f", XMConvertToDegrees(XMVectorGetY(pCube->bones[Select].Rot)));
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"RotationZ : %.3f", XMConvertToDegrees(XMVectorGetZ(pCube->bones[Select].Rot)));
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"ScaleX : %.3f", XMVectorGetX(pCube->bones[Select].Scl));
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"ScaleY : %.3f", XMVectorGetY(pCube->bones[Select].Scl));
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"ScaleZ : %.3f", XMVectorGetZ(pCube->bones[Select].Scl));
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"PositionX : %.3f", XMVectorGetX(pCube->bones[Select].Pos));
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"PositionY : %.3f", XMVectorGetY(pCube->bones[Select].Pos));
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"PositionZ : %.3f", XMVectorGetZ(pCube->bones[Select].Pos));
	vnFont::print(100.0f, 100.0f + (float)((++line)++ * 16), L"With [Shift] Operate All Bones");
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"With [Ctrl] Operate All Descendant Bones");
	vnFont::print(100.0f, 100.0f + (float)((++line)++ * 16), L"[W] Draw Wire Frames");
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"[P] Draw Polygon");

	vnDebugDraw::Grid();
	vnDebugDraw::Axis();

	operateCamera();

	vnScene::execute();
}

//描画関数
void SceneSkinTest::render()
{
	//キューブの描画
	if(pCube)pCube->render();

	vnScene::render();
}

void SceneSkinTest::operateCamera(bool forceUpdate)
{
	if (forceUpdate==false && vnKeyboard::on(DIK_LMENU) == false && vnKeyboard::on(DIK_RMENU) == false)return;

	bool update = forceUpdate;

	if (vnMouse::onL())
	{
		cam_theta += (float)vnMouse::getDX() * 0.01f;
		cam_phi += -(float)vnMouse::getDY() * 0.01f;
		if (cam_phi > XMConvertToRadians(87.0f))
		{
			cam_phi = XMConvertToRadians(87.0f);
		}
		else if (cam_phi < XMConvertToRadians(-87.0f))
		{
			cam_phi = XMConvertToRadians(-87.0f);
		}
		update = true;
	}
	else if (vnMouse::onR())
	{
		cam_distance += -(float)vnMouse::getDX() * 0.02f;
		if (cam_distance < 1.0f)cam_distance = 1.0f;
		update = true;
	}

	if (update)
	{
		XMVECTOR v = XMVectorSet(0.0f, 0.0f, cam_distance, 0.0f);
		XMMATRIX rotate = XMMatrixRotationRollPitchYaw(cam_phi, cam_theta, 0.0f);

		v = XMVector3TransformCoord(v, rotate);

		v = XMVectorAdd(v, *vnCamera::getTarget());

		vnCamera::setPosition(&v);
	}
}

