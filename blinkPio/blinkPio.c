#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "blinkPio.pio.h"

void blinkPioPut(PIO pio, uint sm, uint x) {
	printf("pio0Put 0x%03x\n", x);
	pio_sm_put_blocking(pio, sm, x);
}

int blinkPioInit(PIO pio, uint sm) {
	// init program
	uint offset = pio_add_program(pio, &blinkPio_program);
	sm = pio_claim_unused_sm(pio, true);
	printf("blinkPioInit offset=%u sm=%u\n", offset, sm);
	pio_sm_config c = blinkPio_program_get_default_config(offset);
	sm_config_set_out_pins(&c, PICO_DEFAULT_LED_PIN, 1);
	sm_config_set_out_shift(&c, /*shift_right*/ true, /*autopull*/ false, /*pull_threshold*/ 32);
	//sm_config_set_clkdiv_int_frac(&c, 125, 0); // 1us todo

	// init pins
	pio_gpio_init(pio, PICO_DEFAULT_LED_PIN);
	pio_sm_set_consecutive_pindirs(pio, sm, PICO_DEFAULT_LED_PIN, 1, true);

	pio_sm_init(pio, sm, offset, &c);
	pio_sm_set_enabled(pio, sm, true);
	printf("blinkPioInit enabled\n");
	return sm;
}

void blinkPio() {
	PIO pio = pio0;
	uint sm = blinkPioInit(pio, 0);
	while(true) {
		blinkPioPut(pio, sm, 1);
		sleep_ms(1000);
		blinkPioPut(pio, sm, 0);
		sleep_ms(1000);
	}
}

int main() {
	stdio_init_all();
	sleep_ms(3000);
	printf("main\n");
	blinkPio();
	return 0;
}
