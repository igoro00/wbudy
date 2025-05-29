#include "states.h"
#include "Context.h"
#include "stdio.h"

uint8_t hue = 0;
void sMain(void *pvParameters) {
    while(xSemaphoreTake(ctx.taskMutex, portMAX_DELAY) != pdTRUE) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    printf("[Main] Printing\n");
    ctx.lcd.setCursor(0,0);
    ctx.lcd.print("Wbudy LED\n");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    while(1) {
        printf("[Main] Toggling backlight...\n");
        ctx.lcd.backlightOff();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ctx.lcd.backlightOn();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // printf("hue: %d\n", hue);
        // ctx.rgb.setHSL(hue++, 255, 128);
        // vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}