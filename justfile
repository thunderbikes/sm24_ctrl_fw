alias b := build
alias f := flash
alias m := monitor

# build project
build:
  make clean
  make -j5

# flash using pyocd
flash:
  python -m pyocd flash -t STM32F205RB build/sm24_ctrl_fw.elf

# monitor the UART output using tio
monitor:
  tio --auto-connect latest --map ODELBS
