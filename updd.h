#ifndef UPDD_H
#define UPDD_H

struct updd_data;

void updd_start();
void updd_stop();
void inject_touch(struct updd_data* ud);

#endif
