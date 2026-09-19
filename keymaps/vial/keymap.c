#include QMK_KEYBOARD_H

#include "analog.h"
#include "rgblight.h"
#include "split_util.h"

#define RELAY_PIN 11
#define RELAY_ON 1
#define RELAY_OFF 0

bool last_led_state;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, 		KC_6, KC_7, KC_8   , KC_9  , KC_0   , KC_DEL , 
				KC_TAB , KC_Q, KC_W, KC_E, KC_R, KC_T, 		KC_Y, KC_U, KC_I   , KC_O  , KC_P   , KC_ENT , 
				KC_LSFT, KC_A, KC_S, KC_D, KC_F, KC_G, 	    KC_H, KC_J, KC_K   , KC_L  , KC_QUOT, KC_RSFT, 
				KC_LCTL, KC_Z, KC_X, KC_C, KC_V, KC_B, 	    KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RCTL, 

				KC_SPACE, KC_SPACE, // TODO: Remove Joystick pushbutton incorrect
				KC_KB_MUTE, KC_MEDIA_PLAY_PAUSE, // Encoders

                // Thumb Clusters
				KC_LALT , KC_SPC, KC_LSFT,   			    KC_C, KC_SPC, KC_A, 
				KC_COMMA, KC_LALT, 							KC_V, KC_C),
};
    // [0] = LAYOUT(KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, 		KC_6, KC_7, KC_8   , KC_9  , KC_0   , KC_DEL , 
	// 			KC_TAB , KC_Q, KC_W, KC_E, KC_R, KC_T, 		KC_Y, KC_U, KC_I   , KC_O  , KC_P   , KC_ENT , 
	// 			KC_LSFT, KC_A, KC_S, KC_D, KC_F, KC_G, 	    KC_H, KC_J, KC_K   , KC_L  , KC_SCLN, KC_RSFT, 
	// 			KC_LCTL, KC_Z, KC_X, KC_C, KC_V, KC_B, 	    KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RCTL, 

	// 			KC_SPACE, KC_SPACE, // TODO: Remove Joystick pushbutton incorrect
	// 			KC_KB_MUTE, KC_MEDIA_PLAY_PAUSE, // Encoders

    //             // Thumb Clusters
	// 			KC_LALT , KC_SPC, KC_LSFT,   			    KC_C, KC_SPC, KC_A, 
	// 			KC_COMMA, JS_0, 	

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_RIGHT, KC_LEFT) },
};
#endif

void keyboard_post_init_user(void) {
    debug_enable=true;
    debug_matrix=true;

    last_led_state = rgblight_get_val() > 0 ? RELAY_ON : RELAY_OFF;
    gpio_set_pin_output(RELAY_PIN);
    gpio_write_pin(RELAY_PIN, last_led_state);
}

void housekeeping_task_user(void) {
    
    bool leds_on = rgblight_get_val() > 0 ? RELAY_ON : RELAY_OFF;
    if (leds_on != last_led_state) {
        last_led_state = leds_on;
        gpio_write_pin(RELAY_PIN, leds_on);
    }

    // Callibration
    // uint16_t x = analogReadPin(GP26);
    // uint16_t y = analogReadPin(GP27);

    // uprintf("Joystick ADC: X=%u Y=%u\n", x, y);
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_90;
}


bool oled_task_user(void) {
    oled_write_ln_P(PSTR("TWUBOARD"), false);
    oled_write_ln_P(PSTR("OLED TEST"), false);
    oled_write_ln_P(PSTR("SDA: GP8"), false);
    oled_write_ln_P(PSTR("SCL: GP9"), false);

    return false;
}

// Callibrate to your joysticks!
joystick_config_t joystick_axes[JOYSTICK_AXIS_COUNT] = {
    JOYSTICK_AXIS_IN(GP26, 280, 555, 810),
    JOYSTICK_AXIS_IN(GP27, 860, 563, 210),
};

// #define JOYSTICK_MAX ((1 << (JOYSTICK_AXIS_RESOLUTION - 1)) - 1)

// static int16_t joystick_from_adc(uint16_t value, uint16_t low, uint16_t center, uint16_t high) {
//     int32_t result;

//     if (value < center) {
//         result = ((int32_t)value - center) * JOYSTICK_MAX / (center - low);
//     } else {
//         result = ((int32_t)value - center) * JOYSTICK_MAX / (high - center);
//     }

//     if (result < -JOYSTICK_MAX) {
//         result = -JOYSTICK_MAX;
//     } else if (result > JOYSTICK_MAX) {
//         result = JOYSTICK_MAX;
//     }

//     return (int16_t)result;
// }

// Pins are shared across both halves
// #define X_PIN GP26
// #define Y_PIN GP27

// #define RIGHT_X_REST 510
// #define RIGHT_Y_REST 512
// #define RIGHT_X_DEADZONE 150
// #define RIGHT_Y_DEADZONE 150

// static uint8_t right_numpad_key = KC_NO;

// void matrix_scan_user(void) {

//     if (is_keyboard_left()) {
//         joystick_set_axis(
//             0, joystick_from_adc(analogReadPin(X_PIN), 280, 555, 810)
//         );
//         joystick_set_axis(
//             1, joystick_from_adc(analogReadPin(Y_PIN), 860, 563, 210)
//         );
//         return;
//     }

//     if (!is_keyboard_left()) {

//         uint16_t x = analogReadPin(X_PIN);
//         uint16_t y = analogReadPin(Y_PIN);

//         bool left  = x < (RIGHT_X_REST - RIGHT_X_DEADZONE);
//         bool right = x > (RIGHT_X_REST + RIGHT_X_DEADZONE);

//         bool up    = y > (RIGHT_Y_REST + RIGHT_Y_DEADZONE);
//         bool down  = y < (RIGHT_Y_REST - RIGHT_Y_DEADZONE);

//         uint8_t new_key = KC_NO;

//         // Diagonals
//         if (up && left) {
//             new_key = KC_P7;
//         } else if (up && right) {
//             new_key = KC_P9;
//         } else if (down && left) {
//             new_key = KC_P1;
//         } else if (down && right) {
//             new_key = KC_P3;
//         }

//         // Cardinal directions
//         else if (up) {
//             new_key = KC_P8;
//         } else if (down) {
//             new_key = KC_P2;
//         } else if (left) {
//             new_key = KC_P4;
//         } else if (right) {
//             new_key = KC_P6;
//         }

//         // Change the currently held key
//         if (new_key != right_numpad_key) {

//             if (right_numpad_key != KC_NO) {
//                 unregister_code(right_numpad_key);
//             }

//             if (new_key != KC_NO) {
//                 register_code(new_key);
//             }

//             right_numpad_key = new_key;
//         }
//     }
// }
