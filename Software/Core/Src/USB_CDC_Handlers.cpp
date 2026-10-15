
#include "USB_CDC_Handlers.h"
#include "usb_cdc.h"
#include "di.h"
#include "usb_cdc_common.h"

#include "etl/string.h"
#include "etl/string_view.h"
#include "etl/string_utilities.h"
#include "etl/to_string.h"
#include "etl/format_spec.h"

#include <stdint.h>

void GET_FREQ(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd);
void GET_DUTY(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd);
void GET_CURRENT(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd);
void GET_PIN_STATE(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd);
void GET_PULSE(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd);
void SET_CONFIG_MEASURE_PULSE(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd);
void SET_CONFIG_MEASURE_FREQ(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd);
void GET_CONFIG_MEASURE(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd);
void HELP(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd);

etl::string<MAX_SIZE_USB_CDC_MESSAGE> response_usb_cdc;

using HandlerFuncUsbCdc = void (*)(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd);

struct CommandHandlerUsbCdc
{
    const char *id;
    HandlerFuncUsbCdc handler;
};

CommandHandlerUsbCdc UsbCdcHandlers[NUMBER_OF_USB_CDC_HANDLERS] = {
    {"get_freq", GET_FREQ},
    {"get_duty", GET_DUTY},
    {"get_current", GET_CURRENT},
    {"get_pin_state", GET_PIN_STATE},
    {"get_pulse", GET_PULSE},
    {"set_config_measure_pulse", SET_CONFIG_MEASURE_PULSE},
    {"set_config_measure_freq", SET_CONFIG_MEASURE_FREQ},
    {"get_config_measure", GET_CONFIG_MEASURE},
    {"help", HELP}};

void GET_FREQ(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd)
{
    UsbCdcMessage msg = {};
    uint16_t freq = DI::get_freq();

    etl::to_string(freq, response_usb_cdc, true);
    response_usb_cdc += " Hz\n";

    msg.size = response_usb_cdc.size();
    memcpy(msg.data, response_usb_cdc.c_str(), msg.size);
    Usb_Cdc::send(&msg);
    response_usb_cdc.clear();

    return;
}

void GET_DUTY(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd)
{
    UsbCdcMessage msg = {};
    uint16_t duty = DI::get_duty();

    etl::to_string(duty, response_usb_cdc, true);
    response_usb_cdc += " procent\n";

    msg.size = response_usb_cdc.size();
    memcpy(msg.data, response_usb_cdc.c_str(), msg.size);
    Usb_Cdc::send(&msg);
    response_usb_cdc.clear();

    return;
}

void GET_CURRENT(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd)
{
    UsbCdcMessage msg = {};
    int16_t current = DI::get_current_mA();

    etl::to_string(current, response_usb_cdc, true);
    response_usb_cdc += " mA\n";

    msg.size = response_usb_cdc.size();
    memcpy(msg.data, response_usb_cdc.c_str(), msg.size);
    Usb_Cdc::send(&msg);
    response_usb_cdc.clear();

    return;
}

void GET_PIN_STATE(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd)
{
    UsbCdcMessage msg = {};
    bool state = DI::get_pin_state();

    if (state)
    {
        response_usb_cdc += "HIGH\n";
    }
    else
    {
        response_usb_cdc += "LOW\n";
    }

    msg.size = response_usb_cdc.size();
    memcpy(msg.data, response_usb_cdc.c_str(), msg.size);
    Usb_Cdc::send(&msg);
    response_usb_cdc.clear();
    return;
}

void GET_PULSE(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd)
{
    UsbCdcMessage msg = {};
    uint16_t pulse_ms = DI::get_pulse_ms();
    uint16_t pulse_us = DI::get_pulse_us();

    etl::to_string(pulse_ms, response_usb_cdc, true);
    response_usb_cdc += " ms\n";

    etl::to_string(pulse_us, response_usb_cdc, true);
    response_usb_cdc += " us\n";

    msg.size = response_usb_cdc.size();
    memcpy(msg.data, response_usb_cdc.c_str(), msg.size);
    Usb_Cdc::send(&msg);
    response_usb_cdc.clear();

    return;
}

