#ifndef VOODOO_I2C_UPDD_CLIENT
#define VOODOO_I2C_UPDD_CLIENT

#include "updd_data.h"

#define GESTURE_CTL_NAME "com.alexandred.VoodooI2C.GestureSocket"

enum gesture_socket_cmd_type {
    GESTURE_DATA,
    GESTURE_QUIT
};

struct gesture_socket_cmd {
    enum gesture_socket_cmd_type type;
    struct updd_data gesture;
};

#endif
