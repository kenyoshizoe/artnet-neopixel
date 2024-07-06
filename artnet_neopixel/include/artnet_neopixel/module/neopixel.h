#ifndef ARTNET_NEOPIXEL_MODULE_NEOPIXEL_H_
#define ARTNET_NEOPIXEL_MODULE_NEOPIXEL_H_

#include <cstring>

#include "main.h"

namespace artnet_neopixel {
template <int kLedNum>
class NeoPixel {
 public:
  NeoPixel(TIM_HandleTypeDef* htim, uint32_t channel)
      : htim_(htim), channel_(channel) {
    for (int i = 0; i < kFirstPadding + kLedNum * 24 + kNeoPixelLastPadding;
         i++) {
      if (9 < i && i < kLedNum * 24) {
        buf_[i] = kNeoPixelZero;
      } else {
        buf_[i] = 0;
      }
    }
  }

  void SetColor(uint8_t led_id, uint8_t r, uint8_t g, uint8_t b) {
    if (led_id >= kLedNum) {
      return;
    }

    for (int i = 0; i < 8; i++) {
      buf_[kFirstPadding + led_id * 24 + 0 + i] =
          (r & (1 << (7 - i))) ? kNeoPixelOne : kNeoPixelZero;
      buf_[kFirstPadding + led_id * 24 + 8 + i] =
          (g & (1 << (7 - i))) ? kNeoPixelOne : kNeoPixelZero;
      buf_[kFirstPadding + led_id * 24 + 16 + i] =
          (b & (1 << (7 - i))) ? kNeoPixelOne : kNeoPixelZero;
    }
  }

  void StartTX() {
    HAL_TIM_PWM_Start_DMA(htim_, channel_, (uint32_t*)buf_,
                          kLedNum * 24 + kNeoPixelLastPadding);
  }

 private:
  TIM_HandleTypeDef* const htim_;
  const uint32_t channel_;

  static constexpr int kFirstPadding = 3;
  static constexpr int kNeoPixelLastPadding = 1;
  static constexpr int buf_size =
      kFirstPadding + kLedNum * 24 + kNeoPixelLastPadding;
  uint8_t buf_[kFirstPadding + kLedNum * 24 + kNeoPixelLastPadding];

  // 1.25us / 40 * 10 = 0.3125us
  static constexpr uint8_t kNeoPixelZero = 10 - 1;
  // 1.25us / 40 * 20 = 0.625us
  static constexpr uint8_t kNeoPixelOne = 20 - 1;
};
}  // namespace artnet_neopixel
#endif  // ARTNET_NEOPIXEL_MODULE_NEOPIXEL_H_
