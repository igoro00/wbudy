#pragma once

#include <cstdint>

class WbudyBUTTON {
    public:
        WbudyBUTTON(uint32_t pin, bool statePressed = false, uint32_t debounce = 50, uint32_t longPress = 1000);
        WbudyBUTTON();
        void init(uint32_t pin, bool statePressed = false, uint32_t debounce = 50, uint32_t longPress = 1000);

        void setOnPressed(void (*cb)(WbudyBUTTON *btn));
        void setOnReleased(void (*cb)(WbudyBUTTON *btn));
        void setOnLongPressed(void (*cb)(WbudyBUTTON *btn));
        void setOnChanged(void (*cb)(WbudyBUTTON *btn, bool pressed));

        uint32_t msSincePress();

        bool isPressed();

        bool debounced;

        uint32_t getPin() const;
    private:
        bool initDone = false;
        uint32_t pin;
        bool statePressed; 
        uint32_t debounce;
        uint32_t longPress;
        uint32_t ticksSincePress = 0;
        uint32_t ticksSinceOnLongPress = 0;

        static void tButton(void *pvParameters);
        void (*onPressed)(WbudyBUTTON *btn);
        void (*onReleased)(WbudyBUTTON *btn);
        void (*onLongPressed)(WbudyBUTTON *btn);
        void (*onChanged)(WbudyBUTTON *btn, bool pressed);
};