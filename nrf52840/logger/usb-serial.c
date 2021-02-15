#include "nrf.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_power.h"
#include "nrf_drv_usbd.h"
#include "app_usbd.h"
#include "app_usbd_core.h"
#include "app_usbd_string_desc.h"
#include "app_usbd_cdc_acm.h"
#include "app_usbd_serial_num.h"



#include "log.h"

#include <stdint.h>
/**---------------------------------------------------------------------------*/
static void cdc_acm_port_ev_handler(app_usbd_class_inst_t const *p_inst, app_usbd_cdc_acm_user_event_t event);
/**---------------------------------------------------------------------------*/
#define CDC_ACM_COMM_INTERFACE  0
#define CDC_ACM_COMM_EPIN       NRF_DRV_USBD_EPIN2

#define CDC_ACM_DATA_INTERFACE  1
#define CDC_ACM_DATA_EPIN       NRF_DRV_USBD_EPIN1
#define CDC_ACM_DATA_EPOUT      NRF_DRV_USBD_EPOUT1



/**
 * @brief CDC_ACM class instance
 * */
APP_USBD_CDC_ACM_GLOBAL_DEF(m_app_cdc_acm,
                            cdc_acm_port_ev_handler,
                            CDC_ACM_COMM_INTERFACE,
                            CDC_ACM_DATA_INTERFACE,
                            CDC_ACM_COMM_EPIN,
                            CDC_ACM_DATA_EPIN,
                            CDC_ACM_DATA_EPOUT,
                            APP_USBD_CDC_COMM_PROTOCOL_AT_V250
);
/**---------------------------------------------------------------------------*/
#define TX_BUFFER_SIZE                1024
#define RX_BUFFER_SIZE                256

static uint8_t usb_rx_data[RX_BUFFER_SIZE];
static uint8_t usb_tx_data[TX_BUFFER_SIZE];

