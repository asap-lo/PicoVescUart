#include <stdio.h>

#include "PicoVescUart.h"
#include "vesc/bldc_interface.h"

void on_value_received(mc_values *val)
{
	current_data_values = *val;  // Make available to rest of application
    
    // Print out members of `mc_values` struct. See `vesc/datatypes.h` for more.
    printf("Input voltage: %.2f V\r\n", val->v_in);
    printf("Temp mos:    %.2f degC\r\n", val->temp_mos);
	printf("Temp motor:  %.2f degC\r\n", val->temp_motor);
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

int main() {
    comm_uart_init(on_value_received); // pass function as a parameter. 

    // Your application main loop/other code here!
    while(1) {}
}