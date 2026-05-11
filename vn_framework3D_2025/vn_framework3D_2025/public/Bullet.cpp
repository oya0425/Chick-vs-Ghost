#include"../framework.h"
#include"../framework/vn_environment.h"
namespace
{
	// --- UpDate ---

	constexpr float boxColSize = 2.0f;

}

Bullet::Bullet() :CharacterBase(),m_velocity(XMVectorZero())
{
	m_isActive = false;
	GetCollision().SetSize(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f) * boxColSize);
	GetCollision().SetCenter(XMVectorSet(0, boxColSize / 2, 0, 0));

}

// 反射が必要か（当たり判定）のチェック
bool Bullet::IsReflectionRequired(Bullet& bullet) {
	// ここで壁との当たり判定を行い、当たっていたら true を返す
	// ついでにこの中で反射回数のチェックをしてもいいですね
	return false;
}

void Bullet::Update(float deltaTime)
{
	if (!m_isActive || !GetModel()) return;

	// ① 現在の座標と速度を取得
	XMVECTOR pos = *GetModel()->getPosition();
	XMVECTOR vel = GetVelocity();

	// ② 移動（仮移動）
	pos += vel * (deltaTime)*m_currentSpeed;

	// ③ 反射判定（壁に当たっていたら）
	if (m_isHitWall)
	{
		if (m_bounceCount < m_maxBounce)
		{
			/*
			① speed保存
			② 方向を変える（反転 or 反射）
			③ ランダム回転
			④ 正規化＋速度戻す
			⑤ 制約（Y補正など）
			*/
			// 元の速度保存
			float speed = XMVectorGetX(XMVector3Length(vel));

			// 反転
			vel = -vel;

			// ランダム角度
			float angle = (rand() % 61 - 30) * XM_PI / 180.0f;

			// 回転
			XMMATRIX rot = XMMatrixRotationY(angle);
			vel = XMVector3TransformNormal(vel, rot);

			// 正規化＋速度復元
			vel = XMVector3Normalize(vel) * speed;

			// 下に行かないように
			if (XMVectorGetY(vel) < 0)
			{
				vel = XMVectorSetY(vel, -XMVectorGetY(vel));
			}

			SetVelocity(vel);

			// 押し戻し
			pos += vel * (deltaTime * 1.2f/*壁に埋まるようになったらすこし値を入れる*/)* m_currentSpeed;

			m_bounceCount++;
			m_isHitWall = false; // フラグを戻す
		}
		else
		{
			//// 消滅処理
			//for (int i = 0; i < GetModel()->getPartsNum(); i++)
			//{
			//	GetModel()->getParts(i)->setRenderEnable(false);
			//}
			//m_isActive = false;
			//return; // 死亡したらここで終了
		}
	}
	if (m_bounceCount >= m_maxBounce)
	{
		// 消滅処理
		for (int i = 0; i < GetModel()->getPartsNum(); i++)
		{
			GetModel()->getParts(i)->setRenderEnable(false);
		}
		m_isActive = false;
		GetModel()->setPosition(0.0f, -10.0f, 0.0f);
		return; // 死亡したらここで終了

	}
	// ④ モデルへの最終反映（ここが重要！）
	// 反射してようがしてまいが、最後に確定した pos と vel から向きをセットする
	GetModel()->setPosition(&pos);

	// 向きの更新（速度ベクトルから角度を算出）
	float angle = atan2f(XMVectorGetX(vel), XMVectorGetZ(vel));
	GetModel()->setRotationY(angle);


	//XMVECTOR ropecenter = XMVectorAdd(*GetModel()->getPosition(), GetCollision().GetCenter());
	////DWORD debugColor = m_isExpanding ? GAME_COLOR_RED : GAME_COLOR_LIME; // 攻撃中は赤くする
	//float radius = XMVectorGetX(GetCollision().GetSize());
	//vnDebugDraw::Sphere(ropecenter, radius, GAME_COLOR_RED);


}

void Bullet::Shoot(XMVECTOR playerPos,XMVECTOR playerDir,float playerRotY)
{
		if (m_isActive)
		{
			// Yに足したい値
			XMVECTOR offset = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

			m_isHitWall = false;
			// プレイヤー位置から発射
			//XMVECTOR currentPlayerPos = playerPos;
			// 2. 今の座標に足し算する
			XMVECTOR currentPlayerPos = XMVectorAdd(playerPos, offset);

			XMVECTOR dir = playerDir;

			if (!XMVector3Equal(dir, XMVectorZero()))
			{
				dir = XMVector3Normalize(dir);
				SetVelocity(dir * 10.0f);
			}
			else
			{
				float rotY = playerRotY;

				// 入力ないときは向いている方向
				//float rotY = GetModel()->getRotationY();
				//float rotY = XMVectorGetY(dir);
				SetVelocity(XMVectorSet(sinf(rotY), 0, cosf(rotY), 0) * 10.0f);
			}

			// 初期設定
			// b->m_radius = 1.0f; // CharacterBaseの半径を使うならそちらをセット
			m_bounceCount = -1;
			//m_maxBounce = 3;  // ここをレベルアップで変えられるようにすると4番に繋がります
			SetIsActive(true);

			GetModel()->setPosition(&currentPlayerPos);
			for (int i = 0; i < GetModel()->getPartsNum(); i++)
			{
				GetModel()->getParts(i)->setRenderEnable(true);
			}

		}
}


// --- 速度変更 ---
void Bullet::SetSpeed(float multiplier)
{
	m_currentSpeed = m_defaultSpeed * multiplier;
}