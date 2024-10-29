/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// using constants to represent state as int
#define STARTUP 0
#define OPERATION 1
#define CHARGING 2
#define ERROR 3

#define PRECHARGE 1
#define DISCHARGE 2

#define AUX_SET_DELAY 300
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief   Error handler for error detection in operational logic. Currently implemented as a while(1) loop.
 * @param   None
 * @author  Peter Woolsey
 */
void internal_error_handler(void)
{
  HAL_GPIO_WritePin(DEBUG_1_GPIO_Port, DEBUG_1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MCU_OK_GPIO_Port, MCU_OK_Pin, GPIO_PIN_RESET);
  // close all relays
  HAL_GPIO_WritePin(HVCP_EN_GPIO_Port,HVCP_EN_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(HVCN_EN_GPIO_Port,HVCN_EN_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(PRECHRG_EN_GPIO_Port,PRECHRG_EN_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CHRGP_EN_GPIO_Port,CHRGP_EN_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CHRGN_EN_GPIO_Port,CHRGN_EN_Pin, GPIO_PIN_RESET);
  // comms over canbus of what the error was?
  while (1)
  { // freeze everything off
    HAL_Delay(500);
  }
  return;
}

/**
 * @brief   Checks the precharge voltage until voltage threshold is reached a certain number of times. If max tries is reached, the error handler is called. MODE is either DISCHARGE OR PRECHARGE. 
 * @param   vsense_target target threshold
 * @param   num_tries number of vsense read attempts
 * @param   MODE either PRECHARGE or DISCHARGE
 * @author  Alex Martinez
 */
int vsense(int MODE, uint16_t vsense_target, int num_tries)
{
    int i=0;
    uint16_t value_adc;
    while(i<num_tries)
    {
    HAL_GPIO_TogglePin(DEBUG_1_GPIO_Port, DEBUG_1_Pin); //LED1 will flash 

    HAL_ADC_Start(&hadc1); //Needs to be called every time
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    value_adc = HAL_ADC_GetValue(&hadc1);

    if(MODE == PRECHARGE){ //PRECHARGE vsense logic 
      if (value_adc > vsense_target) { 
        HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_SET); //LED2
        return 1;
      } else {
        HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_RESET); //LED2
      }
    } else if (MODE == DISCHARGE){ //DISCHARGE vsense logic 
        if (value_adc < vsense_target) { 
        HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_SET); //LED2
        return 1;
      } else {
        HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_RESET); //LED2
      }
    } else {
      internal_error_handler();
    }
    HAL_Delay(500);
    i++;
    }

  return 0; //num_tries exceeded
}

/**
 * @brief Handles the state transition from OPERATION to STARTUP
 * @param  None
 * @author Peter Woolsey
 */
