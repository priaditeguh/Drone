/*------------------------------------------------------------------
 *  mpu_wrapper.c -- invensense sdk setup
 *
 *  I. Protonotarios
 *  Embedded Software Lab
 *
 *  July 2016
 *------------------------------------------------------------------
 */
#include "in4073.h"

#define QUAT_SENS       0x040000000 //1073741824.f //2^30

void Kalmanfilter_phi_theta();
//Kalman constants
int16_t sptemp[2] = {0}, phi_kalmantemp[2] = {0},p_biastemp[2] = {0};
int16_t sqtemp[2] = {0}, theta_kalmantemp[2] = {0},q_biastemp[2] = {0};
int16_t p_q_kalman;
int32_t phi_theta_error;

/* initializeValues()
 * Author: 		Prashanth G L
 * Function: 	Initialize the values required for Kalman filter (roll/pitch)
*/
void initializeValues(){

for (uint8_t i = 0; i < 2; ++i)
{
	sptemp[i] = 0;
	sqtemp[i] = 0;
	phi_kalmantemp[i] = 0;
	theta_kalmantemp[i] = 0;
	p_biastemp[i] = 0;
	q_biastemp[i] = 0;
}
}

/* initializeValues()
 * Author: 		Prashanth G L
 * Function: 	Kalman filter implementation for roll and pitch,
 				Stores the filtered vale in phi and theta variable, so that
 				these values can be directly used as in DMP mode.
*/
void Kalmanfilter_phi_theta(){

//Kalman filter for roll & pitch control

//Roll control
	
//History update
	    sptemp[1] = sptemp[0];
	    phi_kalmantemp[1] = phi_kalmantemp[0];
	    p_biastemp[1] = p_biastemp[0];

	    sptemp[0] = sp;

	    p_q_kalman = sptemp[1] - p_biastemp[1];
	    phi_kalmantemp[0] = phi_kalmantemp[1] + (p_q_kalman >>7);    //shift 3 right so that p2phi = 0.0081 * 2^10, then shift right 10 times
	    phi_theta_error = phi_kalmantemp[0] - say;
	    phi_kalmantemp[0] = phi_kalmantemp[0] - (phi_theta_error>>8);
	    //p_bias1 = biasholder + ((phi_error1/p2phi)/1048576);
	    p_biastemp[0] = p_biastemp[1] + (phi_theta_error/8100); //2^20 * 0.0081 = 8494, log(8494)base2 = 13

	    sp = p_q_kalman;

//Pitch control

//History update
		sqtemp[1] = sqtemp[0];
	    theta_kalmantemp[1] = theta_kalmantemp[0];
	    q_biastemp[1] = q_biastemp[0];

	    sqtemp[0] = sq;

	    p_q_kalman = sqtemp[1] - q_biastemp[1];
	    theta_kalmantemp[0] = theta_kalmantemp[1] + (p_q_kalman >>7);    //shift 3 right so that p2phi = 0.0081 * 2^10, then shift right 10 times
	    phi_theta_error = theta_kalmantemp[0] - sax;
	    theta_kalmantemp[0] = theta_kalmantemp[0] - (phi_theta_error>>8);
	    //p_bias1 = biasholder + ((phi_error1/p2phi)/1048576);
	    q_biastemp[0] = q_biastemp[1] + (phi_theta_error/8100); //2^20 * 0.0081 = 8494, log(8494)base2 = 13

/*Place the filtered results in phi and theta */
	    sq = p_q_kalman;
	    phi = phi_kalmantemp[0];
	    theta = theta_kalmantemp[0];
}

void update_euler_from_quaternions(int32_t *quat) 
{
	float q[4];

	q[0] = (float)quat[0]/QUAT_SENS;
	q[1] = (float)quat[1]/QUAT_SENS;
	q[2] = (float)quat[2]/QUAT_SENS;
	q[3] = (float)quat[3]/QUAT_SENS;

	phi = (int16_t) (atan2(2.0*(q[2]*q[3] + q[0]*q[1]), q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3])*10430.0);
	theta = (int16_t) (-1.0 * asin(-2.0*(q[1]*q[3] - q[0]*q[2]))*10430.0);
	psi = (int16_t) (atan2(2.0*(q[1]*q[2] + q[0]*q[3]), q[0]*q[0] + q[1]*q[1] - q[2]*q[2] - q[3]*q[3])*10430.0);
}

