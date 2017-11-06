#include "upddapi.h"
#include "updd.h"
#include "VoodooI2CUPDDData.h"

#include <string.h>
#include <iostream>
#include <CoreGraphics/CoreGraphics.h>
#include <stdio.h>
#include <unistd.h>
#include <mutex>

using namespace std;

bool keep_running = true;
bool updd_connected = false;

#define MAX_FINGERS 5
#define SCREEN_NORMALISE(value, physical_size, screen_size) ((float)value / physical_size) * screen_size
static mutex inject_touch_lock;

static struct updd_data ud_old;
static bool multi_touch = false;
static void TBCALL connected_callback(unsigned long context, struct _PointerEvent* data);

// connect to the UPDD driver
void updd_start() {
	TBApiRegisterEvent(0, 0, _EventConfiguration, connected_callback);

	TBApiOpen();

	for(;;) {
		if(updd_connected) break;

		usleep(100);
	}

	ud_old.current_x[0] = -1;
	ud_old.current_y[0] = -1;
}



void normalised_inject_touch(int x, int y, int resx, int resy, int finger, bool touching) {
	HTBDEVICE device = TBApiGetRelativeDevice(0);

    auto mainDisplayId = CGMainDisplayID();
    int scrnwidth = CGDisplayPixelsWide(mainDisplayId);
    int scrnheight = CGDisplayPixelsHigh(mainDisplayId);
    
    inject_touch_lock.lock();
	float x1 = (x * 1.0f / resx) * 1368;
	float y2 = (y * 1.0f / resy) * 912;

	TBApiInjectTouch(device, (int)x1, (int)y2, finger, touching);
	inject_touch_lock.unlock();
}


void inject_touch(struct updd_data* ud) {
	if(!updd_connected) return;
    
    if (ud->finger_lift) {
        for(int i = 0; i < MAX_FINGERS; i++) {
            normalised_inject_touch(ud_old.current_x[i], ud_old.current_y[i], ud->logical_x, ud->logical_y, i, false);
        }
        ud->finger_lift = false;
        delete ud;
        return;
    }
	
	if(ud_old.current_x[0] == -1 && ud_old.current_y[0] == -1) {
		
		memcpy(&ud_old, ud, sizeof(struct updd_data));
		
		delete ud;

		return;
	}

	bool pressed = false;
	int nfingers = 0;
    for (int i = 0;i < MAX_FINGERS;i++) {
        if (ud->current_x[i] != -1)
            nfingers++;
    }

    
        if(nfingers > 1) {
			for(int i = 0; i < nfingers; i++) {
				normalised_inject_touch(ud->current_x[i], ud->current_y[i], ud->logical_x, ud->logical_y, i, true);
			}
		}


	inject_touch_lock.lock();
	memcpy(&ud_old, ud, sizeof(struct updd_data));
	inject_touch_lock.unlock();

	delete ud;
}

// 
void updd_stop() {
	TBApiUnregisterEvent(connected_callback);
	TBApiClose();
}

static void TBCALL connected_callback(unsigned long context, struct _PointerEvent* ev) {
	if (ev->pe.config.configEventType == CONFIG_EVENT_CONNECT) {
		updd_connected = true; 
	}
}
