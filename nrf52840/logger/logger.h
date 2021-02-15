#ifndef __LOGGER_H__
#define __LOGGER_H__
#include <stdint.h>

void logger_init(void);
char* log_level_to_str(uint8_t lefel);

#endif //__LOGGER_H__
