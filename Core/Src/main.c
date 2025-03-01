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
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

CAN_HandleTypeDef hcan1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

CAN_TxHeaderTypeDef   TxHeader;
CAN_RxHeaderTypeDef   RxHeader; //CAN Bus Transmit Header
uint8_t               TxData[8];
uint32_t              TxMailbox;
CAN_FilterTypeDef canfil; //CAN Bus Filter
uint32_t canMailbox; //CAN Bus Mail box variable

uint8_t canRX[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}; //CAN Bus Receive Buffer

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_CAN1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_ADC2_Init();
  MX_CAN1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  canfil.FilterBank = 0;
  canfil.FilterMode = CAN_FILTERMODE_IDMASK;
  canfil.FilterFIFOAssignment = CAN_RX_FIFO0;
  canfil.FilterIdHigh = 0;
  canfil.FilterIdLow = 0;
  canfil.FilterMaskIdHigh = 0;
  canfil.FilterMaskIdLow = 0;
  canfil.FilterScale = CAN_FILTERSCALE_32BIT;
  canfil.FilterActivation = ENABLE;
  canfil.SlaveStartFilterBank = 14;

  HAL_CAN_ConfigFilter(&hcan1,&canfil);
  HAL_CAN_Start(&hcan1);
  HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);

  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
	  Error_Handler();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // HAL_GPIO_TogglePin(DEBUG_1_GPIO_Port, DEBUG_1_Pin); 
    // printf("Test\n");
    // HAL_Delay(100);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
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
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
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
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 16;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_2TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_4TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = ENABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, HVCN_EN_Pin|HVCP_EN_Pin|CHRGP_EN_Pin|CHRGN_EN_Pin
                          |PRECHRG_EN_Pin|PUMP_EN_Pin|LATCH_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DEBUG_1_Pin|DEBUG_2_Pin|IMD_IO_H_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MCU_OK_GPIO_Port, MCU_OK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : HVCN_EN_Pin HVCP_EN_Pin CHRGP_EN_Pin CHRGN_EN_Pin
                           PRECHRG_EN_Pin PUMP_EN_Pin LATCH_RST_Pin */
  GPIO_InitStruct.Pin = HVCN_EN_Pin|HVCP_EN_Pin|CHRGP_EN_Pin|CHRGN_EN_Pin
                          |PRECHRG_EN_Pin|PUMP_EN_Pin|LATCH_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : BMS_OUT2_Pin BMS_OUT3_Pin */
  GPIO_InitStruct.Pin = BMS_OUT2_Pin|BMS_OUT3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SAFETY_LOOP_ST_Pin */
  GPIO_InitStruct.Pin = SAFETY_LOOP_ST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SAFETY_LOOP_ST_GPIO_Port, &GPIO_InitStruct);

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

  /*Configure GPIO pins : DEBUG_1_Pin DEBUG_2_Pin IMD_IO_H_Pin */
  GPIO_InitStruct.Pin = DEBUG_1_Pin|DEBUG_2_Pin|IMD_IO_H_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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

