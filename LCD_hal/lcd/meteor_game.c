#include "meteor_game.h"
#include <string.h>

static uint32_t Game_Random(MeteorGame *game)
{
    uint32_t value = game->random;
    value ^= value << 13;
    value ^= value >> 17;
    value ^= value << 5;
    game->random = value;
    return value;
}

static uint8_t Game_Overlaps(int16_t ax, int16_t ay, int16_t aw, int16_t ah,
                             int16_t bx, int16_t by, int16_t bw, int16_t bh)
{
    return (uint8_t)(ax < bx + bw && ax + aw > bx &&
                      ay < by + bh && ay + ah > by);
}

static void Game_AddScore(MeteorGame *game, uint32_t amount)
{
    if (game->score < 999999U - amount) game->score += amount;
    else game->score = 999999U;
    if (game->score > game->best) game->best = game->score;
    game->level = (uint8_t)(game->score / 100U + 1U);
    if (game->score >= 800U) game->level = 9U;
}

static void Game_Spark(MeteorGame *game, int16_t x, int16_t y)
{
    uint8_t i;
    for (i = 0U; i < METEOR_SPARKS; ++i)
    {
        if (game->sparks[i].life == 0U)
        {
            game->sparks[i].x = x;
            game->sparks[i].y = y;
            game->sparks[i].life = 6U;
            return;
        }
    }
}

static void Game_Start(MeteorGame *game)
{
    uint16_t width = game->width;
    uint16_t height = game->height;
    uint32_t best = game->best;
    uint32_t random = game->random;

    memset(game, 0, sizeof(*game));
    game->width = width;
    game->height = height;
    game->best = best;
    game->random = random;
    game->ship_x = (int16_t)((width - METEOR_SHIP_SIZE) / 2U);
    game->ship_y = (int16_t)(height - 88U);
    game->lives = 3U;
    game->level = 1U;
    game->spawn_cooldown = 15U;
    game->state = METEOR_PLAYING;
}

void MeteorGame_Init(MeteorGame *game, uint16_t width, uint16_t height, uint32_t seed)
{
    memset(game, 0, sizeof(*game));
    game->width = width;
    game->height = height;
    game->random = seed != 0U ? seed : 0x4D455445U;
    game->state = METEOR_TITLE;
    game->lives = 3U;
    game->level = 1U;
}

void MeteorGame_Step(MeteorGame *game, uint8_t held, uint8_t pressed)
{
    uint8_t i;
    uint8_t j;
    MeteorObject *rock;
    MeteorObject *shot;
    int16_t old_y;

    if ((pressed & METEOR_ACTION) != 0U)
    {
        if (game->state == METEOR_TITLE || game->state == METEOR_GAME_OVER)
            Game_Start(game);
        else if (game->state == METEOR_PLAYING)
            game->state = METEOR_PAUSED;
        else
            game->state = METEOR_PLAYING;
        return;
    }
    if (game->state == METEOR_TITLE)
    {
        /* Time spent on the start screen changes the next random sequence. */
        Game_Random(game);
        return;
    }
    if (game->state != METEOR_PLAYING) return;

    ++game->ticks;
    if (game->ticks % 25U == 0U) Game_AddScore(game, 1U);
    if (game->invulnerable != 0U) --game->invulnerable;
    if (game->fire_cooldown != 0U) --game->fire_cooldown;
    for (i = 0U; i < METEOR_SPARKS; ++i)
        if (game->sparks[i].life != 0U) --game->sparks[i].life;

    if ((held & (METEOR_LEFT | METEOR_RIGHT)) == METEOR_LEFT) game->ship_x -= 6;
    if ((held & (METEOR_LEFT | METEOR_RIGHT)) == METEOR_RIGHT) game->ship_x += 6;
    if (game->ship_x < 8) game->ship_x = 8;
    if (game->ship_x > (int16_t)game->width - METEOR_SHIP_SIZE - 8)
        game->ship_x = (int16_t)game->width - METEOR_SHIP_SIZE - 8;

    if (((held | pressed) & METEOR_FIRE) != 0U && game->fire_cooldown == 0U)
    {
        for (i = 0U; i < METEOR_SHOTS; ++i)
        {
            shot = &game->shots[i];
            if (shot->active == 0U)
            {
                shot->x = game->ship_x + 14;
                shot->y = game->ship_y - 8;
                shot->active = 1U;
                game->fire_cooldown = 4U;
                break;
            }
        }
    }

    if (game->spawn_cooldown != 0U) --game->spawn_cooldown;
    if (game->spawn_cooldown == 0U)
    {
        for (i = 0U; i < METEOR_ROCKS; ++i)
        {
            rock = &game->rocks[i];
            if (rock->active == 0U)
            {
                rock->active = 1U;
                rock->x = (int16_t)(8U + Game_Random(game) %
                                     (game->width - METEOR_ROCK_SIZE - 16U));
                rock->y = METEOR_TOP;
                rock->speed = (uint8_t)(2U + (game->level - 1U) / 2U + Game_Random(game) % 2U);
                break;
            }
        }
        game->spawn_cooldown = (uint8_t)(22U - game->level * 2U);
    }

    for (i = 0U; i < METEOR_ROCKS; ++i)
    {
        rock = &game->rocks[i];
        if (rock->active == 0U) continue;
        rock->y += rock->speed;
        if (rock->y >= (int16_t)game->height - METEOR_BOTTOM_MARGIN)
            rock->active = 0U;
    }
    for (i = 0U; i < METEOR_SHOTS; ++i)
    {
        shot = &game->shots[i];
        if (shot->active == 0U) continue;
        old_y = shot->y;
        shot->y -= 12;
        for (j = 0U; j < METEOR_ROCKS; ++j)
        {
            rock = &game->rocks[j];
            /* Sweep the bullet's old/new positions to avoid tunneling. */
            if (rock->active != 0U &&
                Game_Overlaps(shot->x, shot->y, 4, old_y + 10 - shot->y,
                              rock->x + 2, rock->y + 2, 20, 20))
            {
                shot->active = 0U;
                rock->active = 0U;
                Game_AddScore(game, 10U);
                Game_Spark(game, rock->x + 12, rock->y + 12);
                break;
            }
        }
        if (shot->y + 10 <= METEOR_TOP) shot->active = 0U;
    }
    for (i = 0U; i < METEOR_ROCKS; ++i)
    {
        rock = &game->rocks[i];
        if (rock->active != 0U &&
            Game_Overlaps(game->ship_x + 6, game->ship_y + 4, 20, 24,
                          rock->x + 3, rock->y + 3, 18, 18))
        {
            rock->active = 0U;
            Game_Spark(game, rock->x + 12, rock->y + 12);
            if (game->invulnerable == 0U)
            {
                --game->lives;
                game->invulnerable = 30U;
                if (game->lives == 0U)
                {
                    game->state = METEOR_GAME_OVER;
                    break;
                }
            }
        }
    }
}
