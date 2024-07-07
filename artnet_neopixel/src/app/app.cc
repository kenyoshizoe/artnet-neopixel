#include "artnet_neopixel/app/app.h"

#include "SEGGER_RTT.h"
#include "artnet_neopixel/artnet/ArtNode.h"
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
uint8_t dhcp_buf[2048];
uint8_t SOCK_UDPS = 0;
uint8_t SOCK_DHCP = 6;

ArtConfig artnet_config = {
    .mac = {0x00, 0x08, 0xdc, 0x00, 0x00, 0x00},  // MAC
    .ip = {2, 3, 4, 5},                           // IP
    .mask = {255, 0, 0, 0},                       // Subnet mask
    .udpPort = 0x1936,
    .dhcp = true,
    .net = 0,                           // Net (0-127)
    .subnet = 0,                        // Subnet (0-15)
    "artnet-neopixel",                  // Short name
    "artnet-neopixel by ken yoshizoe",  // Long name
    .numPorts = 12,
    .portTypes = {PortTypeDmx | PortTypeOutput, PortTypeDmx | PortTypeOutput,
                  PortTypeDmx | PortTypeOutput, PortTypeDmx | PortTypeOutput},
    .portAddrIn = {0, 0, 0, 0},   // Port input universes (0-15)
    .portAddrOut = {0, 1, 2, 3},  // Port output universes (0-15)
    .verHi = 0,
    .verLo = 1,
};
uint8_t artnet_buffer[1024];
ArtNode art_node(artnet_config, sizeof(artnet_buffer), artnet_buffer);

void ip_assign(void) {
  getIPfromDHCP(gWIZNETINFO.ip);
  getGWfromDHCP(gWIZNETINFO.gw);
  getSNfromDHCP(gWIZNETINFO.sn);
  getDNSfromDHCP(gWIZNETINFO.dns);
  setSIPR(gWIZNETINFO.ip);
  setSUBR(gWIZNETINFO.sn);
  setGAR(gWIZNETINFO.gw);
  setSHAR(gWIZNETINFO.mac);

  for (int i = 0; i < 4; i++) {
    artnet_config.ip[i] = gWIZNETINFO.ip[i];
  }

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
  HAL_Delay(5);
  uint8_t memsize[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
  ctlwizchip(CW_INIT_WIZCHIP, (void*)memsize);
  ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
  // DHCP
  setSHAR(gWIZNETINFO.mac);
  DHCP_init(SOCK_DHCP, dhcp_buf);
  reg_dhcp_cbfunc(ip_assign, ip_assign, ip_conflict);
  // UDP
  setSn_RXBUF_SIZE(SOCK_UDPS, 4);
  int res = socket(SOCK_UDPS, Sn_MR_UDP, 0x1936, SF_IO_NONBLOCK);
  if (res != SOCK_UDPS) {
    SEGGER_RTT_printf(0, "socket() failed: %d\n", res);
    Error_Handler();
  }

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

    // check phy link status
    if (!(getPHYCFGR() & PHYCFGR_LNK_ON)) {
      continue;
    }

    // check available data
    if (getSn_RX_RSR(SOCK_UDPS) == 0) {
      continue;
    }

    uint8_t ip[4];
    uint16_t port;
    int ret =
        recvfrom(SOCK_UDPS, artnet_buffer, sizeof(artnet_buffer), ip, &port);
    if (ret <= 0) {
      continue;
    }

    if (art_node.isPacketValid()) {
      uint16_t op_code = art_node.getOpCode();
      if (op_code == OpPoll) {
        art_node.createPollReply();
        sendto(SOCK_UDPS, artnet_buffer, art_node.getPacketSize(), ip,
               artnet_config.udpPort);
      } else if (op_code == OpDmx) {
        ArtDmx* dmx = (ArtDmx*)artnet_buffer;

        if (dmx->getNet() != artnet_config.net) {
          continue;
        }
        if (dmx->getSub() != artnet_config.subnet) {
          continue;
        }

        artnet_neopixel::NeoPixel<170>* neopixel = nullptr;
        switch (dmx->getUni()) {
          case 0:
            neopixel = &neopixel1;
            break;
          case 1:
            neopixel = &neopixel2;
            break;
          case 2:
            neopixel = &neopixel3;
            break;
          case 3:
            neopixel = &neopixel4;
            break;
        }
        if (neopixel == nullptr) {
          continue;
        }

        for (int i = 0; i < 170; i++) {
          neopixel->SetColor(i, dmx->Data[i * 3 + 0], dmx->Data[i * 3 + 1],
                             dmx->Data[i * 3 + 2]);
        }
      } else if (op_code == OpSync) {
        // NOP
      }
    } else {
      SEGGER_RTT_printf(0, "Invalid packet\n");
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
