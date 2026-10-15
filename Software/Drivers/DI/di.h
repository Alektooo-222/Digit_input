#ifndef DI_H
#define DI_H

#include <assert.h>

#include "hal_di.h"
#include "bsp_di.h"

extern "C"
{
/* FreeRTOS */
#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "arm_math.h"
#include <string.h>
}

#include "etl/algorithm.h"
#include "dwt.h"
#include <cstdlib>

#define CCMRAM __attribute__((section(".ccmram"), aligned(4)))

extern UBaseType_t watermark_measure;

class DI
{
public:
    using CONST = BSP::DI::CONST;
    using Type_filter = BSP::DI::Type_filter;

    // arm_biquad_cascade_df1_init_f32();
    // arm_pid_f32();
    // arm_biquad_cascade_df1_init_f32();

    static void init()
    {
        /* Create transmit queue */
        Queue = xQueueCreate(QueueLength, sizeof(StatusMeasurement));
        assert(Queue != nullptr);

        if (pdPASS != xTaskCreate(lpf_processTask, "lpf_processTask", configMINIMAL_STACK_SIZE * 2U,
                                  NULL, tskIDLE_PRIORITY + 4, &lpf_processHandle))
        {
            assert(false);
        }

        if (pdPASS != xTaskCreate(freq_measurement_processTask, "freq_measurement_processTask", configMINIMAL_STACK_SIZE * 3U,
                                  NULL, tskIDLE_PRIORITY + 4, &freq_measurement_processHandle))
        {
            assert(false);
        }

        xTimer = xTimerCreate(
            "Timer",
            pdMS_TO_TICKS(timeout),
            pdFALSE,
            (void *)0,
            vTimerCallback);

        arm_biquad_cascade_df1_init_f32(&filter, number_of_biquad_stage, iir_coeffs, filter_state);

        DIPort::init(dmaADCCallback, dmaTIMCallback);
    }

    static int16_t get_current_mA()
    {
        float32_t current_mA = 0.0f;
        float32_t voltage_mV = 0.0f;

        voltage_mV = current_row / 4095.0f * 3300.0f;
        current_mA = 0.7802197f * voltage_mV - 1250;

        // return /* static_cast<uint16_t>( */ /* current_row / 4095.0f * 3300.0f / 1.28169f */;
        return static_cast<int16_t>(current_mA);
    }

    static int16_t get_current_row()
    {
        return static_cast<uint16_t>(current_row);
    }

    static uint16_t get_freq()
    {
        return freq;
    }

    static uint16_t get_duty()
    {
        return duty;
    }

    static uint16_t get_pulse_ms()
    {
        return pulse / 1000;
    }

    static uint16_t get_pulse_us()
    {
        return pulse;
    }

    static bool get_pin_state()
    {
        return DIPort::get_pin_state();
    }

    enum ConfigTypeMeasure : uint8_t
    {
        FREQ_MEASURE = 0,
        PULSE_MEASURE,
    };

    static void config_measure_freq()
    {
        type_measure = ConfigTypeMeasure::FREQ_MEASURE;
        return;
    }

    static void config_measure_pulse()
    {
        type_measure = ConfigTypeMeasure::PULSE_MEASURE;
        return;
    }

    static ConfigTypeMeasure get_type_measure()
    {
        return type_measure;
    }

private:
    using DIPort = HAL::DI<BSP::DI, CONST, Type_filter>;
    static inline TaskHandle_t lpf_processHandle;
    static inline TaskHandle_t freq_measurement_processHandle;

    static constexpr uint32_t QueueLength = 1U;

    static inline QueueHandle_t Queue = nullptr;

    enum StatusMeasurement : uint8_t
    {
        TIMOUT = 0,
        MEASURMENT_OK
    };

    static inline TimerHandle_t xTimer;

    static void dmaADCCallback()
    {
        // const uint16_t * p = BSP::DI::get_adc_buffer_pointer();
        BaseType_t higherPriorityTaskWoken = pdFALSE;

        vTaskNotifyGiveFromISR(
            lpf_processHandle,
            &higherPriorityTaskWoken);

        portYIELD_FROM_ISR(higherPriorityTaskWoken);
        return;
    }

