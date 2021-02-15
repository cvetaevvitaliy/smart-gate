#include "stdio.h"
#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"

#include "logger.h"

#include "log.h"
#include "nrf_drv_clock.h"

/** If you need Jink RTT logger please defined LOGGER_RTT in CMake */
#if  defined(LOGGER_RTT)
#define LOGGER_RTT
#else
#include "tinyprintf.h"
#define LOGGER_USB_UART
#endif

void log_callback_usb_uart(uint32_t dbg_level, const char * p_filename, uint16_t line,
                         uint32_t timestamp, const char * format, va_list arguments)
{
    printf("[%s]: %s:%u: ", log_level_to_str(dbg_level), p_filename, line);
    (void) vprintf(format, arguments);
}

int main(void)
{
    nrf_drv_clock_init();

#ifdef LOGGER_USB_UART
    logger_init();
    log_init(LOG_SRC_APP | LOG_SRC_ACCESS, LOG_LEVEL_INFO, log_callback_usb_uart);
    nrf_delay_ms(1500); // wait for init PC usb host
#else
    log_init(LOG_SRC_APP | LOG_SRC_ACCESS, LOG_LEVEL_INFO, LOG_CALLBACK_DEFAULT);
#endif

    bsp_board_init(BSP_INIT_LEDS);

    __LOG(LOG_SRC_APP, LOG_LEVEL_INFO, "App init\n");

    uint32_t tmp = 0;
    while (1)
    {
        for (int i = 0; i < LEDS_NUMBER; i++)
        {
            bsp_board_led_invert(i);
            nrf_delay_ms(500);
            tmp++;
        }
    }
}