static volatile uint8_t enabled = 0;
static volatile uint16_t buffered_data = 0;
static volatile uint8_t tx_buffer_busy = 0;
/*---------------------------------------------------------------------------*/
/* Callback to the input handler */
static int (*input_handler)(unsigned char c);
/*---------------------------------------------------------------------------*/
static void
cdc_acm_port_ev_handler(app_usbd_class_inst_t const *p_inst, app_usbd_cdc_acm_user_event_t event)
{
    app_usbd_cdc_acm_t const *p_cdc_acm = app_usbd_cdc_acm_class_get(p_inst);

    //__LOG(LOG_SRC_APP, LOG_LEVEL_INFO, "cdc_acm_port_ev_handler: %d\n", event);
    switch(event) {
        case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
        {
            /* Set up first transfer */
            app_usbd_cdc_acm_read_any(&m_app_cdc_acm, usb_rx_data, RX_BUFFER_SIZE);
            enabled = 1;
            //__LOG(LOG_SRC_APP, LOG_LEVEL_INFO, "APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN: %d\n", event);
            break;
        }

        case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
        {
            tx_buffer_busy = 0;
            enabled = 0;
            //__LOG(LOG_SRC_APP, LOG_LEVEL_INFO, "APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE: %d\n", event);
            break;
        }

        case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
        {
            tx_buffer_busy = 0;
            //__LOG(LOG_SRC_APP, LOG_LEVEL_INFO, "APP_USBD_CDC_ACM_USER_EVT_TX_DONE: %d\n", event);
            break;
        }

        case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
        {
            ret_code_t ret;

            do {
                size_t rx_size = app_usbd_cdc_acm_rx_size(p_cdc_acm);

                if(input_handler) {
                    uint8_t i;

                    for(i = 0; i < rx_size; i++) {
                        input_handler(usb_rx_data[i]);
                    }
                }

                /* Fetch up to RX_BUFFER_SIZE bytes from the internal buffer */
                ret = app_usbd_cdc_acm_read_any(&m_app_cdc_acm,
                                                usb_rx_data, RX_BUFFER_SIZE);
            } while(ret == NRF_SUCCESS);

            //__LOG(LOG_SRC_APP, LOG_LEVEL_INFO, "APP_USBD_CDC_ACM_USER_EVT_RX_DONE: %d\n", event);
            break;
        }

        default:
            break;
    }
}
/*---------------------------------------------------------------------------*/
static void
usbd_user_ev_handler(app_usbd_event_type_t event)
{
    //__LOG(LOG_SRC_APP, LOG_LEVEL_INFO, "usbd_user_ev_handler: %d\n", event);
    switch(event) {
        case APP_USBD_EVT_STOPPED:
        {
            tx_buffer_busy = 0;
            enabled = 0;
            app_usbd_disable();
            break;
        }

        case APP_USBD_EVT_POWER_DETECTED:
        {
            if(!nrf_drv_usbd_is_enabled()) {
                app_usbd_enable();
            }
            break;
        }

        case APP_USBD_EVT_POWER_REMOVED:
        {
            tx_buffer_busy = 0;
            enabled = 0;
            app_usbd_stop();
            break;
        }

        case APP_USBD_EVT_POWER_READY:
        {
            app_usbd_start();
            break;
        }

        default:
            //__LOG(LOG_SRC_APP, LOG_LEVEL_INFO, "usbd_user_ev_handler: %d\n", event);
            break;
    }
}
/*---------------------------------------------------------------------------*/
void
usb_serial_init(void)
{
    static const app_usbd_config_t usbd_config = {
            .ev_state_proc = usbd_user_ev_handler
    };

    ret_code_t ret;
    app_usbd_class_inst_t const *class_cdc_acm;

    app_usbd_serial_num_generate();

    ret = app_usbd_init(&usbd_config);
    if(ret != NRF_SUCCESS) {
        //__LOG(LOG_SRC_APP, LOG_LEVEL_INFO, "app_usbd_init error: %d\n", ret);
        return;
    }

    class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&m_app_cdc_acm);

    ret = app_usbd_class_append(class_cdc_acm);
    if(ret != NRF_SUCCESS) {
        //__LOG(LOG_SRC_APP, LOG_LEVEL_INFO, "app_usbd_class_append error: %d\n", ret);
        return;
    }

    ret = app_usbd_power_events_enable();
    if(ret != NRF_SUCCESS) {
        //__LOG(LOG_SRC_APP, LOG_LEVEL_INFO, "app_usbd_power_events_enable error: %d\n", ret);
        return;
    }

    app_usbd_enable();
    app_usbd_start();

    enabled = 0;
    buffered_data = 0;
    //__LOG(LOG_SRC_APP, LOG_LEVEL_INFO, "USB enabled %d\n", ret);

}
/*---------------------------------------------------------------------------*/
void
usb_serial_flush()
{
    if(!enabled || tx_buffer_busy || buffered_data == 0) {
        return;
    }

    ret_code_t ret;

    tx_buffer_busy = 1;
    do {
        ret = app_usbd_cdc_acm_write(&m_app_cdc_acm, usb_tx_data, buffered_data);
        //watchdog_periodic();
    } while(ret != NRF_SUCCESS);

    /* Block until APP_USBD_CDC_ACM_USER_EVT_TX_DONE fires */
    while(tx_buffer_busy) {
        //watchdog_periodic();
    }

    buffered_data = 0;
}
/*---------------------------------------------------------------------------*/
void
usb_serial_writeb(uint8_t b)
{
    if(!enabled) {
        buffered_data = 0;
        return;
    }

    if(buffered_data < TX_BUFFER_SIZE) {
        usb_tx_data[buffered_data] = b;
        buffered_data++;
        if(buffered_data >= TX_BUFFER_SIZE) {
            usb_serial_flush();
            buffered_data = 0;
        }
        usb_serial_flush();
    }
}
/*---------------------------------------------------------------------------*/
void
usb_serial_set_input(int (*input)(unsigned char c))
{
    input_handler = input;
}
/*---------------------------------------------------------------------------*/

/** @} */
