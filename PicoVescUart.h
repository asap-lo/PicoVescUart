#pragma once

#include "vesc/bldc_interface.h"

#define UART_ID uart1
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 8
#define UART_RX_PIN 9

extern mc_values current_data_values;

static void send_packet(unsigned char *data, unsigned int len);
void comm_uart_init(void(*func)(mc_values *values));

void on_uart_rx();