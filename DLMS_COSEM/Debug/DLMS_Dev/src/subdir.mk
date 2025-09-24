################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DLMS_Dev/src/apdu.c \
../DLMS_Dev/src/asn1Parser.c \
../DLMS_Dev/src/bigInteger.c \
../DLMS_Dev/src/bitarray.c \
../DLMS_Dev/src/bytebuffer.c \
../DLMS_Dev/src/ciphering.c \
../DLMS_Dev/src/client.c \
../DLMS_Dev/src/converters.c \
../DLMS_Dev/src/cosem.c \
../DLMS_Dev/src/curve.c \
../DLMS_Dev/src/datainfo.c \
../DLMS_Dev/src/date.c \
../DLMS_Dev/src/dlms.c \
../DLMS_Dev/src/dlmsSettings.c \
../DLMS_Dev/src/eccPoint.c \
../DLMS_Dev/src/gx509Certificate.c \
../DLMS_Dev/src/gxPkcs10.c \
../DLMS_Dev/src/gxaes.c \
../DLMS_Dev/src/gxarray.c \
../DLMS_Dev/src/gxecdsa.c \
../DLMS_Dev/src/gxget.c \
../DLMS_Dev/src/gxinvoke.c \
../DLMS_Dev/src/gxkey.c \
../DLMS_Dev/src/gxmd5.c \
../DLMS_Dev/src/gxobjects.c \
../DLMS_Dev/src/gxserializer.c \
../DLMS_Dev/src/gxset.c \
../DLMS_Dev/src/gxsetignoremalloc.c \
../DLMS_Dev/src/gxsetmalloc.c \
../DLMS_Dev/src/gxsha1.c \
../DLMS_Dev/src/gxsha256.c \
../DLMS_Dev/src/gxsha384.c \
../DLMS_Dev/src/gxvalueeventargs.c \
../DLMS_Dev/src/helpers.c \
../DLMS_Dev/src/message.c \
../DLMS_Dev/src/notify.c \
../DLMS_Dev/src/objectarray.c \
../DLMS_Dev/src/parameters.c \
../DLMS_Dev/src/privateKey.c \
../DLMS_Dev/src/publicKey.c \
../DLMS_Dev/src/replydata.c \
../DLMS_Dev/src/server.c \
../DLMS_Dev/src/serverevents.c \
../DLMS_Dev/src/shamirs.c \
../DLMS_Dev/src/variant.c 

OBJS += \
./DLMS_Dev/src/apdu.o \
./DLMS_Dev/src/asn1Parser.o \
./DLMS_Dev/src/bigInteger.o \
./DLMS_Dev/src/bitarray.o \
./DLMS_Dev/src/bytebuffer.o \
./DLMS_Dev/src/ciphering.o \
./DLMS_Dev/src/client.o \
./DLMS_Dev/src/converters.o \
./DLMS_Dev/src/cosem.o \
./DLMS_Dev/src/curve.o \
./DLMS_Dev/src/datainfo.o \
./DLMS_Dev/src/date.o \
./DLMS_Dev/src/dlms.o \
./DLMS_Dev/src/dlmsSettings.o \
./DLMS_Dev/src/eccPoint.o \
./DLMS_Dev/src/gx509Certificate.o \
./DLMS_Dev/src/gxPkcs10.o \
./DLMS_Dev/src/gxaes.o \
./DLMS_Dev/src/gxarray.o \
./DLMS_Dev/src/gxecdsa.o \
./DLMS_Dev/src/gxget.o \
./DLMS_Dev/src/gxinvoke.o \
./DLMS_Dev/src/gxkey.o \
./DLMS_Dev/src/gxmd5.o \
./DLMS_Dev/src/gxobjects.o \
./DLMS_Dev/src/gxserializer.o \
./DLMS_Dev/src/gxset.o \
./DLMS_Dev/src/gxsetignoremalloc.o \
./DLMS_Dev/src/gxsetmalloc.o \
./DLMS_Dev/src/gxsha1.o \
./DLMS_Dev/src/gxsha256.o \
./DLMS_Dev/src/gxsha384.o \
./DLMS_Dev/src/gxvalueeventargs.o \
./DLMS_Dev/src/helpers.o \
./DLMS_Dev/src/message.o \
./DLMS_Dev/src/notify.o \
./DLMS_Dev/src/objectarray.o \
./DLMS_Dev/src/parameters.o \
./DLMS_Dev/src/privateKey.o \
./DLMS_Dev/src/publicKey.o \
./DLMS_Dev/src/replydata.o \
./DLMS_Dev/src/server.o \
./DLMS_Dev/src/serverevents.o \
./DLMS_Dev/src/shamirs.o \
./DLMS_Dev/src/variant.o 

