#include "main.h"
#include "bsp_di.h"

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *adcHandle);
void HAL_ADC_MspInit(ADC_HandleTypeDef *adcHandle);
void MX_ADC1_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *timHandle);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *tim_baseHandle);
void MX_TIM8_Init(void);
void MX_TIM2_Init(void);
void MX_TIM1_Init(void);

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim8;
DMA_HandleTypeDef hdma_tim8_ch1;
DMA_HandleTypeDef hdma_tim8_ch2;
DMA_HandleTypeDef hdma_tim8_ch3_up;
DMA_HandleTypeDef hdma_tim8_ch4_trig_com;

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *tim_baseHandle);

void BSP::DI::init(CallbackType ADCCallback, CallbackType TIMCallback)
{
    dmaADCCallback = ADCCallback;
    dmaTIMCallback = TIMCallback;

    // arm_biquad_cascade_df1_init_f32();

    /* DMA interrupt init */
    /* DMA1_Channel1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5U, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    /* DMA2_Channel1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Channel1_IRQn, 5U, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel1_IRQn);
    /* DMA2_Channel2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Channel2_IRQn, 5U, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel2_IRQn);
    /* DMA2_Channel3_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Channel3_IRQn, 5U, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel3_IRQn);
    /* DMA2_Channel5_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Channel5_IRQn, 5U, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel5_IRQn);

    MX_ADC1_Init();
    MX_TIM1_Init();
    MX_TIM8_Init();
    MX_TIM2_Init();

    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }

    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_buffer[0], static_cast<uint16_t>(CONST::size_adc_buffer)) != HAL_OK)
    {
        /* Start Conversation Error */
        Error_Handler();
    }

    if (HAL_TIM_Base_Start(&htim1) != HAL_OK)
    {
        /* Counter Enable Error */
        Error_Handler();
    }

    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK)
    {
        /* Counter Enable Error */
        Error_Handler();
    }

    if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) != HAL_OK)
    {
        /* Counter Enable Error */
        Error_Handler();
    }

    if (HAL_TIM_Base_Start(&htim8) != HAL_OK)
    {
        /* Counter Enable Error */
        Error_Handler();
    }
}

const uint16_t *BSP::DI::get_adc_buffer_pointer()
{
    return adc_buffer;
}

const uint16_t *BSP::DI::get_freq_buffer_pointer()
{
    return freq_buffer;
}

const uint16_t *BSP::DI::get_duty_cycle_buffer_pointer()
{
    return duty_cycle_buffer;
}

void BSP::DI::start_measurement(uint16_t number_of_measurement)
{
//    if (HAL_TIM_Base_Start(&htim8) != HAL_OK)
//    {
//        /* Counter Enable Error */
//        Error_Handler();
//    }
HAL_StatusTypeDef st = HAL_TIM_IC_Start_DMA(&htim8, TIM_CHANNEL_1, (uint32_t *)freq_buffer, number_of_measurement);
    if (st != HAL_OK)
    {
        /* Counter Enable Error */
        Error_Handler();
    }
    __HAL_DMA_DISABLE_IT(&hdma_tim8_ch1, DMA_IT_HT);

    /* HAL_TIM_IC_Stop_DMA()
    __HAL_TIM_CLEAR_FLAG(&htim8, TIM_FLAG_CC1); */

    if (HAL_TIM_IC_Start_DMA(&htim8, TIM_CHANNEL_2, (uint32_t *)duty_cycle_buffer, number_of_measurement) != HAL_OK)
    {
        /* Counter Enable Error */
        Error_Handler();
    }
    __HAL_DMA_DISABLE_IT(&hdma_tim8_ch2, DMA_IT_HT);

    return;
}

void BSP::DI::stop_measurement()
{
//    if (HAL_TIM_Base_Stop(&htim8) != HAL_OK)
//    {
//        /* Counter Enable Error */
//        Error_Handler();
//    }

    if (HAL_TIM_IC_Stop_DMA(&htim8, TIM_CHANNEL_1) != HAL_OK)
    {
        /* Counter Enable Error */
        Error_Handler();
    }
    __HAL_TIM_CLEAR_FLAG(&htim8, TIM_FLAG_CC1);

    /* HAL_TIM_IC_Stop_DMA()
    __HAL_TIM_CLEAR_FLAG(&htim8, TIM_FLAG_CC1); */

    if (HAL_TIM_IC_Stop_DMA(&htim8, TIM_CHANNEL_2) != HAL_OK)
    {
        /* Counter Enable Error */
        Error_Handler();
    }
    __HAL_TIM_CLEAR_FLAG(&htim8, TIM_FLAG_CC2);

    return;
}

void BSP::DI::config_measurement(uint8_t number_of_measurement, Type_filter filter)
{
    return;
}