    static void lpf_processTask(void *pvParameters)
    {
        const uint16_t *adc_buffer = DIPort::get_adc_buffer_pointer();

        const float32_t *p_iir_coeffs = iir_coeffs;
        const float32_t *p_output_buffer = output_buffer;

        /* static inline float32_t output_buffer[static_cast<uint16_t>(CONST::size_adc_buffer)] = {}; */
        volatile uint32_t counter = 0;
        volatile uint32_t dwt_counter = 0;

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        for (;;)
        {
            /* ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            continue; */

            DWT_CYCCNT = 0;
            for (uint16_t i = 0; i < static_cast<uint16_t>(CONST::size_adc_buffer) / 2U; i++)
            {
                input_buffer[i] = static_cast<float32_t>(adc_buffer[i]);
            }

            arm_mean_f32(input_buffer, static_cast<uint16_t>(CONST::size_adc_buffer) / 2U, &current_row);

            /* arm_biquad_cascade_df1_f32(&filter, input_buffer, output_buffer, static_cast<uint16_t>(CONST::size_adc_buffer) / 2U);
            dwt_counter = DWT_CYCCNT; */
            // arm_sort_f32(buffer, sorted, 5, ARM_SORT_ASCENDING);
            // memcpy(&output_buffer[(static_cast<uint16_t>(CONST::size_adc_buffer) / 2U) - 5U], sorted, 5U);
            /* memcpy(sorted, &output_buffer[(static_cast<uint16_t>(CONST::size_adc_buffer) / 2U) - median_window], median_window * 4U); */

            /* DWT_CYCCNT = 0;
            etl::sort(sorted, sorted + median_window); */
            // etl::sort(&output_buffer[(static_cast<uint16_t>(CONST::size_adc_buffer) / 2U) - 5U], &output_buffer[(static_cast<uint16_t>(CONST::size_adc_buffer) / 2U) - 5U] + 5U);
            /* dwt_counter = DWT_CYCCNT; */
            // current_row = sorted[median_window / 2]; // output_buffer[(static_cast<uint16_t>(CONST::size_adc_buffer) / 2U) - 5U];
            /* current_row = output_buffer[(static_cast<uint16_t>(CONST::size_adc_buffer) / 2U) - 1U]; */

            counter++;
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

            DWT_CYCCNT = 0;
            for (uint16_t i = static_cast<uint16_t>(CONST::size_adc_buffer) / 2U; i < static_cast<uint16_t>(CONST::size_adc_buffer); i++)
            {
                input_buffer[i - static_cast<uint16_t>(CONST::size_adc_buffer) / 2U] = static_cast<float32_t>(adc_buffer[i]);
            }
            dwt_counter = DWT_CYCCNT;
            DWT_CYCCNT = 0;
            arm_mean_f32(input_buffer, static_cast<uint16_t>(CONST::size_adc_buffer) / 2U, &current_row);
            dwt_counter = DWT_CYCCNT;
            /* DWT_CYCCNT = 0;
            arm_biquad_cascade_df1_f32(&filter, input_buffer, &output_buffer[static_cast<uint16_t>(CONST::size_adc_buffer) / 2U], static_cast<uint16_t>(CONST::size_adc_buffer) / 2U);
            dwt_counter = DWT_CYCCNT; */

            // memcpy(&output_buffer[(static_cast<uint16_t>(CONST::size_adc_buffer)) - 5U], sorted, 5U);
            /* memcpy(sorted, &output_buffer[(static_cast<uint16_t>(CONST::size_adc_buffer)) - median_window], median_window * 4U);
            etl::sort(sorted, sorted + median_window); */
            // current_row = sorted[median_window / 2];

            /* current_row = output_buffer[static_cast<uint16_t>(CONST::size_adc_buffer) - 1U]; */

            counter++;
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }
    }

    static void vTimerCallback(TimerHandle_t xTimer)
    {
        StatusMeasurement st = StatusMeasurement::TIMOUT;

        xQueueSend(Queue, (void *)&st, (TickType_t)0U);
        return;
    }

    static void dmaTIMCallback()
    {
        StatusMeasurement st = StatusMeasurement::MEASURMENT_OK;
        /* static uint8_t counter = 0; */
        BaseType_t higherPriorityTaskWoken = pdFALSE;

        counter++;
        if (counter % 2 == 0)
        {
            xQueueSendFromISR(Queue, (void *)&st, &higherPriorityTaskWoken);

            portYIELD_FROM_ISR(higherPriorityTaskWoken);
        }

        return;
    }

