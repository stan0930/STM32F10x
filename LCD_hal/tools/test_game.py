"""Host checks for the real C game, GPIO debounce and LCD renderer.

Run: python tools/test_game.py
Requires GCC and Pillow. No STM32 connection is required.
"""

from pathlib import Path
import os
import subprocess
import tempfile

from PIL import Image


ROOT = Path(__file__).resolve().parents[1]

HAL_STUB = r'''
#ifndef HOST_HAL_H
#define HOST_HAL_H
#include <stdint.h>
typedef struct { uint32_t IDR; } GPIO_TypeDef;
typedef struct { uint32_t Pin, Mode, Pull, Speed; } GPIO_InitTypeDef;
extern GPIO_TypeDef host_gpiof;
#define GPIOF (&host_gpiof)
#define GPIO_PIN_8 (1U << 8)
#define GPIO_PIN_9 (1U << 9)
#define GPIO_PIN_10 (1U << 10)
#define GPIO_PIN_11 (1U << 11)
#define GPIO_MODE_INPUT 0U
#define GPIO_PULLUP 1U
#define GPIO_PULLDOWN 2U
#define __HAL_RCC_GPIOF_CLK_ENABLE() ((void)0)
uint32_t HAL_GetTick(void);
void HAL_GPIO_Init(GPIO_TypeDef *, GPIO_InitTypeDef *);
#endif
'''

LCD_STUB = r'''
#ifndef HOST_LCD_H
#define HOST_LCD_H
#include "main.h"
typedef uint8_t u8;
typedef uint16_t u16;
typedef struct { uint16_t width, height, id; } _lcd_dev;
extern _lcd_dev lcddev;
#define WHITE 0xFFFFU
#define BLACK 0U
void LCD_Clear(uint16_t);
void LCD_Fill(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
void LCD_DrawPoint(uint16_t, uint16_t, uint16_t);
void LCD_ShowString(uint16_t, uint16_t, const u8 *, uint16_t, uint16_t, u8, u8);
#endif
'''

