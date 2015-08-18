################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../ADIS_16364_V2.cpp 

OBJS += \
./ADIS_16364_V2.o 

CPP_DEPS += \
./ADIS_16364_V2.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -I"/home/developer/workspace/ADIS_16364_V2" -I"/home/developer/workspace/ADIS_16364_V2/lib/BlackLib/BlackADC" -I"/home/developer/workspace/ADIS_16364_V2/lib/BlackLib" -I"/home/developer/workspace/ADIS_16364_V2/lib/BlackLib/BlackDirectory" -I"/home/developer/workspace/ADIS_16364_V2/lib/BlackLib/BlackGPIO" -I"/home/developer/workspace/ADIS_16364_V2/lib/BlackLib/BlackI2C" -I"/home/developer/workspace/ADIS_16364_V2/lib/BlackLib/BlackPWM" -I"/home/developer/workspace/ADIS_16364_V2/lib/BlackLib/BlackSPI" -I"/home/developer/workspace/ADIS_16364_V2/lib/BlackLib/BlackTime" -I"/home/developer/workspace/ADIS_16364_V2/lib/BlackLib/BlackUART" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