void discharge_handler(void)
{
  /*
   Insert code for checking that aux opened
  */
  HAL_GPIO_WritePin(HVCP_EN_GPIO_Port,HVCP_EN_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(HVCN_EN_GPIO_Port,HVCN_EN_Pin, GPIO_PIN_RESET);
  // actually discharge the board
  HAL_Delay(AUX_SET_DELAY);
  
  if (HAL_GPIO_ReadPin(HVCN_AUX_GPIO_Port, HVCN_AUX_Pin) == GPIO_PIN_SET || HAL_GPIO_ReadPin(HVCP_AUX_GPIO_Port, HVCP_AUX_Pin) == GPIO_PIN_SET){
    internal_error_handler();
  }

  HAL_GPIO_WritePin(MCU_OK_GPIO_Port, MCU_OK_Pin, GPIO_PIN_RESET);

  uint16_t vsense_target = 200; // 4095 * V(target voltage) / 103.6
  int num_tries = 100; // 50ms delay * num_tries = max time in vsense
  
  if(!vsense(DISCHARGE, vsense_target, num_tries)){ //vsense has its own loop based on num_tries. Returns 0 if tries are exceeded. 
    internal_error_handler();
  }
  HAL_GPIO_WritePin(MCU_OK_GPIO_Port, MCU_OK_Pin, GPIO_PIN_SET);

  // LED demo code
  HAL_GPIO_WritePin(DEBUG_1_GPIO_Port, DEBUG_1_Pin, GPIO_PIN_RESET);
  return;
}



 
/**
 * @brief   Handles the change of state from STARTUP to OPERATION
 * @param   None
 * @author  Peter Woolsey
 */
void toggle_precharge(void)
{
  HAL_GPIO_WritePin(HVCN_EN_GPIO_Port,HVCN_EN_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(PRECHRG_EN_GPIO_Port,PRECHRG_EN_Pin, GPIO_PIN_SET);

  HAL_Delay(AUX_SET_DELAY);

  if (HAL_GPIO_ReadPin(HVCN_AUX_GPIO_Port, HVCN_AUX_Pin) == GPIO_PIN_RESET || HAL_GPIO_ReadPin(PRECHRG_AUX_GPIO_Port, PRECHRG_AUX_Pin) == GPIO_PIN_RESET){
    internal_error_handler();
  }

  HAL_GPIO_WritePin(DEBUG_1_GPIO_Port, DEBUG_1_Pin, GPIO_PIN_SET);
  uint16_t vsense_target = 980; // 4095 * 24(target voltage) / 103.6
  int num_tries = 100; // 50ms delay * num_tries = max time in vsense
  
  if(!vsense(PRECHARGE, vsense_target, num_tries)){ //vsense has its own loop based on num_tries. Returns 0 if tries are exceeded. 
    internal_error_handler();
  }

  HAL_GPIO_WritePin(HVCP_EN_GPIO_Port,HVCP_EN_Pin, GPIO_PIN_SET);
  HAL_Delay(1000);
  HAL_GPIO_WritePin(PRECHRG_EN_GPIO_Port,PRECHRG_EN_Pin, GPIO_PIN_RESET);
  
  HAL_Delay(AUX_SET_DELAY);
  
  if (HAL_GPIO_ReadPin(HVCN_AUX_GPIO_Port, HVCN_AUX_Pin) == GPIO_PIN_RESET || HAL_GPIO_ReadPin(HVCP_AUX_GPIO_Port, HVCP_AUX_Pin) == GPIO_PIN_RESET){
    internal_error_handler();
  }
  // LED demo code
  HAL_GPIO_WritePin(DEBUG_1_GPIO_Port, DEBUG_1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_RESET);
  return;
}

/**
 * @brief   Responsible from switching from STARTUP to CHARGING
 * @param   None
 * @author  Peter Woolsey
 */
void toggle_charging(void)
{
  HAL_GPIO_WritePin(CHRGP_EN_GPIO_Port,CHRGP_EN_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CHRGN_EN_GPIO_Port,CHRGN_EN_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_SET);
  return;
}

/**
 * @brief   Responsible from switching from CHARGING to STARTUP
 * @param   None
 * @author  Peter Woolsey
 */
void untoggle_charging(void)
{
  HAL_GPIO_WritePin(CHRGP_EN_GPIO_Port,CHRGP_EN_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CHRGN_EN_GPIO_Port,CHRGN_EN_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_RESET);
  return;
}

/**
 * @brief   Gets the switch status through a digital read of the switches' GPIO pins.
 * @param   None
 * @return  uint8-t - a status macro which fits into a uint8-t
 * @author  Peter Woolsey
 */
uint8_t get_switch_status(void)
{
  if (HAL_GPIO_ReadPin(IGNITION_SW_GPIO_Port, IGNITION_SW_Pin) == GPIO_PIN_SET)
  {
    if (HAL_GPIO_ReadPin(CHARGE_SW_GPIO_Port, CHARGE_SW_Pin) == GPIO_PIN_SET)
    {
      return ERROR;
    }
    else
    {
      return OPERATION;
    }
  }
  else
  {
    if (HAL_GPIO_ReadPin(CHARGE_SW_GPIO_Port, CHARGE_SW_Pin) == GPIO_PIN_SET)
    {
      return CHARGING;
    }
    else
    {
      return STARTUP;
    }
  }
  return ERROR;
}

/**
 * @brief   Checks to ensure that the aux contactors are in the expected position for the current state. 
 * @param   current_status 
 * @author  Peter Woolsey
 */
void aux_check(uint8_t current_status)
{
  if (HAL_GPIO_ReadPin(PRECHRG_AUX_GPIO_Port, PRECHRG_AUX_Pin) == GPIO_PIN_SET){
    internal_error_handler();
  }
  if (current_status == OPERATION){
    if (HAL_GPIO_ReadPin(HVCP_AUX_GPIO_Port, HVCP_AUX_Pin) == GPIO_PIN_RESET || HAL_GPIO_ReadPin(HVCN_AUX_GPIO_Port, HVCN_AUX_Pin) == GPIO_PIN_RESET){
      internal_error_handler();
    } 
  } else {
    if (HAL_GPIO_ReadPin(HVCP_AUX_GPIO_Port, HVCP_AUX_Pin) == GPIO_PIN_SET || HAL_GPIO_ReadPin(HVCN_AUX_GPIO_Port, HVCN_AUX_Pin) == GPIO_PIN_SET){
      internal_error_handler();
    } 
  }
  HAL_Delay(100);
  return;
  // placeholder
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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  // Setup
  uint8_t status = STARTUP;
  uint8_t new_status;
  
  HAL_Delay(500);
  HAL_GPIO_WritePin(MCU_OK_GPIO_Port, MCU_OK_Pin, GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    new_status = get_switch_status();
    if (new_status == ERROR)
    {
      internal_error_handler();
    }

    if (new_status != status)
    { // handles change of switch state
      if (status == STARTUP)
      { // where can you go from startup:
        if (new_status == OPERATION)
        {
          toggle_precharge();
          status = OPERATION;
        }
        else if (new_status == CHARGING)
        {
          toggle_charging();
          status = CHARGING;
        }
        else
        {
          internal_error_handler(); // should never reach here
        }
      }
      else if (status == OPERATION)
      { // where can you go from operation:
        if (new_status == STARTUP)
        {
          discharge_handler();
          status = STARTUP;
        }
        else
        {
          internal_error_handler(); // should never reach here
        }
      }
      else if (status == CHARGING)
      { // where can you go from operation:
        if (new_status == STARTUP)
        {
          untoggle_charging();
          status = STARTUP;
        }
        else
        {
          internal_error_handler(); // should never reach here
        }
      }
      else
      {
        status = ERROR;
        internal_error_handler();
      }
    }

    aux_check(status); // to be implemented
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, HVCN_EN_Pin|HVCP_EN_Pin|CHRGP_EN_Pin|CHRGN_EN_Pin
                          |PRECHRG_EN_Pin|PUMP_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DEBUG_1_Pin|DEBUG_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MCU_OK_GPIO_Port, MCU_OK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : HVCN_EN_Pin HVCP_EN_Pin CHRGP_EN_Pin CHRGN_EN_Pin
                           PRECHRG_EN_Pin PUMP_EN_Pin */
  GPIO_InitStruct.Pin = HVCN_EN_Pin|HVCP_EN_Pin|CHRGP_EN_Pin|CHRGN_EN_Pin
                          |PRECHRG_EN_Pin|PUMP_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : BMS_GPIO1_Pin BMS_GPIO2_Pin BMS_GPIO3_Pin */
  GPIO_InitStruct.Pin = BMS_GPIO1_Pin|BMS_GPIO2_Pin|BMS_GPIO3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : IGNITION_SW_Pin */
  GPIO_InitStruct.Pin = IGNITION_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(IGNITION_SW_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CHARGE_SW_Pin HVCP_AUX_Pin HVCN_AUX_Pin PRECHRG_AUX_Pin */
  GPIO_InitStruct.Pin = CHARGE_SW_Pin|HVCP_AUX_Pin|HVCN_AUX_Pin|PRECHRG_AUX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : DEBUG_1_Pin DEBUG_2_Pin */
  GPIO_InitStruct.Pin = DEBUG_1_Pin|DEBUG_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : SAFETY_LOOP_STATUS_Pin */
  GPIO_InitStruct.Pin = SAFETY_LOOP_STATUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SAFETY_LOOP_STATUS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MCU_OK_Pin */
  GPIO_InitStruct.Pin = MCU_OK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MCU_OK_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
