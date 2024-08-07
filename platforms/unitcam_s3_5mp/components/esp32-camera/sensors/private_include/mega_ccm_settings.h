#ifndef _MEGA_CCM_SETTINGS_H_
#define _MEGA_CCM_SETTINGS_H_

#include <stdint.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "mega_ccm_regs.h"


#define REG_DLY 0xffff
#define REGLIST_TAIL 0x0000


/*
 * The default register settings, as obtained from OmniVision.  There
 * is really no making sense of most of these - lots of "reserved" values
 * and such.
 *
 */
static const uint16_t mega_ccm_default_regs[][2] = {
    {REGLIST_TAIL, 0x00},
};

#endif