bool BSP::DI::get_pin_state()
{
    return static_cast<bool>(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15));
}

extern "C"
{
    void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim)
    {
        return;
    }

    void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
    {
        BSP::DI::dmaTIMCallback();
        return;
    }

    void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
    {
        // обнуляем счётчик
        BSP::DI::dmaADCCallback();

        volatile uint32_t count_tic = 0; // кол-во тактов
        if (hadc->Instance == ADC1)
        {
            /* DWT_CYCCNT = 0;

            for (uint16_t i = SIZE_BUF / 2U; i < SIZE_BUF; i++)
            {
              LOGV(3, data[i]);
            }

            count_tic = DWT_CYCCNT;

            return; */
            // flag_cpl = true;
        }
    }

    void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
    {
        volatile uint32_t count_tic = 0; // кол-во тактов

        BSP::DI::dmaADCCallback();
        if (hadc->Instance == ADC1)
        {
            /* DWT_CYCCNT = 0;

            for (uint16_t i = 0; i < SIZE_BUF / 2U; i++)
            {
              LOGV(3, data[i]);
            }

            count_tic = DWT_CYCCNT;

            return; */
            // flag_half_cpl = true;
        }
    }

    void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
    {
        if (hadc->Instance == ADC1)
        {
            uint32_t error = HAL_ADC_GetError(hadc);
            uint32_t dma_error = HAL_DMA_GetError(hadc->DMA_Handle);
            return;
        }
    }
}

