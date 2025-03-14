/*
 * Made by Jack Lombardo and Nicholas Hyder, 2024
 */
#include "PicoVescUart.h"

#include <string.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "vesc/bldc_interface_uart.h"
#include "vesc/bldc_interface.h"

mc_values current_data_values;

// RX interrupt handler
void on_uart_rx()
{
	while (uart_is_readable(UART_ID))
	{
		uint8_t ch = uart_getc(UART_ID);
		bldc_interface_uart_process_byte(ch);
	}
}

static void send_packet(unsigned char *data, unsigned int len)
{
	if (len > (PACKET_MAX_PL_LEN + 5))
	{
		return;
	}

	static uint8_t buffer[PACKET_MAX_PL_LEN + 5];
	memcpy(buffer, data, len);
	uart_write_blocking(UART_ID, buffer, len);
}

void comm_uart_init(void(*func)(mc_values *values))
{
    // Initialize PICO SDK
    stdio_init_all();
	uart_init(UART_ID, BAUD_RATE);

	// Set the TX and RX pins by using the function select on the GPIO
	// Set datasheet for more information on function select
	gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
	gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));

	// Set our data format
	uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

	// Turn off FIFO's - we want to do this character by character
	uart_set_fifo_enabled(UART_ID, false);

	// Select correct interrupt handler for either UART0 or UART1
	int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

	// And set up and enable the interrupt handlers
	irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
	irq_set_enabled(UART_IRQ, true);

	// Now enable the UART to send interrupts - RX only
	uart_set_irq_enables(UART_ID, true, false);

    // Init UART interface with send_packet implementation
	bldc_interface_uart_init(send_packet);
	
    // init reader callback
    if (func != NULL)
	    bldc_interface_set_rx_value_func(func);
}