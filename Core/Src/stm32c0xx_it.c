/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32c0xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32c0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
#include "fifo.h"
#include "special.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

extern uint8_t it_ready;

static volatile uint8_t  bit_idx = 0;
static volatile uint8_t  data_byte = 0;
static volatile uint8_t  parity_bit = 0;
static volatile uint16_t last_edge_us = 0;
static volatile uint8_t  frame_ready = 0;

extern fifo_queue q;
extern queue_item item;
extern uint8_t tim16_left;

extern  uint8_t xt_tx_busy;
extern uint8_t xt_tx_byte;
extern uint8_t xt_tx_par;   // paridade ímpar
extern uint8_t xt_bit_idx;  // 0..10


/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
extern flag_t Flg;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

void pulses(void){
	XT_DATA_ON;
	XT_DATA_ON;
	XT_DATA_ON;
    XT_DATA_ON;
    XT_DATA_ON;
	XT_DATA_OFF;

}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


static inline uint16_t micros16(void) {
	//one pulse each 0.994us (ideal is 1us but we need use a external crystal, and we are using internal RC)
	return (uint16_t)LL_TIM_GetCounter(TIM17);
}

void delay_us16(uint16_t us)
{
    uint16_t start = micros16();
    while ((uint16_t)(micros16() - start) < us) {
        // espera ativa
    }
}


static inline void reset_rx(void) {
    bit_idx = 0;
    data_byte = 0;
    parity_bit = 0;
    frame_ready = 0;
}

void CLK_EXTI_Falling_IRQHandler(void) {

    uint16_t now = micros16();
    uint16_t dt  = (uint16_t)(now - last_edge_us);  // subtração modular 16-bit
    if (dt > 1000) reset_rx();
    last_edge_us = now;


    uint8_t bit = AT_DATA_BIT; // amostra em FALLING

    if (bit_idx == 0) {
        // START
   	   //pulses();

        if (bit != 0) { reset_rx(); return; } //descarta se start bit nao for 0
    } else if (bit_idx >= 1 && bit_idx <= 8) {
        // Dados LSB-first
        data_byte >>= 1;
        //pulses();
        if (bit) data_byte |= 0x80;
    } else if (bit_idx == 9) {
        parity_bit = bit;
    } else if (bit_idx == 10) {
        // STOP
        if (bit == 1) {
            // valida paridade ímpar
            uint8_t ones = __builtin_popcount((unsigned)data_byte) + (parity_bit ? 1 : 0);
            if (ones & 1) {
                frame_ready = 1; // quadro OK
                ps2_feed_byte(data_byte);
            }
        }
    }

    bit_idx++;
    if (bit_idx > 10) {
        bit_idx = 0; // pronto para próximo quadro
    }
}



/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32C0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32c0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles RTC interrupts through EXTI lines 19 and 21.
  */
void RTC_IRQHandler(void)
{
  /* USER CODE BEGIN RTC_IRQn 0 */
	if (LL_RTC_IsActiveFlag_ALRA(RTC)) {
	        LL_RTC_ClearFlag_ALRA(RTC);
	        LL_EXTI_ClearRisingFlag_0_31(LL_EXTI_LINE_19);

	        // Acordou do modo STOP
	        // Aqui você pode acender um LED, setar uma flag, etc.
	}
  /* USER CODE END RTC_IRQn 0 */
  /* USER CODE BEGIN RTC_IRQn 1 */

  /* USER CODE END RTC_IRQn 1 */
}

/**
  * @brief This function handles Flash global interrupt.
  */
void FLASH_IRQHandler(void)
{
  /* USER CODE BEGIN FLASH_IRQn 0 */

  /* USER CODE END FLASH_IRQn 0 */
  HAL_FLASH_IRQHandler();
  /* USER CODE BEGIN FLASH_IRQn 1 */

  /* USER CODE END FLASH_IRQn 1 */
}

