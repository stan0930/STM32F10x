#include "lcd_game.h"
#include "meteor_game.h"
#include "board_keys.h"
#include "lcd.h"
#include <string.h>

#define GAME_BG 0x0884U
#define GAME_TEAL 0x6EDCU
#define GAME_GOLD 0xFD48U
#define GAME_MUTED 0x7C94U

static MeteorGame game;
static MeteorGame previous;
static uint32_t last_frame;
static uint8_t game_ready;

/* Original code-drawn sprites: '.' is transparent, digits index the palette. */
static const char ship_pixels[] =
    ".......22......."
    "......2112......"
    "......2112......"
    ".....211112....."
    ".....213312....."
    "....21133112...."
    "....21133112...."
    "...2211111122..."
    "..211211112112.."
    ".21112111121112."
    ".21112111121112."
    "2222221111222222"
    "....22111122...."
    ".....244442....."
    ".....455554....."
    "......4554......";

static const char rock_pixels[] =
    "....2222...."
    "..22111122.."
    ".2111111112."
    ".2113311112."
    "211333111112"
    "211331111112"
    "211111113312"
    "211111133312"
    ".2111113312."
    ".2111111112."
    "..22111122.."
    "....2222....";

static const uint16_t ship_palette[] =
    {GAME_BG, 0xEFBFU, 0x3497U, 0x6F7EU, 0xFC66U, 0xFFE9U};
static const uint16_t rock_palette[] =
    {GAME_BG, 0xA46CU, 0x6248U, 0x7328U};

static void Text(uint16_t x, uint16_t y, const char *value, uint16_t color, uint8_t size)
{
    LCD_ShowString(x, y, (const u8 *)value, color, GAME_BG, size, 0U);
}

static void CenterText(uint16_t y, const char *value, uint16_t color, uint8_t size)
{
    uint16_t width = (uint16_t)(strlen(value) * (size / 2U));
    Text((game.width - width) / 2U, y, value, color, size);
}

static void Number(uint16_t x, uint16_t y, uint32_t value, uint8_t digits, uint16_t color)
{
    char text[7];
    uint8_t i = digits;
    text[digits] = '\0';
    while (i != 0U)
    {
        --i;
        text[i] = (char)('0' + value % 10U);
        value /= 10U;
    }
    Text(x, y, text, color, 16U);
}

/* Clip every moving object to the playfield; LCD_Fill uses exclusive ends. */
static void PlayFill(int16_t x, int16_t y, int16_t width, int16_t height, uint16_t color)
{
    int16_t right = x + width;
    int16_t bottom = y + height;
    int16_t limit = (int16_t)game.height - METEOR_BOTTOM_MARGIN;
    if (x < 0) x = 0;
    if (y < METEOR_TOP) y = METEOR_TOP;
    if (right > (int16_t)game.width) right = (int16_t)game.width;
    if (bottom > limit) bottom = limit;
    if (right > x && bottom > y)
        LCD_Fill((u16)x, (u16)y, (u16)right, (u16)bottom, color);
}

/* Render same-color horizontal runs, rather than addressing every pixel. */
static void Sprite(int16_t x, int16_t y, const char *pixels,
                   uint8_t width, uint8_t height, const uint16_t *palette,
                   uint8_t flame_phase)
{
    uint8_t row;
    uint8_t column;
    uint8_t start;
    uint8_t index;
    char pixel;
    for (row = 0U; row < height; ++row)
    {
        column = 0U;
        while (column < width)
        {
            pixel = pixels[(uint16_t)row * width + column];
            start = column++;
            while (column < width && pixels[(uint16_t)row * width + column] == pixel)
                ++column;
            if (pixel == '.') continue;
            index = (uint8_t)(pixel - '0');
            if (index == 5U && flame_phase != 0U) index = 4U;
            PlayFill(x + start * 2, y + row * 2, (column - start) * 2, 2, palette[index]);
        }
    }
}

static void Stars(void)
{
    uint16_t i;
    uint16_t x;
    uint16_t y;
    for (i = 0U; i < 32U; ++i)
    {
        x = (uint16_t)((19UL + i * 79UL) % game.width);
        y = (uint16_t)(METEOR_TOP + (i * 53UL + 11UL) %
                        (game.height - METEOR_TOP - METEOR_BOTTOM_MARGIN));
        LCD_DrawPoint(x, y, i % 4U == 0U ? GAME_MUTED : 0x29EBU);
    }
}

static void Hud(void)
{
    Text(8U, 7U, "METEOR GUARD", GAME_TEAL, 16U);
    Text(game.width - 48U, 7U, "LV", GAME_GOLD, 16U);
    Number(game.width - 24U, 7U, game.level, 1U, GAME_GOLD);
    Text(8U, 29U, "SCORE", GAME_MUTED, 16U);
    Number(56U, 29U, game.score, 6U, WHITE);
    Text(game.width - 72U, 29U, "LIFE", GAME_MUTED, 16U);
    Number(game.width - 24U, 29U, game.lives, 1U, GAME_GOLD);
    LCD_Fill(8U, 49U, game.width - 8U, 50U, 0x29EBU);
    CenterText(game.height - 28U, "K1< K2>  K3:FIRE  K4:PAUSE", GAME_MUTED, 16U);
}

