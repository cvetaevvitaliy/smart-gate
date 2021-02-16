#include "ws2812.h"
#include "spi.h"
#include "log.h"
#include "nrfx_spim.h"
#include "nrf_delay.h"

uint8_t pixelBuffer[PIXEL_NUM][24] = {0};
nrfx_spim_xfer_desc_t xfer = NRFX_SPIM_XFER_TX(pixelBuffer, sizeof(pixelBuffer));

extern const nrfx_spim_t spi;

void ws281x_init(void)
{
    spi_init();

    nrf_delay_ms(100);
    ws281x_clear_all();
    nrf_delay_ms(200);
}

void ws281x_clear_all(void)
{
    uint16_t i;
    uint8_t j;

    for (i = 0; i < PIXEL_NUM; ++i)
    {
        for (j = 0; j < 24; ++j)
        {
            pixelBuffer[i][j] = WS_LOW;
        }
    }
    ws281x_show();
}

uint32_t ws281x_get_color(uint8_t red, uint8_t green, uint8_t blue)
{
    return green << 16 | red << 8 | blue;
}

void ws281x_set_pixel_color(uint16_t n ,uint32_t GRBcolor)
{
    uint8_t i = 0;
    if (n < PIXEL_NUM)
    {
        for (i = 0; i < 24; ++i)
        {
            pixelBuffer[n][i] = (((GRBcolor << i) & 0X800000) ? WS_HIGH : WS_LOW);
        }
    }
}

void ws281x_set_pixel_color_RGB(uint16_t n ,uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t i;

    if (n < PIXEL_NUM)
    {
        for (i = 0; i < 24; ++i)
        {
            pixelBuffer[n][i] = (((ws281x_get_color(red, green, blue) << i) & 0X800000) ? WS_HIGH : WS_LOW);
        }
    }
}


void  ws28xx_set_color_array_and_other_off(uint16_t PixelStart,uint16_t PixelEnd, uint32_t GRBcolor)
{

    for (uint16_t i = 0; i < PixelStart; i++)
        ws281x_set_pixel_color(i, ws28xx_Black);

    for (uint16_t i = PixelStart; i <= PixelEnd; i++)
        ws281x_set_pixel_color(i, GRBcolor);

    for (uint16_t i = PixelEnd + 1; i < PIXEL_NUM; i++)
        ws281x_set_pixel_color(i, ws28xx_Black);

}

void  ws28xx_set_color_array(uint16_t PixelStart,uint16_t PixelEnd, uint32_t GRBcolor)
{
    for (uint16_t i = PixelStart; i <= PixelEnd; i++)
        ws281x_set_pixel_color(i, GRBcolor);
}

void ws281x_show(void)
{
    nrfx_spim_xfer(&spi, &xfer, 0);
    //__LOG(LOG_SRC_APP, LOG_LEVEL_INFO, "ws281x updated : %s\n", ret > 0 ? "ERROR" : "OK");
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t ws281x_wheel(uint8_t wheelPos)
{
    wheelPos = 255 - wheelPos;
    if (wheelPos < 85)
    {
        return ws281x_get_color(255 - wheelPos * 3, 0, wheelPos * 3);
    }
    if (wheelPos < 170)
    {
        wheelPos -= 85;
        return ws281x_get_color(0, wheelPos * 3, 255 - wheelPos * 3);
    }
    wheelPos -= 170;
    return ws281x_get_color(wheelPos * 3, 255 - wheelPos * 3, 0);
}

// Fill the dots one after the other with a color
void ws281x_color_fill(uint32_t GRBcolor, uint32_t wait)
{
    for (uint16_t i = 0; i < PIXEL_NUM; i++)
    {
        ws281x_set_pixel_color(i, GRBcolor);
        ws281x_show();
        nrf_delay_ms(wait);
    }
}

void ws281x_rainbow(uint32_t wait)
{
    uint16_t i, j;

    for (j = 0; j < 256; j++)
    {
        for (i = 0; i < PIXEL_NUM; i++)
        {
            ws281x_set_pixel_color(i, ws281x_wheel((i + j) & 255));
        }
        ws281x_show();
        nrf_delay_ms(wait);
    }
}

// Slightly different, this makes the rainbow equally distributed throughout
void ws281x_rainbow_cycle(uint32_t wait)
{
    uint16_t i, j;

    for (j = 0; j < 256 * 5; j++)
    {
        // 5 cycles of all colors on wheel
        for (i = 0; i < PIXEL_NUM; i++)
        {
            ws281x_set_pixel_color(i, ws281x_wheel(((i * 256 / PIXEL_NUM) + j) & 255));
        }
        ws281x_show();
        nrf_delay_ms(wait);
    }
}

//Theatre-style crawling lights.
void ws281x_theater_chase(uint32_t GRBcolor, uint32_t wait)
{
    for (int j = 0; j < 10; j++)
    {
        //do 10 cycles of chasing
        for (int q = 0; q < 3; q++)
        {
            for (uint16_t i = 0; i < PIXEL_NUM; i = i + 3)
            {
                ws281x_set_pixel_color(i + q, GRBcolor);    //turn every third pixel on
            }
            ws281x_show();

            nrf_delay_ms(wait);

            for (uint16_t i = 0; i < PIXEL_NUM; i = i + 3)
            {
                ws281x_set_pixel_color(i + q, 0);        //turn every third pixel off
            }
        }
    }
}

//Theatre-style crawling lights with rainbow effect
void ws281x_theater_chase_rainbow(uint32_t wait)
{
    for (int j = 0; j < 256; j++)
    {
        // cycle all 256 colors in the wheel
        for (int q = 0; q < 3; q++)
        {
            for (uint16_t i = 0; i < PIXEL_NUM; i = i + 3)
            {
                ws281x_set_pixel_color(i + q, ws281x_wheel((i + j) % 255));    //turn every third pixel on
            }

            ws281x_show();

            nrf_delay_ms(wait);

            for (uint16_t i = 0; i < PIXEL_NUM; i = i + 3)
            {
                ws281x_set_pixel_color(i + q, 0);        //turn every third pixel off
            }
        }
    }
}
