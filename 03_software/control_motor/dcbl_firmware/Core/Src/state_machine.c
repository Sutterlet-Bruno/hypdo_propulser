/*
 * state_machine.c
 *
 *  Created on: Sep 4, 2025
 *      Author: Bruno
 *     Version: 0.0.1
 *
 * Description:
 *   BLDC commutation state machine with timing control.
 *   Assumption: T1..T6 map to S1..S6 as follows:
 *     T1=U+, T2=U−, T3=V+, T4=V−, T5=W+, T6=W−
 */

#include "state_machine.h"
#include "main.h"   // HAL_GPIO_WritePin(), GPIO ports/pins

// Pin mapping
#define T1_PORT GPIOC
#define T1_PIN  GPIO_PIN_15

#define T2_PORT GPIOB
#define T2_PIN  GPIO_PIN_3

#define T3_PORT GPIOA
#define T3_PIN  GPIO_PIN_5

#define T4_PORT GPIOB
#define T4_PIN  GPIO_PIN_4

#define T5_PORT GPIOC
#define T5_PIN  GPIO_PIN_14

#define T6_PORT GPIOB
#define T6_PIN  GPIO_PIN_12

// Helpers
static inline void all_transistors_off(void)
{
    HAL_GPIO_WritePin(T1_PORT, T1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(T2_PORT, T2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(T3_PORT, T3_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(T4_PORT, T4_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(T5_PORT, T5_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(T6_PORT, T6_PIN, GPIO_PIN_RESET);
}

static inline void turn_on(GPIO_TypeDef *port, uint16_t pin)
{
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
}

int8_t state_machine(uint32_t current_time, uint32_t commutation_time)
{
    static int8_t   current_state     = 0;    // -1:Error, 0:IDLE, 1..6
    static uint32_t last_switch_time  = 0;

    int8_t   ret     = 0;
    uint32_t elapsed = 0;

    // Error
    if (current_state == -1) {
        return -1;
    }

    // IDLE → utiliser 6 comme pré-état pour commencer proprement
    if (current_state == 0) {
        current_state    = 6;
        last_switch_time = current_time - commutation_time;
    }

    // State machine
    elapsed = current_time - last_switch_time;
    if (elapsed >= commutation_time) {
        current_state    = (current_state == 6) ? 1 : (int8_t)(current_state + 1);
        last_switch_time = current_time;

        // Toujours: d'abord tout à LOW, puis activer la paire de l'état
        all_transistors_off();

        switch (current_state) {
            case 1: // ON: S4(V−)=T4, S5(W+)=T5
                turn_on(T4_PORT, T4_PIN);
                turn_on(T5_PORT, T5_PIN);
                break;

            case 2: // ON: S1(U+)=T1, S4(V−)=T4
                turn_on(T1_PORT, T1_PIN);
                turn_on(T4_PORT, T4_PIN);
                break;

            case 3: // ON: S1(U+)=T1, S6(W−)=T6
                turn_on(T1_PORT, T1_PIN);
                turn_on(T6_PORT, T6_PIN);
                break;

            case 4: // ON: S3(V+)=T3, S6(W−)=T6
                turn_on(T3_PORT, T3_PIN);
                turn_on(T6_PORT, T6_PIN);
                break;

            case 5: // ON: S2(U−)=T2, S3(V+)=T3
                turn_on(T2_PORT, T2_PIN);
                turn_on(T3_PORT, T3_PIN);
                break;

            case 6: // ON: S2(U−)=T2, S5(W+)=T5
                turn_on(T2_PORT, T2_PIN);
                turn_on(T5_PORT, T5_PIN);
                break;

            default:
                current_state = -1;
                return -1;
        }
    }

    ret = current_state;
    return ret;
}
