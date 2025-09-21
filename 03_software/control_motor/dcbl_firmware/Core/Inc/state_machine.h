/*
 * state_machine.h
 *
 *  Created on: Sep 4, 2025
 *      Author: Bruno
 *     Version: 0.0.1
 *
 * Description:
 *   This function implements the state machine that determines
 *   which commutation step should be applied to the motor phases.
 *
 *   Input:
 *     current_time : uint32_t (system time in ms)
 *     commutation_time : uint_32 (time interval between two commutions)
 *
 *   Output:
 *     int8_t
 *       -1 : Error
 *        0 : IDLE
 *      1-6 : Current commutation state
 */

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdint.h>

int8_t state_machine(uint32_t current_time, uint32_t commutation_time);

#endif // STATE_MACHINE_H
