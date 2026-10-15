#include "di.h"

float32_t DI::output_buffer[static_cast<uint16_t>(BSP::DI::CONST::size_adc_buffer)]  = {};
float32_t DI::input_buffer[static_cast<uint16_t>(BSP::DI::CONST::size_adc_buffer) / 2U]  = {};