C_DEPS += \
./DLMS_Dev/src/apdu.d \
./DLMS_Dev/src/asn1Parser.d \
./DLMS_Dev/src/bigInteger.d \
./DLMS_Dev/src/bitarray.d \
./DLMS_Dev/src/bytebuffer.d \
./DLMS_Dev/src/ciphering.d \
./DLMS_Dev/src/client.d \
./DLMS_Dev/src/converters.d \
./DLMS_Dev/src/cosem.d \
./DLMS_Dev/src/curve.d \
./DLMS_Dev/src/datainfo.d \
./DLMS_Dev/src/date.d \
./DLMS_Dev/src/dlms.d \
./DLMS_Dev/src/dlmsSettings.d \
./DLMS_Dev/src/eccPoint.d \
./DLMS_Dev/src/gx509Certificate.d \
./DLMS_Dev/src/gxPkcs10.d \
./DLMS_Dev/src/gxaes.d \
./DLMS_Dev/src/gxarray.d \
./DLMS_Dev/src/gxecdsa.d \
./DLMS_Dev/src/gxget.d \
./DLMS_Dev/src/gxinvoke.d \
./DLMS_Dev/src/gxkey.d \
./DLMS_Dev/src/gxmd5.d \
./DLMS_Dev/src/gxobjects.d \
./DLMS_Dev/src/gxserializer.d \
./DLMS_Dev/src/gxset.d \
./DLMS_Dev/src/gxsetignoremalloc.d \
./DLMS_Dev/src/gxsetmalloc.d \
./DLMS_Dev/src/gxsha1.d \
./DLMS_Dev/src/gxsha256.d \
./DLMS_Dev/src/gxsha384.d \
./DLMS_Dev/src/gxvalueeventargs.d \
./DLMS_Dev/src/helpers.d \
./DLMS_Dev/src/message.d \
./DLMS_Dev/src/notify.d \
./DLMS_Dev/src/objectarray.d \
./DLMS_Dev/src/parameters.d \
./DLMS_Dev/src/privateKey.d \
./DLMS_Dev/src/publicKey.d \
./DLMS_Dev/src/replydata.d \
./DLMS_Dev/src/server.d \
./DLMS_Dev/src/serverevents.d \
./DLMS_Dev/src/shamirs.d \
./DLMS_Dev/src/variant.d 


# Each subdirectory must supply rules for building sources it contributes
DLMS_Dev/src/%.o DLMS_Dev/src/%.su DLMS_Dev/src/%.cyclo: ../DLMS_Dev/src/%.c DLMS_Dev/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G030xx -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/singh/STM32CubeIDE/workspace_1.18.1/DLMS_COSEM/DLMS_Dev" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-DLMS_Dev-2f-src

