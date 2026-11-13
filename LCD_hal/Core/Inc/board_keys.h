#ifndef BOARD_KEYS_H
#define BOARD_KEYS_H

#include <stdint.h>

#define BOARD_KEY1 0x01U
#define BOARD_KEY2 0x02U
#define BOARD_KEY3 0x04U
#define BOARD_KEY4 0x08U

void BoardKeys_Init(void);
/* Call every main-loop iteration, including between game frames. */
void BoardKeys_Task(void);
uint8_t BoardKeys_Held(void);
/* Latched press edges survive until the game consumes them. */
uint8_t BoardKeys_TakePressed(void);

#endif
