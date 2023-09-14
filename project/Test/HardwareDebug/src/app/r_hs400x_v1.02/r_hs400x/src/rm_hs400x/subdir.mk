################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../src/app/r_hs400x_v1.02/r_hs400x/src/rm_hs400x/rm_hs400x.c \
../src/app/r_hs400x_v1.02/r_hs400x/src/rm_hs400x/rm_hs400x_instance.c \
../src/app/r_hs400x_v1.02/r_hs400x/src/rm_hs400x/rm_hs400x_rl_driver.c 

COMPILER_OBJS += \
src/app/r_hs400x_v1.02/r_hs400x/src/rm_hs400x/rm_hs400x.obj \
src/app/r_hs400x_v1.02/r_hs400x/src/rm_hs400x/rm_hs400x_instance.obj \
src/app/r_hs400x_v1.02/r_hs400x/src/rm_hs400x/rm_hs400x_rl_driver.obj 

C_DEPS += \
src/app/r_hs400x_v1.02/r_hs400x/src/rm_hs400x/rm_hs400x.d \
src/app/r_hs400x_v1.02/r_hs400x/src/rm_hs400x/rm_hs400x_instance.d \
src/app/r_hs400x_v1.02/r_hs400x/src/rm_hs400x/rm_hs400x_rl_driver.d 

# Each subdirectory must supply rules for building sources it contributes
src/app/r_hs400x_v1.02/r_hs400x/src/rm_hs400x/%.obj: ../src/app/r_hs400x_v1.02/r_hs400x/src/rm_hs400x/%.c 
	@echo 'Scanning and building file: $<'
	@echo 'Invoking: Scanner and Compiler'
	@echo src\app\r_hs400x_v1.02\r_hs400x\src\rm_hs400x\cDepSubCommand.tmp=
	@sed -e "s/^/ /" "src\app\r_hs400x_v1.02\r_hs400x\src\rm_hs400x\cDepSubCommand.tmp"
	ccrl -subcommand="src\app\r_hs400x_v1.02\r_hs400x\src\rm_hs400x\cDepSubCommand.tmp" -o "$(@:%.obj=%.d)"  -MT="$(@:%.obj=%.obj)"  -MT="$(@:%.obj=%.d)" -msg_lang=english "$<"
	@echo src\app\r_hs400x_v1.02\r_hs400x\src\rm_hs400x\cSubCommand.tmp=
	@sed -e "s/^/ /" "src\app\r_hs400x_v1.02\r_hs400x\src\rm_hs400x\cSubCommand.tmp"
	ccrl -subcommand="src\app\r_hs400x_v1.02\r_hs400x\src\rm_hs400x\cSubCommand.tmp" -msg_lang=english -o "$(@:%.d=%.obj)" "$<"
	@echo 'Finished Scanning and building: $<'
	@echo ''

