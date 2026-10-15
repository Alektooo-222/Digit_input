#ifndef BSP_DI_H
#define BSP_DI_H

#include <cstdint>
#include "bsp.h"

#define CCMRAM __attribute__((section(".ccmram"), aligned(4)))

extern "C"
{
#include "arm_math.h"
}

namespace BSP
{
    class DI
    {
    public:
        enum class CONST : uint16_t
        {
            //size_adc_buffer = 1024U,
            size_adc_buffer = 400U,
            size_freq_buffer = 1000U,
            size_result_freq_buffer = static_cast<uint16_t>(size_freq_buffer) / 2U,
        };

        enum Type_filter : uint8_t
        {
            none = 0,
            mean,
            median,
            min,
            max
        };

        static void init(CallbackType ADCCallback, CallbackType TIMCallback);
        static const uint16_t *get_adc_buffer_pointer();
        static const uint16_t *get_freq_buffer_pointer();
        static const uint16_t *get_duty_cycle_buffer_pointer();
        static void config_measurement(uint8_t number_of_measurement, Type_filter filter);
        static void start_measurement(uint16_t number_of_measurement);
        static void stop_measurement();
        static bool get_pin_state();

        static inline CallbackType dmaADCCallback = nullptr;
        static inline CallbackType dmaTIMCallback = nullptr;

    private:
        static inline uint16_t number_of_measurement = 2;
        static inline Type_filter filter = none;
        static inline uint16_t timeout = 50; // ms, 40 + 10

        static inline uint16_t adc_buffer[static_cast<uint16_t>(CONST::size_adc_buffer)] = {
            // CCMRAM
            0,
        };
        static inline uint16_t freq_buffer[static_cast<uint16_t>(CONST::size_freq_buffer)] = {
            0,
        };
        static inline uint16_t duty_cycle_buffer[static_cast<uint16_t>(CONST::size_freq_buffer)] = {
            0,
        };
    };
}

#endif