/**
  * @brief This function handles EXTI line 4 to 15 interrupts.
  */
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  /* USER CODE END EXTI4_15_IRQn 0 */
  if (LL_EXTI_IsActiveFallingFlag_0_31(LL_EXTI_LINE_12) != RESET)
  {
    LL_EXTI_ClearFallingFlag_0_31(LL_EXTI_LINE_12);
    /* USER CODE BEGIN LL_EXTI_LINE_12_FALLING */
    CLK_EXTI_Falling_IRQHandler();


    /* USER CODE END LL_EXTI_LINE_12_FALLING */
  }
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */

  /* USER CODE END EXTI4_15_IRQn 1 */
}

/**
  * @brief This function handles TIM1 break, update, trigger and commutation interrupts.
  */
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_BRK_UP_TRG_COM_IRQn 0 */

  /* USER CODE END TIM1_BRK_UP_TRG_COM_IRQn 0 */
  /* USER CODE BEGIN TIM1_BRK_UP_TRG_COM_IRQn 1 */

  /* USER CODE END TIM1_BRK_UP_TRG_COM_IRQn 1 */
}

/**
  * @brief This function handles TIM1 capture compare interrupt.
  */
void TIM1_CC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_CC_IRQn 0 */

  /* USER CODE END TIM1_CC_IRQn 0 */
  /* USER CODE BEGIN TIM1_CC_IRQn 1 */

  /* USER CODE END TIM1_CC_IRQn 1 */
}

/**
  * @brief This function handles TIM16 global interrupt.
  */
void TIM16_IRQHandler(void)
{
  /* USER CODE BEGIN TIM16_IRQn 0 */

	   if (!LL_TIM_IsActiveFlag_UPDATE(TIM16)) return;
	    LL_TIM_ClearFlag_UPDATE(TIM16);

	    if (!xt_tx_busy) return;

	    uint8_t out_bit = 1;  // default = STOP/idle

	    if (xt_bit_idx == 0) {
	        out_bit = 0;                                   // START
	    } else if (xt_bit_idx >= 1 && xt_bit_idx <= 8) {
	        out_bit = (xt_tx_byte >> (xt_bit_idx - 1)) & 1u;  // dados LSB-first
	    } else if (xt_bit_idx == 9) {
	        out_bit = xt_tx_par;                           // paridade ímpar
	    } else if (xt_bit_idx == 10) {
	        out_bit = 1;                                   // STOP
	    }

	    if (out_bit) XT_DATA_ON; else XT_DATA_OFF;

	    if (++xt_bit_idx > 10) {
	        // terminou 11 bits
	        xt_tx_busy = 0;
	        XT_DATA_ON;                         // volta ao idle alto
	        LL_TIM_DisableCounter(TIM16);
	        LL_TIM_DisableIT_UPDATE(TIM16);
	    }

  /* USER CODE END TIM16_IRQn 0 */
  /* USER CODE BEGIN TIM16_IRQn 1 */

  /* USER CODE END TIM16_IRQn 1 */
}

/**
  * @brief This function handles TIM17 global interrupt.
  */
void TIM17_IRQHandler(void)
{
  /* USER CODE BEGIN TIM17_IRQn 0 */
   if (LL_TIM_IsActiveFlag_UPDATE(TIM17)) {
	 LL_TIM_ClearFlag_UPDATE(TIM17);  // sempre limpar flag!
	 //XT_DATA_ON;
	 //XT_DATA_OFF;
   }
  /* USER CODE END TIM17_IRQn 0 */
  /* USER CODE BEGIN TIM17_IRQn 1 */

  /* USER CODE END TIM17_IRQn 1 */
}

/**
  * @brief This function handles USART1 interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */


static inline uint8_t odd_parity(uint8_t b) {
    return (uint8_t)(!__builtin_parity((unsigned)b));  // 1 se total (dados+par) deve ser ímpar
}

void xt_send_byte(uint8_t b)
{
    while (xt_tx_busy) { /* ou enfileire se preferir */ }

    xt_tx_busy  = 1;
    xt_tx_byte  = b;
    xt_tx_par   = odd_parity(b);
    xt_bit_idx  = 0;

    XT_DATA_ON;                              // idle alto antes de começar
    LL_TIM_SetCounter(TIM16, 0);
    LL_TIM_ClearFlag_UPDATE(TIM16);
    LL_TIM_EnableIT_UPDATE(TIM16);
    LL_TIM_EnableCounter(TIM16);             // cada UPDATE = próximo bit
}



/* USER CODE END 1 */