// enum class ParseResult
//{
//     Ok,
//     InvalidCommand,
//     InvalidArgument,
//     WrongArgumentCount,
//     Overflow
// };
//
// ParseResult inline parse_set_voltage(
//     etl::string_view message,
//     SetVoltageCommand &command)
//{
//     // Ищем пробелы и выделяем 3 токенов:
//     //
//     // mid_point_mV
//     // channel
//
//     static constexpr uint8_t number_of_tokens = 2U;
//
//     etl::string_view tokens[number_of_tokens];
//
//     size_t token_count = 0;
//     size_t start = 0;
//
//     for (size_t i = 0; i <= message.size(); ++i)
//     {
//         if (i == message.size() || message[i] == ' ' || message[i] == '\n')
//         {
//             if (i > start)
//             {
//                 if (token_count >= number_of_tokens)
//                     return ParseResult::WrongArgumentCount;
//
//                 tokens[token_count++] =
//                     message.substr(start, i - start);
//             }
//
//             start = i + 1;
//         }
//     }
//
//     if (token_count != number_of_tokens)
//         return ParseResult::WrongArgumentCount;
//
//     // Проверяем имя команды
//     if (tokens[0] != "set_voltage")
//         return ParseResult::InvalidCommand;
//
//     if (!parse_uint32(tokens[1], command.mid_point_mV))
//         return ParseResult::InvalidArgument;
//
//     if (!parse_uint32(tokens[2], command.channel))
//         return ParseResult::InvalidArgument;
//
//     return ParseResult::Ok;
// }

void SET_CONFIG_MEASURE_PULSE(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd)
{
    DI::config_measure_pulse();
    return;
}

void SET_CONFIG_MEASURE_FREQ(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd)
{
    DI::config_measure_freq();
    return;
}

void GET_CONFIG_MEASURE(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd)
{
    UsbCdcMessage msg = {};
    DI::ConfigTypeMeasure type_measure = DI::get_type_measure();

    if (type_measure == DI::ConfigTypeMeasure::FREQ_MEASURE)
    {
        response_usb_cdc += "FREQ_MEASURE\n";

        msg.size = response_usb_cdc.size();
        memcpy(msg.data, response_usb_cdc.c_str(), msg.size);
        Usb_Cdc::send(&msg);
        response_usb_cdc.clear();
        return;
    }
    else if (type_measure == DI::ConfigTypeMeasure::PULSE_MEASURE)
    {
        response_usb_cdc += "PULSE_MEASURE\n";

        msg.size = response_usb_cdc.size();
        memcpy(msg.data, response_usb_cdc.c_str(), msg.size);
        Usb_Cdc::send(&msg);
        response_usb_cdc.clear();
        return;
    }
    return;
}

void HELP(etl::string<MAX_SIZE_USB_CDC_MESSAGE> &cmd)
{
    UsbCdcMessage msg = {};

    for (uint8_t i = 0; i < NUMBER_OF_USB_CDC_HANDLERS - 1U; i++)
    {
        response_usb_cdc += UsbCdcHandlers[i].id;
        response_usb_cdc += "\n";

        msg.size = response_usb_cdc.size();
        memcpy(msg.data, response_usb_cdc.c_str(), msg.size);
        while (Usb_Cdc::send(&msg) != Status::OK)
            ;
        response_usb_cdc.clear();
    }

    return;
}

void UsbCdcCommmandHandler(UsbCdcMessage *usb_cdc_rx_message)
{
    etl::string<MAX_SIZE_USB_CDC_MESSAGE> str(reinterpret_cast<char *>(usb_cdc_rx_message->data), usb_cdc_rx_message->size);

    etl::string_view cmd_name;

    etl::to_lower_case(str);

    for (size_t i = 0; i <= str.size(); ++i)
    {
        if (i == str.size() || str[i] == ' ' || str[i] == '\n')
        {
            cmd_name = str.substr(0, i);
            break;
        }
    }

    for (uint8_t i = 0; i < NUMBER_OF_USB_CDC_HANDLERS; i++)
    {
        if (cmd_name.compare(reinterpret_cast<const char *>(UsbCdcHandlers[i].id)) == 0)
        {
            UsbCdcHandlers[i].handler(str);
            continue;
        }
    }
    return;
}
