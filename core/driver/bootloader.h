/*
 * bootloader.h
 *
 *  Software entry into the ROM system bootloader (no BOOT0 jumper).
 *
 *  Two-step pattern, deliberately not a direct jump: the IWDG cannot be
 *  stopped once enabled and would reset the chip mid-flash inside the ROM
 *  bootloader. Bootloader_Request() plants a magic word in .noinit RAM and
 *  system-resets; Bootloader_CheckAndEnter(), called at the very top of
 *  main() BEFORE any init (and thus before the IWDG ever starts), sees the
 *  magic and jumps to system memory with the chip still in reset state.
 *
 *  C linkage: Bootloader_CheckAndEnter() is called from the boards'
 *  CubeMX-generated C main().
 *
 *  The board's linker script must provide a .noinit (NOLOAD) section.
 */

#ifndef DRIVER_BOOTLOADER_H_
#define DRIVER_BOOTLOADER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* First statement of main(): jumps to the ROM bootloader iff the magic was
 * planted by Bootloader_Request() on the previous run; otherwise returns. */
void Bootloader_CheckAndEnter(void);

/* Plants the magic and system-resets. Does not return. */
void Bootloader_Request(void);

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_BOOTLOADER_H_ */
