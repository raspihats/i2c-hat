extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "cortexm/ExceptionHandlers.h"
}

#include "I2CHat.h"

using namespace i2c_hat;

static I2CHat* i2cHat;

extern "C" {
void tick(void) {
    /* Cooperative scheduler tick period is 1 milliseconds */
    i2cHat->tick();
}
}

/* At this stage the system clock should have already been configured at high speed. */
void main(void) {
    uint32_t returnCode;
    i2cHat = new I2CHat();

    returnCode = SysTick_Config(SystemCoreClock / 1000);      /* Configure SysTick to generate an interrupt every millisecond */
    if (returnCode != 0)  {                                   /* Check return code for errors */
        while(1);
    }

    while(1) {
        i2cHat->run();
    }
}

// ----------------------------------------------------------------------------
