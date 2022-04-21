/*
 * SPDX-FileCopyrightText: 2020 Efabless Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * SPDX-License-Identifier: Apache-2.0
 */

// This include is relative to $CARAVEL_PATH (see Makefile)
#include <defs.h>
#include <stub.c>


int clk = 0;
int i;

void main()
{
        /* Set up the housekeeping SPI to be connected internally so	*/
	/* that external pin changes don't affect it.			*/

	// reg_spimaster_config = 0xa002;	// Enable, prescaler = 2,
        reg_spi_enable = 1;
                                        // connect to housekeeping SPI

	// Connect the housekeeping SPI to the SPI master
	// so that the CSB line is not left floating.  This allows
	// all of the GPIO pins to be used for user functions.


	// All GPIO pins are configured to be output
	// Used to flad the start/end of a test 

        reg_mprj_io_31 = GPIO_MODE_MGMT_STD_OUTPUT;
        reg_mprj_io_30 = GPIO_MODE_MGMT_STD_OUTPUT;
        reg_mprj_io_29 = GPIO_MODE_MGMT_STD_OUTPUT;
        reg_mprj_io_28 = GPIO_MODE_MGMT_STD_OUTPUT;
        reg_mprj_io_27 = GPIO_MODE_MGMT_STD_OUTPUT;
        reg_mprj_io_26 = GPIO_MODE_MGMT_STD_OUTPUT;
        reg_mprj_io_25 = GPIO_MODE_MGMT_STD_OUTPUT;
        reg_mprj_io_24 = GPIO_MODE_MGMT_STD_OUTPUT;
        reg_mprj_io_23 = GPIO_MODE_MGMT_STD_OUTPUT;
        reg_mprj_io_22 = GPIO_MODE_MGMT_STD_OUTPUT;
        reg_mprj_io_21 = GPIO_MODE_MGMT_STD_OUTPUT;
        reg_mprj_io_20 = GPIO_MODE_MGMT_STD_OUTPUT;
        reg_mprj_io_19 = GPIO_MODE_MGMT_STD_OUTPUT;
        reg_mprj_io_18 = GPIO_MODE_MGMT_STD_OUTPUT;
        reg_mprj_io_17 = GPIO_MODE_MGMT_STD_OUTPUT;
        reg_mprj_io_16 = GPIO_MODE_MGMT_STD_OUTPUT;

        reg_mprj_io_15 = GPIO_MODE_USER_STD_OUTPUT;
        reg_mprj_io_14 = GPIO_MODE_USER_STD_OUTPUT;
        reg_mprj_io_13 = GPIO_MODE_USER_STD_OUTPUT;
        reg_mprj_io_12 = GPIO_MODE_USER_STD_OUTPUT;
        reg_mprj_io_11 = GPIO_MODE_USER_STD_OUTPUT;
        reg_mprj_io_10 = GPIO_MODE_USER_STD_OUTPUT;
        reg_mprj_io_9  = GPIO_MODE_USER_STD_OUTPUT;
        reg_mprj_io_8  = GPIO_MODE_USER_STD_OUTPUT;
        reg_mprj_io_7  = GPIO_MODE_USER_STD_OUTPUT;
        reg_mprj_io_5  = GPIO_MODE_USER_STD_OUTPUT;
        reg_mprj_io_4  = GPIO_MODE_USER_STD_OUTPUT;
        reg_mprj_io_3  = GPIO_MODE_USER_STD_OUTPUT;
        reg_mprj_io_2  = GPIO_MODE_USER_STD_OUTPUT;
        reg_mprj_io_1  = GPIO_MODE_USER_STD_OUTPUT;
        reg_mprj_io_0  = GPIO_MODE_USER_STD_OUTPUT;

        /* Apply configuration */
        reg_mprj_xfer = 1;
        while (reg_mprj_xfer == 1);

	// Configure All LA probes as inputs to the cpu 
	reg_la0_oenb = reg_la0_iena = 0x00000000;    // [31:0]
	reg_la1_oenb = reg_la1_iena = 0x00000000;    // [63:32]
	reg_la2_oenb = reg_la2_iena = 0x00000000;    // [95:64]
	reg_la3_oenb = reg_la3_iena = 0x00000000;    // [127:96]

	// Flag start of the test
	reg_mprj_datal = 0x00ff0000;

	// Configure LA[96] LA[97] as outputs from the cpu
	reg_la3_oenb = reg_la3_iena = 0x00000003;

	// Set clk & reset to one
	reg_la3_data = 0x00000003;

	// Toggle clk & de-assert reset
	/*for (i=0; i<11; i=i+1) {
		clk = !clk;
		reg_la3_data = 0x00000000 | clk;
	}*/

	////////////////
	// Start test

	const int EBREAK = 0x00100073;
	const int ECALL = 0x00000073;

	const int romSize = 1024;
	const int romAddress = 0x00000400;
	const int[romSize] rom = [
		// addi r1, r0, 65
	        0x02100093,  // addi r1, r0, 65

		// assert r1 == 65
	        0x02100f93,  // addi r31, r0, 65
	        0x001f8463,  // beq r1, r31, 4
	        0x00000f93,  // addi r31, r0, 5000
	        EBREAK,  // ebreak

		// end of program
	        0x00000f93,  // addi r31, r0, 2000
	        ECALL,  // ecall
	];

	const int ramSize = 1024;
	const int ramAddress = 0x00000800;
	int[256] ram;

	const int registerFileAddress = 0x00000880;
	int[128] registerFile;

	const int diskAddress = 0x00000900;
	int[64] disk;

	const int networkInterfaceAddress = 0x00000940;
	int[64] networkInterface;


	int address;
	int dataIn;
	int transactionBegin;
	int writeEnable;
	int writeMask;

	int dataOut;
	int transactionEnd;

	int instructionFetch = 0;

        while (1){
		// toggle clk 1 full cycle
		clk = !clk;
		reg_la3_data = 0x00000000 | clk;
		clk = !clk;
		reg_la3_data = 0x00000000 | clk;

		// read signals
		address = reg_la0_data_in;
		dataIn = reg_la1_data_in;
		transactionBegin = reg_la2_data_in & 0x1;
		writeEnable = reg_la2_data_in >> 1 & 0x1;
		writeMask = reg_la2_data_in >> 2 & 0xf;

		if(transactionBegin) {
			instructionFetch = address >= romAddress && address < romAddress + romSize;
			if(instructionFetch) {
				dataout = rom[address - romAddress];
			}
			else if(address >= ramAddress && address < ramAddress + ramSize) {
				if(writeEnable) {
					ram[address - ramAddress] = (writeMask & 0x8) ? dataIn & 0xff000000 : 0 |
					(writeMask & 0x4) ? dataIn & 0x00ff0000 : 0 |
					(writeMask & 0x2) ? dataIn & 0x0000ff00 : 0 |
					(writeMask & 0x1) ? dataIn & 0x000000ff : 0;
				else {
					dataOut = ram[address - ramAddress];
				}
			}
			transactionEnd = 1;

			if(instructionFetch && dataOut == EBREAK) {
				reg_mprj_datal = 0xc0000000 | (address & 0x3fffffff);
                        	break;
			}
			if(instructionFetch && dataOut == ECALL) {
				reg_mprj_datal = 0x80000000 | (address & 0x3fffffff);
                        	break;
			}

	                reg_la0_data = dataOut;
	                reg_la1_data = transactionEnd;
	        }
        }
}
