/*
 * bootloader.h
 *
 *  Software entry into the ROM system bootloader, following ST's published
 *  sequence ("Calling the STM32 SystemMemory Bootloader from your
 *  application"):
 *      #1 shut down any tasks running
 *      #2 switch to the HSI clock source - no PLL
 *      #3 disable interrupts
 *      #4 set the main stack pointer (MSP) to its default value
 *      #5 load the program counter with the SystemMemory reset vector
 *
 *  Direct jump from the running application: no reset, no magic word in
 *  .noinit, no startup-code hook.
 *
 *  Plus one addition ST's sequence does not have: the F04x BOOT0 pad is
 *  driven high before the jump, because that ROM's empty check otherwise
 *  hands a non-empty flash straight back to the application. That is the
 *  thing this branch exists to test - whether the pad drive alone is enough
 *  without the reset that master's version performs.
 */

#ifndef DRIVER_BOOTLOADER_H_
#define DRIVER_BOOTLOADER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Jumps to the ROM bootloader. Does not return. */
void Bootloader_Request(void);

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_BOOTLOADER_H_ */
