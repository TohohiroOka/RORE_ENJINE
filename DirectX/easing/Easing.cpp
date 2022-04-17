#include "Easing.h"
#include <cmath>

//“™‘¬’¼ü‰^“®
float Easing::Lerp(const float& start, const float& end, const float time)
{
	return start * (1.0f - time) + end * time;
}

//sin In
float Easing::InSine(const float& start, const float& end, const float time)
{
	float y = (1 - cosf((time * 3.14159265359f) / 2));
	return start * (1.0f - y) + end * y;
}

//sin Out
float Easing::OutSine(const float& start, const float& end, const float time)
{
	float y = sinf((time * 3.14159265359f) / 2);
	return start * (1.0f - y) + end * y;
}

//sin InOut
float Easing::InOutSine(const float& start, const float& end, const float time)
{
	float y = (-(cosf(3.14159265359f * time) - 1) / 2);
	return start * (1.0f - y) + end * y;
}

//2æ In
float Easing::InQuad(const float& start, const float& end, const float time)
{
	float y = time * time;
	return start * (1.0f - y) + end * y;
}

//2æ Out
float Easing::OutQuad(const float& start, const float& end, const float time)
{
	float y = (1 - (1 - time) * (1 - time));
	return start * (1.0f - y) + end * y;
}

//2æ InOut
float Easing::InOutQuad(const float& start, const float& end, const float time)
{
	float y;
	if (time < 0.5)
	{
		y = 2 * time * time;
	} else
	{
		y = (1 - powf(-2 * time + 2, 2) / 2);
	}

	return start * (1.0f - y) + end * y;
}

//3æ In
float Easing::InCubic(const float& start, const float& end, const float time)
{
	float y = time * time * time;
	return start * (1.0f - y) + end * y;
}

//3æ Out
float Easing::OutCubic(const float& start, const float& end, const float time)
{
	float y = (1 - powf(1 - time, 3));
	return start * (1.0f - y) + end * y;
}

//3æ InOut
float Easing::InOutCubic(const float& start, const float& end, const float time)
{
	float y;
	if (time < 0.5)
	{
		y = 4 * time * time * time;
	} else
	{
		y = (1 - powf(-2 * time + 2, 3) / 2);
	}

	return start * (1.0f - y) + end * y;
}

//4æ In
float Easing::InQuart(const float& start, const float& end, const float time)
{
	float y = time * time * time * time;
	return start * (1.0f - y) + end * y;
}

//4æ Out
float Easing::OutQuart(const float& start, const float& end, const float time)
{
	float y = (1 - powf(1 - time, 4));
	return start * (1.0f - y) + end * y;
}

//4æ InOut
float Easing::InOutQuart(const float& start, const float& end, const float time)
{
	float y;
	if (time < 0.5f)
	{
		y = 8 * time * time * time * time;
	} else
	{
		y = (1 - powf(-2 * time + 2, 4) / 2);
	}

	return start * (1.0f - y) + end * y;
}

//5æ In
float Easing::InQuint(const float& start, const float& end, const float time)
{
	float y = time * time * time * time * time;
	return start * (1.0f - y) + end * y;
}

//5æ Out
float Easing::OutQuint(const float& start, const float& end, const float time)
{
	float y = (1 - powf(1 - time, 5));
	return start * (1.0f - y) + end * y;
}

//5æ InOut
float Easing::InOutQuint(const float& start, const float& end, const float time)
{
	float y;
	if (time < 0.5f)
	{
		y = 16 * time * time * time * time * time;
	} else
	{
		y = (1 - powf(-2 * time + 2, 5) / 2);
	}

	return start * (1.0f - y) + end * y;
}

//Expo In
float Easing::InExpo(const float& start, const float& end, const float time)
{
	float y;
	if (time == 0)
	{
		y = 0;
	} else
	{
		y = powf(2, 10 * time - 10);
	}

	return start * (1.0f - y) + end * y;
}

//Expo Out
float Easing::OutExpo(const float& start, const float& end, const float time)
{
	float y;
	if (time == 1)
	{
		y = 1;
	} else
	{
		y = (1 - powf(2, -10 * time));
	}

	return start * (1.0f - y) + end * y;
}

//Expo InOut
float Easing::InOutExpo(const float& start, const float& end, const float time)
{
	float y;
	if (time == 0)
	{
		y = 0;
	} else if (time == 1)
	{
		y = 1;
	} else if (time < 0.5)
	{
		y = powf(2, 20 * time - 10) / 2;
	} else
	{
		y = (2 - powf(2, -20 * time + 10)) / 2;
	}

	return start * (1.0f - y) + end * y;
}

