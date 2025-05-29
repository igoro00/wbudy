#include "WbudyBUTTON.h"

#include <hardware/gpio.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

WbudyBUTTON::WbudyBUTTON(uint32_t pin, bool statePressed, uint32_t debounce) {
    init(pin, statePressed, debounce);
}

WbudyBUTTON::WbudyBUTTON() {}

void WbudyBUTTON::init(uint32_t pin, bool statePressed, uint32_t debounce) {
    this->pin = pin;
    this->statePressed = statePressed;
    this->debounce = debounce;

    gpio_set_dir(pin, GPIO_IN);
    gpio_put(pin, 0);
    gpio_set_function(pin, GPIO_FUNC_SIO);
    if (statePressed) {
        gpio_pull_down(pin);
    } else {
        gpio_pull_up(pin);
    }
    this->initDone = true;
    this->debounced = this->isPressed();
    char taskName[16];
    sprintf(taskName, "tButton_%d\n", pin);
    xTaskCreate(
        tButton,
        taskName,
        configMINIMAL_STACK_SIZE,
        this,
        tskIDLE_PRIORITY,
        NULL
    );
}

void WbudyBUTTON::setCallback(void (*cb)(uint32_t pin, bool pressed)) {
    this->callback = cb;
}

bool WbudyBUTTON::isPressed() { 
    if(!initDone) {
        return false;
    }
    return gpio_get(pin) == statePressed;
}

void WbudyBUTTON::tButton(void *pvParameters) {
    WbudyBUTTON *button = static_cast<WbudyBUTTON*>(pvParameters);
    bool lastStableState = button->isPressed();
    bool lastReadState = lastStableState;
    uint32_t lastChangeTime = xTaskGetTickCount();

    while (1) {
        bool currentState = button->isPressed();
        if (currentState != lastReadState) {
            lastReadState = currentState;
            lastChangeTime = xTaskGetTickCount();
        }

        // If state is stable for debounce period
        if ((xTaskGetTickCount() - lastChangeTime) * portTICK_PERIOD_MS >= button->debounce) {
            if (lastStableState != currentState) {
                lastStableState = currentState;
                button->debounced = currentState;
                if (button->callback) {
                    button->callback(button->pin, currentState);
                }
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}