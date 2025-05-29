#include "states.h"
#include "Context.h"
#include "stdio.h"

uint8_t hue = 0;
void sMain(void *pvParameters) {
    while(xSemaphoreTake(ctx.taskMutex, portMAX_DELAY) != pdTRUE) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    while(1) {
        printf("hue: %d\n", hue);
        ctx.rgb.setHSL(hue++, 255, 128);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}