/**
  * @brief  Retargets the C library printf function to the USART.
  * @retval None
  */
 PUTCHAR_PROTOTYPE
 {
   /* Place your implementation of fputc here */
   /* e.g. write a character to the USART1 and Loop until the end of transmission */
   HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
 
   return ch;
 }

 /**
  * @brief Print CANBUS Error messages 
  * @author Alex
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan1)
{
    if (HAL_CAN_GetRxMessage(hcan1, CAN_RX_FIFO0, &RxHeader, canRX) != HAL_OK)
    {
        printf("CAN Message Read Failed. HAL ERROR... \r\n");
        return;
    }
    else
    {
        HAL_GPIO_TogglePin(DEBUG_1_GPIO_Port, DEBUG_1_Pin);
        if (RxHeader.IDE == CAN_ID_STD)
        {
            printf("Message has standard ID type...\r\n");
            printf("Message ID:\t%#lx\r\n", RxHeader.StdId);

            if(RxHeader.StdId == 0x6B0){ // BMS
              uint16_t pack_current     = (canRX[1] << 8) | canRX[0];
              uint16_t pack_voltage     = (canRX[3] << 8) | canRX[2];
              uint8_t pack_soc          = canRX[4];
              uint8_t pack_relay_state  = canRX[5];

              printf("Pack Current: %d\r\n", pack_current);
              printf("Pack Voltage: %d\r\n", pack_voltage);
              printf("Pack SoC: %d\r\n", pack_soc);
              printf("Pack Relay Status: %d\r\n", pack_relay_state);

            }
            else if(RxHeader.StdId == 0x6B1){ // BMS
              uint32_t pack_DCL = (canRX[3]<<24) | (canRX[2]<<16) | (canRX[1]<<8) | canRX[0];
              uint8_t high_temp_derating = canRX[4];
              uint8_t low_temp_derating   = canRX[5];

              printf("Pack DCL: %d\r\n", pack_DCL);
              printf("High Temp Derating: %d\r\n", high_temp_derating);
              printf("low_temp_derating: %d\r\n", low_temp_derating);
            }
            else
            {
            printf("ERROR: Unknown IDE type\r\n");
            return;
            }
        }
        else if (RxHeader.IDE == CAN_ID_EXT)
        {
            printf("Message has extended ID type...\r\n");
            printf("Message ID:\t%#lx\r\n", RxHeader.ExtId);

            if (RxHeader.ExtId == 0x0CF11E05)
            {
                uint16_t RPM = (canRX[1] << 8) | canRX[0];
                uint16_t Current = (canRX[3] << 8) | canRX[2];
                uint16_t Voltage = (canRX[5] << 8) | canRX[4];

                printf("R  P  M = %u rpm\r\n", RPM);
                printf("Current = %u A\r\n", Current/ 10);
                printf("Voltage = %u V\r\n", Voltage / 10);

                
            }
          
            else if (RxHeader.ExtId == 0x0CF11F05)
            {
                
                uint8_t throttle_signal = canRX[0];
                int8_t controller_temp = canRX[1] - 40; 
                int8_t motor_temp = canRX[2] - 30; 
                printf("Throttle Signal: %d V\r\n", throttle_signal);
                printf("Controller Temperature: %d ℃\r\n", controller_temp);
                printf("Motor Temperature: %d ℃\r\n", motor_temp);

               
                if (canRX[4] & 0x01)
                    printf("Controller Command: Forward\r\n");
                else if (canRX[4] & 0x02)
                    printf("Controller Command: Backward\r\n");
                else if (canRX[4] & 0x03)
                    printf("Controller Command: Reserved\r\n");
                else printf("Controller Command: Neutral\r\n");

                if (((canRX[4] >> 2) & 0x03 )== 0x00)
                    printf("Feedback: Stationary\r\n");
                else if (((canRX[4] >> 2) & 0x03 ) == 0x01)
                    printf("Feedback: Forward\r\n");
                else if (((canRX[4] >> 2) & 0x03 ) == 0x02)
                    printf("Feedback: Backward\r\n");
                else
                    printf("Feedback: Reserved\r\n");

                printf("Switch Status:\r\n");
                printf("  Boost: %s\r\n", (canRX[5] & 0x80) ? "ON" : "OFF");
                printf("  Foot Switch: %s\r\n", (canRX[5] & 0x40) ? "ON" : "OFF");
                printf("  Forward Switch: %s\r\n", (canRX[5] & 0x20) ? "ON" : "OFF");
                printf("  Backward Switch: %s\r\n", (canRX[5] & 0x10) ? "ON" : "OFF");
                printf("  12V Brake Switch: %s\r\n", (canRX[5] & 0x08) ? "ON" : "OFF");
                printf("  Hall C: %s\r\n", (canRX[5] & 0x04) ? "ON" : "OFF");
                printf("  Hall B: %s\r\n", (canRX[5] & 0x02) ? "ON" : "OFF");
                printf("  Hall A: %s\r\n", (canRX[5] & 0x01) ? "ON" : "OFF");
            }
        }
        else
        {
            printf("ERROR: Unknown IDE type\r\n");
            return;
        }

    
        printf("Message length is %ld byte(s)\r\n", RxHeader.DLC);
        for (uint8_t i = 0; i < 8; i++)
        {
            printf("Byte %d: 0x%02X\r\n", i, canRX[i]);
        }
    }
}



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
