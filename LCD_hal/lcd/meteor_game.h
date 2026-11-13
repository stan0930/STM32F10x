#ifndef METEOR_GAME_H
#define METEOR_GAME_H

#include <stdint.h>

#define METEOR_FRAME_MS 40U
#define METEOR_LEFT 0x01U
#define METEOR_RIGHT 0x02U
#define METEOR_FIRE 0x04U
#define METEOR_ACTION 0x08U
#define METEOR_ROCKS 8U
#define METEOR_SHOTS 8U
#define METEOR_SPARKS 6U
#define METEOR_SHIP_SIZE 32
#define METEOR_ROCK_SIZE 24
#define METEOR_TOP 52
#define METEOR_BOTTOM_MARGIN 40

typedef enum
{
    METEOR_TITLE,
    METEOR_PLAYING,
    METEOR_PAUSED,
    METEOR_GAME_OVER
} MeteorState;

typedef struct
{
    int16_t x;
    int16_t y;
    uint8_t active;
    uint8_t speed;
} MeteorObject;

typedef struct
{
    int16_t x;
    int16_t y;
    uint8_t life;
} MeteorSpark;

typedef struct
{
    MeteorState state;
    uint16_t width;
    uint16_t height;
    int16_t ship_x;
    int16_t ship_y;
    uint32_t score;
    uint32_t best;
    uint32_t random;
    uint32_t ticks;
    uint8_t lives;
    uint8_t level;
    uint8_t invulnerable;
    uint8_t fire_cooldown;
    uint8_t spawn_cooldown;
    MeteorObject rocks[METEOR_ROCKS];
    MeteorObject shots[METEOR_SHOTS];
    MeteorSpark sparks[METEOR_SPARKS];
} MeteorGame;

void MeteorGame_Init(MeteorGame *game, uint16_t width, uint16_t height, uint32_t seed);
/* Fixed 40 ms simulation step. Held inputs move/fire; ACTION uses press edges. */
void MeteorGame_Step(MeteorGame *game, uint8_t held, uint8_t pressed);

#endif
