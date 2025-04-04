#include "main.h"

// Custom clamp function to replace std::clamp
template <typename T> T clamp(T value, T min, T max) {
	if (value < min)
		return min;
	if (value > max)
		return max;
	return value;
}

uint32_t HSL2RGB(uint8_t h, uint8_t s, uint8_t l) {
	// Convert from uint8_t range to float range
	float hue = h / 255.0f * 360.0f;
	float saturation = s / 255.0f;
	float lightness = l / 255.0f;

	// Handle grayscale case
	if (saturation == 0.0f) {
		uint8_t gray = static_cast<uint8_t>(lightness * 255.0f);
		return (gray << 16) | (gray << 8) | gray;
	}

	auto hueToRgb = [](float p, float q, float t) {
		if (t < 0.0f)
			t += 1.0f;
		if (t > 1.0f)
			t -= 1.0f;
		if (t < 1.0f / 6.0f)
			return p + (q - p) * 6.0f * t;
		if (t < 1.0f / 2.0f)
			return q;
		if (t < 2.0f / 3.0f)
			return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
		return p;
	};

	float q = lightness < 0.5f
				  ? lightness * (1.0f + saturation)
				  : lightness + saturation - lightness * saturation;
	float p = 2.0f * lightness - q;
	float r = hueToRgb(p, q, (hue / 360.0f) + 1.0f / 3.0f);
	float g = hueToRgb(p, q, hue / 360.0f);
	float b = hueToRgb(p, q, (hue / 360.0f) - 1.0f / 3.0f);

	// Convert to uint8_t and pack into uint32_t using custom clamp
	uint8_t red = static_cast<uint8_t>(clamp(r * 255.0f, 0.0f, 255.0f));
	uint8_t green = static_cast<uint8_t>(clamp(g * 255.0f, 0.0f, 255.0f));
	uint8_t blue = static_cast<uint8_t>(clamp(b * 255.0f, 0.0f, 255.0f));

	// Return as bitshifted uint32 (0x00RRGGBB format)
	return (red << 16) | (green << 8) | blue;
}

uint8_t simpleHash(uint32_t input) {
	uint8_t hash = 0;

	// Process each byte of the input
	for (int i = 0; i < 4; ++i) {
		hash ^= (input >> (i * 8)) & 0xFF; // XOR each byte into hash
		hash = (hash << 3) | (hash >> 5);  // Rotate left by 3 bits
	}

	return hash;
}

void setLEDByUID(u_int32_t uid) {
	uint8_t hue = simpleHash(uid);

	u_int32_t color = HSL2RGB(hue, 0xff, 0x7f);
	byte r = color >> 16;
	byte g = color >> 8;
	byte b = color >> 0;

	setLED(r, g, b);
}

void setLED(byte r, byte g, byte b) {
	analogWrite(LED_R, 255 - r);
	analogWrite(LED_G, 255 - g);
	analogWrite(LED_B, 255 - b);
}