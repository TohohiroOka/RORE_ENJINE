#include "PmxLoader.h"
#include <fstream>
#include <array>
#include <string>
#include <vector>

using namespace DirectX;

bool getPMXStringUTF16(std::ifstream& _file, std::wstring& output)
{
	std::array<wchar_t, 512> wBuffer{};
	int textSize;

	_file.read(reinterpret_cast<char*>(&textSize), 4);

	_file.read(reinterpret_cast<char*>(&wBuffer), textSize);
	output = std::wstring(&wBuffer[0], &wBuffer[0] + textSize / 2);

	return true;
}

std::vector<PmxLoader::Vertex> PmxLoader::CollectVertex(std::ifstream& pmxFile, std::array<byte, 8> hederData)
{
	// 頂点
	int numberOfVertex{};
	pmxFile.read(reinterpret_cast<char*>(&numberOfVertex), 4);
	std::vector<Vertex> vertices;
	vertices.resize(numberOfVertex);

	for (int i = 0; i < numberOfVertex; i++)
	{
		pmxFile.read(reinterpret_cast<char*>(&vertices[i].position), 12);
		pmxFile.read(reinterpret_cast<char*>(&vertices[i].normal), 12);
		pmxFile.read(reinterpret_cast<char*>(&vertices[i].uv), 8);
		if (hederData[NUMBER_OF_ADD_UV] != 0)
		{
			for (int j = 0; j < hederData[NUMBER_OF_ADD_UV]; ++j)
			{
				pmxFile.read(reinterpret_cast<char*>(&vertices[i].additionalUV[j]), 16);
			}
		}

		const byte weightMethod = pmxFile.get();
		//ボーンタイプ1
		if (weightMethod == WeightType::BDEF1)
		{
			vertices[i].weight.type = WeightType::BDEF1;
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
			vertices[i].weight.born2 = NO_DATA_FLAG;
			vertices[i].weight.born3 = NO_DATA_FLAG;
			vertices[i].weight.born4 = NO_DATA_FLAG;
			vertices[i].weight.weight1 = 1.0f;
		}
		//ボーンタイプ2
		else if (weightMethod == WeightType::BDEF2)
		{
			vertices[i].weight.type = WeightType::BDEF2;
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born2), hederData[BONE_INDEX_SIZE]);
			vertices[i].weight.born3 = NO_DATA_FLAG;
			vertices[i].weight.born4 = NO_DATA_FLAG;
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.weight1), 4);
			vertices[i].weight.weight2 = 1.0f - vertices[i].weight.weight1;
		}
		//ボーンタイプ4
		else if (weightMethod == WeightType::BDEF4)
		{
			vertices[i].weight.type = WeightType::BDEF4;
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born2), hederData[BONE_INDEX_SIZE]);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born3), hederData[BONE_INDEX_SIZE]);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born4), hederData[BONE_INDEX_SIZE]);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.weight1), 4);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.weight2), 4);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.weight3), 4);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.weight4), 4);
		}
		//ボーンタイプ4
		else if (weightMethod == WeightType::SDEF)
		{
			vertices[i].weight.type = WeightType::SDEF;
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.born2), hederData[BONE_INDEX_SIZE]);
			vertices[i].weight.born3 = NO_DATA_FLAG;
			vertices[i].weight.born4 = NO_DATA_FLAG;
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.weight1), 4);
			vertices[i].weight.weight2 = 1.0f - vertices[i].weight.weight1;
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.c), 12);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.r0), 12);
			pmxFile.read(reinterpret_cast<char*>(&vertices[i].weight.r1), 12);
		}
		//それ以外はy読み込み失敗なのでエラーを出力
		else
		{
			pmxFile.close();
			assert(0);
		}

		pmxFile.read(reinterpret_cast<char*>(&vertices[i].edgeMagnif), 4);

		//データが無いためエラーを出力
		if (vertices[i].weight.born1 == NO_DATA_FLAG)
		{
			pmxFile.close();
			assert(0);
		}
	}

	return vertices;
}

std::vector<PmxLoader::Surface> PmxLoader::CollectSurface(std::ifstream& pmxFile, std::array<byte, 8> hederData)
{
	int numOfSurface{};
	pmxFile.read(reinterpret_cast<char*>(&numOfSurface), 4);
	std::vector<Surface> surfaces;
	surfaces.resize(numOfSurface);

	for (int i = 0; i < numOfSurface; i++)
	{
		pmxFile.read(reinterpret_cast<char*>(&surfaces[i].vertexIndex), hederData[VERTEX_INDEX_SIZE]);

		if (surfaces[i].vertexIndex == NO_DATA_FLAG ||
			surfaces[i].vertexIndex == NO_DATA_FLAG ||
			surfaces[i].vertexIndex == NO_DATA_FLAG)
		{
			pmxFile.close();
			assert(0);
		}
	}

	return surfaces;
}