HARNESS = r'''
#include "lcd.h"
#include "board_keys.h"
#include "meteor_game.h"
#include "lcd_game.h"
#include "host_fonts.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

GPIO_TypeDef host_gpiof;
_lcd_dev lcddev = {320, 480, 0};
static uint32_t tick;
static uint16_t framebuffer[480][480];
static unsigned gpio_calls;

uint32_t HAL_GetTick(void) { return tick; }
void HAL_GPIO_Init(GPIO_TypeDef *port, GPIO_InitTypeDef *config)
{
    assert(port == GPIOF && config->Mode == GPIO_MODE_INPUT);
    if (config->Pin == (GPIO_PIN_8 | GPIO_PIN_9)) assert(config->Pull == GPIO_PULLUP);
    else { assert(config->Pin == (GPIO_PIN_10 | GPIO_PIN_11)); assert(config->Pull == GPIO_PULLDOWN); }
    ++gpio_calls;
}
void LCD_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    uint16_t x, y;
    assert(x0 < x1 && y0 < y1 && x1 <= lcddev.width && y1 <= lcddev.height);
    for (y = y0; y < y1; ++y) for (x = x0; x < x1; ++x) framebuffer[y][x] = color;
}
void LCD_Clear(uint16_t color) { LCD_Fill(0, 0, lcddev.width, lcddev.height, color); }
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    assert(x < lcddev.width && y < lcddev.height);
    framebuffer[y][x] = color;
}
void LCD_ShowString(uint16_t x, uint16_t y, const u8 *s, uint16_t fg, uint16_t bg, u8 size, u8 mode)
{
    uint16_t width = size / 2U, row, col;
    unsigned num, offset;
    const unsigned char *data;
    assert(mode == 0 && y + size <= lcddev.height);
    while (*s != '\0')
    {
        assert(x + width <= lcddev.width && *s >= 32 && *s <= 126);
        num = *s++ - 32U;
        if (size == 12) data = ascii_1206[num];
        else if (size == 16) data = ascii_1608[num];
        else if (size == 24) data = ascii_2412[num];
        else { assert(size == 32); data = ascii_3216[num]; }
        for (row = 0; row < size; ++row) for (col = 0; col < width; ++col)
        {
            offset = row * ((width + 7U) / 8U) + col / 8U;
            LCD_DrawPoint(x + col, y + row, (data[offset] & (1U << (col % 8U))) ? fg : bg);
        }
        x += width;
    }
}
static void Dump(const char *name)
{
    unsigned x, y;
    uint16_t c;
    unsigned char rgb[3];
    FILE *file = fopen(name, "wb");
    assert(file != NULL);
    fprintf(file, "P6\n%u %u\n255\n", lcddev.width, lcddev.height);
    for (y = 0; y < lcddev.height; ++y) for (x = 0; x < lcddev.width; ++x)
    {
        c = framebuffer[y][x];
        rgb[0] = (unsigned char)(((c >> 11) << 3) | ((c >> 11) >> 2));
        rgb[1] = (unsigned char)((((c >> 5) & 63) << 2) | (((c >> 5) & 63) >> 4));
        rgb[2] = (unsigned char)(((c & 31) << 3) | ((c & 31) >> 2));
        assert(fwrite(rgb, 1, 3, file) == 3);
    }
    fclose(file);
}
static void Pins(uint8_t pressed)
{
    host_gpiof.IDR = GPIO_PIN_8 | GPIO_PIN_9;
    if (pressed & BOARD_KEY1) host_gpiof.IDR &= ~GPIO_PIN_8;
    if (pressed & BOARD_KEY2) host_gpiof.IDR &= ~GPIO_PIN_9;
    if (pressed & BOARD_KEY3) host_gpiof.IDR |= GPIO_PIN_10;
    if (pressed & BOARD_KEY4) host_gpiof.IDR |= GPIO_PIN_11;
}
static void KeysTest(void)
{
    uint8_t i, bit;
    Pins(0); tick = 0; BoardKeys_Init();
    tick = 50; BoardKeys_Task(); assert(BoardKeys_Held() == 0);
    assert(gpio_calls == 2);
    for (i = 0; i < 4; ++i)
    {
        bit = (uint8_t)(1U << i);
        Pins(bit); BoardKeys_Task();
        tick += 5; Pins(0); BoardKeys_Task();
        tick += 5; Pins(bit); BoardKeys_Task();
        tick += 19; BoardKeys_Task(); assert(BoardKeys_Held() == 0);
        ++tick; BoardKeys_Task(); assert(BoardKeys_Held() == bit);
        assert(BoardKeys_TakePressed() == bit && BoardKeys_TakePressed() == 0);
        tick += 200; BoardKeys_Task(); assert(BoardKeys_TakePressed() == 0);
        Pins(0); BoardKeys_Task(); tick += 20; BoardKeys_Task();
        assert(BoardKeys_Held() == 0);
    }
    tick = UINT32_MAX - 10U; Pins(0); BoardKeys_Init();
    Pins(BOARD_KEY1 | BOARD_KEY3); BoardKeys_Task();
    tick = 8U; BoardKeys_Task(); assert(BoardKeys_Held() == 0);
    tick = 9U; BoardKeys_Task(); assert(BoardKeys_Held() == (BOARD_KEY1 | BOARD_KEY3));
    assert(BoardKeys_TakePressed() == (BOARD_KEY1 | BOARD_KEY3));
    puts("PASS: GPIO polarity, pull resistors, independent debounce, bounce, held keys, edges, tick wrap");
}
static void LogicTest(void)
{
    MeteorGame g, saved;
    unsigned i, j, k;
    int16_t x;
    MeteorGame_Init(&g, 320, 480, 1234); assert(g.state == METEOR_TITLE);
    MeteorGame_Step(&g, 0, METEOR_ACTION); assert(g.state == METEOR_PLAYING && g.lives == 3);
    for (i = 0; i < 100; ++i) MeteorGame_Step(&g, METEOR_LEFT, 0);
    assert(g.ship_x == 8);
    for (i = 0; i < 100; ++i) MeteorGame_Step(&g, METEOR_RIGHT, 0);
    assert(g.ship_x == 280);
    x = g.ship_x; MeteorGame_Step(&g, METEOR_LEFT | METEOR_RIGHT, 0); assert(g.ship_x == x);
    MeteorGame_Step(&g, 0, METEOR_ACTION); assert(g.state == METEOR_PAUSED);
    saved = g;
    for (i = 0; i < 30; ++i) MeteorGame_Step(&g, METEOR_LEFT | METEOR_FIRE, 0);
    assert(memcmp(&g, &saved, sizeof(g)) == 0);
    MeteorGame_Step(&g, 0, METEOR_ACTION); assert(g.state == METEOR_PLAYING);

    MeteorGame_Init(&g, 320, 480, 1); MeteorGame_Step(&g, 0, METEOR_ACTION);
    g.spawn_cooldown = 255;
    g.rocks[0] = (MeteorObject){100, 100, 1, 2};
    g.shots[0] = (MeteorObject){108, 125, 1, 0};
    MeteorGame_Step(&g, 0, 0);
    assert(g.rocks[0].active == 0 && g.shots[0].active == 0 && g.score == 10 && g.best == 10);
    assert(g.sparks[0].life != 0);
    MeteorGame_Step(&g, METEOR_FIRE, 0); assert(g.shots[0].active != 0);
    g.fire_cooldown = 0; memset(g.shots, 0, sizeof(g.shots));
    MeteorGame_Step(&g, 0, METEOR_FIRE); assert(g.shots[0].active != 0);
    memset(g.shots, 0, sizeof(g.shots));
    for (i = 0; i < 3; ++i)
    {
        g.invulnerable = 0;
        g.rocks[0] = (MeteorObject){g.ship_x + 4, g.ship_y, 1, 2};
        MeteorGame_Step(&g, 0, 0);
        assert(g.lives == 2 - i);
        if (i == 0)
        {
            g.rocks[0] = (MeteorObject){g.ship_x + 4, g.ship_y, 1, 2};
            MeteorGame_Step(&g, 0, 0); assert(g.lives == 2);
        }
    }
    assert(g.state == METEOR_GAME_OVER);
    MeteorGame_Step(&g, 0, METEOR_ACTION);
    assert(g.state == METEOR_PLAYING && g.score == 0 && g.best == 10 && g.lives == 3);

    for (j = 0; j < 2; ++j)
    {
        MeteorGame_Init(&g, j ? 480 : 320, j ? 320 : 480, 9876543);
        MeteorGame_Step(&g, 0, METEOR_ACTION);
        for (i = 0; i < 40000; ++i)
        {
            if (g.state == METEOR_GAME_OVER) MeteorGame_Step(&g, 0, METEOR_ACTION);
            MeteorGame_Step(&g, (uint8_t)(METEOR_FIRE | (i % 100 < 50 ? METEOR_LEFT : METEOR_RIGHT)), 0);
            assert(g.ship_x >= 8 && g.ship_x + 32 <= g.width - 8 && g.lives <= 3);
            assert(g.level >= 1 && g.level <= 9 && g.score <= 999999 && g.best >= g.score);
            for (k = 0; k < METEOR_ROCKS; ++k) if (g.rocks[k].active)
                assert(g.rocks[k].x >= 8 && g.rocks[k].x + 24 < g.width - 7 &&
                       g.rocks[k].y >= METEOR_TOP && g.rocks[k].y < g.height - METEOR_BOTTOM_MARGIN);
        }
        g.state = METEOR_PLAYING; g.invulnerable = 255;
        g.score = g.best = 999998; g.ticks = 24; MeteorGame_Step(&g, 0, 0);
        assert(g.score == 999999 && g.level == 9);
    }
    puts("PASS: movement limits, pause, shooting, collisions, invulnerability, restart, high score, 80000 simulation steps");
}
static void Advance(uint32_t ms, uint8_t keys)
{
    uint32_t i;
    Pins(keys);
    for (i = 0; i < ms; ++i) { ++tick; LCD_GameTask(); }
}
static void RendererTest(void)
{
    uint32_t i;
    static uint16_t paused_frame[480][480];
    tick = 0; Pins(0); LCD_GameInit(); Dump("title.ppm");
    Advance(80, BOARD_KEY4); Advance(80, 0);
    Advance(1200, BOARD_KEY3); Dump("playing.ppm");
    Advance(80, BOARD_KEY4); Dump("paused.ppm");
    memcpy(paused_frame, framebuffer, sizeof(framebuffer)); Advance(200, BOARD_KEY1);
    assert(memcmp(framebuffer, paused_frame, sizeof(framebuffer)) == 0);
    Advance(80, 0); Advance(80, BOARD_KEY4); Advance(80, 0);
    for (i = 0; i < 400; ++i)
    {
        Advance(40, (uint8_t)(BOARD_KEY3 | (i % 80 < 40 ? BOARD_KEY1 : BOARD_KEY2)));
        if (i == 65) Dump("action.ppm");
    }
    /* Leave the ship centered without firing until collisions end the game. */
    LCD_GameInit(); Advance(80, BOARD_KEY4); Advance(80, 0);
    Advance(180000, 0); Dump("game_over.ppm");
    lcddev.width = 480; lcddev.height = 320; Pins(0); LCD_GameInit(); Dump("landscape_title.ppm");
    Advance(80, BOARD_KEY4); Advance(80, 0); Advance(6000, BOARD_KEY3); Dump("landscape.ppm");
    tick = UINT32_MAX - 10U; LCD_GameInit(); Advance(100, BOARD_KEY4); Advance(100, 0);
    puts("PASS: actual renderer coordinates, all screen states, portrait/landscape, main-loop integration, tick wrap");
}
int main(void) { KeysTest(); LogicTest(); RendererTest(); return 0; }
'''


