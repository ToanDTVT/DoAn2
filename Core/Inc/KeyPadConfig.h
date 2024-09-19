#ifndef	_KEYPADCONFIG_H
#define	_KEYPADCONFIG_H
//#include "gpio.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

#define           _KEYPAD_DEBOUNCE_TIME_MS        20
#define           _KEYPAD_USE_FREERTOS            0

#define KEYPAD_ROW1_PIN    GPIO_PIN_13
#define KEYPAD_ROW2_PIN    GPIO_PIN_10
#define KEYPAD_ROW3_PIN    GPIO_PIN_9
#define KEYPAD_ROW4_PIN    GPIO_PIN_15

#define KEYPAD_COL1_PIN    GPIO_PIN_14
#define KEYPAD_COL2_PIN    GPIO_PIN_12
#define KEYPAD_COL3_PIN    GPIO_PIN_8



#endif
