1. Download Renode [Windows](https://github.com/renode/renode/releases/download/v1.15.3/renode_1.15.3.msi)

```shell
renode --version
```

2. Move ```efr32mg24.repl``` to ```D:\Renode\platforms\cpus\silabs\efr32```
3. Move ```efr32mg24board.repl``` to  ```D:\Renode\platforms\boards\silabs```

4. Simulate in Renode

```shell

renode --console

# show all available command
help 

# create socket to send file via usart
emulation CreateServerSocketTerminal 12345 "terminal" false

# create machine
mach create

machine LoadPlatformDescription @platforms/boards/silabs/efr32mg24board.repl
connector Connect sysbus.usart2 terminal

# Load compiled ELF file 
sysbus LoadELF 'C:\Users\nvd\SimplicityStudio\v5_workspace\silmotion_xG12\GNU ARM v12.2.1 - Debug\silmotion_xG12.axf'

showAnalyzer sysbus.usart2

start

```

Send file via socket in another termial in windows
- download nmap, C:\Program Files (x86)\Nmap\ncat.exe
```shell
cat E:\AIoT\Project\Nova\assets\frame1.bin | ncat localhost 12345
echo "Test data" | ncat localhost 12345
```



---

efr32mg12 cpu

```yaml
using "./efr32mg1.repl"

rtcc: Timers.EFR32_RTCC @ sysbus 0x40042000
    frequency: 32768
    -> nvic@30

i2c0:
    -> nvic@17
i2c1: I2C.EFR32_I2CController @ sysbus <0x4000c400, +0x400>
    -> nvic@42

usart0:
    ReceiveIRQ -> nvic@12
    TransmitIRQ -> nvic@13
usart1:
    ReceiveIRQ -> nvic@20
    TransmitIRQ -> nvic@21
usart2: UART.EFR32_USART @ sysbus <0x40010800, +0x400>
    ReceiveIRQ -> nvic@40
    TransmitIRQ -> nvic@41
usart3: UART.EFR32_USART @ sysbus <0x40010c00, +0x400>
    ReceiveIRQ -> nvic@43
    TransmitIRQ -> nvic@44

leUart0:
    -> nvic@22

gpioPort:
    EvenIRQ -> nvic@10
    OddIRQ -> nvic@18

seq_sram: Memory.MappedMemory @ sysbus 0x21000000
    size: 0x00002000

sram:
    size: 0x00040000

flash:
    size: 0x00100000

flashCtrl: MTD.EFR32xg13FlashController @ sysbus 0x400E0000
    flash: flash

timer0:
    -> nvic@11

timer1:
    -> nvic@19

wtimer0: Timers.EFR32_Timer @ sysbus 0x4001a000
    frequency: 0x1000000 //bogus
    width: TimerWidth.Bit32
    -> nvic@36

wtimer1: Timers.EFR32_Timer @ sysbus 0x4001a400
    frequency: 0x1000000 //bogus
    width: TimerWidth.Bit32
    -> nvic@37

deviceInformation: Miscellaneous.SiLabs.EFR32xG12DeviceInformation @ sysbus 0x0FE081B0
    deviceFamily: DeviceFamily.EFR32MG12P
    deviceNumber: 0x1
    flashDevice: flash
    sramDevice: sram

emu_pwrcfg: Python.PythonPeripheral @ sysbus 0x400E3038
    size: 0x8
    initable: true
    filename: "scripts/pydev/repeater.py"

emu_if: Python.PythonPeripheral @ sysbus 0x400E3024
    size: 0x4
    initable: true
    script: "request.value = 0xffffffff"

bitclear: @ sysbus <0x44000000, +0x1000000>

bitset: @ sysbus <0x46000000, +0x1000000>

nvic:
    priorityMask: 0xe0

sysbus:
    init add:
        Tag <0x400f0000,0x400f03ff> "CRYPTO0"
        Tag <0x4004e400,0x4004e7ff> "PCNT1"
        Tag <0x4004e800,0x4004ebff> "PCNT2"
        Tag <0x40008000,0x400083ff> "VDAC0"
        Tag <0x4001f000,0x4001f3ff> "CSEN"
        Tag <0x40055000,0x400553ff> "LESENSE"
        Tag <0x40052400,0x400527ff> "WDOG1"
        Tag <0x40022000,0x400223ff> "SMU"
        Tag <0x4001d000,0x4001d3ff> "TRNG0"
        Tag <0x4001d004 0x4> "FIFOLEVEL" 0x40
        Tag <0xe00ff000, 0xe00fffff> "CM4_ROM_Table"
        Tag <0xe0041000, 0xe0041fff> "ETM"
        Tag <0xe0040000, 0xe0040fff> "TPIU"
        Tag <0xe000e000, 0xe000efff> "System_Control_Space"
        Tag <0xe0002000, 0xe0002fff> "FPB"
        Tag <0xe0001000, 0xe0001fff> "DWT"
        Tag <0xe0000000, 0xe0000fff> "ITM"
        Tag <0x400e4000, 0x400e43ff> "CMU"
        Tag <0x400e2000, 0x400e23ff> "LDMA"
        Tag <0x40052000, 0x400523ff> "WDOG0"
        Tag <0x4004a000, 0x4004a3ff> "LEUART0"
        Tag <0x40042000, 0x400423ff> "RTCC"
        Tag <0x4001c000, 0x4001c3ff> "GPCRC"
        Tag <0x4001a400, 0x4001a7ff> "WTIMER1"
        Tag <0x4001a000, 0x4001a3ff> "WTIMER0"
        Tag <0x40018400, 0x400187ff> "TIMER1"
        Tag <0x40018000, 0x400183ff> "TIMER0"
        Tag <0x40010c00, 0x40010fff> "USART3"
        Tag <0x40010800, 0x40010bff> "USART2"
        Tag <0x40010400, 0x400107ff> "USART1"
        Tag <0x40010000, 0x400103ff> "USART0"
        Tag <0x4000c400, 0x4000c7ff> "I2C1"
        Tag <0x4000c000, 0x4000c3ff> "I2C0"
        Tag <0x4000a000, 0x4000afff> "GPIO"
        Tag <0x40006000, 0x400063ff> "IDAC0"
        Tag <0x40002000, 0x400023ff> "ADC0"
        Tag <0x40002038 0x4> "IF" 0x1 #single conversion finished
        Tag <0x40000400, 0x400007ff> "ACMP1"
        Tag <0x40000000, 0x400003ff> "ACMP0"
        Tag <0x0fe08000, 0x0fe083ff> "Chip_config"
        Tag <0x0fe04000, 0x0fe047ff> "Lock_bits"
        Tag <0x0fe00000, 0x0fe007ff> "User_Data"

```

efr32mg1
```yaml
i2c0: I2C.EFR32_I2CController @ sysbus <0x4000c000, +0x400>
    -> nvic@16

usart0: UART.EFR32_USART @ sysbus <0x40010000, +0x400>
    ReceiveIRQ -> nvic@11
    TransmitIRQ -> nvic@12
    RxDataAvailableRequest -> ldma@0x00c0
    RxDataAvailableSingleRequest -> ldma@0x10c0
    TxBufferLowRequest -> ldma@0x00c1
    TxBufferLowSingleRequest -> ldma@0x10c1
    TxEmptyRequest -> ldma@0x00c2
usart1: UART.EFR32_USART @ sysbus <0x40010400, +0x400>
    ReceiveIRQ -> nvic@19
    TransmitIRQ -> nvic@20
    RxDataAvailableRequest -> ldma@0x00d0
    RxDataAvailableSingleRequest -> ldma@0x10d0
    TxBufferLowRequest -> ldma@0x00d1
    TxBufferLowSingleRequest -> ldma@0x10d1
    TxEmptyRequest -> ldma@0x00d2
    RxDataAvailableRightRequest -> ldma@0x00d3
    RxDataAvailableRightSingleRequest -> ldma@0x10d3
    TxBufferLowRightRequest -> ldma@0x00d4
    TxBufferLowRightSingleRequest -> ldma@0x10d4

leUart0: UART.LEUART @ sysbus <0x4004a000, +0x400>
    -> nvic@21

gpioPort: GPIOPort.EFR32_GPIOPort @ sysbus 0x4000a000
    EvenIRQ -> nvic@9
    OddIRQ -> nvic@17

bitband_peripherals: Miscellaneous.BitBanding @ sysbus <0x42000000, +0x2000000>
    peripheralBase: 0x40000000

bitclear: Miscellaneous.BitAccess @ sysbus <0x44000000, +0xf0400>
    address: 0x40000000
    mode: BitAccessMode.Clear

bitset: Miscellaneous.BitAccess @ sysbus <0x46000000, +0xf0400>
    address: 0x40000000
    mode: BitAccessMode.Set

bitband_sram: Miscellaneous.BitBanding @ sysbus <0x22000000, +0x2000000>
    peripheralBase: 0x20000000

sram: Memory.MappedMemory @ sysbus 0x20000000
    size: 0x00008000

flash: Memory.MappedMemory @ sysbus 0x00000000
    size: 0x00040000

nvic: IRQControllers.NVIC @ sysbus 0xE000E000
    IRQ -> cpu@0

cpu: CPU.CortexM @ sysbus
    nvic: nvic
    cpuType: "cortex-m4f"

gpcrc: Miscellaneous.SiLabs.EFR32_GPCRC @ sysbus 0x4001c000

timer0: Timers.EFR32_Timer @ sysbus 0x40018000
    frequency: 0x1000000 //bogus
    width: TimerWidth.Bit16
    -> nvic@10

timer1: Timers.EFR32_Timer @ sysbus 0x40018400
    frequency: 0x1000000 //bogus
    width: TimerWidth.Bit16
    -> nvic@18

cmu: Miscellaneous.EFR32_CMU @ sysbus 0x400e4000

ldma: DMA.EFR32MG12_LDMA @ sysbus 0x400e2000
    -> nvic@9

sysbus:
    init:
        Tag <0x400e0000,0x400e07ff> "MSC"
        Tag <0x400e3000,0x400e33ff> "EMU"
        Tag <0x400e5000,0x400e53ff> "RMU"
        Tag <0x400e6000,0x400e63ff> "PRS"
        Tag <0x400e1000,0x400e13ff> "FPUEH"
        Tag <0x40046000,0x400463ff> "LETIMER0"
        Tag <0x4001e000,0x4001e3ff> "CRYOTIMER"
        Tag <0x4004e000,0x4004e3ff> "PCNT0"

```