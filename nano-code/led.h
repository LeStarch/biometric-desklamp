/*
 * led.h
 *
 *  Created on: Oct 14, 2016
 *      Author: starchmd
 */
#ifndef LED_H_
#define LED_H_
    //Maximum power output percentage (used to limited dangerous leds)
    #define MAX_RATE 66
    //Number of steps to cycle through
    #define TOGGLE_STEPS 3
	/**
	 * Class used to represent the Led module and its cooling fan
	 */
    class Led
    {
        private:
            int led;
            int fan;
    	    uint32_t rate;
        public:
    	    /**
    	     * Construct this led-module and fan around the supplied pins
    	     */
            Led(int led, int fan);
            /**
             * Toggles the led and fan to the next in a certain number of steps
             */
            void toggle();
    };
#endif /* LED_H_ */
