/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define NET0_Pin GPIO_PIN_0
#define NET0_GPIO_Port GPIOA
#define NET1_Pin GPIO_PIN_1
#define NET1_GPIO_Port GPIOA
#define NET2_Pin GPIO_PIN_2
#define NET2_GPIO_Port GPIOA
#define NET3_Pin GPIO_PIN_3
#define NET3_GPIO_Port GPIOA
#define SUBNET0_Pin GPIO_PIN_4
#define SUBNET0_GPIO_Port GPIOA
#define SUBNET1_Pin GPIO_PIN_5
#define SUBNET1_GPIO_Port GPIOA
#define NEOPIXEL4_Pin GPIO_PIN_6
#define NEOPIXEL4_GPIO_Port GPIOA
#define NEOPIXEL3_Pin GPIO_PIN_7
#define NEOPIXEL3_GPIO_Port GPIOA
#define NEOPIXEL1_Pin GPIO_PIN_0
#define NEOPIXEL1_GPIO_Port GPIOB
#define NEOPIXEL2_Pin GPIO_PIN_1
#define NEOPIXEL2_GPIO_Port GPIOB
#define W5500_RESET_Pin GPIO_PIN_2
#define W5500_RESET_GPIO_Port GPIOB
#define W5500_ITR_Pin GPIO_PIN_10
#define W5500_ITR_GPIO_Port GPIOB
#define RUN_LED_Pin GPIO_PIN_11
#define RUN_LED_GPIO_Port GPIOA
#define SUBNET2_Pin GPIO_PIN_3
#define SUBNET2_GPIO_Port GPIOB
#define SUBNET3_Pin GPIO_PIN_4
#define SUBNET3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
