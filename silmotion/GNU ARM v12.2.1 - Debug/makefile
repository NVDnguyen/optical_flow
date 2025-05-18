################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include simplicity_sdk_2024.12.2/platform/service/udelay/src/subdir.mk
-include simplicity_sdk_2024.12.2/platform/service/system/src/subdir.mk
-include simplicity_sdk_2024.12.2/platform/service/memory_manager/src/subdir.mk
-include simplicity_sdk_2024.12.2/platform/service/interrupt_manager/src/subdir.mk
-include simplicity_sdk_2024.12.2/platform/service/device_manager/src/subdir.mk
-include simplicity_sdk_2024.12.2/platform/service/device_manager/clocks/subdir.mk
-include simplicity_sdk_2024.12.2/platform/service/device_init/src/subdir.mk
-include simplicity_sdk_2024.12.2/platform/service/clock_manager/src/subdir.mk
-include simplicity_sdk_2024.12.2/platform/peripheral/src/subdir.mk
-include simplicity_sdk_2024.12.2/platform/emlib/src/subdir.mk
-include simplicity_sdk_2024.12.2/platform/driver/gpio/src/subdir.mk
-include simplicity_sdk_2024.12.2/platform/common/src/subdir.mk
-include simplicity_sdk_2024.12.2/platform/Device/SiliconLabs/EFR32MG24/Source/subdir.mk
-include simplicity_sdk_2024.12.2/hardware/driver/mx25_flash_shutdown/src/sl_mx25_flash_shutdown_eusart/subdir.mk
-include simplicity_sdk_2024.12.2/hardware/board/src/subdir.mk
-include autogen/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := silmotion
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: main-build

# Main-build Target
main-build: silmotion.axf

# Tool invocations
silmotion.axf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -T "E:\AIoT\Project\Nova\silmotion\autogen\linkerfile.ld" -fno-lto -Wl,--no-warn-rwx-segments -Xlinker --gc-sections -Xlinker -Map="silmotion.map" -mfpu=fpv5-sp-d16 -mfloat-abi=hard --specs=nano.specs -o silmotion.axf -Wl,--start-group "./app.o" "./main.o" "./autogen/sl_board_default_init.o" "./autogen/sl_event_handler.o" "./simplicity_sdk_2024.12.2/hardware/board/src/sl_board_control_gpio.o" "./simplicity_sdk_2024.12.2/hardware/board/src/sl_board_init.o" "./simplicity_sdk_2024.12.2/hardware/driver/mx25_flash_shutdown/src/sl_mx25_flash_shutdown_eusart/sl_mx25_flash_shutdown.o" "./simplicity_sdk_2024.12.2/platform/Device/SiliconLabs/EFR32MG24/Source/startup_efr32mg24.o" "./simplicity_sdk_2024.12.2/platform/Device/SiliconLabs/EFR32MG24/Source/system_efr32mg24.o" "./simplicity_sdk_2024.12.2/platform/common/src/sl_assert.o" "./simplicity_sdk_2024.12.2/platform/common/src/sl_core_cortexm.o" "./simplicity_sdk_2024.12.2/platform/common/src/sl_syscalls.o" "./simplicity_sdk_2024.12.2/platform/driver/gpio/src/sl_gpio.o" "./simplicity_sdk_2024.12.2/platform/emlib/src/em_cmu.o" "./simplicity_sdk_2024.12.2/platform/emlib/src/em_core.o" "./simplicity_sdk_2024.12.2/platform/emlib/src/em_emu.o" "./simplicity_sdk_2024.12.2/platform/emlib/src/em_eusart.o" "./simplicity_sdk_2024.12.2/platform/emlib/src/em_gpio.o" "./simplicity_sdk_2024.12.2/platform/emlib/src/em_msc.o" "./simplicity_sdk_2024.12.2/platform/emlib/src/em_system.o" "./simplicity_sdk_2024.12.2/platform/peripheral/src/sl_hal_gpio.o" "./simplicity_sdk_2024.12.2/platform/service/clock_manager/src/sl_clock_manager.o" "./simplicity_sdk_2024.12.2/platform/service/clock_manager/src/sl_clock_manager_hal_s2.o" "./simplicity_sdk_2024.12.2/platform/service/clock_manager/src/sl_clock_manager_init.o" "./simplicity_sdk_2024.12.2/platform/service/clock_manager/src/sl_clock_manager_init_hal_s2.o" "./simplicity_sdk_2024.12.2/platform/service/device_init/src/sl_device_init_dcdc_s2.o" "./simplicity_sdk_2024.12.2/platform/service/device_init/src/sl_device_init_emu_s2.o" "./simplicity_sdk_2024.12.2/platform/service/device_manager/clocks/sl_device_clock_efr32xg24.o" "./simplicity_sdk_2024.12.2/platform/service/device_manager/src/sl_device_clock.o" "./simplicity_sdk_2024.12.2/platform/service/device_manager/src/sl_device_gpio.o" "./simplicity_sdk_2024.12.2/platform/service/interrupt_manager/src/sl_interrupt_manager_cortexm.o" "./simplicity_sdk_2024.12.2/platform/service/memory_manager/src/sl_memory_manager_region.o" "./simplicity_sdk_2024.12.2/platform/service/system/src/sl_system_init.o" "./simplicity_sdk_2024.12.2/platform/service/system/src/sl_system_process_action.o" "./simplicity_sdk_2024.12.2/platform/service/udelay/src/sl_udelay.o" "./simplicity_sdk_2024.12.2/platform/service/udelay/src/sl_udelay_armv6m_gcc.o" -lgcc -lc -lm -lnosys -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building out file: silmotion.out'
	arm-none-eabi-objcopy "silmotion.axf" "silmotion.out"
	@echo ' '

	@echo 'Building hex file: silmotion.hex'
	arm-none-eabi-objcopy -O ihex "silmotion.axf" "silmotion.hex"
	@echo ' '

	@echo 'Building bin file: silmotion.bin'
	arm-none-eabi-objcopy -O binary "silmotion.axf" "silmotion.bin"
	@echo ' '

	@echo 'Building s37 file: silmotion.s37'
	arm-none-eabi-objcopy -O srec "silmotion.axf" "silmotion.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "silmotion.axf" -A
	@echo ' '

# Other Targets
clean:
	-@$(foreach FILE, $(EXECUTABLES), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(OBJS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(C_DEPS), $(shell $(RM) $(FILE)) )
	-$(RM) silmotion.axf
	-@echo ' '

.PHONY: all clean dependents main-build

-include ../makefile.targets
