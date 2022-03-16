#include "Audio.h"
#include <fstream>
#include <cassert>

#pragma comment(lib,"xaudio2.lib")

std::vector<Audio::SoundData*> Audio::SData;
Microsoft::WRL::ComPtr<IXAudio2>Audio::xAudio2;
IXAudio2MasteringVoice* Audio::masterVoice;

Audio::Audio()
{
	XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);//XAudioエンジンのインスタンス生成
	xAudio2->CreateMasteringVoice(&masterVoice);//マスターボイス生成
	masterVoice->SetVolume(0.1f);//全体の音量}
}

Audio::~Audio()
{
	//xAudio2解放
	xAudio2.Reset();

	const int size = SData.size();

	//バッファのメモリを解放
	for (int i = 0; i < size; i++)
	{
		delete[] SData[i]->pBuffer;

		SData[i]->pBuffer = 0;
		SData[i]->bufferSize = 0;
		SData[i]->wfex = {};
	}

	SData.clear();
}

//音声データ読み込み
int Audio::SoundLoadWave(const char* fileName)
{
	//現在の配列サイズ
	int size = SData.size();

	SoundData* soundData = nullptr;
	soundData = new SoundData();

	//ファイル入力ストリームのインスタンス
	std::ifstream file;

	//wavファイルをバイナリモードで開く
	file.open(fileName, std::ios_base::binary);

	//ファイルオープン失敗を検知
	assert(file.is_open());

	///wavデータ読み込み
	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	//ファイルがRIFFがチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}

	//タイプがWAVEがチェック
	if (strncmp(riff.type, "WAVE", 4) != 0)
	{
		assert(0);
	}

	//Formatチャンクの読み込み
	FormatChunk format = {};

	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));

	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK ", 4) == 0 ||
		strncmp(data.id, "LIST", 4) == 0)
	{
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);

		//再度読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0)
	{
		assert(0);
	}

	//Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	///ファイルクローズ
	//Waveファイルを閉じる
	file.close();

	///読み込んだ音声データを返す
	//returnするための音声データ
	soundData->wfex = format.fmt;
	soundData->pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData->bufferSize = data.size;

	CreateSoundData(*soundData);
	float SourceVoiceChannelVolumes[1] = { 0.0 };
	soundData->pSourceVoice->SetChannelVolumes(1, SourceVoiceChannelVolumes);

	SData.push_back(soundData);

	return size;
}

//データの生成
void Audio::CreateSoundData(SoundData& soundData) {
	HRESULT result;

	//波形フォーマットを元にSourceVoiceの生成
	result = xAudio2->CreateSourceVoice(&soundData.pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));
}

//音声再生
bool Audio::SoundPlayWava(int number, bool roop) {
	HRESULT result;

	XAUDIO2_VOICE_STATE xa3state;
	SData[number]->pSourceVoice->GetState(&xa3state);
	//0でないなら音楽が再生中になるためスルーする
	if (xa3state.BuffersQueued != 0)
	{
		return false;
	}

	//データ生成
	CreateSoundData(*SData[number]);

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = SData[number]->pBuffer;
	buf.AudioBytes = SData[number]->bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	if (roop == true)
	{
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	//波形データの再生
	result = SData[number]->pSourceVoice->SubmitSourceBuffer(&buf);
	result = SData[number]->pSourceVoice->Start();

	buf = { NULL };

	return true;
}

//音楽の停止
void Audio::StopSound(int number)
{
	SData[number]->pSourceVoice->Stop(0);
	SData[number]->pSourceVoice->FlushSourceBuffers();
	SData[number]->pSourceVoice->SubmitSourceBuffer(&buf);
}