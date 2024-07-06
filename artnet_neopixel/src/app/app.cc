#include "artnet_neopixel/app/app.h"

#include "SEGGER_RTT.h"
#include "artnet_neopixel/module/neopixel.h"
#include "main.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;
artnet_neopixel::NeoPixel<170> neopixel1(&htim3, TIM_CHANNEL_4);
artnet_neopixel::NeoPixel<170> neopixel2(&htim3, TIM_CHANNEL_3);
artnet_neopixel::NeoPixel<170> neopixel3(&htim3, TIM_CHANNEL_1);
artnet_neopixel::NeoPixel<170> neopixel4(&htim17, TIM_CHANNEL_1);

void InitApp() {
  HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(50);
  HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_SET);

  HAL_TIM_Base_Start_IT(&htim7);
  HAL_TIM_Base_Start_IT(&htim16);
  SEGGER_RTT_printf(0, "artnet_neopixel ver.PROTO\n");
}

void MainApp() {
  int i = 0;
  while (true) {
    for (int j = 0; j < 3; j++) {
      neopixel1.SetColor(j, 0, i, i);
      neopixel2.SetColor(j, i, 0, i);
      neopixel3.SetColor(j, i, i, 0);
      neopixel4.SetColor(j, i, i, i);
    }

    i = (i + 1) % 255;
    HAL_Delay(10);
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
  // Toggle RUN_LED
  if (htim == &htim7) {
    HAL_GPIO_TogglePin(RUN_LED_GPIO_Port, RUN_LED_Pin);
  }

  // Start DMA transfer for each NeoPixel
  static int neopixel_count = 0;
  if (htim == &htim16) {
    switch (neopixel_count) {
      case 0:
        neopixel1.StartTX();
        break;
      case 1:
        neopixel2.StartTX();
        break;
      case 2:
        neopixel3.StartTX();
        break;
      case 3:
        neopixel4.StartTX();
        break;
    }
    neopixel_count = (neopixel_count + 1) % 4;
  }
}
