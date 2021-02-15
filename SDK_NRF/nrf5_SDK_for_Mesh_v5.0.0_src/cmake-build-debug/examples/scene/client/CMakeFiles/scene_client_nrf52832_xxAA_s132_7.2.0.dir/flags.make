# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# compile ASM with /usr/local/bin/arm-none-eabi-gcc
# compile C with /usr/local/bin/arm-none-eabi-gcc
ASM_FLAGS = -g   -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16

ASM_DEFINES = -DBOARD_PCA10040 -DCONFIG_APP_IN_CORE -DCONFIG_GPIO_AS_PINRESET -DNRF52832 -DNRF52832_XXAA -DNRF52_SERIES -DNRF_SD_BLE_API_VERSION=7 -DS132 -DSOFTDEVICE_PRESENT -DUSE_APP_CONFIG

ASM_INCLUDES = -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/examples/scene/client/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/examples/common/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/external/rtt/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/ble/common -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/softdevice/common -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/strerror -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/atomic -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/models/model_spec/scene/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/models/model_spec/common/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/models/foundation/config/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/models/foundation/health/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/stack/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/core/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/core/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/access/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/access/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/dfu/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/dfu/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/prov/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/prov/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/bearer/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/bearer/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/gatt/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/gatt/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/friend/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/friend/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/softdevice/s132/headers -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/softdevice/s132/headers/nrf52 -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/modules/nrfx -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/modules/nrfx/mdk -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/modules/nrfx/hal -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/toolchain/cmsis/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/toolchain/gcc -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/toolchain/cmsis/dsp/GCC -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/boards -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/integration/nrfx -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/util -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/timer -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/log -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/log/src -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/experimental_section_vars -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/delay -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/modules/nrfx/drivers/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/modules/nrfx/drivers -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/external/micro-ecc -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/external/micro-ecc/../../mesh/core/include 

C_FLAGS = --std=gnu99 -Wall -Wno-attributes -Wno-format -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin --short-enums -Og -g3   -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16

C_DEFINES = -DBOARD_PCA10040 -DCONFIG_APP_IN_CORE -DCONFIG_GPIO_AS_PINRESET -DNRF52832 -DNRF52832_XXAA -DNRF52_SERIES -DNRF_SD_BLE_API_VERSION=7 -DS132 -DSOFTDEVICE_PRESENT -DUSE_APP_CONFIG

C_INCLUDES = -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/examples/scene/client/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/examples/common/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/external/rtt/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/ble/common -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/softdevice/common -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/strerror -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/atomic -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/models/model_spec/scene/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/models/model_spec/common/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/models/foundation/config/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/models/foundation/health/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/stack/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/core/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/core/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/access/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/access/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/dfu/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/dfu/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/prov/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/prov/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/bearer/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/bearer/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/gatt/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/gatt/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/friend/api -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/mesh/friend/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/softdevice/s132/headers -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/softdevice/s132/headers/nrf52 -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/modules/nrfx -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/modules/nrfx/mdk -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/modules/nrfx/hal -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/toolchain/cmsis/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/toolchain/gcc -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/toolchain/cmsis/dsp/GCC -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/boards -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/integration/nrfx -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/util -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/timer -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/log -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/log/src -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/experimental_section_vars -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/libraries/delay -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/modules/nrfx/drivers/include -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/modules/nrfx/drivers -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/external/micro-ecc -I/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/external/micro-ecc/../../mesh/core/include 

