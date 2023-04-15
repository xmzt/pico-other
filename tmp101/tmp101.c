#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#define TMP101_PIN_VCC 11 // yel tmp101.4.v+
#define TMP101_PIN_SDA 12 // i2c0 blu tmp101.6.sda
#define TMP101_PIN_SCL 13 // i2c0 yel tmp101.1.scl
// GND blu tmp101.1.gnd

#define TMP101_ADDR 0b1001001

#define TMP101_PTR_TEMP   0
#define TMP101_PTR_CONFIG 1
#define TMP101_PTR_TLO    2
#define TMP101_PTR_THI    3

#define TMP101_CONFIG_SHUTDOWN_DIS          (0<<0)
#define TMP101_CONFIG_SHUTDOWN_EN           (1<<0)
#define TMP101_CONFIG_THERMOSTAT_COMPARATOR (0<<1)
#define TMP101_CONFIG_THERMOSTAT_INTERRUPT  (1<<1)
#define TMP101_CONFIG_POLARITY_ALERT_LO     (0<<2)
#define TMP101_CONFIG_POLARITY_ALERT_HI     (1<<2)
#define TMP101_CONFIG_FAULT_QUEUE_1         (0b00<<3)
#define TMP101_CONFIG_FAULT_QUEUE_2         (0b01<<3)
#define TMP101_CONFIG_FAULT_QUEUE_4         (0b10<<3)
#define TMP101_CONFIG_FAULT_QUEUE_6         (0b11<<3)
#define TMP101_CONFIG_RESOLUTION_9          (0b00<<5) // 0.5C    40ms
#define TMP101_CONFIG_RESOLUTION_10         (0b01<<5) // 0.25C   80ms
#define TMP101_CONFIG_RESOLUTION_11         (0b10<<5) // 0.125C  160ms
#define TMP101_CONFIG_RESOLUTION_12         (0b11<<5) // 0.0625C 320ms
#define TMP101_CONFIG_ONESHOT_DIS           (0<<7)
#define TMP101_CONFIG_ONESHOT_EN            (1<<7)
#define TMP101_CONFIG_ALERT                 (1<<7)

//-----------------------------------------------------------------------------------------------------------------------
// main

void tmp101Write1(i2c_inst_t *i2c, uint8_t a) {
	int ret = i2c_write_blocking(i2c, TMP101_ADDR, &a, 1, /*nostop*/ true);
	printf("tmp101Write1 %02x [ret=%d]\n", a, ret);
}

void tmp101Write2(i2c_inst_t *i2c, uint8_t a, uint8_t b) {
	int ret = i2c_write_blocking(i2c, TMP101_ADDR, (uint8_t[]) { a, b }, 2, /*nostop*/ true);
	printf("tmp101Write2 %02x,%02x [ret=%d]\n", a, b, ret);
}

void tmp101Write3(i2c_inst_t *i2c, uint8_t a, uint8_t b, uint8_t c) {
	int ret = i2c_write_blocking(i2c, TMP101_ADDR, (uint8_t[]) { a, b, c }, 3, /*nostop*/ true);
	printf("tmp101Write3 %02x,%02x,%02x [ret=%d]\n", a, b, c, ret);
}

void tmp101Read1(i2c_inst_t *i2c, uint8_t *buf) {
	int ret = i2c_read_blocking(i2c, TMP101_ADDR, buf, 1, /*nostop*/ true);
	printf("tmp101Read1 %02x [ret=%d]\n", buf[0], ret);
}

void tmp101Read2(i2c_inst_t *i2c, uint8_t *buf) {
	int ret = i2c_read_blocking(i2c, TMP101_ADDR, buf, 2, /*nostop*/ true);
	printf("tmp101Read2 %02x,%02x [ret=%d]\n", buf[0], buf[1], ret);
}

void tmp101Read2t(i2c_inst_t *i2c, uint8_t *buf) {
	int ret = i2c_read_blocking(i2c, TMP101_ADDR, buf, 2, /*nostop*/ true);
	float tC = (int16_t)(buf[0] << 8 | buf[1]) / 256.0;
	float tF = 1.8 * tC + 32.0;
	printf("tmp101Read2t %02x,%02x C=%.4f F=%.4f [ret=%d]\n", buf[0], buf[1], tC, tF, ret);
}

void tmp101Init(i2c_inst_t *i2c) {
    gpio_init(TMP101_PIN_VCC);
    gpio_set_dir(TMP101_PIN_VCC, GPIO_OUT);
	gpio_put(TMP101_PIN_VCC, 1);
	i2c_init(i2c0, 100000);
	i2c_set_slave_mode(i2c0, /*slave*/ false, /*slave_addr*/ 0);
	gpio_set_function(TMP101_PIN_SDA, GPIO_FUNC_I2C);
	gpio_set_function(TMP101_PIN_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(TMP101_PIN_SDA);
	gpio_pull_up(TMP101_PIN_SCL);
}

void tmp101Test(i2c_inst_t *i2c) {
	uint8_t buf[2];

	tmp101Write2(i2c0, TMP101_PTR_CONFIG, TMP101_CONFIG_RESOLUTION_12);
	tmp101Read1(i2c0, buf);
	tmp101Write3(i2c0, TMP101_PTR_TLO, 0x80, 0x00);
	tmp101Read2t(i2c0, buf);
	tmp101Write3(i2c0, TMP101_PTR_THI, 0x7F, 0xFF);
	tmp101Read2t(i2c0, buf);
	tmp101Write1(i2c0, TMP101_PTR_TEMP);
	while(true) {
		tmp101Read2t(i2c0, buf);
		sleep_ms(1000);
	}
}

int main() {
	stdio_init_all();
	tmp101Init(i2c0);

	sleep_ms(3000);
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	gpio_put(PICO_DEFAULT_LED_PIN, 1);
	printf("main\n");

	tmp101Test(i2c0);
	return 0;
}
