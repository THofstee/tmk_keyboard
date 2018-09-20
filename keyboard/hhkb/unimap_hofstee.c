#include "unimap_trans.h"
#include "mousekey.h"


/* id for user defined functions */
enum function_id {
    FN0, FN2,
    MNU,
    STB,
};

#define AC_FN0     ACTION_FUNCTION_TAP(FN0)
#define AC_FN2     ACTION_FUNCTION_TAP(FN2)
#define AC_MNU     ACTION_FUNCTION_TAP(MNU) // TODO does this need to be TAP?
#define AC_STB     ACTION_FUNCTION_TAP(STB) // TODO does this need to be TAP?

#ifdef KEYMAP_SECTION_ENABLE
const action_t actionmaps[][UNIMAP_ROWS][UNIMAP_COLS] __attribute__ ((section (".keymap.keymaps"))) = {
#else
const action_t actionmaps[][UNIMAP_ROWS][UNIMAP_COLS] PROGMEM = {
#endif
    [0] = UNIMAP_HHKB(
    ESC, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   MINS,EQL, BSLS,GRV ,
    STB, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,RBRC,     BSPC,
    LCTL,A,   S,   D,   F,   G,   H,   J,   K,   L,   SCLN,QUOT,          ENT,
    LSFT,     Z,   X,   C,   V,   B,   N,   M,   COMM,DOT, SLSH,     RSFT,FN2,
         LGUI,LALT,               SPC,                     RALT,RGUI),

    [1] = UNIMAP_HHKB(
    TRNS,F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12, INS, DEL,
    TRNS,MUTE,VOLD,VOLU,TRNS,TRNS,TRNS,TRNS,PSCR,SLCK,PAUS,UP,  TRNS,     TRNS,
    TRNS,MPLY,MPRV,MNXT,TRNS,TRNS,PAST,PSLS,HOME,PGUP,LEFT,RGHT,          TRNS,
    TRNS,     TRNS,TRNS,TRNS,TRNS,TRNS,PPLS,PMNS,END, PGDN,DOWN,     TRNS,TRNS,
         TRNS,TRNS,               TRNS,                    TRNS,MNU),

    [2] = UNIMAP_HHKB(
    TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,
    TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,UP,  TRNS,     TRNS,
    TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,LEFT,RGHT,          TRNS,
    TRNS,     TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,DOWN,     TRNS,FN0,
         TRNS,TRNS,               TRNS,                    TRNS,TRNS),
};



/*
 * Macro definition
 */
const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    return MACRO_NONE;
}



/*
 * user defined action function
 */
void action_function(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    if (record->event.pressed) dprint("P"); else dprint("R");
    dprintf("%d", record->tap.count);
    if (record->tap.interrupted) dprint("i");
    dprint("\n");

    switch (id) {
        case FN0:
            if (record->event.pressed) {
                layer_switch(1);
            } else {
                if (record->tap.count >= TAPPING_TOGGLE && !(record->tap.interrupted)) {
                    dprint("Switching to layer 0.\n");
                    layer_switch(0);
                    record->tap.count = 0;  // ad hoc: cancel tap
                } else {
                    layer_switch(2);
                }
            }
            break;
        case FN2:
            if (record->event.pressed) {
                // TODO this has about a 300ms latency or so before it recognizes a chord in layer 1
                layer_switch(1);
            } else {
                if (record->tap.count >= TAPPING_TOGGLE && !(record->tap.interrupted)) {
                    dprint("Switching to layer 2.\n");
                    layer_switch(2);
                    record->tap.count = 0;  // ad hoc: cancel tap
                } else {
                    layer_switch(0);
                }
            }
            break;
        case MNU:
            // If tap.count is 0, then we know the action is a long press
            if (record->event.pressed) {
                if (record->tap.count > 0 && !record->tap.interrupted) {
                    register_code(KC_APP);
                } else {
                    // Both press and release are here because otherwise you
                    // have to release the key and it's not as intuitive
                    mousekey_on(KC_MS_BTN2);
                    mousekey_send();
                    mousekey_off(KC_MS_BTN2);
                    mousekey_send();
                }
            } else  {
                if (record->tap.count > 0 && !(record->tap.interrupted)) {
                    unregister_code(KC_APP);
                }
            }
            break;
        case STB:
            // Doing it this way makes it so double tapping TAB sends repeat TAB
            // TODO Is this actually the behavior we want?
            if (record->event.pressed) {
                // We want this behavior even if interrupted
                if (record->tap.count > 0) {
                    register_code(KC_TAB);
                } else {
                    add_weak_mods(MOD_BIT(KC_LSHIFT));
                    send_keyboard_report();
                    register_code(KC_TAB);
                    unregister_code(KC_TAB);
                    del_weak_mods(MOD_BIT(KC_LSHIFT));
                    send_keyboard_report();
                    record->tap.count = 0;  // ad hoc: cancel tap
                }
            } else {
                if (record->tap.count > 0) {
                    unregister_code(KC_TAB);
                }
            }
            break;
    }
}
