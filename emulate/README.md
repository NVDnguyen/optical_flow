1. Download Renode [Windows](https://github.com/renode/renode/releases/download/v1.15.3/renode_1.15.3.msi)

```shell
renote --version
```

2. Move ```efr32mg24.repl``` to ```D:\Renode\platforms\cpus\silabs\efr32```
3. Move ```efr32mg24board.repl``` to  ```D:\Renode\platforms\boards\silabs```

4. Simulate in Renode

```shell

renode --console

mach create

machine LoadPlatformDescription @platforms/boards/silabs/efr32mg24board.repl

# Load compiled ELF file 
sysbus LoadELF @path_to_your_file/your_efr32_project.elf

# Configure UART for input and output logs
uart0 CreateBridge uart_bridge
connector Connect sysbus.uart0 uart_bridge

# enable log
logLevel -1 sysbus.uart0
logLevel -1 gpioPortD.UserLED

#
uart_bridge SendFile image1.bin
uart_bridge SendFile image2.bin

start
```

5. Debugging and Logs

* Enable logging to monitor specific events, such as GPIO or UART activity:

  ```shell
  logLevel -1 sysbus.uart0
  ```

* To inspect GPIOs or peripherals:

  ```shell
  logLevel -1 gpioPortD.UserLED
  ```
