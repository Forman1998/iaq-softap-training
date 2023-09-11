################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../src/smc_gen/Config_INTC/Config_INTC.c \
../src/smc_gen/Config_INTC/Config_INTC_user.c 

COMPILER_OBJS += \
src/smc_gen/Config_INTC/Config_INTC.obj \
src/smc_gen/Config_INTC/Config_INTC_user.obj 

C_DEPS += \
src/smc_gen/Config_INTC/Config_INTC.d \
src/smc_gen/Config_INTC/Config_INTC_user.d 

# Each subdirectory must supply rules for building sources it contributes
src/smc_gen/Config_INTC/%.obj: ../src/smc_gen/Config_INTC/%.c 
	@echo 'Scanning and building file: $<'
	@echo 'Invoking: Scanner and Compiler'
	@echo src\smc_gen\Config_INTC\cDepSubCommand.tmp=
	@sed -e "s/^/ /" "src\smc_gen\Config_INTC\cDepSubCommand.tmp"
	ccrl -subcommand="src\smc_gen\Config_INTC\cDepSubCommand.tmp" -o "$(@:%.obj=%.d)"  -MT="$(@:%.obj=%.obj)"  -MT="$(@:%.obj=%.d)" -msg_lang=english "$<"
	@echo src\smc_gen\Config_INTC\cSubCommand.tmp=
	@sed -e "s/^/ /" "src\smc_gen\Config_INTC\cSubCommand.tmp"
	ccrl -subcommand="src\smc_gen\Config_INTC\cSubCommand.tmp" -msg_lang=english -o "$(@:%.d=%.obj)" "$<"
	@echo 'Finished Scanning and building: $<'
	@echo ''

