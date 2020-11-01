// ABX switch box (tayda version).
// Copyright 2020 Jason Pepas
// Released under the terms of the MIT license.
// See https://opensource.org/licenses/MIT

// Forum thread:
// https://www.diyaudio.com/forums/equipment-and-tools/361743-abx-box.html


#include <EEPROM.h>


// Pin mapping:

#define LED_A 2
#define LED_B 3
#define LED_X 4
#define LED_Y 5

#define PCH1 A3
#define NCH1 10
#define PCH2 9
#define NCH2 A2
#define PCH3 A5
#define NCH3 A6
#define PCH4 A7
#define NCH4 A4
#define PCH5 11
#define NCH5 A0
#define PCH6 A1
#define NCH6 12

#define MODE_PIN_REROLL 8
#define MODE_PIN_REVEAL 7
#define MODE_PIN_SWITCH 6


// Misc. constants:

#define MAPPING_A_IS_X 101
#define MAPPING_A_IS_Y 102
#define MAPPING_B_IS_X 102
#define MAPPING_B_IS_Y 101

#define POSITION_X 103
#define POSITION_Y 104

#define RELAY_DURATION 4


// Global variables:

byte g_eeprom_initialized;
byte g_random_seed;
byte g_random_seq;
byte g_abxy_mapping;
byte g_input_position;
byte g_dummy_position;


// EEPROM addresses

#define G_EEPROM_INITIALIZED_ADDR 0
#define G_RANDOM_SEED_ADDR 1
#define G_RANDOM_SEQ_ADDR 2
#define G_ABXY_MAPPING_ADDR 3
#define G_INPUT_POSITION_ADDR 4
#define G_DUMMY_POSITION_ADDR 5


// Misc. functions:

void panic() {
    pinMode(LED_A, OUTPUT);
    pinMode(LED_B, OUTPUT);
    while (true) {
        digitalWrite(LED_A, HIGH);
        digitalWrite(LED_B, HIGH);
    }
}


// EEPROM functions

void load_globals_from_eeprom() {
    g_eeprom_initialized = EEPROM.read(G_EEPROM_INITIALIZED_ADDR);
    if (g_eeprom_initialized != 42) {
        // this is the first time we have ever been run.  initialize all state.
        EEPROM.write(G_EEPROM_INITIALIZED_ADDR, 42);
        EEPROM.write(G_RANDOM_SEED_ADDR, 0);
        EEPROM.write(G_RANDOM_SEQ_ADDR, 0);
        EEPROM.write(G_ABXY_MAPPING_ADDR, MAPPING_A_IS_X);
        EEPROM.write(G_INPUT_POSITION_ADDR, POSITION_X);
        EEPROM.write(G_DUMMY_POSITION_ADDR, POSITION_X);
        disconnect_output_relays();
        set_input_relays_to_x();
        set_dummy_relays_to_x();
        connect_output_relays();
    }

    g_random_seed = EEPROM.read(G_RANDOM_SEED_ADDR);
    g_random_seq = EEPROM.read(G_RANDOM_SEQ_ADDR);
    g_abxy_mapping = EEPROM.read(G_ABXY_MAPPING_ADDR);
    g_input_position = EEPROM.read(G_INPUT_POSITION_ADDR);
    g_dummy_position = EEPROM.read(G_DUMMY_POSITION_ADDR);
}

void store_globals_in_eeprom() {
    EEPROM.write(G_RANDOM_SEED_ADDR, g_random_seed);
    EEPROM.write(G_RANDOM_SEQ_ADDR, g_random_seq);
    EEPROM.write(G_ABXY_MAPPING_ADDR, g_abxy_mapping);
    EEPROM.write(G_INPUT_POSITION_ADDR, g_input_position);
    EEPROM.write(G_DUMMY_POSITION_ADDR, g_dummy_position);
}


// LED functions:

void turn_on_LED(int led) {
    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH);
}

void turn_off_LED(int led) {
    pinMode(led, INPUT);
}


// Relay functions:

void set_input_relays_to_x() {
    pinMode(PCH1, OUTPUT);
    digitalWrite(PCH1, LOW);
    pinMode(NCH1, OUTPUT);
    digitalWrite(NCH1, HIGH);
    delay(RELAY_DURATION);
    pinMode(PCH1, INPUT);
    pinMode(NCH1, INPUT);
    delay(1);
    g_input_position = POSITION_X;
}

void set_input_relays_to_y() {
    pinMode(PCH2, OUTPUT);
    digitalWrite(PCH2, LOW);
    pinMode(NCH2, OUTPUT);
    digitalWrite(NCH2, HIGH);
    delay(RELAY_DURATION);
    pinMode(PCH2, INPUT);
    pinMode(NCH2, INPUT);
    delay(1);
    g_input_position = POSITION_Y;
}

void set_dummy_relays_to_x() {
    pinMode(PCH5, OUTPUT);
    digitalWrite(PCH5, LOW);
    pinMode(NCH5, OUTPUT);
    digitalWrite(NCH5, HIGH);
    delay(RELAY_DURATION);
    pinMode(PCH5, INPUT);
    pinMode(NCH5, INPUT);
    delay(1);
    g_dummy_position = POSITION_X;
}