static void DrawObjects(void)
{
    uint8_t i;
    int16_t radius;
    const MeteorSpark *spark;
    for (i = 0U; i < METEOR_ROCKS; ++i)
        if (game.rocks[i].active != 0U)
            Sprite(game.rocks[i].x, game.rocks[i].y, rock_pixels, 12U, 12U, rock_palette, 0U);
    for (i = 0U; i < METEOR_SHOTS; ++i)
        if (game.shots[i].active != 0U)
        {
            PlayFill(game.shots[i].x, game.shots[i].y, 4, 10, GAME_TEAL);
            PlayFill(game.shots[i].x + 1, game.shots[i].y, 2, 7, WHITE);
        }
    if (game.invulnerable == 0U || game.invulnerable % 4U < 2U)
        Sprite(game.ship_x, game.ship_y, ship_pixels, 16U, 16U,
               ship_palette, (uint8_t)(game.ticks % 2U));
    for (i = 0U; i < METEOR_SPARKS; ++i)
    {
        spark = &game.sparks[i];
        if (spark->life == 0U) continue;
        radius = (int16_t)((7U - spark->life) * 2U);
        PlayFill(spark->x - radius, spark->y - 1, 3, 3, GAME_GOLD);
        PlayFill(spark->x + radius - 2, spark->y - 1, 3, 3, GAME_GOLD);
        PlayFill(spark->x - 1, spark->y - radius, 3, 3, WHITE);
        PlayFill(spark->x - 1, spark->y + radius - 2, 3, 3, WHITE);
    }
}

static void ErasePrevious(void)
{
    uint8_t i;
    PlayFill(previous.ship_x, previous.ship_y, 32, 32, GAME_BG);
    for (i = 0U; i < METEOR_ROCKS; ++i)
        if (previous.rocks[i].active != 0U)
            PlayFill(previous.rocks[i].x, previous.rocks[i].y, 24, 24, GAME_BG);
    for (i = 0U; i < METEOR_SHOTS; ++i)
        if (previous.shots[i].active != 0U)
            PlayFill(previous.shots[i].x, previous.shots[i].y, 4, 10, GAME_BG);
    for (i = 0U; i < METEOR_SPARKS; ++i)
        if (previous.sparks[i].life != 0U)
            PlayFill(previous.sparks[i].x - 14, previous.sparks[i].y - 14, 28, 28, GAME_BG);
}

static void Title(void)
{
    uint16_t middle = game.height / 2U;
    LCD_Clear(GAME_BG);
    Stars();
    CenterText(18U, "STM32 ARCADE", GAME_MUTED, 16U);
    CenterText(middle - 96U, "METEOR GUARD", GAME_TEAL, 24U);
    CenterText(middle - 62U, "DODGE. SHOOT. SURVIVE.", WHITE, 16U);
    Sprite((int16_t)game.width / 2 - 16, (int16_t)middle - 30,
           ship_pixels, 16U, 16U, ship_palette, 0U);
    Sprite((int16_t)game.width / 2 - 80, (int16_t)middle - 26,
           rock_pixels, 12U, 12U, rock_palette, 0U);
    Sprite((int16_t)game.width / 2 + 56, (int16_t)middle - 26,
           rock_pixels, 12U, 12U, rock_palette, 0U);
    CenterText(middle + 26U, "K4: START", GAME_GOLD, 24U);
    CenterText(middle + 68U, "K1 LEFT   K2 RIGHT", WHITE, 16U);
    CenterText(middle + 90U, "HOLD K3 TO FIRE", WHITE, 16U);
    CenterText(game.height - 22U, "3 LIVES - BEAT YOUR BEST", GAME_MUTED, 12U);
}

static void Overlay(void)
{
    uint16_t middle = game.height / 2U;
    uint16_t left = (game.width - 240U) / 2U;
    LCD_Fill(left, middle - 62U, left + 240U, middle + 68U, GAME_BG);
    LCD_Fill(left, middle - 62U, left + 240U, middle - 60U, GAME_TEAL);
    LCD_Fill(left, middle + 66U, left + 240U, middle + 68U, GAME_TEAL);
    if (game.state == METEOR_PAUSED)
    {
        CenterText(middle - 38U, "PAUSED", GAME_GOLD, 24U);
        CenterText(middle + 4U, "K4: CONTINUE", WHITE, 16U);
    }
    else
    {
        CenterText(middle - 40U, "GAME OVER", GAME_GOLD, 24U);
        Text(left + 32U, middle - 4U, "BEST", GAME_MUTED, 16U);
        Number(left + 112U, middle - 4U, game.best, 6U, WHITE);
        CenterText(middle + 32U, "K4: PLAY AGAIN", WHITE, 16U);
    }
}

static void Render(void)
{
    if (game.state != previous.state)
    {
        LCD_Clear(GAME_BG);
        Hud();
        Stars();
        DrawObjects();
        if (game.state == METEOR_PAUSED || game.state == METEOR_GAME_OVER) Overlay();
    }
    else if (game.state == METEOR_PLAYING)
    {
        /* Erase ALL old objects before drawing ANY new object, including overlaps. */
        ErasePrevious();
        Stars();
        DrawObjects();
        if (game.score != previous.score || game.lives != previous.lives || game.level != previous.level)
            Hud();
    }
    previous = game;
}

void LCD_GameInit(void)
{
    game_ready = 0U;
    BoardKeys_Init();
    if (lcddev.width < 240U || lcddev.height < 320U)
    {
        LCD_Clear(BLACK);
        LCD_ShowString(0U, 0U, (const u8 *)"LCD too small", WHITE, BLACK, 16U, 0U);
        return;
    }
    MeteorGame_Init(&game, lcddev.width, lcddev.height, HAL_GetTick() ^ 0x53544D32U);
    previous = game;
    Title();
    last_frame = HAL_GetTick();
    game_ready = 1U;
}

void LCD_GameTask(void)
{
    uint32_t now;
    BoardKeys_Task();
    if (game_ready == 0U) return;
    now = HAL_GetTick();
    if ((uint32_t)(now - last_frame) < METEOR_FRAME_MS) return;
    last_frame = now;
    MeteorGame_Step(&game, BoardKeys_Held(), BoardKeys_TakePressed());
    Render();
}
