################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
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
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
fan_ctl_tiny.lss \

FLASH_IMAGE += \
fan_ctl_tiny.hex \

EEPROM_IMAGE += \
fan_ctl_tiny.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: fan_ctl_tiny.elf secondary-outputs

# Tool invocations
fan_ctl_tiny.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,fan_ctl_tiny.map -mmcu=attiny85 -o "fan_ctl_tiny.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

fan_ctl_tiny.lss: fan_ctl_tiny.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S fan_ctl_tiny.elf  >"fan_ctl_tiny.lss"
	@echo 'Finished building: $@'
	@echo ' '

fan_ctl_tiny.hex: fan_ctl_tiny.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex fan_ctl_tiny.elf  "fan_ctl_tiny.hex"
	@echo 'Finished building: $@'
	@echo ' '

fan_ctl_tiny.eep: fan_ctl_tiny.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex fan_ctl_tiny.elf  "fan_ctl_tiny.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: fan_ctl_tiny.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=attiny85 fan_ctl_tiny.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) fan_ctl_tiny.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
