#pragma once
#include <Windows.h>
#include <xaudio2.h>
#include <wrl.h>
#include <d3dx12.h>

class Audio
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//チャンクヘッダ
	struct ChunkHeader
	{
		char id[4];//チャンク毎のID
		int	size;  // チャンクサイズ
	};

	//RIFFヘッダチャンク
	struct RiffHeader
	{
		ChunkHeader chunk;//"RIFF"
		char type[4];//"WAVE"
	};

	//FMTチャンク
	struct FormatChunk
	{
		ChunkHeader chunk;//"fmt"
		WAVEFORMATEX fmt;//波形フォーマット
	};

	//音声データ
	struct SoundData
	{
		WAVEFORMATEX wfex;//波形フォーマット
		BYTE* pBuffer;//バッファの先頭アドレス
		unsigned int bufferSize;//バッファのサイズ
		IXAudio2SourceVoice* pSourceVoice;
	};


public:

	Audio();
	~Audio();

	//音声データ読み込み
	int SoundLoadWave(const char* fileName);
	
	//データの生成
	void CreateSoundData(SoundData& soundData);

	//音声再生
	bool SoundPlayWava(int number, bool roop);

	//サウンドの停止
	void StopSound(int number);

private:
	//サウンドデータ配列
	static std::vector<SoundData*> SData;

	//サウンドのバッファ
	XAUDIO2_BUFFER buf{};

	static ComPtr<IXAudio2>xAudio2;
	static IXAudio2MasteringVoice* masterVoice;
};