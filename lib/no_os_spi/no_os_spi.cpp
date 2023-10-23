/**
 *   @file   no_os_spi.c
 *   @brief  Implementation of the SPI Interface
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 ********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#include <inttypes.h>
#include "no_os_spi.h"
#include <stdlib.h>
#include "no_os_error.h"
#include <SPI.h>

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_spi_init(struct no_os_spi_desc **desc,
					   const struct no_os_spi_init_param *param)
{

	*desc = (no_os_spi_desc *)malloc(sizeof(*(*desc)));

	no_os_spi_desc *spi_desc = *desc;

	spi_desc->bit_order = param->bit_order;
	spi_desc->chip_select = param->chip_select;
	// spi_desc->spiClass = param->spiClass;
	// spi_desc->extra = param->extra;
	spi_desc->max_speed_hz = param->max_speed_hz;
	spi_desc->mode = param->mode;
	// spi_desc->parent = param->parent;
	// spi_desc->platform_ops = param->platform_ops;

	pinMode(param->chip_select, OUTPUT);
	// spi_desc->spiClass->begin();
	SPI.begin();
	return 0;
}

/**
 * @brief Free the resources allocated by no_os_spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_spi_remove(struct no_os_spi_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->remove)
		return -ENOSYS;

	return desc->platform_ops->remove(desc);
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_spi_write_and_read(struct no_os_spi_desc *desc,
								 uint8_t *data,
								 uint16_t bytes_number)
{
	uint8_t result[bytes_number] = {0};

	// Serial.print("Transmitted:    ");
	// Serial.print(data[0], BIN);
	// Serial.print(" , ");
	// Serial.println(data[1], BIN);

	SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));
	digitalWrite(desc->chip_select, LOW);
	delayMicroseconds(2);

	for (uint16_t i = 0; i < bytes_number; i++)
	{
		result[i] = SPI.transfer(data[i]);
	}

	delayMicroseconds(2);
	digitalWrite(desc->chip_select, HIGH);
	SPI.endTransaction();
	// delayMicroseconds(2);

#ifdef SPI_DEBUG
#ifdef DEBUG
	Serial.print("Received:    ");
	Serial.print(result[0], BIN);
	Serial.print("(HEADER) , ");
	Serial.print(result[1], BIN);
	Serial.println("(DATA)");
#endif
#endif

	for (uint16_t i = 0; i < bytes_number; i++)
	{
		data[i] = result[i];
	}

	return 0;
}

/**
 * @brief  Iterate over head list and send all spi messages
 * @param desc - The SPI descriptor.
 * @param msgs - Array of messages.
 * @param len - Number of messages in the array.
 * @return 0 in case of success, negativ error code otherwise.
 */
int32_t no_os_spi_transfer(struct no_os_spi_desc *desc,
						   struct no_os_spi_msg *msgs,
						   uint32_t len)
{
	int32_t ret;
	uint32_t i;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (desc->platform_ops->transfer)
		return desc->platform_ops->transfer(desc, msgs, len);

	for (i = 0; i < len; i++)
	{
		if (msgs[i].rx_buff != msgs[i].tx_buff || !msgs[i].tx_buff)
			return -EINVAL;
		ret = no_os_spi_write_and_read(desc, msgs[i].rx_buff,
									   msgs[i].bytes_number);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;
	}

	return 0;
}
