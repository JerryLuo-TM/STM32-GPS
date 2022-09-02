#ifndef _LED_H_
#define _LED_H_

#include "sys.h"

#define LED_TEST  PBout(8)

#define LED_R  PBout(15)
#define LED_B  PBout(14)

void LED_init(void);

#endif
