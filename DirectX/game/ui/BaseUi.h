#pragma once
#include "Sprite.h"

class BaseUi
{
protected: // �G�C���A�X
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;

protected:

	struct SPRITE_INFO
	{
		std::string name;
		XMFLOAT2 texSize;
	};

	enum class SPRITE {
		BOSS_HP_IN,//�{�Xhp�o�[���g
		BOSS_HP_OUT,//�{�Xhp�o�[�O�g
	};

public:

	/// <summary>
	/// ���O��ۑ�����
	/// </summary>
	/// <param name="_name">�t�@�C����</param>
	/// <param name="_sprite">enum��</param>
	/// <param name="_texSize">�e�N�X�`���̑傫��</param>
	static void KeepName(const std::string& _name, const SPRITE _sprite, const XMFLOAT2& _texSize);

	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize();

protected:

	//�e�N�X�`���ۑ���
	static std::map<SPRITE, SPRITE_INFO> spriteName;

protected:

	//�`��t���O
	bool isDraw;
};