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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticSemaphore_t osStaticMutexDef_t;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for readTask */
osThreadId_t readTaskHandle;
uint32_t readTaskBuffer[ 128 ];
osStaticThreadDef_t readTaskControlBlock;
const osThreadAttr_t readTask_attributes = {
  .name = "readTask",
  .cb_mem = &readTaskControlBlock,
  .cb_size = sizeof(readTaskControlBlock),
  .stack_mem = &readTaskBuffer[0],
  .stack_size = sizeof(readTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for writeTask */
osThreadId_t writeTaskHandle;
uint32_t writeTaskBuffer[ 128 ];
osStaticThreadDef_t writeTaskControlBlock;
const osThreadAttr_t writeTask_attributes = {
  .name = "writeTask",
  .cb_mem = &writeTaskControlBlock,
  .cb_size = sizeof(writeTaskControlBlock),
  .stack_mem = &writeTaskBuffer[0],
  .stack_size = sizeof(writeTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for buffer1Mutex */
osMutexId_t buffer1MutexHandle;
osStaticMutexDef_t buffer1MutexControlBlock;
const osMutexAttr_t buffer1Mutex_attributes = {
  .name = "buffer1Mutex",
  .cb_mem = &buffer1MutexControlBlock,
  .cb_size = sizeof(buffer1MutexControlBlock),
};
/* Definitions for buffer2Mutex */
osMutexId_t buffer2MutexHandle;
osStaticMutexDef_t buffer2MutexControlBlock;
const osMutexAttr_t buffer2Mutex_attributes = {
  .name = "buffer2Mutex",
  .cb_mem = &buffer2MutexControlBlock,
  .cb_size = sizeof(buffer2MutexControlBlock),
};
/* Definitions for buffer1Semaphore */
osSemaphoreId_t buffer1SemaphoreHandle;
osStaticSemaphoreDef_t buffer1SemaphoreControlBlock;
const osSemaphoreAttr_t buffer1Semaphore_attributes = {
  .name = "buffer1Semaphore",
  .cb_mem = &buffer1SemaphoreControlBlock,
  .cb_size = sizeof(buffer1SemaphoreControlBlock),
};
/* Definitions for buffer2Semaphore */
osSemaphoreId_t buffer2SemaphoreHandle;
osStaticSemaphoreDef_t buffer2SemaphoreControlBlock;
const osSemaphoreAttr_t buffer2Semaphore_attributes = {
  .name = "buffer2Semaphore",
  .cb_mem = &buffer2SemaphoreControlBlock,
  .cb_size = sizeof(buffer2SemaphoreControlBlock),
};
/* USER CODE BEGIN PV */
uint16_t buffer1[4];
uint16_t buffer2[4];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
void StartDefaultTask(void *argument);
void StartTaskRead(void *argument);
void StartTaskWrite(void *argument);

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
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of buffer1Mutex */
  buffer1MutexHandle = osMutexNew(&buffer1Mutex_attributes);

  /* creation of buffer2Mutex */
  buffer2MutexHandle = osMutexNew(&buffer2Mutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of buffer1Semaphore */
  buffer1SemaphoreHandle = osSemaphoreNew(1, 0, &buffer1Semaphore_attributes);

  /* creation of buffer2Semaphore */
  buffer2SemaphoreHandle = osSemaphoreNew(1, 0, &buffer2Semaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of readTask */
  readTaskHandle = osThreadNew(StartTaskRead, NULL, &readTask_attributes);

  /* creation of writeTask */
  writeTaskHandle = osThreadNew(StartTaskWrite, NULL, &writeTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
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

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
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
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void encrypt (uint32_t v[2], const uint32_t k[4]) {
  uint32_t v0=v[0], v1=v[1], sum=0, i;   /* set up */
  uint32_t delta=0x9E3779B9;             /* a key schedule constant */
  uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];  /* cache key */
  for (i=0; i<32; i++) {                 /* basic cycle start */
    sum += delta;
    v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
    v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
  }                       /* end cycle */
  v[0]=v0; v[1]=v1;
}


void decrypt (uint32_t v[2], const uint32_t k[4]) {
  /* set up; "sum" was computed from the value of delta
  in the "encrypt" function: sum = (delta << 5) & 0xFFFFFFFF */
  uint32_t v0=v[0], v1=v[1], sum=0xC6EF3720, i;
  uint32_t delta=0x9E3779B9;           /* a key schedule constant */
  uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];  /* cache key */
  for (i=0; i<32; i++) {             /* basic cycle start */
    v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
    v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
    sum -= delta;
  }                       /* end cycle */
  v[0]=v0; v[1]=v1;
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTaskRead */
/**
* @brief Function implementing the readTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskRead */
void StartTaskRead(void *argument)
{
  /* USER CODE BEGIN StartTaskRead */

  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreAcquire(buffer1SemaphoreHandle, 100);
	  osMutexAcquire(buffer1MutexHandle, 100); //Mutex 1 acquire

	  for(uint8_t n = 0;n < 4;n++){
		  HAL_ADC_Start(&hadc1);
		  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		  buffer1[n] = HAL_ADC_GetValue(&hadc1);
	  }

	  osMutexRelease(buffer1MutexHandle);
	  osSemaphoreRelease(buffer1SemaphoreHandle);

	  osSemaphoreAcquire(buffer2SemaphoreHandle, 100);
	  osMutexAcquire(buffer2MutexHandle, 100); //Mutex 1 acquire

	  for(uint8_t n = 0;n < 4;n++){
		  HAL_ADC_Start(&hadc1);
		  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		  buffer2[n] = HAL_ADC_GetValue(&hadc1);
	  }

	  osMutexRelease(buffer2MutexHandle);
	  osSemaphoreRelease(buffer2SemaphoreHandle);
  }
  /* USER CODE END StartTaskRead */
}

/* USER CODE BEGIN Header_StartTaskWrite */
/**
* @brief Function implementing the writeTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskWrite */
void StartTaskWrite(void *argument)
{
  /* USER CODE BEGIN StartTaskWrite */
	uint32_t key[4] = {371, 215, 11, 12};
	GPIOA->ODR |= 1<<6;

  /* Infinite loop */
  for(;;)
  {
	  uint32_t encrypt_vals[2];
	  uint8_t spi_vals[8];

	  // Buffer 1
	  osSemaphoreAcquire(buffer1SemaphoreHandle, 100);
	  osMutexAcquire(buffer1MutexHandle, 100);

	  // Repackage as uint32
	  encrypt_vals[0] = buffer1[1];
	  encrypt_vals[0] = (encrypt_vals[0] << 16) | buffer1[0];
	  encrypt_vals[1] = buffer1[3];
	  encrypt_vals[1] = (encrypt_vals[0] << 16) | buffer1[2];

	  encrypt(encrypt_vals, key);

	  // Repackage as uint8
	  spi_vals[0] = encrypt_vals[0];
	  spi_vals[1] = encrypt_vals[0] >> 8;
	  spi_vals[2] = encrypt_vals[0] >> 16;
	  spi_vals[3] = encrypt_vals[0] >> 24;
	  spi_vals[4] = encrypt_vals[1];
	  spi_vals[5] = encrypt_vals[1] >> 8;
	  spi_vals[6] = encrypt_vals[1] >> 16;
	  spi_vals[7] = encrypt_vals[1] >> 24;

	  // Send out on SPI
	  GPIOA->ODR &= ~(1<<6);
	  HAL_SPI_Transmit(&hspi1, (uint8_t*)spi_vals, 8, 100);
	  GPIOA->ODR |= 1<<6;

	  osMutexRelease(buffer1MutexHandle);
	  osSemaphoreRelease(buffer1SemaphoreHandle);

	  // Buffer 2
	  osSemaphoreAcquire(buffer2SemaphoreHandle, 100);
	  osMutexAcquire(buffer2MutexHandle, 100);

	  // Repackage as uint32
	  encrypt_vals[0] = buffer2[1];
	  encrypt_vals[0] = (encrypt_vals[0] << 16) | buffer2[0];
	  encrypt_vals[1] = buffer2[3];
	  encrypt_vals[1] = (encrypt_vals[0] << 16) | buffer2[2];

	  encrypt(encrypt_vals, key);

	  // Repackage as uint8
	  spi_vals[0] = encrypt_vals[0];
	  spi_vals[1] = encrypt_vals[0] >> 8;
	  spi_vals[2] = encrypt_vals[0] >> 16;
	  spi_vals[3] = encrypt_vals[0] >> 24;
	  spi_vals[4] = encrypt_vals[1];
	  spi_vals[5] = encrypt_vals[1] >> 8;
	  spi_vals[6] = encrypt_vals[1] >> 16;
	  spi_vals[7] = encrypt_vals[1] >> 24;

	  // Send out on SPI
	  GPIOA->ODR &= ~(1<<6);
	  HAL_SPI_Transmit(&hspi1, (uint8_t*)spi_vals, 8, 100);
	  GPIOA->ODR |= 1<<6;

	  osMutexRelease(buffer2MutexHandle);
	  osSemaphoreRelease(buffer2SemaphoreHandle);
  }
  /* USER CODE END StartTaskWrite */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
