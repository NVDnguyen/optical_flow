################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/gecko_sdk/hardware/driver/configuration_over_swo/src/sl_cos.c 

OBJS += \
./gecko_sdk_4.4.6/hardware/driver/configuration_over_swo/src/sl_cos.o 

C_DEPS += \
./gecko_sdk_4.4.6/hardware/driver/configuration_over_swo/src/sl_cos.d 


# Each subdirectory must supply rules for building sources it contributes
gecko_sdk_4.4.6/hardware/driver/configuration_over_swo/src/sl_cos.o: C:/gecko_sdk/hardware/driver/configuration_over_swo/src/sl_cos.c gecko_sdk_4.4.6/hardware/driver/configuration_over_swo/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DEFR32MG12P432F1024GL125=1' '-DSL_BOARD_NAME="BRD4161A"' '-DSL_BOARD_REV="A03"' '-DHARDWARE_BOARD_DEFAULT_RF_BAND_2400=1' '-DHARDWARE_BOARD_SUPPORTS_1_RF_BAND=1' '-DHARDWARE_BOARD_SUPPORTS_RF_BAND_2400=1' '-DHFXO_FREQ=38400000' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"E:\AIoT\Project\Nova\silmotion_xG12\config" -I"E:\AIoT\Project\Nova\silmotion_xG12\autogen" -I"E:\AIoT\Project\Nova\silmotion_xG12" -I"C:/gecko_sdk//platform/Device/SiliconLabs/EFR32MG12P/Include" -I"C:/gecko_sdk//platform/common/inc" -I"C:/gecko_sdk//hardware/board/inc" -I"C:/gecko_sdk//platform/CMSIS/Core/Include" -I"C:/gecko_sdk//hardware/driver/configuration_over_swo/inc" -I"C:/gecko_sdk//platform/driver/debug/inc" -I"C:/gecko_sdk//platform/service/device_init/inc" -I"C:/gecko_sdk//platform/emlib/inc" -I"C:/gecko_sdk//hardware/driver/mx25_flash_shutdown/inc/sl_mx25_flash_shutdown_usart" -I"C:/gecko_sdk//platform/common/toolchain/inc" -I"C:/gecko_sdk//platform/service/system/inc" -I"C:/gecko_sdk//platform/service/sleeptimer/inc" -I"C:/gecko_sdk//platform/service/udelay/inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_4.4.6/hardware/driver/configuration_over_swo/src/sl_cos.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