clean-DLMS_Dev-2f-src:
	-$(RM) ./DLMS_Dev/src/apdu.cyclo ./DLMS_Dev/src/apdu.d ./DLMS_Dev/src/apdu.o ./DLMS_Dev/src/apdu.su ./DLMS_Dev/src/asn1Parser.cyclo ./DLMS_Dev/src/asn1Parser.d ./DLMS_Dev/src/asn1Parser.o ./DLMS_Dev/src/asn1Parser.su ./DLMS_Dev/src/bigInteger.cyclo ./DLMS_Dev/src/bigInteger.d ./DLMS_Dev/src/bigInteger.o ./DLMS_Dev/src/bigInteger.su ./DLMS_Dev/src/bitarray.cyclo ./DLMS_Dev/src/bitarray.d ./DLMS_Dev/src/bitarray.o ./DLMS_Dev/src/bitarray.su ./DLMS_Dev/src/bytebuffer.cyclo ./DLMS_Dev/src/bytebuffer.d ./DLMS_Dev/src/bytebuffer.o ./DLMS_Dev/src/bytebuffer.su ./DLMS_Dev/src/ciphering.cyclo ./DLMS_Dev/src/ciphering.d ./DLMS_Dev/src/ciphering.o ./DLMS_Dev/src/ciphering.su ./DLMS_Dev/src/client.cyclo ./DLMS_Dev/src/client.d ./DLMS_Dev/src/client.o ./DLMS_Dev/src/client.su ./DLMS_Dev/src/converters.cyclo ./DLMS_Dev/src/converters.d ./DLMS_Dev/src/converters.o ./DLMS_Dev/src/converters.su ./DLMS_Dev/src/cosem.cyclo ./DLMS_Dev/src/cosem.d ./DLMS_Dev/src/cosem.o ./DLMS_Dev/src/cosem.su ./DLMS_Dev/src/curve.cyclo ./DLMS_Dev/src/curve.d ./DLMS_Dev/src/curve.o ./DLMS_Dev/src/curve.su ./DLMS_Dev/src/datainfo.cyclo ./DLMS_Dev/src/datainfo.d ./DLMS_Dev/src/datainfo.o ./DLMS_Dev/src/datainfo.su ./DLMS_Dev/src/date.cyclo ./DLMS_Dev/src/date.d ./DLMS_Dev/src/date.o ./DLMS_Dev/src/date.su ./DLMS_Dev/src/dlms.cyclo ./DLMS_Dev/src/dlms.d ./DLMS_Dev/src/dlms.o ./DLMS_Dev/src/dlms.su ./DLMS_Dev/src/dlmsSettings.cyclo ./DLMS_Dev/src/dlmsSettings.d ./DLMS_Dev/src/dlmsSettings.o ./DLMS_Dev/src/dlmsSettings.su ./DLMS_Dev/src/eccPoint.cyclo ./DLMS_Dev/src/eccPoint.d ./DLMS_Dev/src/eccPoint.o ./DLMS_Dev/src/eccPoint.su ./DLMS_Dev/src/gx509Certificate.cyclo ./DLMS_Dev/src/gx509Certificate.d ./DLMS_Dev/src/gx509Certificate.o ./DLMS_Dev/src/gx509Certificate.su ./DLMS_Dev/src/gxPkcs10.cyclo ./DLMS_Dev/src/gxPkcs10.d ./DLMS_Dev/src/gxPkcs10.o ./DLMS_Dev/src/gxPkcs10.su ./DLMS_Dev/src/gxaes.cyclo ./DLMS_Dev/src/gxaes.d ./DLMS_Dev/src/gxaes.o ./DLMS_Dev/src/gxaes.su ./DLMS_Dev/src/gxarray.cyclo ./DLMS_Dev/src/gxarray.d ./DLMS_Dev/src/gxarray.o ./DLMS_Dev/src/gxarray.su ./DLMS_Dev/src/gxecdsa.cyclo ./DLMS_Dev/src/gxecdsa.d ./DLMS_Dev/src/gxecdsa.o ./DLMS_Dev/src/gxecdsa.su ./DLMS_Dev/src/gxget.cyclo ./DLMS_Dev/src/gxget.d ./DLMS_Dev/src/gxget.o ./DLMS_Dev/src/gxget.su ./DLMS_Dev/src/gxinvoke.cyclo ./DLMS_Dev/src/gxinvoke.d ./DLMS_Dev/src/gxinvoke.o ./DLMS_Dev/src/gxinvoke.su ./DLMS_Dev/src/gxkey.cyclo ./DLMS_Dev/src/gxkey.d ./DLMS_Dev/src/gxkey.o ./DLMS_Dev/src/gxkey.su ./DLMS_Dev/src/gxmd5.cyclo ./DLMS_Dev/src/gxmd5.d ./DLMS_Dev/src/gxmd5.o ./DLMS_Dev/src/gxmd5.su ./DLMS_Dev/src/gxobjects.cyclo ./DLMS_Dev/src/gxobjects.d ./DLMS_Dev/src/gxobjects.o ./DLMS_Dev/src/gxobjects.su ./DLMS_Dev/src/gxserializer.cyclo ./DLMS_Dev/src/gxserializer.d ./DLMS_Dev/src/gxserializer.o ./DLMS_Dev/src/gxserializer.su ./DLMS_Dev/src/gxset.cyclo ./DLMS_Dev/src/gxset.d ./DLMS_Dev/src/gxset.o ./DLMS_Dev/src/gxset.su ./DLMS_Dev/src/gxsetignoremalloc.cyclo ./DLMS_Dev/src/gxsetignoremalloc.d ./DLMS_Dev/src/gxsetignoremalloc.o ./DLMS_Dev/src/gxsetignoremalloc.su ./DLMS_Dev/src/gxsetmalloc.cyclo ./DLMS_Dev/src/gxsetmalloc.d ./DLMS_Dev/src/gxsetmalloc.o ./DLMS_Dev/src/gxsetmalloc.su ./DLMS_Dev/src/gxsha1.cyclo ./DLMS_Dev/src/gxsha1.d ./DLMS_Dev/src/gxsha1.o ./DLMS_Dev/src/gxsha1.su ./DLMS_Dev/src/gxsha256.cyclo ./DLMS_Dev/src/gxsha256.d ./DLMS_Dev/src/gxsha256.o ./DLMS_Dev/src/gxsha256.su ./DLMS_Dev/src/gxsha384.cyclo ./DLMS_Dev/src/gxsha384.d ./DLMS_Dev/src/gxsha384.o ./DLMS_Dev/src/gxsha384.su ./DLMS_Dev/src/gxvalueeventargs.cyclo ./DLMS_Dev/src/gxvalueeventargs.d ./DLMS_Dev/src/gxvalueeventargs.o ./DLMS_Dev/src/gxvalueeventargs.su ./DLMS_Dev/src/helpers.cyclo ./DLMS_Dev/src/helpers.d ./DLMS_Dev/src/helpers.o ./DLMS_Dev/src/helpers.su ./DLMS_Dev/src/message.cyclo ./DLMS_Dev/src/message.d ./DLMS_Dev/src/message.o ./DLMS_Dev/src/message.su ./DLMS_Dev/src/notify.cyclo ./DLMS_Dev/src/notify.d ./DLMS_Dev/src/notify.o ./DLMS_Dev/src/notify.su ./DLMS_Dev/src/objectarray.cyclo ./DLMS_Dev/src/objectarray.d ./DLMS_Dev/src/objectarray.o ./DLMS_Dev/src/objectarray.su ./DLMS_Dev/src/parameters.cyclo ./DLMS_Dev/src/parameters.d ./DLMS_Dev/src/parameters.o ./DLMS_Dev/src/parameters.su ./DLMS_Dev/src/privateKey.cyclo ./DLMS_Dev/src/privateKey.d ./DLMS_Dev/src/privateKey.o ./DLMS_Dev/src/privateKey.su ./DLMS_Dev/src/publicKey.cyclo ./DLMS_Dev/src/publicKey.d ./DLMS_Dev/src/publicKey.o ./DLMS_Dev/src/publicKey.su ./DLMS_Dev/src/replydata.cyclo ./DLMS_Dev/src/replydata.d ./DLMS_Dev/src/replydata.o ./DLMS_Dev/src/replydata.su ./DLMS_Dev/src/server.cyclo ./DLMS_Dev/src/server.d ./DLMS_Dev/src/server.o ./DLMS_Dev/src/server.su ./DLMS_Dev/src/serverevents.cyclo ./DLMS_Dev/src/serverevents.d ./DLMS_Dev/src/serverevents.o ./DLMS_Dev/src/serverevents.su ./DLMS_Dev/src/shamirs.cyclo ./DLMS_Dev/src/shamirs.d ./DLMS_Dev/src/shamirs.o ./DLMS_Dev/src/shamirs.su ./DLMS_Dev/src/variant.cyclo ./DLMS_Dev/src/variant.d ./DLMS_Dev/src/variant.o ./DLMS_Dev/src/variant.su

.PHONY: clean-DLMS_Dev-2f-src

