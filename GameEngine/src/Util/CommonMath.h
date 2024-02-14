#pragma once
class CommonMath
{
public:
	static float Lerp(float a, float b, float f) {
		return a + CommonMath::Clamp(f,0.0f,1.0f) * (b - a);
	}

	static float Clamp(float value, float min, float max) {
		if (value < min) {
			return min;
		}
		else if (value > max) {
			return max;
		}
		else {
			return value;
		}
	}

	static float SmoothStep(float edge0, float edge1, float x) {
		float t = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		return t * t * (3.0f - 2.0f * t);
	}
};

