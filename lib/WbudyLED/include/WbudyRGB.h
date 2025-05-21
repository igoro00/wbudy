#pragma once

#include <cstdint>
#include <memory>

#include "WbudyLED.h"

class WbudyRGB {
    public:
        WbudyRGB(uint32_t r, uint32_t g, uint32_t b);

        void setRGB(uint8_t r, uint8_t g, uint8_t b);
        void setRGB(uint32_t rgb);
        void setHSL(uint8_t h, uint8_t s, uint8_t l);
        void setHSL(uint32_t hsl);
    private:
        std::shared_ptr<WbudyLED>* led_r;
        std::shared_ptr<WbudyLED>* led_g;
        std::shared_ptr<WbudyLED>* led_b;

        uint32_t hslToRGB(uint8_t h, uint8_t s, uint8_t l);
        template <typename T> T clamp(T value, T min, T max);
};
