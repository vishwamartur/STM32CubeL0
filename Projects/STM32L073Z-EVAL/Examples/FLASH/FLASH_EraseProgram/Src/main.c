/**
  ******************************************************************************
  * @file    FLASH/FLASH_EraseProgram/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a description of how to erase and program the
  *          STM32L0xx FLASH.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32L0xx_HAL_Examples
  * @{
  */

/** @addtogroup FLASH_EraseProgram
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FLASH_USER_START_ADDR   (FLASH_BASE + FLASH_PAGE_SIZE * 256)             /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     (FLASH_USER_START_ADDR + FLASH_PAGE_SIZE * 10)   /* End @ of user Flash area */

#define DATA_32                 ((uint32_t)0x12345678)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t Address = 0, PAGEError = 0;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;

/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32L0xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 2 MHz */
  SystemClock_Config();

  /* Initialize LED1, LED2 and LED3 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  /* Fill EraseInit structure*/
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
  EraseInitStruct.NbPages     = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
  {
    /*
      Error occurred while page erase.
      User can add here some code to deal with this error.
      PAGEError will contain the faulty page and then to know the code error on this page,
      user can call function 'HAL_FLASH_GetError()'
    */
    /* Infinite loop */
    while (1)
    {
      BSP_LED_On(LED3);
    }
  }

  /* Program the user Flash area word by word
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  Address = FLASH_USER_START_ADDR;

  while (Address < FLASH_USER_END_ADDR)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, DATA_32) == HAL_OK)
    {
      Address = Address + 4;
    }
    else
    {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error */
      while (1)
      {
        BSP_LED_On(LED3);
      }
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* Check if the programmed data is OK
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
  Address = FLASH_USER_START_ADDR;
  MemoryProgramStatus = 0x0;

  while (Address < FLASH_USER_END_ADDR)
  {
    data32 = *(__IO uint32_t *)Address;

    if (data32 != DATA_32)
    {
      MemoryProgramStatus++;
    }
    Address = Address + 4;
  }

  /*Check if there is an issue to program data*/
  if (MemoryProgramStatus == 0)
  {
    /* No error detected. Switch on LED1*/
    BSP_LED_On(LED1);
  }
  else
  {
    /* Error detected. Switch on LED2*/
    BSP_LED_On(LED2);
  }

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = MSI
  *            SYSCLK(Hz)                     = 2000000
  *            HCLK(Hz)                       = 2000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            Flash Latency(WS)              = 0
  *            Main regulator output voltage  = Scale3 mode
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  
  /* Enable MSI Oscillator */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.MSICalibrationValue=0x00;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
  
  /* Select MSI as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
/**
  * @}
  */

/**
  * @}
  */
