/*
 * demo_ledtrig.c
 * - Demonstrate how to flash an LED using a custom trigger.
 *      Author: Brian Fraser
 */
#include <linux/module.h>
#include <linux/miscdevice.h>		// for misc-driver calls.
#include <linux/fs.h>
#include <linux/delay.h>
//#error Are we building this?

#define MY_DEVICE_FILE  "demo_ledtrig"

// Morse Code Encodings (from http://en.wikipedia.org/wiki/Morse_code)
//   Encoding created by Brian Fraser. Released under GPL.
//
// Encoding description:
// - msb to be output first, followed by 2nd msb... (left to right)
// - each bit gets one "dot" time.
// - "dashes" are encoded here as being 3 times as long as "dots". Therefore
//   a single dash will be the bits: 111.
// - ignore trailing 0's (once last 1 output, rest of 0's ignored).
// - Space between dashes and dots is one dot time, so is therefore encoded
//   as a 0 bit between two 1 bits.
//
// Example:
//   R = dot   dash   dot       -- Morse code
//     =  1  0 111  0  1        -- 1=LED on, 0=LED off
//     =  1011 101              -- Written together in groups of 4 bits.
//     =  1011 1010 0000 0000   -- Pad with 0's on right to make 16 bits long.
//     =  B    A    0    0      -- Convert to hex digits
//     = 0xBA00                 -- Full hex value (see value in table below)
//
// Between characters, must have 3-dot times (total) of off (0's) (not encoded here)
// Between words, must have 7-dot times (total) of off (0's) (not encoded here).
//
static unsigned short morsecode_codes[] = {
        0xB800,	// A 1011 1
        0xEA80,	// B 1110 1010 1
        0xEBA0,	// C 1110 1011 101
        0xEA00,	// D 1110 101
        0x8000,	// E 1
        0xAE80,	// F 1010 1110 1
        0xEE80,	// G 1110 1110 1
        0xAA00,	// H 1010 101
        0xA000,	// I 101
        0xBBB8,	// J 1011 1011 1011 1
        0xEB80,	// K 1110 1011 1
        0xBA80,	// L 1011 1010 1
        0xEE00,	// M 1110 111
        0xE800,	// N 1110 1
        0xEEE0,	// O 1110 1110 111
        0xBBA0,	// P 1011 1011 101
        0xEEB8,	// Q 1110 1110 1011 1
        0xBA00,	// R 1011 101
        0xA800,	// S 1010 1
        0xE000,	// T 111
        0xAE00,	// U 1010 111
        0xAB80,	// V 1010 1011 1
        0xBB80,	// W 1011 1011 1
        0xEAE0,	// X 1110 1010 111
        0xEBB8,	// Y 1110 1011 1011 1
        0xEEA0	// Z 1110 1110 101
};

/******************************************************
 * LED
 ******************************************************/
#include <linux/leds.h>

DEFINE_LED_TRIGGER(ledtrig_demo);

#define LED_ON_TIME_ms 100
#define LED_OFF_TIME_ms 900

static void my_led_blink(void)
{
	led_trigger_event(ledtrig_demo, LED_FULL);
	msleep(LED_ON_TIME_ms);
	led_trigger_event(ledtrig_demo, LED_OFF);
	msleep(LED_OFF_TIME_ms);
}

static void led_register(void)
{
	// Setup the trigger's name:
	led_trigger_register_simple("custom_led", &ledtrig_demo);
}

static void led_unregister(void)
{
	// Cleanup
	led_trigger_unregister_simple(ledtrig_demo);
}


/******************************************************
 * Callbacks
 ******************************************************/
static ssize_t my_write(struct file* file, const char *buff,
		size_t count, loff_t* ppos)
{
	int i;
	printk(KERN_INFO "demo_ledtrig: Flashing %d times for string.\n", count);

	// Blink once per character (-1 to skip end null)
	for (i = 0; i < count-1; i++) {
		my_led_blink();
	}

	// Return # bytes actually written.
	return count;
}


/******************************************************
 * Misc support
 ******************************************************/
// Callbacks:  (structure defined in <kernel>/include/linux/fs.h)
struct file_operations my_fops = {
	.owner    =  THIS_MODULE,
	.write    =  my_write,
};

// Character Device info for the Kernel:
static struct miscdevice my_miscdevice = {
		.minor    = MISC_DYNAMIC_MINOR,         // Let the system assign one.
		.name     = MY_DEVICE_FILE,             // /dev/.... file.
		.fops     = &my_fops                    // Callback functions.
};


/******************************************************
 * Driver initialization and exit:
 ******************************************************/
static int __init my_init(void)
{
	int ret;
	printk(KERN_INFO "----> demo_misc driver init(): file /dev/%s.\n", MY_DEVICE_FILE);

	// Register as a misc driver:
	ret = misc_register(&my_miscdevice);

	// LED:
	led_register();

	return ret;
}

static void __exit my_exit(void)
{
	printk(KERN_INFO "<---- demo_misc driver exit().\n");

	// Unregister misc driver
	misc_deregister(&my_miscdevice);

	// LED:
	led_unregister();
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Brian Fraser");
MODULE_DESCRIPTION("A simple test driver");
MODULE_LICENSE("GPL");