std::vector<std::wstring> PmxLoader::CollectTexturePaths(std::ifstream& pmxFile, std::wstring folderPath)
{
	int numOfTexture{};
	pmxFile.read(reinterpret_cast<char*>(&numOfTexture), 4);

	std::vector<std::wstring> texturePaths;
	texturePaths.resize(numOfTexture);

	std::wstring texturePath{};
	for (int i = 0; i < numOfTexture; i++)
	{
		texturePaths[i] = folderPath;
		getPMXStringUTF16(pmxFile, texturePath);
		texturePaths[i] += texturePath;
	}

	return texturePaths;
}

std::vector<PmxLoader::Material> PmxLoader::CollectMaterial(std::ifstream& pmxFile,
	std::array<byte, 8> hederData, unsigned arrayLength)
{
	int numOfMaterial{};
	pmxFile.read(reinterpret_cast<char*>(&numOfMaterial), 4);

	std::vector<Material>materials;
	materials.resize(numOfMaterial);

	for (int i = 0; i < numOfMaterial; i++)
	{
		for (int j = 0; j < 2; ++j)
		{
			pmxFile.read(reinterpret_cast<char*>(&arrayLength), 4);
			for (unsigned i = 0; i < arrayLength; i++)
			{
				pmxFile.get();
			}
		}

		pmxFile.read(reinterpret_cast<char*>(&materials[i].diffuse), 16);
		pmxFile.read(reinterpret_cast<char*>(&materials[i].specular), 12);
		pmxFile.read(reinterpret_cast<char*>(&materials[i].specularity), 4);
		pmxFile.read(reinterpret_cast<char*>(&materials[i].ambient), 12);

		pmxFile.get();
		for (int i = 0; i < 16; i++)
		{
			pmxFile.get();
		}
		for (int i = 0; i < 4; i++)
		{
			pmxFile.get();
		}

		pmxFile.read(reinterpret_cast<char*>(&materials[i].colorMapTextureIndex), hederData[TEXTURE_INDEX_SIZE]);
		for (unsigned char i = 0; i < hederData[TEXTURE_INDEX_SIZE]; i++)
		{
			pmxFile.get();
		}
		pmxFile.get();

		const byte shareToonFlag = pmxFile.get();
		if (shareToonFlag)
		{
			pmxFile.get();
		} else
		{
			pmxFile.read(reinterpret_cast<char*>(&materials[i].toonTextureIndex), hederData[TEXTURE_INDEX_SIZE]);
		}

		pmxFile.read(reinterpret_cast<char*>(&arrayLength), 4);
		for (unsigned i = 0; i < arrayLength; i++)
		{
			pmxFile.get();
		}

		pmxFile.read(reinterpret_cast<char*>(&materials[i].vertexNum), 4);
	}

	return materials;
}

std::vector<PmxLoader::Bone> PmxLoader::CollectBone(std::ifstream& pmxFile,
	std::array<byte, 8> hederData, unsigned arrayLength)
{
	int numOfBone{};
	pmxFile.read(reinterpret_cast<char*>(&numOfBone), 4);

	std::vector<Bone> bones;
	bones.resize(numOfBone);

	int ikLinkSize = 0;
	unsigned char angleLim = 0;

	for (int i = 0; i < numOfBone; i++)
	{
		getPMXStringUTF16(pmxFile, bones[i].name);
		pmxFile.read(reinterpret_cast<char*>(&arrayLength), 4);
		bones[i].nameEnglish.resize(arrayLength);
		for (unsigned j = 0; j < arrayLength; ++j)
		{
			bones[i].nameEnglish[j] = pmxFile.get();
		}

		pmxFile.read(reinterpret_cast<char*>(&bones[i].position), 12);

		pmxFile.read(reinterpret_cast<char*>(&bones[i].parentIndex), hederData[BONE_INDEX_SIZE]);
		if (numOfBone <= bones[i].parentIndex)
		{
			bones[i].parentIndex = NO_DATA_FLAG;
		}

		pmxFile.read(reinterpret_cast<char*>(&bones[i].transformationLevel), 4);

		pmxFile.read(reinterpret_cast<char*>(&bones[i].flag), 2);

		enum BoneFlagMask
		{
			ACCESS_POINT = 0x0001,
			IK = 0x0020,
			IMPART_TRANSLATION = 0x0100,
			IMPART_ROTATION = 0x0200,
			AXIS_FIXING = 0x0400,
			LOCAL_AXIS = 0x0800,
			EXTERNAL_PARENT_TRANS = 0x2000,
		};

		if (bones[i].flag & ACCESS_POINT)
		{
			pmxFile.read(reinterpret_cast<char*>(&bones[i].childrenIndex), hederData[BONE_INDEX_SIZE]);
			if (numOfBone <= bones[i].childrenIndex)
			{
				bones[i].childrenIndex = NO_DATA_FLAG;
			}
		} else
		{
			bones[i].childrenIndex = NO_DATA_FLAG;
			pmxFile.read(reinterpret_cast<char*>(&bones[i].coordOffset), 12);
		}
		if ((bones[i].flag & IMPART_TRANSLATION) || (bones[i].flag & IMPART_ROTATION))
		{
			pmxFile.read(reinterpret_cast<char*>(&bones[i].impartParentIndex), hederData[BONE_INDEX_SIZE]);
			pmxFile.read(reinterpret_cast<char*>(&bones[i].impartRate), 4);
		}
		if (bones[i].flag & AXIS_FIXING)
		{
			pmxFile.read(reinterpret_cast<char*>(&bones[i].fixedAxis), 12);
		}
		if (bones[i].flag & LOCAL_AXIS)
		{
			pmxFile.read(reinterpret_cast<char*>(&bones[i].localAxisX), 12);
			pmxFile.read(reinterpret_cast<char*>(&bones[i].localAxisZ), 12);
		}
		if (bones[i].flag & EXTERNAL_PARENT_TRANS)
		{
			pmxFile.read(reinterpret_cast<char*>(&bones[i].externalParentKey), 4);
		}
		if (bones[i].flag & IK)
		{
			pmxFile.read(reinterpret_cast<char*>(&bones[i].ikTargetIndex), hederData[5]);
			pmxFile.read(reinterpret_cast<char*>(&bones[i].ikLoopCount), 4);
			pmxFile.read(reinterpret_cast<char*>(&bones[i].ikUnitAngle), 4);
			pmxFile.read(reinterpret_cast<char*>(&ikLinkSize), 4);
			bones[i].ikLinks.resize(ikLinkSize);
			for (int j = 0; j < ikLinkSize; ++j)
			{
				pmxFile.read(reinterpret_cast<char*>(&bones[i].ikLinks[j].index), hederData[5]);
				angleLim = pmxFile.get();
				bones[i].ikLinks[j].existAngleLimited = false;
				if (angleLim == 1)
				{
					pmxFile.read(reinterpret_cast<char*>(&bones[i].ikLinks[j].limitAngleMin), 12);
					pmxFile.read(reinterpret_cast<char*>(&bones[i].ikLinks[j].limitAngleMax), 12);
					bones[i].ikLinks[j].existAngleLimited = true;
				}
			}
		} else
		{
			bones[i].ikTargetIndex = NO_DATA_FLAG;
		}
	}

	return bones;
}

