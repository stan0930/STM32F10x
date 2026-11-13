#include "board_keys.h"
#include "main.h"

#define KEY_DEBOUNCE_MS 20U

static uint8_t keys_candidate;
static uint8_t keys_held;
static uint8_t keys_pressed;
static uint32_t keys_changed_at[4];

static uint8_t Keys_ReadPins(void)
{
    uint32_t pins = GPIOF->IDR;
    uint8_t result = 0U;

    /* SCH_ZET6 page 8: K1/K2 switch to GND, K3/K4 switch to 3.3 V. */
    if ((pins & GPIO_PIN_8) == 0U) result |= BOARD_KEY1;
    if ((pins & GPIO_PIN_9) == 0U) result |= BOARD_KEY2;
    if ((pins & GPIO_PIN_10) != 0U) result |= BOARD_KEY3;
    if ((pins & GPIO_PIN_11) != 0U) result |= BOARD_KEY4;
    return result;
}

void BoardKeys_Init(void)
{
    GPIO_InitTypeDef config = {0};
    uint32_t now = HAL_GetTick();
    uint8_t i;

    __HAL_RCC_GPIOF_CLK_ENABLE();
    config.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    config.Mode = GPIO_MODE_INPUT;
    config.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOF, &config);
    config.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    config.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOF, &config);

    keys_candidate = Keys_ReadPins();
    keys_held = 0U;
    keys_pressed = 0U;
    for (i = 0U; i < 4U; ++i) keys_changed_at[i] = now;
}

void BoardKeys_Task(void)
{
    uint32_t now = HAL_GetTick();
    uint8_t raw = Keys_ReadPins();
    uint8_t bit;
    uint8_t i;

    for (i = 0U; i < 4U; ++i)
    {
        bit = (uint8_t)(1U << i);
        if (((raw ^ keys_candidate) & bit) != 0U)
        {
            keys_candidate ^= bit;
            keys_changed_at[i] = now;
        }
        if (((keys_candidate ^ keys_held) & bit) != 0U &&
            (uint32_t)(now - keys_changed_at[i]) >= KEY_DEBOUNCE_MS)
        {
            keys_held ^= bit;
            if ((keys_held & bit) != 0U) keys_pressed |= bit;
        }
    }
}

uint8_t BoardKeys_Held(void)
{
    return keys_held;
}

uint8_t BoardKeys_TakePressed(void)
{
    uint8_t result = keys_pressed;
    keys_pressed = 0U;
    return result;
}
