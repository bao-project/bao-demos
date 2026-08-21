/*
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/pinctrl.h>

/*
 * Pin muxing is not exposed to the guest VM and the board devicetree defines
 * no pinctrl states, so this only satisfies the link-time dependency of
 * drivers that select PINCTRL (e.g. the CMSDK UART).
 */
int pinctrl_configure_pins(const pinctrl_soc_pin_t *pins, uint8_t pin_cnt,
			   uintptr_t reg)
{
	ARG_UNUSED(pins);
	ARG_UNUSED(pin_cnt);
	ARG_UNUSED(reg);

	return 0;
}
