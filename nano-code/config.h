/*
 * config.h
 *
 *  Created on: Oct 14, 2016
 *      Author: lestarch
 */
#ifndef CONFIG_H_
#define CONFIG_H_
	//Pin used to drive the LED module
    #define LED_PIN 23
    //Pin used to drive the cooling fan
    #define FAN_PIN 22
    //Time in milliseconds to allow for enrollment
    #define ENROLL_PERIOD_TIME 1000
	//Time to delay after light is toggled
    #define TOGGLE_DELAY 500
    //Time to delay on startup
	#define START_UP_DELAY 500

	//Print debug statements to USB serial
	//#define DEBUG
#endif /* CONFIG_H_ */
