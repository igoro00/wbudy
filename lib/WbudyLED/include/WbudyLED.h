#pragma once

#include <cstdint>

class WbudyLED {
    public:
        WbudyLED(uint32_t pin, bool inverted);
        WbudyLED();
        void init(uint32_t pin, bool inverted);

        void set(uint8_t value);

        void operator=(uint8_t value);
    private:
        uint32_t slice_num;
        uint32_t channel_num;
        bool inverted;
};