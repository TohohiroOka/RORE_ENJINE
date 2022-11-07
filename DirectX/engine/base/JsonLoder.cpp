#include "JsonLoder.h"
#include <fstream>
#include <json.hpp>
#include "GameHelper.h"

using json = nlohmann::json;

const std::string JsonLoder::base_directory="Resources/json/";

const std::string JsonLoder::SearchName(const std::string& _name, const std::vector<std::string>& _nameList)
{
	for (auto& target : _nameList)
	{
		//������T��
		int programmerPos = int(_name.find(target));

		//���������ꍇ�͎�
		if (programmerPos == std::string::npos) { continue; }
		return target;
	}

	return "null";
}

JsonObjectData* JsonLoder::LoadFile(const std::string& _fileName, std::vector<std::string> _nameList)
{
	const std::string fullpath = base_directory + _fileName;

	//�t�@�C���X�g���[��
	std::ifstream file;

	//�t�@�C�����J��
	file.open(fullpath);
	if (file.fail()) {
		assert(0);
	}

	//�𓀃f�[�^�ۑ���
	json deserialized;

	//��
	file >> deserialized;

	//�f�[�^�����������̃`�F�b�N
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"�𕶎���Ƃ��Ď擾
	std::string name = deserialized["name"].get<std::string>();
	//���������x���f�[�^�t�@�C�����`�F�b�N
	assert(name.compare("scene") == 0);

	//�i�[�ϐ�
	JsonObjectData* jsonData = new JsonObjectData;

	//���O���X�g�o�^
	size_t listSize = _nameList.size();
	jsonData->nameList.resize(listSize + 1);
	//���O�����Ńq�b�g���Ȃ������I�u�W�F�N�g�p
	jsonData->nameList[0] = "null";
	//���O��ۑ�
	for (int i = 0; i < listSize; i++)
	{
		int inSize = i + 1;
		jsonData->nameList[inSize] = _nameList[i];
	}

	//"objects"�̑S�I�u�W�F�N�g�𑖍�
	for (json& object : deserialized["objects"]) {
		assert(object.contains("type"));

		//��ʂ��擾
		std::string type = object["type"].get<std::string>();

		if (type.compare("MESH") == 0) {
			//���O�T��
			std::string inObjectName = SearchName(object["name"], jsonData->nameList);

			// �v�f�ǉ�
			jsonData->objects[inObjectName].emplace_back(JsonObjectData::ObjectData());
			// ���ǉ������v�f�̎Q�Ƃ𓾂�
			JsonObjectData::ObjectData& objectData = jsonData->objects[inObjectName].back();

			//�g�����X�t�H�[�����
			json& transform = object["transform"];
			// ���s�ړ�
			objectData.pos.x = float(transform["translation"][0]) * mapSize / 2.0f;
			objectData.pos.y = float(transform["translation"][2]);
			objectData.pos.z = float(transform["translation"][1]) * mapSize / 2.0f;
			// ��]�p
			objectData.rotation.x = -float(transform["rotation"][1]);
			objectData.rotation.y = -float(transform["rotation"][2]);
			objectData.rotation.z = float(transform["rotation"][0]);
			// �X�P�[�����O
			objectData.scale.x = float(transform["scaling"][1]);
			objectData.scale.y = float(transform["scaling"][2]);
			objectData.scale.z = float(transform["scaling"][0]);
		}
	}

	return jsonData;
}

JsonMoveData* JsonLoder::LoadMoveFile(const std::string& _fileName)
{
	const std::string fullpath = base_directory + _fileName;

	//�t�@�C���X�g���[��
	std::ifstream file;

	//�t�@�C�����J��
	file.open(fullpath);
	if (file.fail()) {
		assert(0);
	}

	//�𓀃f�[�^�ۑ���
	json deserialized;

	//��
	file >> deserialized;

	//�f�[�^�����������̃`�F�b�N
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"�𕶎���Ƃ��Ď擾
	std::string name = deserialized["name"].get<std::string>();
	//���������x���f�[�^�t�@�C�����`�F�b�N
	assert(name.compare("move_point") == 0);

	//�i�[�ϐ�
	JsonMoveData* jsonData = new JsonMoveData;
	//�T�C�Y�ύX
	jsonData->movePoint.resize(int(deserialized["total"]));

	//"objects"�̑S�I�u�W�F�N�g�𑖍�
	for (json& object : deserialized["posints"]) {
		//�v�f�ԍ�
		int number = object["number"];

		//�g�����X�t�H�[�����
		json& transform = object["transform"];
		// ���s�ړ�
		jsonData->movePoint[number].pos.x = float(object["point"][0]) * mapSize / 2.0f;
		jsonData->movePoint[number].pos.y = 100.0f;
		jsonData->movePoint[number].pos.z = float(object["point"][1]) * mapSize / 2.0f;

		const int move_num = object["list_num"];
		for (int i = 0; i < move_num; i++) {
			jsonData->movePoint[number].moveList.emplace_back(int(object["move_list"][i]));
		}
	}

	return jsonData;
}
