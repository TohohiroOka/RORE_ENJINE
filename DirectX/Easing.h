#pragma once
class Easing
{
public:
	//等速直線運動
	static float Lerp(const float& start, const float& end, const float time);

	//sin
	static float InSine(const float& start, const float& end, const float time);
	static float OutSine(const float& start, const float& end, const float time);
	static float InOutSine(const float& start, const float& end, const float time);

	//2乗
	static float InQuad(const float& start, const float& end, const float time);
	static float OutQuad(const float& start, const float& end, const float time);
	static float InOutQuad(const float& start, const float& end, const float time);

	//3乗
	static float InCubic(const float& start, const float& end, const float time);
	static float OutCubic(const float& start, const float& end, const float time);
	static float InOutCubic(const float& start, const float& end, const float time);

	//4乗
	static float InQuart(const float& start, const float& end, const float time);
	static float OutQuart(const float& start, const float& end, const float time);
	static float InOutQuart(const float& start, const float& end, const float time);

	//5乗
	static float InQuint(const float& start, const float& end, const float time);
	static float OutQuint(const float& start, const float& end, const float time);
	static float InOutQuint(const float& start, const float& end, const float time);

	//Expo
	static float InExpo(const float& start, const float& end, const float time);
	static float OutExpo(const float& start, const float& end, const float time);
	static float InOutExpo(const float& start, const float& end, const float time);

	//Circ
	static float InCirc(const float& start, const float& end, const float time);
	static float OutCirc(const float& start, const float& end, const float time);
	static float InOutCirc(const float& start, const float& end, const float time);

	//Back
	static float InBack(const float& start, const float& end, const float time);
	static float OutBack(const float& start, const float& end, const float time);
	static float InOutBack(const float& start, const float& end, const float time);

	//Elastic
	static float InElastic(const float& start, const float& end, const float time);
	static float OutElastic(const float& start, const float& end, const float time);
	static float InOutElastic(const float& start, const float& end, const float time);

	//Bounce
	static float InBounce(const float& start, const float& end, const float time);
	static float OutBounce(const float& start, const float& end, const float time);
	static float InOutBounce(const float& start, const float& end, const float time);

private:
	/// <summary>
	/// Bounce計算用
	/// </summary>
	/// <param name="time">イージング時間(0～1)</param>
	/// <returns>イージング計算結果</returns>
	static float OutBounceCalculate(float time);
};