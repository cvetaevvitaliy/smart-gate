#ifndef __WS2812_H
#define __WS2812_H
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


#define PIXEL_NUM 8

//3*143 = 429ns   5*143 = 715ns  for WS281X
//  _____
// |     |___|   11111000  high level
//  ___
// |   |_____|   11100000  low level


// !!! Format: GRB (not RGB) green << 16 | red << 8 | blue;
typedef enum {
        ws28xx_Black      = 0,
        ws28xx_Red        = 0x00FF00,
        ws28xx_Green      = 0xFF0000,
        ws28xx_Blue       = 0x0000FF,
        ws28xx_Yellow     = 0x9FFF00,
        ws28xx_Aqua       = 0xFF00FF,
        ws28xx_Magenta    = 0x00FFFF,
        ws28xx_DarkGreen  = 0x3F0000,
        ws28xx_DarkRed    = 0x003F00,
        ws28xx_DarkBlue   = 0x00003F,
        ws28xx_Olive      = 0x3F3F00,
        ws28xx_Purple     = 0x003F3F,
        ws28xx_Teal       = 0x3F003F,
        ws28xx_Silver     = 0x3F3F3F,
        ws28xx_Gray       = 0x3F3F3F,
        ws28xx_Brown      = 0x258F25,
        ws28xx_Orange     = 0x051200,
        ws28xx_Gold       = 0xCFFF00,
        ws28xx_Pink       = 0x20FF90,
        ws28xx_White      = 0xFFFFFF
} ws28xx_Color_t;

#define WS_HIGH 0XF8
#define WS_LOW  0XE0

void ws281x_init(void);
void ws281x_clear_all(void);
uint32_t ws281x_get_color(uint8_t red, uint8_t green, uint8_t blue);
void ws281x_set_pixel_color(uint16_t n ,uint32_t GRBcolor);
void ws281x_set_pixel_color_RGB(uint16_t n ,uint8_t red, uint8_t green, uint8_t blue);
void  ws28xx_set_color_array_and_other_off(uint16_t PixelStart,uint16_t PixelEnd, uint32_t GRBcolor);
void  ws28xx_set_color_array(uint16_t PixelStart,uint16_t PixelEnd, uint32_t GRBcolor);
void ws281x_show(void);

void ws281x_rainbow_cycle(uint32_t wait);
void ws281x_theater_chase(uint32_t c, uint32_t wait);
void ws281x_color_fill(uint32_t GRBcolor, uint32_t wait);
void ws281x_rainbow(uint32_t wait);
void ws281x_theater_chase_rainbow(uint32_t wait);

#endif /* __WS2812_H */
