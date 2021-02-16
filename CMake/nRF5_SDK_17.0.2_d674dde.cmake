add_definitions(-DNRFX_SPIM_ENABLED=1)

set(SRC_UART
        "${SDK_ROOT}/components/libraries/fifo/app_fifo.c"
        "${SDK_ROOT}/components/libraries/uart/app_uart_fifo.c"
        "${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_uart.c"
        "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_uarte.c"
        #"${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_uart.c"
        )

set(INC_UART
        "${SDK_ROOT}/modules/nrfx/hal"
        "${SDK_ROOT}/integration/nrfx/legacy"
        "${SDK_ROOT}/modules/nrfx"
        "${SDK_ROOT}/components/libraries/uart"
        "${SDK_ROOT}/modules/nrfx/drivers/include"
        "${SDK_ROOT}/components/libraries/fifo")


set(SRC_SPIM
        "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_spim.c")

set(INC_SPIM
        "${SDK_ROOT}/modules/nrfx/drivers/include")

set(SRC_USB
        "${SDK_ROOT}/components/libraries/usbd/app_usbd.c"
        "${SDK_ROOT}/components/libraries/usbd/class/cdc/acm/app_usbd_cdc_acm.c"
        "${SDK_ROOT}/components/libraries/usbd/app_usbd_core.c"
        "${SDK_ROOT}/components/libraries/usbd/app_usbd_serial_num.c"
        "${SDK_ROOT}/components/libraries/usbd/app_usbd_string_desc.c"
        "${SDK_ROOT}/components/libraries/usbd/class/cdc/acm/app_usbd_cdc_acm.c"
        "${SDK_ROOT}/components/libraries/atomic/nrf_atomic.c"
        "${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_power.c"
        "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_usbd.c"
        "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_power.c"
        "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_systick.c"
        "${SDK_ROOT}/modules/nrfx/soc/nrfx_atomic.c"
        "${SDK_ROOT}/components/libraries/atomic_fifo/nrf_atfifo.c"
        )

set(INC_USB
        "${SDK_ROOT}/components/libraries/usbd"
        "${SDK_ROOT}/components/libraries/usbd/class/cdc"
        "${SDK_ROOT}/components/libraries/usbd/class/cdc/acm"
        "${SDK_ROOT}/components/libraries/atomic"
        "${SDK_ROOT}/external/utf_converter"
        "${SDK_ROOT}/components/libraries/atomic_fifo"
        "${SDK_ROOT}/integration/nrfx/legacy"
        "${SDK_ROOT}/modules/nrfx/drivers/include"
        )


set(nRF5_SDK_17.0.2_d674dde_SOURCE_FILES
        "${SRC_UART}"
        "${SRC_USB}"
        "${SRC_SPIM}"
        "${SDK_ROOT}/modules/nrfx/mdk/gcc_startup_nrf52840.S"
        "${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_clock.c"
        "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_clock.c"
        "${SDK_ROOT}/components/libraries/util/app_error.c"
        "${SDK_ROOT}/components/libraries/util/app_util_platform.c"
        "${SDK_ROOT}/components/libraries/log/src/nrf_log_frontend.c"
        "${SDK_ROOT}/components/libraries/memobj/nrf_memobj.c"
        "${SDK_ROOT}/components/libraries/balloc/nrf_balloc.c"
        "${SDK_ROOT}/external/fprintf/nrf_fprintf.c"
        "${SDK_ROOT}/external/fprintf/nrf_fprintf_format.c"
        "${SDK_ROOT}/components/libraries/ringbuf/nrf_ringbuf.c"
        "${SDK_ROOT}/components/libraries/log/src/nrf_log_backend_rtt.c"
        "${SDK_ROOT}/components/libraries/log/src/nrf_log_backend_serial.c"
        "${SDK_ROOT}/components/libraries/log/src/nrf_log_default_backends.c"
        "${SDK_ROOT}/components/libraries/log/src/nrf_log_backend_uart.c"
        "${SDK_ROOT}/components/libraries/log/src/nrf_log_str_formatter.c"
        "${SDK_ROOT}/components/libraries/strerror/nrf_strerror.c"

        "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_systick.c"
        "${SDK_ROOT}/components/softdevice/common/nrf_sdh_soc.c"
        "${SDK_ROOT}/components/libraries/queue/nrf_queue.c"
        "${SDK_ROOT}/components/libraries/util/app_util_platform.c"
        "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_gpiote.c"
        "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_rng.c"
        "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_wdt.c"

        )

set(nRF5_SDK_17.0.2_d674dde_INCLUDE_DIRS
        "${INC_UART}"
        "${INC_USB}"
        "${INC_USB}"
        "${SDK_ROOT}/integration/nrfx"
        "${SDK_ROOT}/components/libraries/util"
        "${SDK_ROOT}/components/libraries/timer"
        "${SDK_ROOT}/components/libraries/log"
        "${SDK_ROOT}/components/libraries/log/src"
        "${SDK_ROOT}/components/libraries/experimental_section_vars"
        "${SDK_ROOT}/components/libraries/delay"
        "${SDK_ROOT}/modules/nrfx"
        "${SDK_ROOT}/modules/nrfx/drivers/include"
        "${SDK_ROOT}/modules/nrfx/drivers"
        "${SDK_ROOT}/modules/nrfx/hal"
        "${SDK_ROOT}/modules/nrfx/mdk"
        "${SDK_ROOT}/components/libraries/uart"
        "${SDK_ROOT}/components/libraries/bsp"
        "${SDK_ROOT}/components/libraries/button"
        "${SDK_ROOT}/components/libraries/log/src"
        "${SDK_ROOT}/components/libraries/memobj"
        "${SDK_ROOT}/components/libraries/balloc"
        "${SDK_ROOT}/external/fprintf"
        "${SDK_ROOT}/components/libraries/ringbuf"
        "${SDK_ROOT}//components/libraries/atomic"
        "${SDK_ROOT}/modules/nrfx/drivers/include"
        "${SDK_ROOT}/components/libraries/fifo"
        "${SDK_ROOT}/components/libraries/util"
        "${SDK_ROOT}/components/libraries/queue"
        )

if (TOOLCHAIN STREQUAL "armcc")
    set(nRF5_SDK_17.0.2_d674dde_SOURCE_FILES
        ${nRF5_SDK_17.0.2_d674dde_SOURCE_FILES}
        "${SDK_ROOT}/components/libraries/util/app_error_handler_keil.c")
else ()
    set(nRF5_SDK_17.0.2_d674dde_SOURCE_FILES
        ${nRF5_SDK_17.0.2_d674dde_SOURCE_FILES}
        "${SDK_ROOT}/components/libraries/util/app_error_handler_gcc.c")
endif ()