void MX_ADC1_Init(void)
{
    ADC_MultiModeTypeDef multimode = {};
    ADC_ChannelConfTypeDef sConfig = {};

    /** Common config
     */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure the ADC multi-mode
     */
    multimode.Mode = ADC_MODE_INDEPENDENT;
    if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure Regular Channel
     */
    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.SamplingTime = ADC_SAMPLETIME_61CYCLES_5;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *adcHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {};
    if (adcHandle->Instance == ADC1)
    {
        /* ADC1 clock enable */
        __HAL_RCC_ADC12_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**ADC1 GPIO Configuration
        PA0     ------> ADC1_IN1
        */
        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* ADC1 DMA Init */
        /* ADC1 Init */
        hdma_adc1.Instance = DMA1_Channel1;
        hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc1.Init.Mode = DMA_CIRCULAR;
        hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(adcHandle, DMA_Handle, hdma_adc1);
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *adcHandle)
{

    if (adcHandle->Instance == ADC1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_ADC12_CLK_DISABLE();

        /**ADC1 GPIO Configuration
        PA0     ------> ADC1_IN1
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

        /* ADC1 DMA DeInit */
        HAL_DMA_DeInit(adcHandle->DMA_Handle);
    }
}

void MX_TIM1_Init(void)
{

    TIM_ClockConfigTypeDef sClockSourceConfig = {};
    TIM_MasterConfigTypeDef sMasterConfig = {};
    TIM_OC_InitTypeDef sConfigOC = {};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {};

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 0;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 3599;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1REF;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 1800;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.BreakFilter = 0;
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
    sBreakDeadTimeConfig.Break2Filter = 0;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
    {
        Error_Handler();
    }
}
/* TIM2 init function */
void MX_TIM2_Init(void)
{

    TIM_ClockConfigTypeDef sClockSourceConfig = {};
    TIM_MasterConfigTypeDef sMasterConfig = {};
    TIM_OC_InitTypeDef sConfigOC = {};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 11;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 59999;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 30000;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    __HAL_TIM_DISABLE_OCxPRELOAD(&htim2, TIM_CHANNEL_1);

    HAL_TIM_MspPostInit(&htim2);
}
/* TIM8 init function */
void MX_TIM8_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {};
    TIM_MasterConfigTypeDef sMasterConfig = {};
    TIM_IC_InitTypeDef sConfigIC = {};

    htim8.Instance = TIM8;
    htim8.Init.Prescaler = 71;
    htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim8.Init.Period = 65535;
    htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim8.Init.RepetitionCounter = 0;
    htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_IC_Init(&htim8) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;
    if (HAL_TIM_IC_ConfigChannel(&htim8, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
    sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
    if (HAL_TIM_IC_ConfigChannel(&htim8, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *tim_baseHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {};
    if (tim_baseHandle->Instance == TIM1)
    {
        /* TIM1 clock enable */
        __HAL_RCC_TIM1_CLK_ENABLE();
    }
    else if (tim_baseHandle->Instance == TIM2)
    {
        /* TIM2 clock enable */
        __HAL_RCC_TIM2_CLK_ENABLE();
    }
    else if (tim_baseHandle->Instance == TIM8)
    {
        /* TIM8 clock enable */
        __HAL_RCC_TIM8_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**TIM8 GPIO Configuration
        PA15     ------> TIM8_CH1
        */
        GPIO_InitStruct.Pin = GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM8;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* TIM8 DMA Init */
        /* TIM8_CH1 Init */
        hdma_tim8_ch1.Instance = DMA2_Channel3;
        hdma_tim8_ch1.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_tim8_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim8_ch1.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim8_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tim8_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_tim8_ch1.Init.Mode = DMA_NORMAL;
        hdma_tim8_ch1.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma_tim8_ch1) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(tim_baseHandle, hdma[TIM_DMA_ID_CC1], hdma_tim8_ch1);

        /* TIM8_CH2 Init */
        hdma_tim8_ch2.Instance = DMA2_Channel5;
        hdma_tim8_ch2.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_tim8_ch2.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim8_ch2.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim8_ch2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tim8_ch2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_tim8_ch2.Init.Mode = DMA_NORMAL;
        hdma_tim8_ch2.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma_tim8_ch2) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(tim_baseHandle, hdma[TIM_DMA_ID_CC2], hdma_tim8_ch2);

        /* TIM8_CH3_UP Init */
        hdma_tim8_ch3_up.Instance = DMA2_Channel1;
        hdma_tim8_ch3_up.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_tim8_ch3_up.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim8_ch3_up.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim8_ch3_up.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tim8_ch3_up.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_tim8_ch3_up.Init.Mode = DMA_NORMAL;
        hdma_tim8_ch3_up.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma_tim8_ch3_up) != HAL_OK)
        {
            Error_Handler();
        }

        /* Several peripheral DMA handle pointers point to the same DMA handle.
         Be aware that there is only one channel to perform all the requested DMAs. */
        __HAL_LINKDMA(tim_baseHandle, hdma[TIM_DMA_ID_CC3], hdma_tim8_ch3_up);
        __HAL_LINKDMA(tim_baseHandle, hdma[TIM_DMA_ID_UPDATE], hdma_tim8_ch3_up);

        /* TIM8_CH4_TRIG_COM Init */
        hdma_tim8_ch4_trig_com.Instance = DMA2_Channel2;
        hdma_tim8_ch4_trig_com.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_tim8_ch4_trig_com.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim8_ch4_trig_com.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim8_ch4_trig_com.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tim8_ch4_trig_com.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_tim8_ch4_trig_com.Init.Mode = DMA_NORMAL;
        hdma_tim8_ch4_trig_com.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma_tim8_ch4_trig_com) != HAL_OK)
        {
            Error_Handler();
        }

        /* Several peripheral DMA handle pointers point to the same DMA handle.
         Be aware that there is only one channel to perform all the requested DMAs. */
        __HAL_LINKDMA(tim_baseHandle, hdma[TIM_DMA_ID_CC4], hdma_tim8_ch4_trig_com);
        __HAL_LINKDMA(tim_baseHandle, hdma[TIM_DMA_ID_TRIGGER], hdma_tim8_ch4_trig_com);
        __HAL_LINKDMA(tim_baseHandle, hdma[TIM_DMA_ID_COMMUTATION], hdma_tim8_ch4_trig_com);
    }
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *timHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {};
    if (timHandle->Instance == TIM2)
    {

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**TIM2 GPIO Configuration
        PA5     ------> TIM2_CH1
        */
        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *tim_baseHandle)
{

    if (tim_baseHandle->Instance == TIM1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_TIM1_CLK_DISABLE();
    }
    else if (tim_baseHandle->Instance == TIM2)
    {
        /* Peripheral clock disable */
        __HAL_RCC_TIM2_CLK_DISABLE();
    }
    else if (tim_baseHandle->Instance == TIM8)
    {
        /* Peripheral clock disable */
        __HAL_RCC_TIM8_CLK_DISABLE();

        /**TIM8 GPIO Configuration
        PA15     ------> TIM8_CH1
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);

        /* TIM8 DMA DeInit */
        HAL_DMA_DeInit(tim_baseHandle->hdma[TIM_DMA_ID_CC1]);
        HAL_DMA_DeInit(tim_baseHandle->hdma[TIM_DMA_ID_CC2]);
        HAL_DMA_DeInit(tim_baseHandle->hdma[TIM_DMA_ID_CC3]);
        HAL_DMA_DeInit(tim_baseHandle->hdma[TIM_DMA_ID_UPDATE]);
        HAL_DMA_DeInit(tim_baseHandle->hdma[TIM_DMA_ID_CC4]);
        HAL_DMA_DeInit(tim_baseHandle->hdma[TIM_DMA_ID_TRIGGER]);
        HAL_DMA_DeInit(tim_baseHandle->hdma[TIM_DMA_ID_COMMUTATION]);
    }
}