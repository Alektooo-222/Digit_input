/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
extern "C"
{
#include "swoTrace.h"
#include <stdint.h>
#include <stdbool.h>
#include "dwt.h"

#include "arm_math.h"
}

#include "drivers.h"
/* #include "arm_math.h"
#include "arm_const_structs.h" */

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

TaskHandle_t MainHandle;
static void MainTask(void *pvParameters);

#define SIZE_BUF 1000U

#define SIZE_BUF_CH1 100U
#define SIZE_BUF_CH2 100U

uint16_t data[SIZE_BUF] = {
    0,
};

uint16_t data_tim8_ch1[SIZE_BUF_CH1] = {
    0,
};

uint16_t data_tim8_ch2[SIZE_BUF_CH2] = {
    0,
};

bool flag_half_cpl = false;
bool flag_cpl = false;

volatile static uint8_t test = 0;
volatile static uint8_t *test_pointer = &test;

#define TEST_LENGTH_SAMPLES 2048
UBaseType_t watermark;
UBaseType_t watermark_measure;
/* -------------------------------------------------------------------
 * External Input and Output buffer Declarations for FFT Bin Example
 * ------------------------------------------------------------------- */
/* extern float32_t testInput_f32_10khz[TEST_LENGTH_SAMPLES];
static float32_t testOutput[TEST_LENGTH_SAMPLES / 2]; */

/* ------------------------------------------------------------------
 * Global variables for FFT Bin Example
 * ------------------------------------------------------------------- */
// uint32_t fftSize = 1024;
// uint32_t ifftFlag = 0;
// uint32_t doBitReverse = 1;
// arm_cfft_instance_f32 varInstCfftF32;

/* Reference index at which max energy of bin ocuurs */
// uint32_t refIndex = 213, testIndex = 0;

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
//{
//   // обнуляем счётчик
//
//   volatile uint32_t count_tic = 0; // кол-во тактов
//   if (hadc->Instance == ADC1)
//   {
//     /* DWT_CYCCNT = 0;
//
//     for (uint16_t i = SIZE_BUF / 2U; i < SIZE_BUF; i++)
//     {
//       LOGV(3, data[i]);
//     }
//
//     count_tic = DWT_CYCCNT;
//
//     return; */
//     flag_cpl = true;
//   }
// }

// void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
//{
//   volatile uint32_t count_tic = 0; // кол-во тактов
//
//   if (hadc->Instance == ADC1)
//   {
//     /* DWT_CYCCNT = 0;
//
//     for (uint16_t i = 0; i < SIZE_BUF / 2U; i++)
//     {
//       LOGV(3, data[i]);
//     }
//
//     count_tic = DWT_CYCCNT;
//
//     return; */
//     flag_half_cpl = true;
//   }
// }

