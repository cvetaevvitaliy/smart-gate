#include "logger.h"
#include "boards.h"

#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "tinyprintf.h"
#include <usb-serial.h>


void logger_init(void)
{
    usb_serial_init();
}


char* log_level_to_str(uint8_t lefel)
{
    static char str[10] = {0};
// LOG_LEVEL_ASSERT ( 0) /**< Log level for assertions */
// LOG_LEVEL_ERROR  ( 1) /**< Log level for error messages. */
// LOG_LEVEL_WARN   ( 2) /**< Log level for warning messages. */
// LOG_LEVEL_REPORT ( 3) /**< Log level for report messages. */
// LOG_LEVEL_INFO   ( 4) /**< Log level for information messages. */
    switch (lefel)
    {
        case 0:
            sprintf(str, "ASSERT");
            break;
        case 1:
            sprintf(str, "ERROR");
            break;
        case 2:
            sprintf(str, "WARN");
            break;
        case 3:
            sprintf(str, "REPORT");
            break;
        case 4:
            sprintf(str, "INFO");
            break;
        default:
            sprintf(str, "-----");
            break;
    }

    return str;
}

