#pragma once

#include "../object/skinned_cube.h"

class SceneSkinTest : public vnScene
{
private:
	float Width;
	float Height;
	int BoneNum;
	
	cSkinnedCube* pCube;
	//cSkinnedSimpleCube* pCube;

	const float CubeSizeMin = 0.1f;
	const int BoneNumMin = 2;
	const int BoneNumMax = 64;

	enum eCreation
	{
		CubeBone,
		CubeWidth,
		CubeHeight,
		CreationMax,
	};

	//ëÄçÏÇÃéÌóﬁ
	enum eOperation
	{
		Bone,
		RotateX,
		RotateY,
		RotateZ,
		ScaleX,
		ScaleY,
		ScaleZ,
		PositionX,
		PositionY,
		PositionZ,
		OperationMax,
	};

	int	Cursor;
	int Select;

	float	cam_theta;
	float	cam_phi;
	float	cam_distance;
	float	init_theta;
	float	init_phi;
	float	init_distance;

	void operateCamera(bool forceUpdate = false);

public:
	
	bool initialize();
	void terminate();
	
	void execute();
	void render();
};
