#include "spi.h"
#include "nrfx_spim.h"
#include <log.h>
#include "boards.h"

#define SPI_INSTANCE                        NRFX_SPIM1_ENABLED
const nrfx_spim_t spi = NRFX_SPIM_INSTANCE(SPI_INSTANCE);

void spi_init(void)
{

    nrfx_spim_config_t spi_config = NRFX_SPIM_DEFAULT_CONFIG;

    spi_config.ss_pin   = ARDUINO_4_PIN;
    spi_config.miso_pin = ARDUINO_5_PIN;
    spi_config.mosi_pin = ARDUINO_6_PIN;
    spi_config.sck_pin  = ARDUINO_7_PIN;
    spi_config.irq_priority = APP_IRQ_PRIORITY_HIGH,
    spi_config.orc = 0xff,
    spi_config.mode = NRF_SPIM_MODE_3;
    spi_config.frequency = NRF_SPIM_FREQ_8M;

    nrfx_err_t ret = nrfx_spim_init(&spi, &spi_config, NULL, NULL);
    __LOG(LOG_SRC_APP, LOG_LEVEL_INFO, "SPI init: %d\n", ret);



}
