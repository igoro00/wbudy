#pragma once

#include <cstdint>

class WbudyBUTTON {
    public:
        WbudyBUTTON(uint32_t pin, bool statePressed = false, uint32_t debounce = 50);
        WbudyBUTTON();
        void init(uint32_t pin, bool statePressed = false, uint32_t debounce = 50);

        void setCallback(void (*callback)(uint32_t pin, bool pressed));

        bool isPressed();

        bool debounced;
    private:
        bool initDone = false;
        uint32_t pin;
        bool statePressed; 
        uint32_t debounce;

        static void tButton(void *pvParameters);
        void (*callback)(uint32_t pin, bool pressed);
};