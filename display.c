#include "display.h"
#include "HD44780.h"
#include "log.h"
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/delay.h>

typedef struct  
{
    HD44780_pin_t pin;
    struct gpio config;
} gpio_map_t;

static const gpio_map_t gpio_map[] = {
    {HD44780_PIN_RS, {4, GPIOF_OUT_INIT_LOW, "LCD_RS"}},
    {HD44780_PIN_E, {17, GPIOF_OUT_INIT_LOW, "LCD_E"}},
    {HD44780_PIN_D4, {18, GPIOF_OUT_INIT_LOW, "LCD_D4"}},
    {HD44780_PIN_D5, {22, GPIOF_OUT_INIT_LOW, "LCD_D5"}},
    {HD44780_PIN_D6, {23, GPIOF_OUT_INIT_LOW, "LCD_D6"}},
    {HD44780_PIN_D7, {24, GPIOF_OUT_INIT_LOW, "LCD_D7"}}
};

static int gpio_init(void)
{
    size_t i;
    int err;

    for (i = 0; i < ARRAY_SIZE(gpio_map); ++i) {
        err = gpio_request_one(gpio_map[i].config.gpio, gpio_map[i].config.flags, gpio_map[i].config.label);
        if (err) {
            LOG_ERROR("Failed to request GPIO '%s', error %d!", gpio_map[i].config.label, err);
            break;
        }
    }

    if (err) {
        LOG_DEBUG("Freeing GPIOs after failure...");
        while (i--) {
            gpio_free(gpio_map[i - 1].config.gpio);
        }
    }

    return err;
}

static void gpio_deinit(void)
{
    LOG_DEBUG("Freeing GPIOs...");

    for (size_t i = 0; i < ARRAY_SIZE(gpio_map); ++i) {
        gpio_free(gpio_map[i].config.gpio);
    }
}

static void set_pin_state(HD44780_pin_t pin, HD44780_pin_state_t state) 
{
    for (size_t i = 0; i < ARRAY_SIZE(gpio_map); ++i) {
        if (gpio_map[i].pin == pin) {
            gpio_set_value(gpio_map[i].config.gpio, (state == HD44780_LOW) ? 0 : 1);
            return;
        }
    }

    LOG_ERROR("No GPIO found!");
}

static void delay_us(unsigned short us) 
{
    fsleep(us);
}

static HD44780_io_t display_io = {
    .set_pin_state = set_pin_state,
    .delay_us = delay_us
};

static HD44780_config_t display_config = {
    .io = &display_io,
    .type = HD44780_DISPLAY_20x2,
    .entry_mode_flags = HD44780_INCREASE_CURSOR_ON,
    .on_off_flags = HD44780_DISPLAY_ON
};

int display_init(void) 
{
    int err;

    err = gpio_init();
    if (err) {
        return err;
    }

    HD44780_init(&display_config);

    return 0;
}

void display_write_char(char ch) //TODO this is just for testing
{
    HD44780_write_char(ch);
}

void display_gotoxy(size_t x, size_t y) //TODO this is just for testing
{
    HD44780_gotoxy(x, y);
}

void display_deinit(void) 
{
    gpio_deinit();
}
