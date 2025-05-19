################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app.c \
../main.c \
../nv_optical_flow.c \
../nv_usart.c 

OBJS += \
./app.o \
./main.o \
./nv_optical_flow.o \
./nv_usart.o 

C_DEPS += \
./app.d \
./main.d \
./nv_optical_flow.d \
./nv_usart.d 


# Each subdirectory must supply rules for building sources it contributes
app.o: ../app.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DEFR32MG12P432F1024GL125=1' '-DSL_BOARD_NAME="BRD4161A"' '-DSL_BOARD_REV="A03"' '-DHARDWARE_BOARD_DEFAULT_RF_BAND_2400=1' '-DHARDWARE_BOARD_SUPPORTS_1_RF_BAND=1' '-DHARDWARE_BOARD_SUPPORTS_RF_BAND_2400=1' '-DHFXO_FREQ=38400000' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"E:\AIoT\Project\Nova\silmotion_xG12\config" -I"E:\AIoT\Project\Nova\silmotion_xG12\autogen" -I"E:\AIoT\Project\Nova\silmotion_xG12" -I"C:/gecko_sdk//platform/Device/SiliconLabs/EFR32MG12P/Include" -I"C:/gecko_sdk//platform/common/inc" -I"C:/gecko_sdk//hardware/board/inc" -I"C:/gecko_sdk//platform/CMSIS/Core/Include" -I"C:/gecko_sdk//hardware/driver/configuration_over_swo/inc" -I"C:/gecko_sdk//platform/driver/debug/inc" -I"C:/gecko_sdk//platform/service/device_init/inc" -I"C:/gecko_sdk//platform/emlib/inc" -I"C:/gecko_sdk//hardware/driver/mx25_flash_shutdown/inc/sl_mx25_flash_shutdown_usart" -I"C:/gecko_sdk//platform/common/toolchain/inc" -I"C:/gecko_sdk//platform/service/system/inc" -I"C:/gecko_sdk//platform/service/sleeptimer/inc" -I"C:/gecko_sdk//platform/service/udelay/inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"app.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.o: ../main.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DEFR32MG12P432F1024GL125=1' '-DSL_BOARD_NAME="BRD4161A"' '-DSL_BOARD_REV="A03"' '-DHARDWARE_BOARD_DEFAULT_RF_BAND_2400=1' '-DHARDWARE_BOARD_SUPPORTS_1_RF_BAND=1' '-DHARDWARE_BOARD_SUPPORTS_RF_BAND_2400=1' '-DHFXO_FREQ=38400000' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"E:\AIoT\Project\Nova\silmotion_xG12\config" -I"E:\AIoT\Project\Nova\silmotion_xG12\autogen" -I"E:\AIoT\Project\Nova\silmotion_xG12" -I"C:/gecko_sdk//platform/Device/SiliconLabs/EFR32MG12P/Include" -I"C:/gecko_sdk//platform/common/inc" -I"C:/gecko_sdk//hardware/board/inc" -I"C:/gecko_sdk//platform/CMSIS/Core/Include" -I"C:/gecko_sdk//hardware/driver/configuration_over_swo/inc" -I"C:/gecko_sdk//platform/driver/debug/inc" -I"C:/gecko_sdk//platform/service/device_init/inc" -I"C:/gecko_sdk//platform/emlib/inc" -I"C:/gecko_sdk//hardware/driver/mx25_flash_shutdown/inc/sl_mx25_flash_shutdown_usart" -I"C:/gecko_sdk//platform/common/toolchain/inc" -I"C:/gecko_sdk//platform/service/system/inc" -I"C:/gecko_sdk//platform/service/sleeptimer/inc" -I"C:/gecko_sdk//platform/service/udelay/inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"main.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

nv_optical_flow.o: ../nv_optical_flow.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DEFR32MG12P432F1024GL125=1' '-DSL_BOARD_NAME="BRD4161A"' '-DSL_BOARD_REV="A03"' '-DHARDWARE_BOARD_DEFAULT_RF_BAND_2400=1' '-DHARDWARE_BOARD_SUPPORTS_1_RF_BAND=1' '-DHARDWARE_BOARD_SUPPORTS_RF_BAND_2400=1' '-DHFXO_FREQ=38400000' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"E:\AIoT\Project\Nova\silmotion_xG12\config" -I"E:\AIoT\Project\Nova\silmotion_xG12\autogen" -I"E:\AIoT\Project\Nova\silmotion_xG12" -I"C:/gecko_sdk//platform/Device/SiliconLabs/EFR32MG12P/Include" -I"C:/gecko_sdk//platform/common/inc" -I"C:/gecko_sdk//hardware/board/inc" -I"C:/gecko_sdk//platform/CMSIS/Core/Include" -I"C:/gecko_sdk//hardware/driver/configuration_over_swo/inc" -I"C:/gecko_sdk//platform/driver/debug/inc" -I"C:/gecko_sdk//platform/service/device_init/inc" -I"C:/gecko_sdk//platform/emlib/inc" -I"C:/gecko_sdk//hardware/driver/mx25_flash_shutdown/inc/sl_mx25_flash_shutdown_usart" -I"C:/gecko_sdk//platform/common/toolchain/inc" -I"C:/gecko_sdk//platform/service/system/inc" -I"C:/gecko_sdk//platform/service/sleeptimer/inc" -I"C:/gecko_sdk//platform/service/udelay/inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"nv_optical_flow.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

nv_usart.o: ../nv_usart.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DEFR32MG12P432F1024GL125=1' '-DSL_BOARD_NAME="BRD4161A"' '-DSL_BOARD_REV="A03"' '-DHARDWARE_BOARD_DEFAULT_RF_BAND_2400=1' '-DHARDWARE_BOARD_SUPPORTS_1_RF_BAND=1' '-DHARDWARE_BOARD_SUPPORTS_RF_BAND_2400=1' '-DHFXO_FREQ=38400000' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"E:\AIoT\Project\Nova\silmotion_xG12\config" -I"E:\AIoT\Project\Nova\silmotion_xG12\autogen" -I"E:\AIoT\Project\Nova\silmotion_xG12" -I"C:/gecko_sdk//platform/Device/SiliconLabs/EFR32MG12P/Include" -I"C:/gecko_sdk//platform/common/inc" -I"C:/gecko_sdk//hardware/board/inc" -I"C:/gecko_sdk//platform/CMSIS/Core/Include" -I"C:/gecko_sdk//hardware/driver/configuration_over_swo/inc" -I"C:/gecko_sdk//platform/driver/debug/inc" -I"C:/gecko_sdk//platform/service/device_init/inc" -I"C:/gecko_sdk//platform/emlib/inc" -I"C:/gecko_sdk//hardware/driver/mx25_flash_shutdown/inc/sl_mx25_flash_shutdown_usart" -I"C:/gecko_sdk//platform/common/toolchain/inc" -I"C:/gecko_sdk//platform/service/system/inc" -I"C:/gecko_sdk//platform/service/sleeptimer/inc" -I"C:/gecko_sdk//platform/service/udelay/inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"nv_usart.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


