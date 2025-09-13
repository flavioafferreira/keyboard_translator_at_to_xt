/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32c0xx_hal.h"

#include "stm32c0xx_ll_rcc.h"
#include "stm32c0xx_ll_bus.h"
#include "stm32c0xx_ll_system.h"
#include "stm32c0xx_ll_exti.h"
#include "stm32c0xx_ll_cortex.h"
#include "stm32c0xx_ll_utils.h"
#include "stm32c0xx_ll_pwr.h"
#include "stm32c0xx_ll_dma.h"
#include "stm32c0xx_ll_rtc.h"
#include "stm32c0xx_ll_tim.h"
#include "stm32c0xx_ll_usart.h"
#include "stm32c0xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct Flags
{
  uint8_t ADCCMPLT;
}flag_t;


extern flag_t Flg;
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
#define TX_Pin LL_GPIO_PIN_14
#define TX_GPIO_Port GPIOC
#define at_data_in_Pin LL_GPIO_PIN_8
#define at_data_in_GPIO_Port GPIOA
#define at_clock_in_Pin LL_GPIO_PIN_12
#define at_clock_in_GPIO_Port GPIOA
#define at_clock_in_EXTI_IRQn EXTI4_15_IRQn
#define xt_data_out_Pin LL_GPIO_PIN_13
#define xt_data_out_GPIO_Port GPIOA
#define xt_clock_out_Pin LL_GPIO_PIN_14
#define xt_clock_out_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

//input ports
#define AT_DATA_BIT LL_GPIO_IsInputPinSet(at_data_in_GPIO_Port,at_data_in_Pin)
//output port

#define XT_DATA_ON LL_GPIO_SetOutputPin(xt_data_out_GPIO_Port, xt_data_out_Pin)
#define XT_DATA_OFF LL_GPIO_ResetOutputPin(xt_data_out_GPIO_Port, xt_data_out_Pin)

//MEAN

#define ON  1
#define OFF 0

#define POWER_ON OFF
#define POWER_OFF ON



typedef struct relay_ {
      uint8_t status;
      uint8_t re1charge;

} relay_st;



#define INITIAL_PAGE 14 //PAGE 14 = ADDRESS 0X8007000
#define PERIOD 3 //MINUTOS
#define QUANT_REGISTRY 2 //QUANTITY OF MEASURES TO SAVE, MAX 128 =512BYTES
typedef struct __attribute__((packed, aligned(8))) {
      float temp[QUANT_REGISTRY];
} flash_st;


typedef struct sensor_item_ {
  float temp;
  float vdda_real;
} sensor_item;





/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
