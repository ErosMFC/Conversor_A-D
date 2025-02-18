#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "lib/ssd1306.h"
#include "lib/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define JOYSTICK_X_PIN 26  // GPIO para eixo X
#define JOYSTICK_Y_PIN 27  // GPIO para eixo Y
#define JOYSTICK_PB 22     // GPIO para botão do Joystick
#define BOTAO_A 5          // GPIO para botão A

#define LED_R 11           // LED Vermelho
#define LED_G 12           // LED Verde
#define LED_B 13           // LED Azul

bool leds_ativos = true;
bool borda_alternada = false;
bool led_verde_estado = false;
uint32_t last_interrupt_time = 0;

void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - last_interrupt_time < 200) return; // Debounce de 200ms
    last_interrupt_time = current_time;
    
    if (gpio == JOYSTICK_PB) {
        led_verde_estado = !led_verde_estado;
        gpio_put(LED_G, led_verde_estado);
        borda_alternada = !borda_alternada;
    } else if (gpio == BOTAO_A) {
        leds_ativos = !leds_ativos;
    }
}

void configurar_pwm(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice, 4095);
    pwm_set_enabled(slice, true);
}

void atualizar_leds(uint16_t x, uint16_t y) {
    if (leds_ativos) {
        pwm_set_gpio_level(LED_R, abs((int)x - 2048) * 2);
        pwm_set_gpio_level(LED_B, abs((int)y - 2048) * 2);
    } else {
        pwm_set_gpio_level(LED_R, 0);
        pwm_set_gpio_level(LED_B, 0);
    }
}

int main() {
    stdio_init_all();
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    ssd1306_t ssd;
    ssd1306_init(&ssd, 128, 64, false, endereco, I2C_PORT);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    
    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);
    gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    configurar_pwm(LED_R);
    configurar_pwm(LED_B);
    
    while (true) {
        adc_select_input(0);
        uint16_t adc_x = adc_read();
        adc_select_input(1);
        uint16_t adc_y = adc_read();
        
        atualizar_leds(adc_x, adc_y);
        
        uint8_t pos_x = (adc_x * 190) / 4095 ;
        uint8_t pos_y = 80 - (adc_y * 48) / 4095; // Inverte a direção do movimento no eixo Y
        
        ssd1306_fill(&ssd, false);
        ssd1306_rect(&ssd, pos_x, pos_y, 8, 8, true, true);
        if (borda_alternada) {
            ssd1306_rect(&ssd, 3, 3, 122, 60, true, false);
        }
        ssd1306_send_data(&ssd);
        
        sleep_ms(100);
    }
}
