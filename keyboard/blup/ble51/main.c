#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include "lufa.h"
#include "print.h"
#include "sendchar.h"
#include "ble51.h"
#include "ble51_task.h"
#include "serial.h"
#include "keyboard.h"
#include "keycode.h"
#include "action.h"
#include "action_util.h"
#include "wait.h"
#include "timer.h"
#include "suspend.h"

extern bool dozing;
extern bool sleeping;
extern bool force_usb;
extern uint32_t kb_idle_timer; 
bool kb_started = 0;

static int8_t sendchar_func(uint8_t c)
{
    //xmit(c);        // SUART
    sendchar(c);    // LUFA
    return 0;
}

static void SetupHardware(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    // Leonardo needs. Without this USB device is not recognized.
    USB_Disable();

    USB_Init();

    // for Console_Task
    USB_Device_EnableSOFEvents();
    print_set_sendchar(sendchar_func);
}

int main(void)  __attribute__ ((weak));
int main(void)
{
    SetupHardware();
    sei();

    /* wait for USB startup to get ready for debug output */
    uint8_t timeout = 255;  // timeout when USB is not available(Bluetooth)
    while (timeout-- && USB_DeviceState != DEVICE_STATE_Configured) {
        wait_ms(4);
#if defined(INTERRUPT_CONTROL_ENDPOINT)
        ;
#else
        USB_USBTask();
#endif
    }
    if (USB_DeviceState == DEVICE_STATE_Configured) {
        force_usb = true;
    }
    print("\nUSB init\n");

    ble51_init();
    ble51_task_init();

    /* init modules */
    keyboard_init();

    print("Keyboard start\n");
    kb_started = 1;
    wdt_enable(WDTO_500MS);
    while (1) {
        while (dozing) { 
            select_all_rows();
            suspend_power_down();
            if (suspend_wakeup_condition()) {
                unselect_rows();
                kb_idle_timer = timer_read32();
                wdt_disable();
                wdt_enable(WDTO_500MS);
                dozing = 0;
                suspend_wakeup_init();
                if (sleeping) {
                    sleeping = 0;
                    turn_on_bt();
                    keyboard_task();
                }
            }
            if (!sleeping) {
                wdt_reset();
                ble51_task();
            }
        }
        if (!sleeping){
            wdt_reset();
            keyboard_task();
            ble51_task();
        }
    }
}
