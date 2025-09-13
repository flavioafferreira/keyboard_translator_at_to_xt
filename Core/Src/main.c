/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
  Normal current sleep consumption (sleeping) = 110uA
  The battery CR2032 (220mAh) will work for 80days (2000 hours)

  STM32C011J6Mx works from 1.7V up to 3.6V
  connections
  GND---ntc10k--PA8(ntc)--res20k-----PA13(alim_ntc)
  PA12(memory_print)--switch---VDD
  PC14(TX)-->UART TX 1200BPS
  PC15--SWDCLK
  PC13--SWDIO
  PF2 --RESET
  PIN2--VDD
  PIN3--VSS GND
  Don't need external Crystal. It uses the internal RC.
  This schematics should consider 2 capacitors 0.1uF and 10uf between GND and VDD near the chip
  and 1uF on Reset Port NRST
  When press the switch send the latest 240(max) readings to UART TX 1200bps and clean the buffer
  You can configure the interval between readings from 1 to 60 minutes on main.h
  "#define SLEEP_TIME 60  //how many minutes between measures"
  The readings are stored on internal RAM, then if you turn off the batteries,
  will lost the measures.



  ******************************************************************************
  ******************************************************************************
  ******************************************************************************
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stdio.h"
#include "math.h"
#include "stm32c0xx_ll_bus.h"
#include "stm32c0xx_hal.h"
#include "stm32c0xx_it.h"
#include "string.h"
#include "fifo.h"
#include "flash_read_write.h"
#include "special.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

// QUEUE DEFINITION
 //fifo to TCP
fifo_queue q;
queue_item item;


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//VARIABLE FOR AD CONVERSION
uint16_t ad_value =0;
uint32_t qty_of_samples=1;

// Estado do parser Set 2
uint8_t ps2_ext = 0;       // 0=normal, 1=E0 ativo, 2+=E1 sequência
uint8_t ps2_break_next = 0; // se 1, próximo byte é break
uint8_t ps2_e1_swallow = 0; // conta bytes para descartar após E1 (opcional)
uint8_t tim16_left = 0;
extern const uint8_t set2_to_set1[256];

uint8_t xt_tx_busy = 0;
uint8_t xt_tx_byte = 0;
uint8_t xt_tx_par = 0;   // paridade ímpar
uint8_t xt_bit_idx = 0;  // 0..10



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
relay_st relay;
relay_st relay_on_off(uint8_t control);


flash_st flash_variable;
volatile uint8_t it_ready=ON;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
typedef struct AdcValues{
  uint16_t Raw[2]; /* Raw values from ADC */
  double IntSensTmp; /* Temperature */
}adcval_t;
adcval_t Adc;
flag_t Flg = {0, };

uint32_t minutos_atual=0;

//redirect the printf to usart1
int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        while (!LL_USART_IsActiveFlag_TXE(USART1));  // Espera o buffer ficar vazio
        LL_USART_TransmitData8(USART1, ptr[i]);  // Envia um caractere
    }
    while (!LL_USART_IsActiveFlag_TC(USART1));  // Espera a transmissão terminar
    return len;
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  init_queue(&q);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  MX_TIM1_Init();
  MX_TIM17_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */
  tim1_clock_transmit();
  tim16_data_transmit();
  XT_DATA_ON;
 printf("OK \n\r");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
   it_ready=ON;

  while (1)
  {

 	//XT_DATA_ON;
 	 LL_mDelay(1);
 	//XT_DATA_OFF;
 	 if(!is_empty(&q)){
 	   LL_mDelay(1);
 	   dequeue(&q, &item);
 	   //printf("0x%X\n",item.data_byte);
 	   xt_send_byte(item.data_byte);
 	   delay_us16(80); //80us
 	   tim1_clock_transmit();
 	 }
	 LL_mDelay(1);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  /* HSI configuration and activation */
  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  LL_RCC_HSI_SetCalibTrimming(64);
  LL_RCC_SetHSIDiv(LL_RCC_HSI_DIV_1);
  /* LSI configuration and activation */
  LL_RCC_LSI_Enable();
  while(LL_RCC_LSI_IsReady() != 1)
  {
  }

  /* Set AHB prescaler*/
  LL_RCC_SetAHBPrescaler(LL_RCC_HCLK_DIV_1);

  /* Sysclk activation on the HSI */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  }

  /* Set APB1 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(48000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