// void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
//{
//   if (hadc->Instance == ADC1)
//   {
//     return;
//   }
// }

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
  // MX_GPIO_Init();
  // MX_DMA_Init();
  // MX_ADC1_Init();
  // MX_TIM1_Init();
  // MX_TIM2_Init();
  //MX_USB_DEVICE_Init();
  // MX_TIM8_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */

  swoInit(0xFFFFFFFF, SystemCoreClock, 2250000U); // SWO_STLINK_V2_MAX_SPEED); // All ITM ports, 72 MHz, 2.25 Mbps (max speed for ST-LINK V2, 8 us per byte)
  // LOGI("Count\n");                                // %d", tick);  // LOGI is info level

  /* LOGV(3, (int32_t)test_pointer);
  LOGV(3, 3); */
  volatile int16_t test_value = static_cast<uint16_t>(465U) - static_cast<uint16_t>(565U);
  Drivers::init();
  dwtInit();

  /* arm_status status;
  float32_t maxValue;

  status = ARM_MATH_SUCCESS;

  status = arm_cfft_init_1024_f32(&varInstCfftF32); */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //  if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
  //  {
  //    /* Calibration Error */
  //    Error_Handler();
  //  }

  //  if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&data[0], SIZE_BUF) != HAL_OK)
  //  {
  //    /* Start Conversation Error */
  //    Error_Handler();
  //  }
  //
  //  if (HAL_TIM_Base_Start(&htim1) != HAL_OK)
  //  {
  //    /* Counter Enable Error */
  //    Error_Handler();
  //  }
  //
  //
  //  if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK)
  //  {
  //    /* Counter Enable Error */
  //    Error_Handler();
  //  }
  // HAL_TIM_PWM_Start_DMA()
  // HAL_TIM_Base_Start_DMA();
  // HAL_TIM_IC_Start_DMA();

  //  if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) != HAL_OK)
  //  {
  //    /* Counter Enable Error */
  //    Error_Handler();
  //  }

  //  if (HAL_TIM_Base_Start(&htim8) != HAL_OK)
  //  {
  //    /* Counter Enable Error */
  //    Error_Handler();
  //  }

  //  DWT_CYCCNT = 0;
  //  if (HAL_TIM_IC_Start_DMA(&htim8, TIM_CHANNEL_1, (uint32_t *)data_tim8_ch1, SIZE_BUF_CH1) != HAL_OK)
  //  {
  //    /* Counter Enable Error */
  //    Error_Handler();
  //  }
  //  count_tic = DWT_CYCCNT;
  //  /* HAL_TIM_IC_Stop_DMA()
  //  __HAL_TIM_CLEAR_FLAG(&htim8, TIM_FLAG_CC1); */
  //
  //  if (HAL_TIM_IC_Start_DMA(&htim8, TIM_CHANNEL_2, (uint32_t *)data_tim8_ch2, SIZE_BUF_CH2) != HAL_OK)
  //  {
  //    /* Counter Enable Error */
  //    Error_Handler();
  //  }

  /* __HAL_TIM_DISABLE(&htim2);
  __HAL_TIM_ENABLE_DMA(); */
  //  TIM8->CR1 &= ~(1U);

  // volatile uint32_t count_tic = 0;
  // arm_biquad_cascade_df1_init_f32()

  if (pdPASS != xTaskCreate(MainTask, "lpf_processTask", configMINIMAL_STACK_SIZE * 3U,
                            NULL, tskIDLE_PRIORITY + 0, &MainHandle))
  {
    assert(false);
  }

  vTaskStartScheduler();

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // кол-во тактов

    if (flag_half_cpl == true)
    {
      DWT_CYCCNT = 0;

      /* for (uint16_t i = 0; i < SIZE_BUF / 2U; i++)
      {
        LOGV(3, data[i]);
      } */

      // count_tic = DWT_CYCCNT;

      flag_half_cpl = false;
    }

    if (flag_cpl == true)
    {
      DWT_CYCCNT = 0;

      /* for (uint16_t i = SIZE_BUF / 2U; i < SIZE_BUF; i++)
      {
        LOGV(3, data[i]);
      } */

      // count_tic = DWT_CYCCNT;
      flag_cpl = false;
    }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB | RCC_PERIPHCLK_TIM1 | RCC_PERIPHCLK_TIM8 | RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV4;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  PeriphClkInit.Tim8ClockSelection = RCC_TIM8CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

static void MainTask(void *pvParameters)
{
  volatile int16_t current = 0;
  volatile uint16_t current_row = 0;
  volatile uint16_t freq = 0;
  volatile uint16_t duty = 0;
  volatile uint16_t pulse_us = 0;
  
  
  for (;;)
  {
    current = DI::get_current_mA();
    current_row = DI::get_current_row();
    freq = DI::get_freq();
    duty = DI::get_duty();
    pulse_us = DI::get_pulse_us();


    /* LOGI("%d mA\n", current);
    vTaskDelay(100U);
    LOGW("%d Hz\n", freq);
    vTaskDelay(100U);
    LOGE("%d pr\n", duty);
    vTaskDelay(100U); */

    LOGI("%d us\n", pulse_us);
    vTaskDelay(100U);
    LOGW("%d Hz\n", freq);
    vTaskDelay(100U);
    LOGE("%d pr\n", duty);
    vTaskDelay(100U);

    LOGV(3, current_row);
    watermark = uxTaskGetStackHighWaterMark(nullptr);
    vTaskDelay(100U);
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

#ifdef USE_FULL_ASSERT
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