// reading & conversion takes 3.2 ms!!! hurray (still lots of time till 10)
void get_dmp_data(void)
{
	int8_t read_stat;
	int16_t gyro[3], accel[3], dmp_sensors;
	int32_t quat[4];

	if (!(read_stat = dmp_read_fifo(gyro, accel, quat, NULL, &dmp_sensors, &sensor_fifo_count)))
	{
		update_euler_from_quaternions(quat);
		if (dmp_sensors & INV_XYZ_GYRO)
		{
			sp = gyro[0];
			sq = gyro[1];
			sr = gyro[2];
		}
		if (dmp_sensors & INV_XYZ_ACCEL)
		{
			sax = accel[0];
			say = accel[1];
			saz = accel[2];
		}
	}
	else printf("Error reading sensor fifo: %d\n", read_stat);
}

void get_raw_sensor_data(void)
{
	int8_t read_stat;
	int16_t gyro[3], accel[3];
	uint8_t sensors;

	if (!(read_stat = mpu_read_fifo(gyro, accel, NULL, &sensors, &sensor_fifo_count)))
	{
		if (sensors & INV_XYZ_GYRO) //16.4 LSB/deg/s (+-2000 deg/s)  You might need this for Kalman
		{
			sp = gyro[0];
			sq = gyro[1];
			sr = gyro[2];
		}
		if (sensors & INV_XYZ_ACCEL) //	16384 LSB/g (+-2g)  You might need this for Kalman
		{
			sax = accel[0];
			say = accel[1];
			saz = accel[2];
		}
	}
	/* call the Kalman filter now to filter the raw data */
	Kalmanfilter_phi_theta();
}



bool check_sensor_int_flag(void)
{
	if (nrf_gpio_pin_read(INT_PIN) || sensor_fifo_count) return true;
	return false;
}

void imu_init(bool dmp, uint16_t freq)
{
	static int8_t gyro_orientation[9] = {	1, 0, 0,
											0, 1, 0,
											0, 0, 1	};

	// tap feature is there to set freq to 100Hz, a bug provided by invensense :)
	uint16_t dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL | DMP_FEATURE_TAP;

	// //mpu	
	// printf("\rmpu init result: %d\n", mpu_init(NULL));
	// printf("\rmpu set sensors: %d\n", mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL));
	// printf("\rmpu conf fifo  : %d\n", mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL));

	//mpu	
	mpu_init(NULL);
	mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
	mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);

	mpu_set_int_level(0);
	mpu_set_int_latched(1);

	if (dmp)
	{
		// printf("\r\ndmp load firm  : %d\n", dmp_load_motion_driver_firmware());
		// printf("\rdmp set orient : %d\n", dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation)));
		// printf("\rdmp en features: %d\n", dmp_enable_feature(dmp_features));
		// printf("\rdmp set rate   : %d\n", dmp_set_fifo_rate(100));
		// printf("\rdmp set state  : %d\n", mpu_set_dmp_state(1));
		// printf("\rdlpf set freq  : %d\n", mpu_set_lpf(10));

		dmp_load_motion_driver_firmware();
		dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation));
		dmp_enable_feature(dmp_features);
		dmp_set_fifo_rate(100);
		mpu_set_dmp_state(1);
		mpu_set_lpf(10);
	} else {
		unsigned char data = 0;
		// printf("\rdisable dlpf   : %d\n", i2c_write(0x68, 0x1A, 1, &data));
		// // if dlpf is disabled (0 or 7) then the sample divider that feeds the fifo is 8kHz (derrived from gyro).
		// data = 8000 / freq - 1;
		// printf("\rset sample rate: %d\n", i2c_write(0x68, 0x19, 1, &data));

		i2c_write(0x68, 0x1A, 1, &data);
		// if dlpf is disabled (0 or 7) then the sample divider that feeds the fifo is 8kHz (derrived from gyro).
		data = 8000 / freq - 1;
		i2c_write(0x68, 0x19, 1, &data);
	}
}