//Circ In
float Easing::InCirc(const float& start, const float& end, const float time)
{
	float y = 1 - sqrtf(1 - powf(time, 2));
	return start * (1.0f - y) + end * y;
}

//Circ Out
float Easing::OutCirc(const float& start, const float& end, const float time)
{
	float y = sqrtf(1 - powf(time - 1, 2));
	return start * (1.0f - y) + end * y;
}

//Circ InOut
float Easing::InOutCirc(const float& start, const float& end, const float time)
{
	float y;
	if (time < 0.5)
	{
		y = (1 - sqrtf(1 - powf(2 * time, 2))) / 2;
	} else
	{
		y = (sqrtf(1 - powf(-2 * time + 2, 2)) + 1) / 2;
	}

	return start * (1.0f - y) + end * y;
}

//Back In
float Easing::InBack(const float& start, const float& end, const float time)
{
	float y = 2.70158f * time * time * time - 1.70158f * time * time;
	return start * (1.0f - y) + end * y;
}

//Back Out
float Easing::OutBack(const float& start, const float& end, const float time)
{
	float y = 1 + 2.70158f * powf(time - 1, 3) + 1.70158f * powf(time - 1, 2);
	return start * (1.0f - y) + end * y;
}

//Back InOut
float Easing::InOutBack(const float& start, const float& end, const float time)
{
	float y;
	if (time < 0.5f)
	{
		y = (powf(2 * time, 2) * ((1.70158f * 1.525f + 1) * 2 * time - 1.70158f * 1.525f)) / 2;
	} else
	{
		y = (powf(2 * time - 2, 2) * ((1.70158f * 1.525f + 1) * (time * 2 - 2) + 1.70158f * 1.525f) + 2) / 2;
	}

	return start * (1.0f - y) + end * y;
}

//Elastic In
float Easing::InElastic(const float& start, const float& end, const float time)
{
	float y;
	if (time == 0)
	{
		y = 0;
	} else if (time == 1)
	{
		y = 1;
	} else
	{
		y = -powf(2, 10 * time - 10) * sinf((time * 10 - 10.75f) * (2 * 3.14159265359f) / 3);
	}

	return start * (1.0f - y) + end * y;
}

//Elastic Out
float Easing::OutElastic(const float& start, const float& end, const float time)
{
	float y;
	if (time == 0)
	{
		y = 0;
	} else if (time == 1)
	{
		y = 1;
	} else
	{
		y = powf(2, -10 * time) * sinf((time * 10 - 0.75f) * (2 * 3.14159265359f) / 3) + 1;
	}

	return start * (1.0f - y) + end * y;
}

//Elastic InOut
float Easing::InOutElastic(const float& start, const float& end, const float time)
{
	float y;
	if (time == 0)
	{
		y = 0;
	} else if (time == 1)
	{
		y = 1;
	} else if (time < 0.5)
	{
		y = -(powf(2, 20 * time - 10) * sinf((20 * time - 11.125f) * (2 * 3.14159265359f) / 4.5f)) / 2;
	} else
	{
		y = (powf(2, -20 * time + 10) * sinf((20 * time - 11.125f) * (2 * 3.14159265359f) / 4.5f)) / 2 + 1;
	}

	return start * (1.0f - y) + end * y;
}

//Bounce In
float Easing::InBounce(const float& start, const float& end, const float time)
{
	float y = 1 - OutBounceCalculate(1 - time);
	return start * (1.0f - y) + end * y;
}

//Bounce Out
float Easing::OutBounce(const float& start, const float& end, const float time)
{
	float y = OutBounceCalculate(time);
	return start * (1.0f - y) + end * y;
}

//Bounce InOut
float Easing::InOutBounce(const float& start, const float& end, const float time)
{
	float y;
	if (time < 0.5)
	{
		y = (1 - OutBounceCalculate(1 - 2 * time)) / 2;
	} else
	{
		y = (1 + OutBounceCalculate(2 * time - 1)) / 2;
	}

	return start * (1.0f - y) + end * y;
}

//BounceŒvZ—p
float Easing::OutBounceCalculate(float time)
{
	float y;
	if (time < 1 / 2.75f)
	{
		y = 7.5625f * time * time;
	} else if (time < 2 / 2.75)
	{
		y = 7.5625f * (time -= 1.5f / 2.75f) * time + 0.75f;
	} else if (time < 2.5 / 2.75)
	{
		y = 7.5625f * (time -= 2.25f / 2.75f) * time + 0.9375f;
	} else
	{
		y = 7.5625f * (time -= 2.625f / 2.75f) * time + 0.984375f;
	}

	return y;
}