# Copyright (c) Bao Project and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Reuse the mps3 SoC pinctrl_soc.h; this board sits on the fvp_aemv8r SoC,
# which provides none, but drivers such as the CMSDK UART select PINCTRL.
zephyr_include_directories(${ZEPHYR_BASE}/soc/arm/mps3)
zephyr_sources_ifdef(CONFIG_PINCTRL pinctrl_stub.c)
