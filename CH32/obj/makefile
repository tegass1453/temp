################################################################################
# MRS Version: 1.9.2
# 自动生成的文件。不要编辑！
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include BSP/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
test1.hex \

SECONDARY_LIST += \
test1.lst \

SECONDARY_SIZE += \
test1.siz \


# 所有目标
all: test1.elf secondary-outputs

# 工具调用
test1.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -T "D:\project\CH32\repository\CH32\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"test1.map" --specs=nano.specs --specs=nosys.specs -o "test1.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
test1.hex: test1.elf
	@	riscv-none-embed-objcopy -O ihex "test1.elf"  "test1.hex"
	@	@
test1.lst: test1.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "test1.elf" > "test1.lst"
	@	@
test1.siz: test1.elf
	@	riscv-none-embed-size --format=berkeley "test1.elf"
	@	@
# 其他目标
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) test1.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