    static void freq_measurement_processTask(void *pvParameters)
    {
        StatusMeasurement st = StatusMeasurement::TIMOUT;
        TickType_t xLastWakeTime;
        const TickType_t xFrequency = pdMS_TO_TICKS(100); // Период 200 мс
        xLastWakeTime = xTaskGetTickCount();

        const uint16_t *freq_buffer = DIPort::get_freq_buffer_pointer();
        const uint16_t *duty_cycle_buffer = DIPort::get_duty_cycle_buffer_pointer();

        for (;;)
        {
            if (type_measure == ConfigTypeMeasure::FREQ_MEASURE)
            {
                xTimerChangePeriod(xTimer, pdMS_TO_TICKS(timeout), 0U);

                DIPort::start_measurement(number_of_measurement);

                if (xTimer != NULL)
                {
                    xTimerStart(xTimer, 0);
                }
            }
            else if (type_measure == ConfigTypeMeasure::PULSE_MEASURE)
            {
                xTimerChangePeriod(xTimer, pdMS_TO_TICKS(2000U), 0U);

                DIPort::start_measurement(1U);

                if (xTimer != NULL)
                {
                    xTimerStart(xTimer, 0U);
                }
            }

            xQueueReceive(Queue, (void *)&st, portMAX_DELAY);

            if (st == StatusMeasurement::MEASURMENT_OK)
            {
                xTimerStop(xTimer, 0U);

                uint16_t cnt1 = freq_buffer[0];
                uint16_t cnt2 = freq_buffer[1];

                uint16_t cnt_duty_1 = duty_cycle_buffer[0];
                uint16_t cnt_duty_2 = duty_cycle_buffer[1];

                if (type_measure == ConfigTypeMeasure::PULSE_MEASURE)
                {
                    freq = 0;
                    duty = 0;
                    /* watermark_measure = uxTaskGetStackHighWaterMark(nullptr); */
                    pulse = abs(static_cast<int16_t>(duty_cycle_buffer[0] - freq_buffer[0]));
                    continue;
                }

                pulse = 0;

                for (uint16_t i = 0; i < (number_of_measurement - 1U); i++)
                {
                    out_freq_buffer[i] = freq_buffer[i + 1] - freq_buffer[i];

                    if (freq_buffer[i + 1] < freq_buffer[i])
                    {
                        out_freq_buffer[i] -= 1U;
                    }
                }

                for (uint16_t i = 0; i < number_of_measurement; i++)
                {
                    out_duty_cycle_buffer[i] = static_cast<uint16_t>(duty_cycle_buffer[i] - freq_buffer[i]);
                    uint16_t j = i >= number_of_measurement - 1U ? i - 1 : i;
                    if (out_duty_cycle_buffer[i] > out_freq_buffer[j])
                    {
                        out_duty_cycle_buffer[i] = out_freq_buffer[j] - static_cast<uint16_t>(freq_buffer[i] - duty_cycle_buffer[i]);
                    }
                }

                freq = static_cast<uint16_t>(1000000.0f / static_cast<float32_t>(out_freq_buffer[0]));
                duty = static_cast<uint16_t>(static_cast<float32_t>(out_duty_cycle_buffer[0]) / static_cast<float32_t>(out_freq_buffer[0]) * 100.0f);
            }
            else if (st == StatusMeasurement::TIMOUT)
            {
                DIPort::stop_measurement();
                xTimerStop(xTimer, 0U);
                counter = 0;

                freq = 0;
                duty = 0;
                pulse = 0;
            }

            watermark_measure = uxTaskGetStackHighWaterMark(nullptr);
            vTaskDelayUntil(&xLastWakeTime, xFrequency);
        }
    }

    /* Filter config */
    static float32_t output_buffer[static_cast<uint16_t>(CONST::size_adc_buffer)] __attribute__((section(".ccmram"), aligned(4)));
    static float32_t input_buffer[static_cast<uint16_t>(CONST::size_adc_buffer) / 2U] CCMRAM;
    static inline constexpr uint8_t number_of_biquad_stage = 4U;
    static inline arm_biquad_casd_df1_inst_f32 filter;
    static inline float32_t filter_state[4U * number_of_biquad_stage];
    static inline constexpr uint8_t median_window = 11U;
    static inline float32_t sorted[median_window] = {};
    /* static inline float32_t iir_coeffs[] = {
        3.93251936508487e-05f,
        7.86503873016974e-05f,
        3.93251936508487e-05f,
        1.99210650006223f,
        -0.992263800836836f,

        3.90805992557493e-05f,
        7.81611985114986e-05f,
        3.90805992557493e-05f,
        1.97971601856372f,
        -0.979872340960743f,

        0.00624403504634285f,
        0.00624403504634285f,
        0.0f,
        0.987511929907314f,
        0.0f}; */
    static inline float32_t iir_coeffs[] =
        {
            // Section 1
            4.98027552888283e-05f,
            9.96055105776566e-05f,
            4.98027552888283e-05f,
            1.99430141002153f,
            -0.994500621042688f,

            // Section 2
            4.95509983769489e-05f,
            9.91019967538978e-05f,
            4.95509983769489e-05f,
            1.98422005686282f,
            -0.984418260856324f,

            // Section 3
            4.93600248984040e-05f,
            9.87200497968080e-05f,
            4.93600248984040e-05f,
            1.97657271535872f,
            -0.976770155458312f,

            // Section 4
            4.92572834309494e-05f,
            9.85145668618988e-05f,
            4.92572834309494e-05f,
            1.97245853628922f,
            -0.972655565422947f};

    static inline float32_t current_row = 0;
    static inline float32_t current_mean_row = 0;
    static inline uint16_t timeout = 50; // ms
    static inline uint16_t number_of_measurement = 2U;
    static inline uint16_t out_freq_buffer[100U] = {};
    static inline uint16_t out_duty_cycle_buffer[100U] = {};
    static inline uint16_t freq = 0;
    static inline uint16_t duty = 0;
    static inline uint16_t pulse = 0;
    static inline uint8_t counter = 0;
    static inline ConfigTypeMeasure type_measure = ConfigTypeMeasure::FREQ_MEASURE;
};
#endif