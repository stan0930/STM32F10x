#include "lcd_animation.h"
#include "lcd.h"
#include "space_cat_frames.h"

#define ANIMATION_SCALE 2U
#define ANIMATION_WIDTH (SPACE_CAT_WIDTH * ANIMATION_SCALE)
#define ANIMATION_HEIGHT (SPACE_CAT_HEIGHT * ANIMATION_SCALE)

static uint32_t animation_last_tick;
static uint16_t animation_x;
static uint16_t animation_y;
static uint8_t animation_frame;
static uint8_t animation_ready;

/* Stream one frame from Flash to FSMC. No full-screen RAM buffer is needed. */
static void Animation_DrawFrame(uint8_t frame)
{
    const uint8_t *pixels = space_cat_frames[frame];
    uint16_t row;
    uint16_t column;
    uint16_t repeat;
    uint16_t color;
    uint32_t offset;

    LCD_Address_Set(animation_x, animation_y,
                    animation_x + ANIMATION_WIDTH - 1U,
                    animation_y + ANIMATION_HEIGHT - 1U);

    for (row = 0U; row < SPACE_CAT_HEIGHT; ++row)
    {
        for (repeat = 0U; repeat < ANIMATION_SCALE; ++repeat)
        {
            offset = (uint32_t)row * SPACE_CAT_WIDTH * 2U;
            for (column = 0U; column < SPACE_CAT_WIDTH; ++column)
            {
                /* The asset stores RGB565 with the high byte first. */
                color = ((uint16_t)pixels[offset] << 8) | pixels[offset + 1U];
                offset += 2U;
                LCD->LCD_RAM = color;
                LCD->LCD_RAM = color;
            }
        }
    }
}

static void Animation_DrawScene(void)
{
    uint16_t x;
    uint16_t y;
    uint16_t i;

    LCD_Clear(SPACE_CAT_BACKGROUND);
    LCD_ShowString((lcddev.width - 9U * 12U) / 2U, 18U,
                   (const u8 *)"SPACE CAT", WHITE, SPACE_CAT_BACKGROUND, 24U, 0U);

    /* Static stars stay outside the area replaced by each image frame. */
    for (i = 0U; i < 48U; ++i)
    {
        x = (uint16_t)((17UL + (uint32_t)i * 73UL) % lcddev.width);
        y = (uint16_t)(58UL + (uint32_t)i * 47UL % (lcddev.height - 100U));
        if (x >= animation_x && x < animation_x + ANIMATION_WIDTH &&
            y >= animation_y && y < animation_y + ANIMATION_HEIGHT)
        {
            continue;
        }
        LCD_DrawPoint(x, y, (i % 3U == 0U) ? WHITE : 0x53B4U);
    }

    LCD_ShowString((lcddev.width - 15U * 8U) / 2U, lcddev.height - 30U,
                   (const u8 *)"HELLO, UNIVERSE!", 0x7EFFU,
                   SPACE_CAT_BACKGROUND, 16U, 0U);
}

void LCD_AnimationInit(void)
{
    animation_ready = 0U;

    /* Keep all writes inside the screen, including after a direction change. */
    if (lcddev.width < ANIMATION_WIDTH ||
        lcddev.height < ANIMATION_HEIGHT + 112U)
    {
        return;
    }

    animation_x = (lcddev.width - ANIMATION_WIDTH) / 2U;
    animation_y = (lcddev.height - ANIMATION_HEIGHT) / 2U;
    animation_frame = 0U;

    Animation_DrawScene();
    Animation_DrawFrame(animation_frame);
    animation_last_tick = HAL_GetTick();
    animation_ready = 1U;
}

void LCD_AnimationTask(void)
{
    uint32_t now;
    uint32_t elapsed;
    uint32_t steps;

    if (animation_ready == 0U)
    {
        return;
    }

    now = HAL_GetTick();
    elapsed = (uint32_t)(now - animation_last_tick);
    if (elapsed < LCD_ANIMATION_FRAME_MS)
    {
        return;
    }

    /* Unsigned subtraction also works when the HAL tick counter wraps. */
    steps = elapsed / LCD_ANIMATION_FRAME_MS;
    animation_last_tick += steps * LCD_ANIMATION_FRAME_MS;
    animation_frame = (uint8_t)((animation_frame + steps) % SPACE_CAT_FRAME_COUNT);

    /* Each image contains its background, so a separate erase is unnecessary. */
    Animation_DrawFrame(animation_frame);
}