void PmxLoader::LoadPMX(const std::wstring& _filePath, std::vector<Vertex>& vertices,
	std::vector<Surface>& surfaces, std::vector<std::wstring>& texturePaths,
	std::vector<Material>& materials, std::vector<Bone>& bones)
{
	//パスが空なら返す
	if (_filePath.empty())
	{
		assert(0);
	}

	// モデルファイルのパスからモデルフォルダのパスを抽出
	std::wstring folderPath{ _filePath.begin(), _filePath.begin() + _filePath.rfind(L'\\') + 1 };

	// ファイルオープン
	std::ifstream pmxFile{ _filePath, (std::ios::binary | std::ios::in) };
	if (pmxFile.fail())
	{
		pmxFile.close();
		assert(0);
	}

	// ヘッダー
	std::array<byte, 4> pmxHeader{};
	constexpr std::array<byte, 4> PMX_MAGIC_NUMBER{ 0x50, 0x4d, 0x58, 0x20 };

	for (int i = 0; i < 4; i++)
	{
		pmxHeader[i] = pmxFile.get();
	}
	if (pmxHeader != PMX_MAGIC_NUMBER)
	{
		pmxFile.close();
		assert(0);
	}

	// ver2.0以外は非対応
	float version{};
	pmxFile.read(reinterpret_cast<char*>(&version), 4);
	if (!XMScalarNearEqual(version, 2.0f, g_XMEpsilon.f[0]))
	{
		pmxFile.close();
		assert(0);
	}

	byte hederDataLength = pmxFile.get();
	if (hederDataLength != 8)
	{
		pmxFile.close();
		assert(0);
	}
	std::array<byte, 8> hederData{};
	for (int i = 0; i < hederDataLength; i++)
	{
		hederData[i] = pmxFile.get();
	}
	//UTF-8は非対応
	if (hederData[0] != 0)
	{
		pmxFile.close();
		assert(0);
	}

	//配列数
	unsigned arrayLength{};
	for (int i = 0; i < 4; i++)
	{
		pmxFile.read(reinterpret_cast<char*>(&arrayLength), 4);
		for (unsigned j = 0; j < arrayLength; j++)
		{
			pmxFile.get();
		}
	}

	//頂点の読み込み
	vertices = CollectVertex(pmxFile, hederData);

	//面の読み込み
	surfaces = CollectSurface(pmxFile, hederData);

	//テクスチャの読み込み
	texturePaths = CollectTexturePaths(pmxFile, folderPath);

	//マテリアルの読み込み
	materials = CollectMaterial(pmxFile, hederData, arrayLength);

	//ボーンの読み込み
	bones = CollectBone(pmxFile, hederData, arrayLength);

	pmxFile.close();
}