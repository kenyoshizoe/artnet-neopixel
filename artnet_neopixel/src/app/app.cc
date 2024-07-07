#include "artnet_neopixel/app/app.h"

#include "SEGGER_RTT.h"
#include "artnet_neopixel/module/neopixel.h"
#include "dhcp.h"
#include "main.h"
#include "socket.h"
#include "w5500.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;
artnet_neopixel::NeoPixel<170> neopixel1(&htim3, TIM_CHANNEL_4);
artnet_neopixel::NeoPixel<170> neopixel2(&htim3, TIM_CHANNEL_3);
artnet_neopixel::NeoPixel<170> neopixel3(&htim3, TIM_CHANNEL_1);
artnet_neopixel::NeoPixel<170> neopixel4(&htim17, TIM_CHANNEL_1);

wiz_NetInfo gWIZNETINFO = {
    {0x00, 0x08, 0xdc, 0x00, 0x00, 0x00},  // Mac address
    {192, 168, 1, 255},                    // IP address
    {255, 255, 255, 0},                    // Subnet mask
    {192, 168, 1, 1},                      // Gateway
    {8, 8, 8, 8},                          // DNS server
    NETINFO_DHCP                           // DHCP
};
uint8_t SOCK_DHCP = 0;

void ip_assign(void) {
  getIPfromDHCP(gWIZNETINFO.ip);
  getGWfromDHCP(gWIZNETINFO.gw);
  getSNfromDHCP(gWIZNETINFO.sn);
  getDNSfromDHCP(gWIZNETINFO.dns);
  setSIPR(gWIZNETINFO.ip);
  setSUBR(gWIZNETINFO.sn);
  setGAR(gWIZNETINFO.gw);
  setSHAR(gWIZNETINFO.mac);

  SEGGER_RTT_printf(0, "DHCP LEASED IP %d.%d.%d.%d\n", gWIZNETINFO.ip[0],
                    gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);
}
void ip_conflict(void) {
  SEGGER_RTT_printf(0, "IP CONFLICT\n");
  Error_Handler();
}

void InitApp() {
  // W5500
  HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_SET);
  HAL_Delay(1);
  uint8_t memsize[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
  ctlwizchip(CW_INIT_WIZCHIP, (void*)memsize);
  ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
  setSHAR(gWIZNETINFO.mac);
  DHCP_init(SOCK_DHCP, gWIZNETINFO.mac);
  reg_dhcp_cbfunc(ip_assign, ip_assign, ip_conflict);
  uint8_t tmpstr[6] = {};
  ctlwizchip(CW_GET_ID, (void*)tmpstr);

  HAL_TIM_Base_Start_IT(&htim7);
  HAL_TIM_Base_Start_IT(&htim16);
  SEGGER_RTT_printf(0, "artnet_neopixel ver.PROTO\n");
}

void MainApp() {
  while (true) {
    switch (DHCP_run()) {
      case DHCP_IP_ASSIGN:
      case DHCP_IP_CHANGED:
      case DHCP_IP_LEASED:
        break;
      case DHCP_FAILED:
        SEGGER_RTT_printf(0, "DHCP failed\n");
        DHCP_stop();
        break;
      default:
        break;
    }
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
  // 8Hz
  // Toggle RUN_LED
  if (htim == &htim7) {
    HAL_GPIO_TogglePin(RUN_LED_GPIO_Port, RUN_LED_Pin);

    static int count = 0;
    count++;
    if (count % 8 == 0) {
      DHCP_time_handler();
    }
  }

  // 250Hz
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
