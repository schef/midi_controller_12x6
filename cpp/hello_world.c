#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

uint16_t adc_read_channel(uint8_t channel) {
  adc_select_input(channel);
  return adc_read();
}

const float conversion_factor = 3.3f / (1 << 12);

int main() {
  stdio_init_all();
  printf("Boot sequence start\n");

  adc_init();
  adc_gpio_init(26);
  adc_gpio_init(28);

  printf("Boot sequence end\n");
  while (true) {
    auint16_t left = adc_read_channel(0);
    uint16_t right = adc_read_channel(2);
    printf("%04x, %f, %04x, %f\n", left, left * conversion_factor, right, right * conversion_factor);
    sleep_ms(10);
  }
  return 0;
}
