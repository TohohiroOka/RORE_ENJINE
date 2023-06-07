#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

//名前を結合する
static LPCWSTR GetName(std::string _className, std::string _setName)
{
	//名前の結合
	std::string name = _className + _setName;
	//大きさ取得
	size_t size = name.size();
	//名前のサイズ+1の配列に作り直す
	LPWSTR returnName = new wchar_t[size + 1];
	std::copy(name.begin(), name.end(), returnName);
	//配列の末に0を代入
	returnName[size] = 0;

	return returnName;
}