void set_dummy_relays_to_y() {
    pinMode(PCH6, OUTPUT);
    digitalWrite(PCH6, LOW);
    pinMode(NCH6, OUTPUT);
    digitalWrite(NCH6, HIGH);
    delay(RELAY_DURATION);
    pinMode(PCH6, INPUT);
    pinMode(NCH6, INPUT);
    delay(1);
    g_dummy_position = POSITION_Y;
}

void toggle_input_relays() {
    if (g_input_position == POSITION_X) {
        set_input_relays_to_y();
    } else if (g_input_position == POSITION_Y) {
        set_input_relays_to_x();
    } else {
        panic();
    }
}

void toggle_dummy_relays() {
    if (g_dummy_position == POSITION_X) {
        set_dummy_relays_to_y();
    } else if (g_dummy_position == POSITION_Y) {
        set_dummy_relays_to_x();
    } else {
        panic();
    }
}

void connect_output_relays() {
    pinMode(PCH3, OUTPUT);
    digitalWrite(PCH3, LOW);
    pinMode(NCH3, OUTPUT);
    digitalWrite(NCH3, HIGH);
    delay(RELAY_DURATION);
    pinMode(PCH3, INPUT);
    pinMode(NCH3, INPUT);
    delay(1);
}

void disconnect_output_relays() {
    pinMode(PCH4, OUTPUT);
    digitalWrite(PCH4, LOW);
    pinMode(NCH4, OUTPUT);
    digitalWrite(NCH4, HIGH);
    delay(RELAY_DURATION);
    pinMode(PCH4, INPUT);
    pinMode(NCH4, INPUT);
    delay(1);
}


// Primary functions:

int get_next_random_mapping() {
    int new_mapping;
    // use up to 100 numbers from each seed.
    if (g_random_seq >= 100) {
        g_random_seed++;
        g_random_seq = 0;
    }
    randomSeed(g_random_seed);
    // skip over the previously used numbers from this seed.
    for (int skip=0; skip < g_random_seq; skip++) {
        new_mapping = random(MAPPING_A_IS_X, MAPPING_A_IS_Y+1);
    }
    new_mapping = random(MAPPING_A_IS_X, MAPPING_A_IS_Y+1);
    g_random_seq++;
    return new_mapping;
}

void do_reroll() {
    g_abxy_mapping = get_next_random_mapping();

    bool should_toggle_odd;
    if (g_abxy_mapping == MAPPING_A_IS_X && g_input_position == POSITION_X) {
        should_toggle_odd = false;
    } else if (g_abxy_mapping == MAPPING_A_IS_X && g_input_position == POSITION_Y) {
        should_toggle_odd = true;
    } else if (g_abxy_mapping == MAPPING_B_IS_X && g_input_position == POSITION_X) {
        should_toggle_odd = true;
    } else if (g_abxy_mapping == MAPPING_B_IS_X && g_input_position == POSITION_Y) {
        should_toggle_odd = false;
    } else {
        panic();
    }

    disconnect_output_relays();

    if (should_toggle_odd) {
        toggle_dummy_relays();
        toggle_input_relays();
        toggle_dummy_relays();
    } else {
        toggle_input_relays();
        toggle_dummy_relays();
        toggle_input_relays();
    }

    store_globals_in_eeprom();

    connect_output_relays();

    while (true) {
        turn_on_LED(LED_A);
    }
}

void do_reveal() {
    while (true) {
        if (g_input_position == POSITION_X && g_abxy_mapping == MAPPING_A_IS_X) {
            turn_on_LED(LED_X);
            turn_on_LED(LED_A);
        } else if (g_input_position == POSITION_X && g_abxy_mapping == MAPPING_B_IS_X) {
            turn_on_LED(LED_X);
            turn_on_LED(LED_B);
        } else if (g_input_position == POSITION_Y && g_abxy_mapping == MAPPING_A_IS_Y) {
            turn_on_LED(LED_Y);
            turn_on_LED(LED_A);
        } else if (g_input_position == POSITION_Y && g_abxy_mapping == MAPPING_B_IS_Y) {
            turn_on_LED(LED_Y);
            turn_on_LED(LED_B);
        } else {
            panic();
        }
    }
}

void do_switch() {
    disconnect_output_relays();

    if (g_input_position == POSITION_X) {
        set_input_relays_to_y();
    } else if (g_input_position == POSITION_Y) {
        set_input_relays_to_x();
    } else {
        panic();
    }

    store_globals_in_eeprom();

    connect_output_relays();

    while (true) {
        if (g_input_position == POSITION_X && g_abxy_mapping == MAPPING_A_IS_X) {
            turn_on_LED(LED_A);
        } else if (g_input_position == POSITION_X && g_abxy_mapping == MAPPING_B_IS_X) {
            turn_on_LED(LED_B);
        } else if (g_input_position == POSITION_Y && g_abxy_mapping == MAPPING_A_IS_X) {
            turn_on_LED(LED_B);
        } else if (g_input_position == POSITION_Y && g_abxy_mapping == MAPPING_B_IS_X) {
            turn_on_LED(LED_A);
        } else {
            panic();
        }
    }
}


// Arduino functions:

void setup() {
    // Wait long enough to get past any initial switch bouncing.
    delay(1);

    load_globals_from_eeprom();

    if (digitalRead(MODE_PIN_REROLL)) {
        do_reroll();
    } else if (digitalRead(MODE_PIN_REVEAL)) {
        do_reveal();
    } else if (digitalRead(MODE_PIN_SWITCH)) {
        do_switch();
    } else {
        panic();
    }
}

void loop() {

}
