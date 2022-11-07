#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include <unordered_map>

struct JsonObjectData {
	struct ObjectData {
		// ���s�ړ�
		DirectX::XMFLOAT3 pos;
		// ��]�p
		DirectX::XMFLOAT3 rotation;
		// �X�P�[�����O
		DirectX::XMFLOAT3 scale;
	};

	//���O�̃��X�g
	std::vector<std::string> nameList;
	//��ނ��Ƃ̖��O�z��
	std::unordered_map<std::string, std::vector<ObjectData>> objects;
};

struct JsonMoveData {
	struct MoveData {
		//���W
		DirectX::XMFLOAT3 pos;
		//�ړ���
		std::vector<int> moveList;
	};

	std::vector<MoveData> movePoint;
};

class JsonLoder
{
public:

	/// <summary>
	/// ���O�̒T��
	/// </summary>
	/// <param name="_name">�T�����̖��O</param>
	/// <param name="_nameList">���O�̃��X�g</param>
	/// <returns>�����������[�h</returns>
	static const std::string SearchName(const std::string& _name, const std::vector<std::string>& _nameList);

	/// <summary>
	/// json�̓ǂݍ���
	/// </summary>
	/// <param name="_fileName">�t�@�C����</param>
	/// <param name="_nameList">��ޕʖ��O�̃��X�g</param>
	static JsonObjectData* LoadFile(const std::string& _fileName, std::vector<std::string> _nameList);

	/// <summary>
	/// json�̓ǂݍ���
	/// </summary>
	/// <param name="_fileName">�t�@�C����</param>
	static JsonMoveData* LoadMoveFile(const std::string& _fileName);

private:

	static const std::string base_directory;
};