/*------------------------------------------------------------------
 *  in4073.c -- test QR engines and sensors
 *
 *  reads ae[0-3] uart rx queue
 *  (q,w,e,r increment, a,s,d,f decrement)
 *
 *  prints timestamp, ae[0-3], sensors to uart tx queue
 *
 *  I. Protonotarios
 *  Embedded Software Lab
 *
 *  June 2016
 *------------------------------------------------------------------
 */

#include "in4073.h"

/*------------------------------------------------------------------
 * main -- everything you need is here :)
 *------------------------------------------------------------------
 */
int main(void)
{
	uart_init();
	gpio_init();
	timers_init();
	adc_init();
	twi_init();
	imu_init(true, 100);
	baro_init();
	spi_flash_init();
	ble_init();
	data_log_init();	//Initialize data logging 

	uint32_t counter = 0;
	demo_done = false;
	timeSpent=0;
	uartTimeout = 0;
	panicTimeout = 0;
	system_time=0;
	uint32_t address = FLASH_START_ADDRESS;
	calibINrawMode = false;
	desiredPressure = 0;

	while (!demo_done) {

		if (ble_rx_queue.count && smCurrState == S8_WIR)
		{
			setrxByte(dequeue(&ble_rx_queue));	//Read the Bluetooth input	
		} else if (rx_queue.count) {
			// timeSpent = get_time_us();	// For debugging
			setrxByte(dequeue(&rx_queue));	//Read the UART input		    
			// timeSpent = get_time_us()-timeSpent; // For debugging
			// if (timeSpent > 100) // For debugging
				// sendMessageEs(MSG08_TIMER_VALUE); // For debugging
		}

		if (check_timer_flag()) {

			// Go into panic mode if connection was lost.
			// A message is still sent in case the timeout happened
			// for some other reason and the serial connection is still ok.
			// If battery voltage is less than 10.5v, it goes to panic mode but enable this 
			//only when drone is connected, otherwise it goes to panic mode everytime.
			if(uartTimeout >= UART_TIMEOUT_PANIC) { // || bat_volt < 1050) {
				TransValid result = switchState(S1_PAN);
				// if (bat_volt<1050)
				// {
				// 	sendMessageEs(MSG07_STRING);
				// }
				if(result == TRANS_OK) {
					sendMessageEs(MSG02_CURR_STATE);
				}	
			}
			uartTimeout += 1;

			// Panic mode timeout
			if(smCurrState == S1_PAN) {
				if(panicTimeout >= PANIC_MODE_TIMEOUT) {
					TransValid result = switchState(S0_SAF);
					if(result == TRANS_OK) {
						
						
						sendMessageEs(MSG02_CURR_STATE);
					}
				}
				panicTimeout += 1;
			}

			
			// Periodic Telemetry messages
			if(counter % 2 == 0) {
				sendMessageEs(MSG05_SENSOR_DATA);
				sendMessageEs(MSG06_ADC_DATA);
				sendMessageEs(MSG09_ROLLPITCHYAW);
				sendMessageEs(MSG04_DISP_MOTOR);
			}

			// LEDs
			if (counter % 20 == 0) {
				nrf_gpio_pin_toggle(BLUE);	
			}  
			if(smCurrState == S1_PAN) {
				nrf_gpio_pin_toggle(RED);
			}
			
			counter += 1;

			adc_request_sample();
			read_baro();

			clear_timer_flag();
			}

		if (check_sensor_int_flag()) {

		/* Get the raw data or DMP data depending on the mode */
			if (smCurrState==S6_RAW || calibINrawMode==true)
			{
				get_raw_sensor_data();							
			}else{
				get_dmp_data();
      		}

		/* Calibrate if in mode 3 */
			if(smCurrState == S3_CAL) {
				calibrateDmp();
			}
		/* Log the data if requested */
			if (!flag_logging_done) {
				system_time = get_time_us();
				log_one_packet(&address);
			}
		/* Run the control loop now */
			run_filters_and_control();
		}
	}
	
	nrf_delay_ms(100);
	NVIC_SystemReset();
}