def main():
    output = ROOT / "assets" / "game"
    output.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="meteor_game_check_") as directory:
        temp = Path(directory)
        (temp / "main.h").write_text(HAL_STUB, encoding="ascii")
        (temp / "lcd.h").write_text(LCD_STUB, encoding="ascii")
        (temp / "check.c").write_text(HARNESS, encoding="ascii")
        fonts = (ROOT / "lcd" / "lcdfont.h").read_bytes().split(b"typedef struct")[0]
        (temp / "host_fonts.h").write_bytes(fonts + b"\n#endif\n")
        # Copy renderer so its lcd.h include resolves to the checked host shim.
        (temp / "lcd_game.c").write_bytes((ROOT / "lcd" / "lcd_game.c").read_bytes())
        executable = temp / ("check.exe" if os.name == "nt" else "check")
        subprocess.run([
            "gcc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic",
            "-I", str(temp), "-I", str(ROOT / "lcd"), "-I", str(ROOT / "Core" / "Inc"),
            str(temp / "check.c"), str(temp / "lcd_game.c"),
            str(ROOT / "lcd" / "meteor_game.c"), str(ROOT / "Core" / "Src" / "board_keys.c"),
            "-o", str(executable),
        ], check=True)
        subprocess.run([str(executable)], cwd=temp, check=True)
        for path in temp.glob("*.ppm"):
            with Image.open(path) as image:
                image.save(output / (path.stem + ".png"))
    print(f"Actual C framebuffer previews: {output}")


if __name__ == "__main__":
    main()
