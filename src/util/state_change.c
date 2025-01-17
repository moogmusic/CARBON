/*
 * CARBON State Change Dispatch System
 *
 * Written by: Andrew Kilpatrick
 * Copyright 2016: Kilpatrick Audio
 *
 * This file is part of CARBON.
 *
 * CARBON is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CARBON is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CARBON.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "state_change.h"
#include "log.h"
#include <stdlib.h>

// settings
#define STATE_CHANGE_NUM_REGISTERS 256

// state
struct state_change {
    void (*handler[STATE_CHANGE_NUM_REGISTERS])(int event_type, int *data, int data_len);
    int event_class_map[STATE_CHANGE_NUM_REGISTERS];
};
struct state_change schstate;

// init the state change system
void state_change_init(void) {
    int i;
    for(i = 0; i < STATE_CHANGE_NUM_REGISTERS; i ++) {
        schstate.handler[i] = NULL;
        schstate.event_class_map[i] = -1;
    }
}

// register a listener
void state_change_register(void (*handler)(int, int*, int), int event_class) {
    int i;
    // search for a free slot and insert it
    for(i = 0; i < STATE_CHANGE_NUM_REGISTERS; i ++) {
        if(schstate.handler[i] == NULL) {
            schstate.handler[i] = (void (*)(int, int*, int))handler;
            schstate.event_class_map[i] = event_class;
            return;
        }
    }
    log_error("scr - no more handler slots for event_class: %d", event_class);
}

// fire an event
void state_change_fire(int event_type, int *data, int data_len) {
    int i;
    int cls = event_type & 0xff0000;

    // search through registered handlers and fire
    for(i = 0; i < STATE_CHANGE_NUM_REGISTERS; i ++) {
        if(schstate.handler[i] == NULL) {
            continue;
        }
        else if(schstate.event_class_map[i] == cls) {
            (*schstate.handler[i])(event_type, data, data_len);  // call the stored callback
        }
    }
}

// fire an event with no arguments
void state_change_fire0(int event_type) {
    state_change_fire(event_type, NULL, 0);
}

// fire an event with 1 argument
void state_change_fire1(int event_type, int data0) {
    int data[1];
    data[0] = data0;
    state_change_fire(event_type, data, 1);
}

// fire an event with 2 arguments
void state_change_fire2(int event_type, int data0, int data1) {
    int data[2];
    data[0] = data0;
    data[1] = data1;
    state_change_fire(event_type, data, 2);
}

// fire an event with 3 arguments
void state_change_fire3(int event_type, int data0, int data1, int data2) {
    int data[3];
    data[0] = data0;
    data[1] = data1;
    data[2] = data2;
    state_change_fire(event_type, data, 3);
}

