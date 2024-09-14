/*
 * Made by Jack Lombardo, 2024
 */

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "vesc/bldc_interface_uart.h"
#include "vesc/bldc_interface.h"
#include <string.h>
#include <stdio.h>

#define UART_ID 	uart1
#define BAUD_RATE 	115200
#define DATA_BITS 	8
#define STOP_BITS 	1
#define PARITY    	UART_PARITY_NONE

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

static void send_packet(unsigned char *data, unsigned int len) {
	if (len > (PACKET_MAX_PL_LEN + 5)) {
		return;
	}

	// Wait for the previous transmission to finish.
	uart_tx_wait_blocking(UART_ID);

	// Copy this data to a new buffer in case the provided one is re-used
	// after this function returns.
	static uint8_t buffer[PACKET_MAX_PL_LEN + 5];
	memcpy(buffer, data, len);

	// Send the data over UART
	uart_write_blocking(UART_ID, buffer, len);

}

void bldc_val_received(mc_values *val) {
	printf("\r\n");
	printf("Input voltage: %.2f V\r\n", val->v_in);
	printf("Temp:          %.2f degC\r\n", val->temp_mos);
	printf("Current motor: %.2f A\r\n", val->current_motor);
	printf("Current in:    %.2f A\r\n", val->current_in);
	printf("RPM:           %.1f RPM\r\n", val->rpm);
	printf("Duty cycle:    %.1f %%\r\n", val->duty_now * 100.0);
	printf("Ah Drawn:      %.4f Ah\r\n", val->amp_hours);
	printf("Ah Regen:      %.4f Ah\r\n", val->amp_hours_charged);
	printf("Wh Drawn:      %.4f Wh\r\n", val->watt_hours);
	printf("Wh Regen:      %.4f Wh\r\n", val->watt_hours_charged);
	printf("Tacho:         %i counts\r\n", val->tachometer);
	printf("Tacho ABS:     %i counts\r\n", val->tachometer_abs);
	printf("Fault Code:    %s\r\n", bldc_interface_fault_to_string(val->fault_code));	
}

void comm_uart_init(void) {

	uart_init(UART_ID, BAUD_RATE);

	bldc_interface_uart_init(send_packet);
	// init reader callback
	bldc_interface_set_rx_value_func(bldc_val_received);
}

void read_packet(void) {
	for (int i = 0; i < PACKET_MAX_PL_LEN; i++) {
		bldc_interface_uart_process_byte(uart_getc(UART_ID));
	}

}

int main() {
	printf("HELLO THERE!");
    // Set up our UART with a basic baud rate.
	comm_uart_init();
	printf("comm_uart_init ran");
	bldc_interface_get_values();
	printf("bldc_interface_get_values started");
	read_packet();
    
}
