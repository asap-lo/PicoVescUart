/*
 * Made by Jack Lombardo, 2024
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

#include "hardware/spi.h"
#include "ST7735_TFT.h"
#include "hw.h"
#include "math.h"

#define UART_ID uart1
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 8
#define UART_RX_PIN 9

// Time between requests for mc_values in ms
#define TIME_BETWEEN_REQUESTS_MS 50

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

	for (int i = 0; i < len; i++)
	{
		printf("putting char %c\n", data[i]);
	}

	uart_write_blocking(UART_ID, buffer, len);
}

void bldc_val_received(mc_values *val)
{
	current_data_values = *val;
	// printf("\rGOT SOMETHING?\n");
	// printf("Input voltage: %.2f V\r\n", val->v_in);
	// printf("Temp:          %.2f degC\r\n", val->temp_mos);
	// printf("Current motor: %.2f A\r\n", val->current_motor);
	// printf("Current in:    %.2f A\r\n", val->current_in);
	// printf("RPM:           %.1f RPM\r\n", val->rpm);
	// printf("Duty cycle:    %.1f %%\r\n", val->duty_now * 100.0);
	// printf("Ah Drawn:      %.4f Ah\r\n", val->amp_hours);
	// printf("Ah Regen:      %.4f Ah\r\n", val->amp_hours_charged);
	// printf("Wh Drawn:      %.4f Wh\r\n", val->watt_hours);
	// printf("Wh Regen:      %.4f Wh\r\n", val->watt_hours_charged);
	// printf("Tacho:         %i counts\r\n", val->tachometer);
	// printf("Tacho ABS:     %i counts\r\n", val->tachometer_abs);
	// printf("Fault Code:    %s\r\n", bldc_interface_fault_to_string(val->fault_code));
}

void comm_uart_init(void)
{
	uart_init(UART_ID, BAUD_RATE);

	// Set the TX and RX pins by using the function select on the GPIO
	// Set datasheet for more information on function select
	gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
	gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));

	// Set our data format
	uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

	// Turn off FIFO's - we want to do this character by character
	uart_set_fifo_enabled(UART_ID, false);

	// Set up a RX interrupt
	// We need to set up the handler first
	// Select correct interrupt for the UART we are using
	int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

	// And set up and enable the interrupt handlers
	irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
	irq_set_enabled(UART_IRQ, true);

	// Now enable the UART to send interrupts - RX only
	uart_set_irq_enables(UART_ID, true, false);

	bldc_interface_uart_init(send_packet);
	// init reader callback
	// bldc_interface_set_sim_values_func()
	bldc_interface_set_rx_value_func(bldc_val_received);
}





// BELOW THIS LINE IS AN EXAMPLE WHICH DRAWS MOTOR DATA TO A SCREEN

// Convert 24-abit color to 16-abit color
int16_t N_Color565(int16_t r, int16_t g, int16_t b){           
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
// Setting up colors for later use
#define BLACK N_Color565(0, 0, 0)
#define WHITE N_Color565(255, 255, 255)
#define RED N_Color565(255, 0, 0)
#define GREEN N_Color565(0, 255, 0)
#define BLUE N_Color565(0, 0, 255)
#define YELLOW N_Color565(255, 255, 0)
#define CYAN N_Color565(0, 255, 255)
#define MAGENTA N_Color565(255, 0, 255)

void drawNum(uint8_t x, uint8_t y, int val, uint16_t color, uint8_t size, uint8_t len) {
    // len: # total length including leading spaces
    char temp[256];
    sprintf(temp, "%d", val);
    uint8_t spaces = len - strlen(temp);
    for (uint8_t k;k<spaces;k++) {
        drawText(x+k*6*size, y, " ", color, BLACK, size);
    }
    drawText(x+spaces*6*size, y, temp, color, BLACK, size);
}

void drawFlt(uint8_t x, uint8_t y, int val, uint16_t color, uint8_t size, uint8_t len, uint8_t decimal) {
    // len: # total length including leading spaces
    char digits[256];
    int digi = val/pow(10, decimal);
    sprintf(digits, "%d", digi);
    char decimals[256];
    int deci = val%((int)pow(10, decimal));
    sprintf(decimals, "%d", deci);
    uint8_t spaces = len - (strlen(digits)+strlen(decimals))-1;
    for (uint8_t k;k<spaces;k++) {
        drawText(x+k*6*size, y, " ", color, BLACK, size);
    }
    drawText(x+spaces*6*size, y, digits, color, BLACK, size);
    drawChar(x+spaces*6*size+strlen(digits)*6*size, y, 46, color, BLACK, size);
    drawText(x+spaces*6*size+(strlen(digits)+1)*6*size, y, decimals, color, BLACK, size);
}

void batteryBar(float voltage, float upper, float lower) {
    uint8_t height = (voltage-lower)/(upper-lower)*60;
    fillRect(2, 63-height, 16, height+1, GREEN);
    if (height < 59) {fillRect(2, 4, 16, 59-height, BLACK);}
    drawFastHLine(2, 64, 16, BLACK);
}

#define MPH_CONVERSION	(((1.0 / 14.0) / 80.11) / (12.0 / 5280.0)) * 60
int rpm_to_mph(float in) {
	return (int) in * MPH_CONVERSION;
}


void example_implementation()
{   
	sleep_ms(2000);
    //stdio_init_all();
    // SPI initialisation. This example will use SPI at 50MHz.
    spi_init(SPI_PORT, 50000000);
    tft_spi_init();
    TFT_BlackTab_Initialize();
    setAddrWindow(0, 0, 128, 128);
    setRotation(1);
    fillScreen(BLACK);

    uint8_t rows[] = {71, 84, 106};
    // Static Screen elements
    drawRectWH(0, 2, 20, 64, GREEN);
    drawRectWH(5, 0, 10, 2, GREEN);
    drawCircle(4, rows[0]+3, 4, YELLOW);
    drawPixel(4, rows[0]+3, YELLOW);
    drawRectWH(86, rows[0], 9, 8, YELLOW);
    for (uint k=0;k<3;k++) {drawFastVLine(88+k*2, rows[0]+2, 4, YELLOW);}
    drawText(108, 8, "m", BLUE, BLACK, 2);
    drawText(108, 22, "p", BLUE, BLACK, 2);
    drawText(108, 38, "h", BLUE, BLACK, 2);
    drawText(35, rows[0], "C", YELLOW, BLACK, 1);
    drawText(122, rows[0], "C", YELLOW, BLACK, 1);
    drawText(51, rows[1], "V", MAGENTA, BLACK, 2);
    drawText(116, rows[1], "A", MAGENTA, BLACK, 2);
    drawText(51, rows[2], "W", MAGENTA, BLACK, 2);
    drawText(66, rows[2], "-----", MAGENTA, BLACK, 2);


    uint64_t start = 0;
    uint64_t finish = 0;
    uint64_t time = 0;
    
    while (true) {
        // Draw Stuff Here ------------------
        // drawNum(128-24, 0, time, WHITE, 1, 2); // shows fps!!!!1!1!
        drawNum(36, 10, rpm_to_mph(current_data_values.rpm), BLUE, 6, 2);            //mph
        batteryBar(current_data_values.v_in, 73.8, 54);
        drawFlt(10, rows[0], current_data_values.temp_motor*10, YELLOW, 1, 4, 1); //motor temp
        drawFlt(97, rows[0], current_data_values.temp_mos*10, YELLOW, 1, 4, 1); //controller temp
        drawFlt(0, rows[1], current_data_values.v_in*10, MAGENTA, 2, 4, 1); //voltage
        drawFlt(66, rows[1], current_data_values.current_in*10, MAGENTA, 2, 4, 1); //current
        drawNum(0, rows[2], current_data_values.v_in * current_data_values.current_in, MAGENTA, 2, 4); //power

        // ----------------------------------
        // Calculating FPS
        // finish = time_us_64();
        // time = 1000000/(finish-start);
        // start = time_us_64();
        //printf("%d", time);
        //printf("\n");
	
    }
}

// MAIN FN 
int main()
{
	stdio_init_all();
	comm_uart_init();
	multicore_launch_core1(example_implementation);	
	while (true)
	{
		bldc_interface_get_values();
		sleep_ms(TIME_BETWEEN_REQUESTS_MS);
	}
}