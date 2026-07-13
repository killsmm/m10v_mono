/**
 * @file		fj_arm.h
 * @brief		ARM process api header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_ARM_H
#define _FJ_ARM_H

#include "ddim_typedef.h"
#include "fj_std.h"

/** @weakgroup fj_arm_overview
@{
	The ARM controls can be achieved by using these APIs.<br>
	- Functions <br>
	. GIC(General Interrupt Control) unit control <br>
	. IPCU(Inter-Processor Control Unit) use <br>
	. Spin Lock/Unlock for exclusive control between cores <br>
	. TIMESTAMP(Timestamp Counter Regiser) read to calculate the relative time<br>
	<br><br>
	
	- Communication command.<br>
	Command of IPCU to Cortex-M0.<br>
	All command and parameters have a 32bit length respectively.<br>
	<br>
	
	<table>
		<tr>
			<th>CMD code #0</th>
			<th>command</th>
			<th>comment</th>
			<th></th>
		</tr>
		<tr>
			<td>0x01</td>
			<td>GO</td>
			<td>go to sleep state. No parameter</td>
			<td></td>
		</tr>
		<tr>
			<td>0x02</td>
			<td>WRITE</td>
			<td>write to memory and memory-mapped register by a data.</td>
			<td></td>
		</tr>
		<tr>
			<td>0x03</td>
			<td>READ</td>
			<td>read a data from memory and memory-mapped register</td>
			<td>need of response</td>
		</tr>
		<tr>
			<td>0x04</td>
			<td>RMW</td>
			<td>read-modify-write a data from/to memory and memory-mapped register</td>
			<td></td>
		</tr>
		<tr>
			<td>0x05</td>
			<td>SET</td>
			<td>set parameter</td>
			<td></td>
		</tr>
		<tr>
			<td>0x06</td>
			<td>GET</td>
			<td>get parameter</td>
			<td>need of response</td>
		</tr>
		<tr>
			<td>0x07</td>
			<td>START</td>
			<td>function start</td>
			<td></td>
		</tr>
		<tr>
			<td>0x08</td>
			<td>STOP</td>
			<td>function stop</td>
			<td></td>
		</tr>
 --- REMOVE_SECURE BEGIN ---
		<tr>
			<td>0x09</td>
			<td>CRYPTO</td>
			<td>Check Secure Code</td>
			<td>need of response</td>
		</tr>
 --- REMOVE_SECURE END ---
		<tr>
			<td>0x0A</td>
			<td>StateAterStandby</td>
			<td>Set state after standby state and wait time</td>
			<td></td>
		</tr>
	</table><br>
	Cortex-M0 responds to READ/GET
 --- REMOVE_SECURE BEGIN ---
	/CRYPTO
 --- REMOVE_SECURE END ---
	commands and its timing is at Acknowledge.<br>
	IPCU command only.<br>
	CMD code #0 is request CMD code + 0x01000000.<br>
	Parameters have request parameter and response value.<br>
	Request parameters are set by request command.<br>
	Response is returned value.<br>
	<br>
	READ
 --- REMOVE_SECURE BEGIN ---
	and CRYPTO
 --- REMOVE_SECURE END ---
    commands response format.
	<table>
		<tr>
			<th>No.</th>
			<th>code/parameter/return</th>
			<th>comment</th>
		</tr>
		<tr>
			<td>#0</td>
			<td>CMD code</td>
			<td>Request command + 0x01000000</td>
		</tr>
		<tr>
			<td>#1</td>
			<td>1st parameter</td>
			<td>Request paremeter #1</td>
		</tr>
		<tr>
			<td>#2</td>
			<td>2nd parameter</td>
			<td>Request paremeter #2</td>
		</tr>
		<tr>
			<td>...</td>
			<td>...</td>
			<td>...</td>
		</tr>
		<tr>
			<td>#8 (last)</td>
			<td>RETVAL</td>
			<td>Returned Value (Response Value)</td>
		</tr>
	</table><br>
	GET commands response format.
	<table>
		<tr>
			<th>No.</th>
			<th>code/parameter/return</th>
			<th>comment</th>
		</tr>
		<tr>
			<td>#0</td>
			<td>CMD code</td>
			<td>Request command + 0x01000000</td>
		</tr>
		<tr>
			<td>#1</td>
			<td>ID</td>
			<td>GET command ID</td>
		</tr>
		<tr>
			<td>#2</td>
			<td>Get data</td>
			<td>For more information refer to the "GET Command Response Information"</td>
		</tr>
		<tr>
			<td>...</td>
			<td>...</td>
			<td>...</td>
		</tr>
		<tr>
			<td>#8</td>
			<td>Get data</td>
			<td>For more information refer to the "GET Command Response Information"</td>
		</tr>
	</table><br>
		GET Command Response Information
			- Temperature sensor
			<table>
				<tr>
					<th>No.</th>
					<th>return</th>
					<th>comment</th>
				</tr>
				<tr>
					<td>#2</td>
					<td>temperature</td>
					<td>
						Measured temperature data<br>
						-256.000(=0xFFFFF800) degree - 255.875(=0x000007FF) degree
					</td>
				</tr>
			</table><br>
			- Calendar
			<table>
				<tr>
					<th>No.</th>
					<th>return</th>
					<th>comment</th>
				</tr>
				<tr>
					<td>#2</td>
					<td>Second</td>
					<td>0 - 59</td>
				</tr>
				<tr>
					<td>#3</td>
					<td>Minute</td>
					<td>0 - 59</td>
				</tr>
				<tr>
					<td>#4</td>
					<td>Hour</td>
					<td>0 - 23</td>
				</tr>
				<tr>
					<td>#5</td>
					<td>Day</td>
					<td>1 - 31</td>
				</tr>
				<tr>
					<td>#6</td>
					<td>Day of week</td>
					<td>0(Sunday) - 6(Saturday)</td>
				</tr>
				<tr>
					<td>#7</td>
					<td>Month</td>
					<td>1(January) - 12(December)</td>
				</tr>
				<tr>
					<td>#8</td>
					<td>Year</td>
					<td>0 - 99 (Yaer 20"00" - 20"99")</td>
				</tr>
			</table><br>
 --- REMOVE_SECURE BEGIN ---
			- E-Fuse Code
			<table>
				<tr>
					<th>No.</th>
					<th>return</th>
					<th>comment</th>
				</tr>
				<tr>
					<td>#2</td>
					<td>1st parameter</td>
					<td>Request paremeter #1</td>
				</tr>
				<tr>
					<td>...</td>
					<td>...</td>
					<td>...</td>
				</tr>
				<tr>
					<td>#4</td>
					<td>3rd parameter</td>
					<td>Request paremeter #3</td>
				</tr>
				<tr>
					<td>#5</td>
					<td>Reserved</td>
					<td></td>
				</tr>
				<tr>
					<td>...</td>
					<td>...</td>
					<td>...</td>
				</tr>
				<tr>
					<td>#7</td>
					<td>Reserved</td>
					<td></td>
				</tr>
				<tr>
					<td>#8</td>
					<td>RETVAL</td>
					<td>0:OK, -1:No Written, -2:NG</td>
				</tr>
			</table><br>
 --- REMOVE_SECURE END ---
	
	Cortex-M0 will send notification to Cortex-A7 depended on Cortex-A7 SET NOTIFY command.
	- code of send notification
	<table>
		<tr>
			<th>CMD code #0</th>
			<th>Notification</th>
		</tr>
		<tr>
			<td>0x03</td>
			<td>UART send</td>
		</tr>
		<tr>
			<td>0x04</td>
			<td>UART receive</td>
		</tr>
		<tr>
			<td>0x05</td>
			<td>SPI send</td>
		</tr>
		<tr>
			<td>0x06</td>
			<td>SPI receive</td>
		</tr>
		<tr>
			<td>0x07</td>
			<td>I2C send</td>
		</tr>
		<tr>
			<td>0x08</td>
			<td>I2C receive</td>
		</tr>
		<tr>
			<td>0x09</td>
			<td>ADC</td>
		</tr>
		<tr>
			<td>0x0A</td>
			<td>Temperature Sensor</td>
		</tr>
	</table>
		- Temperature sensor
		<table>
			<tr>
				<th>No.</th>
				<th>parameter</th>
				<th>comment</th>
			</tr>
			<tr>
				<td>#1</td>
				<td>Temperature threshold reached</td>
				<td>
					1:low temperature excess<br>
					2:nearby low temperature<br>
					3:nearby high temperature<br>
					4:high temperature excess
				</td>
			</tr>
		</table><br>
	
	<br>
	Detail:
	- Command : WRITE
		<table>
			<tr>
				<th>No.</th>
				<th>parameter</th>
				<th>comment</th>
			</tr>
			<tr>
				<td>#1</td>
				<td>type</td>
				<td>0:word(32bit), 1:hword(16bit), 2:byte(8bit)</td>
			</tr>
			<tr>
				<td>#2</td>
				<td>address</td>
				<td>start address</td>
			</tr>
			<tr>
				<td>#3</td>
				<td>data</td>
				<td>value</td>
			</tr>
		</table><br>
	- Command : READ
		<table>
			<tr>
				<th>No.</th>
				<th>parameter</th>
				<th>comment</th>
			</tr>
			<tr>
				<td>#1</td>
				<td>type</td>
				<td>0:word(32bit), 1:hword(16bit), 2:byte(8bit)</td>
			</tr>
			<tr>
				<td>#2</td>
				<td>address</td>
				<td>start address</td>
			</tr>
		</table><br>
	- Command : RMW<br>
		<table>
			<tr>
				<th>No.</th>
				<th>parameter</th>
				<th>comment</th>
			</tr>
			<tr>
				<td>#1</td>
				<td>type</td>
				<td>0:word(32bit), 1:hword(16bit), 2:byte(8bit)</td>
			</tr>
			<tr>
				<td>#2</td>
				<td>address</td>
				<td>start address</td>
			</tr>
			<tr>
				<td>#3</td>
				<td>data</td>
				<td>value</td>
			</tr>
			<tr>
				<td>#4</td>
				<td>mask</td>
				<td>mask pattern(bit sequence). 0:no used, 1:update</td>
			</tr>
		</table><br>
	- Command : SET<br>
		<table>
			<tr>
				<th>No.</th>
				<th>parameter</th>
				<th>comment</th>
			</tr>
			<tr>
				<td>#1</td>
				<td>ID</td>
				<td>For more information refer to the "SET Command Parameter Information".</td>
			</tr>
		</table><br>
		SET Command Parameter Information<br>
		  - buffer
			<table>
				<tr>
					<th>ID</th>
					<th>comment</th>
				</tr>
				<tr>
					<td>0x01</td>
					<td>-</td>
				</tr>
				<tr>
					<td>0x02</td>
					<td>-</td>
				</tr>
				<tr>
					<td>0x03</td>
					<td>UART output buffer for send</td>
				</tr>
				<tr>
					<td>0x04</td>
					<td>UART input buffer for receive</td>
				</tr>
				<tr>
					<td>0x05</td>
					<td>SPI output buffer for send</td>
				</tr>
				<tr>
					<td>0x06</td>
					<td>SPI input buffer for receive</td>
				</tr>
				<tr>
					<td>0x07</td>
					<td>I2C output buffer for send</td>
				</tr>
				<tr>
					<td>0x08</td>
					<td>I2C input buffer for receive</td>
				</tr>
				<tr>
					<td>0x09</td>
					<td>ADC input data buffer</td>
				</tr>
				<tr>
					<td>0x0A</td>
					<td>Temperature sensor input buffer</td>
				</tr>
			</table><br>
			Temperature input data buffer for recieve
			<table>
				<tr>
					<th>No.</th>
					<th>parameter</th>
					<th>comment</th>
				</tr>
				<tr>
					<td>#2</td>
					<td>high temperature excess</td>
					<td>
						Threshold temperature setting.<br>
						-256.000(=0xFFFFF800) degree - 255.875(=0x000007FF) degree
					</td>
				</tr>
				<tr>
					<td>#3</td>
					<td>nearby high temperature</td>
					<td>
						Threshold temperature setting.<br>
						-256.000(=0xFFFFF800) degree - 255.875(=0x000007FF) degree
					</td>
				</tr>
				<tr>
					<td>#4</td>
					<td>low temperature excess</td>
					<td>
						Threshold temperature setting.<br>
						-256.000(=0xFFFFF800) degree - 255.875(=0x000007FF) degree
					</td>
				</tr>
				<tr>
					<td>#5</td>
					<td>nearby low temperature</td>
					<td>
						Threshold temperature setting.<br>
						-256.000(=0xFFFFF800) degree - 255.875(=0x000007FF) degree
					</td>
				</tr>
			</table><br>
			UART/SPI/I2C/ADC input data buffer for recieve
			<table>
				<tr>
					<th>No.</th>
					<th>parameter</th>
					<th>comment</th>
				</tr>
				<tr>
					<td>#2</td>
					<td>address</td>
					<td>start address of buffer</td>
				</tr>
				<tr>
					<td>#3</td>
					<td>size</td>
					<td>byte size. 0:no-buffer</td>
				</tr>
				<tr>
					<td>#4</td>
					<td>pointer</td>
					<td>address of write pointer</td>
				</tr>
				<tr>
					<td>#5</td>
					<td>overlap</td>
					<td>0:no-overlap, 1:cyclic, 2:same position</td>
				</tr>
			</table><br>
			UART/SPI/I2C output data buffer for send
			<table>
				<tr>
					<th>No.</th>
					<th>parameter</th>
					<th>comment</th>
				</tr>
				<tr>
					<td>#2</td>
					<td>address</td>
					<td>start address of buffer</td>
				</tr>
				<tr>
					<td>#3</td>
					<td>size</td>
					<td>byte size. 0:no-buffer</td>
				</tr>
			</table><br>
		  - NOTIFY<br>
			Notification is worked in normal state.
			<table>
				<tr>
					<th>ID</th>
					<th>comment</th>
				</tr>
				<tr>
					<td>0x1001</td>
					<td>-</td>
				</tr>
				<tr>
					<td>0x1002</td>
					<td>-</td>
				</tr>
				<tr>
					<td>0x1003</td>
					<td>UART send</td>
				</tr>
				<tr>
					<td>0x1004</td>
					<td>UART receive</td>
				</tr>
				<tr>
					<td>0x1005</td>
					<td>SPI send</td>
				</tr>
				<tr>
					<td>0x1006</td>
					<td>SPI receive</td>
				</tr>
				<tr>
					<td>0x1007</td>
					<td>I2C send</td>
				</tr>
				<tr>
					<td>0x1008</td>
					<td>I2C receive</td>
				</tr>
				<tr>
					<td>0x1009</td>
					<td>ADC</td>
				</tr>
				<tr>
					<td>0x100A</td>
					<td>Temperature Sensor</td>
				</tr>
			</table><br>
			notify enable
			<table>
				<tr>
					<th>No.</th>
					<th>parameter</th>
					<th>comment</th>
				</tr>
				<tr>
					<td>#2</td>
					<td>enable flag</td>
					<td>0:no-notify, 1:notify by Cortex-M0 IPCU</td>
				</tr>
			</table><br>
		  - Calendar data (Date and time)
			<table>
				<tr>
					<th>ID</th>
					<th>comment</th>
				</tr>
				<tr>
					<td>0x3001</td>
					<td>Calendar data</td>
				</tr>
			</table><br>
			Date and time
			<table>
				<tr>
					<th>No.</th>
					<th>parameter</th>
					<th>comment</th>
				</tr>
				<tr>
					<td>#2</td>
					<td>Second</td>
					<td>0 - 59</td>
				</tr>
				<tr>
					<td>#3</td>
					<td>Minute</td>
					<td>0 - 59</td>
				</tr>
				<tr>
					<td>#4</td>
					<td>Hour</td>
					<td>0 - 23</td>
				</tr>
				<tr>
					<td>#5</td>
					<td>Day</td>
					<td>1 - 31</td>
				</tr>
				<tr>
					<td>#6</td>
					<td>Day of week</td>
					<td>0(Sunday) - 6(Saturday)</td>
				</tr>
				<tr>
					<td>#7</td>
					<td>Month</td>
					<td>1(January) - 12(December)</td>
				</tr>
				<tr>
					<td>#8</td>
					<td>Year</td>
					<td>0 - 99 (Yaer 20"00" - 20"99")</td>
				</tr>
			</table><br>
			The maximum value of the value of the Day(#5) varies by Month(#7) and Year(#8).<br>
			Month:1,3,5,7,8,10,12 -> 31.<br>
			Month:4,6,9,11 -> 30.<br>
			Month:2, Year:leap year -> 29.<br>
			Month:2, Year:common yaer -> 28.<br>
			<br>
	- Command : GET
		<table>
			<tr>
				<th>No.</th>
				<th>parameter</th>
				<th>comment</th>
			</tr>
			<tr>
				<td>#1</td>
				<td>ID</td>
				<td>For more information refer to the "GET Command Parameter Information".</td>
			</tr>
		</table><br>
		GET Command Parameter Information<br>
		  - Temperature Sensor
			<table>
				<tr>
					<th>ID</th>
					<th>comment</th>
				</tr>
				<tr>
					<td>0x0A</td>
					<td>Temperature Sensor</td>
				</tr>
			</table><br>
		  - Calendar data (Date and time)
			<table>
				<tr>
					<th>ID</th>
					<th>comment</th>
				</tr>
				<tr>
					<td>0x3001</td>
					<td>Calendar data</td>
				</tr>
			</table><br>
 --- REMOVE_SECURE BEGIN ---
		  - E-Fuse code
			<table>
				<tr>
					<th>ID</th>
					<th>comment</th>
				</tr>
				<tr>
					<td>0x3002</td>
					<td>E-Fuse code</td>
				</tr>
			</table><br>
			<table>
				<tr>
					<th>No.</th>
					<th>parameter</th>
					<th>comment</th>
				</tr>
				<tr>
					<td>#2</td>
					<td>address</td>
					<td>start address of read data storage destination.</td>
				</tr>
				<tr>
					<td>#3</td>
					<td>offset</td>
					<td>E-Fuse code offset.</td>
				</tr>
				<tr>
					<td>#4</td>
					<td>size</td>
					<td>
						E-Fuse code read size.<br>
						Reads the code of the size from the position of the offset.
					</td>
				</tr>
			</table><br>
 --- REMOVE_SECURE END ---
	- Command : START<br>
		<table>
			<tr>
				<th>No.</th>
				<th>parameter</th>
				<th>comment</th>
			</tr>
			<tr>
				<td>#1</td>
				<td>ID</td>
				<td>Function ID. For more information refer to the "START Command Function ID".</td>
			</tr>
		</table><br>
		START Command Function ID<br>
		<table>
			<tr>
				<th>ID</th>
				<th>comment</th>
			</tr>
			<tr>
				<td>0x01</td>
				<td>-</td>
			</tr>
			<tr>
				<td>0x02</td>
				<td>-</td>
			</tr>
			<tr>
				<td>0x03</td>
				<td>UART send</td>
			</tr>
			<tr>
				<td>0x04</td>
				<td>UART receive</td>
			</tr>
			<tr>
				<td>0x05</td>
				<td>SPI send</td>
			</tr>
			<tr>
				<td>0x06</td>
				<td>SPI receive</td>
			</tr>
			<tr>
				<td>0x07</td>
				<td>I2C send</td>
			</tr>
			<tr>
				<td>0x08</td>
				<td>I2C receive</td>
			</tr>
			<tr>
				<td>0x09</td>
				<td>ADC</td>
			</tr>
			<tr>
				<td>0x0A</td>
				<td>Temperature Sensor</td>
			</tr>
		</table><br>
	- Command : STOP<br>
		<table>
			<tr>
				<th>No.</th>
				<th>parameter</th>
				<th>comment</th>
			</tr>
			<tr>
				<td>#1</td>
				<td>ID</td>
				<td>Function ID. For more information refer to the "STOP Command Function ID".</td>
			</tr>
		</table><br>
		STOP Command Function ID<br>
		<table>
			<tr>
				<th>ID</th>
				<th>comment</th>
			</tr>
			<tr>
				<td>0x01</td>
				<td>-</td>
			</tr>
			<tr>
				<td>0x02</td>
				<td>-</td>
			</tr>
			<tr>
				<td>0x03</td>
				<td>-</td>
			</tr>
			<tr>
				<td>0x04</td>
				<td>UART receive</td>
			</tr>
			<tr>
				<td>0x05</td>
				<td>-</td>
			</tr>
			<tr>
				<td>0x06</td>
				<td>SPI receive</td>
			</tr>
			<tr>
				<td>0x07</td>
				<td>-</td>
			</tr>
			<tr>
				<td>0x08</td>
				<td>I2C receive</td>
			</tr>
			<tr>
				<td>0x09</td>
				<td>ADC</td>
			</tr>
			<tr>
				<td>0x0A</td>
				<td>Temperature Sensor</td>
			</tr>
		</table><br>
 --- REMOVE_SECURE BEGIN ---
	- Command : CRYPTO<br>
		<table>
			<tr>
				<th>No.</th>
				<th>parameter</th>
				<th>comment</th>
			</tr>
			<tr>
				<td>#1</td>
				<td>digest address</td>
				<td>start address for digest 256bit code.</td>
			</tr>
			<tr>
				<td>#2</td>
				<td>address</td>
				<td>
					start address of target data array.<br>
					configuration of the data array : secure code start address and byte size.
				</td>
			</tr>
			<tr>
				<td>#3</td>
				<td>count</td>
				<td>number of target data array.</td>
			</tr>
		</table><br>
 --- REMOVE_SECURE END ---
	- StateAterStandby<br>
		<table>
			<tr>
				<th>No.</th>
				<th>parameter</th>
				<th>comment</th>
			</tr>
			<tr>
				<td>#1</td>
				<td>State after Standby state</td>
				<td>0 : keep standby state to wait for resume<br>
				    1 : wakeup state<br>
				    2 : power off</td>
			</tr>
			<tr>
				<td>#2</td>
				<td>wait time</td>
				<td>0 : soon<br>
				    other : waiting second</td>
			</tr>
		</table><br>
	
	<br>
	All Command List ==============================================================================
	- Send command from CA7 to CM0
		<table>
			<tr>
				<th>command</th>
				<th>#0</th>
				<th>#1</th>
				<th>#2</th>
				<th>#3</th>
				<th>#4</th>
				<th>#5</th>
				<th>#6</th>
				<th>#7</th>
				<th>#8</th>
			</tr>
			<tr>
				<td>GO</td>
				<td>0x01</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>WRITE</td>
				<td>0x02</td>
				<td>type</td>
				<td>address</td>
				<td>data</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>READ</td>
				<td>0x03</td>
				<td>type</td>
				<td>address</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>RMW</td>
				<td>0x04</td>
				<td>type</td>
				<td>address</td>
				<td>data</td>
				<td>mask</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    UART output buffer for send</td>
				<td>0x05</td>
				<td>0x03</td>
				<td>address</td>
				<td>size</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    UART input buffer for receive</td>
				<td>0x05</td>
				<td>0x04</td>
				<td>address</td>
				<td>size</td>
				<td>pointer</td>
				<td>overlap</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    SPI output buffer for send</td>
				<td>0x05</td>
				<td>0x05</td>
				<td>address</td>
				<td>size</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    SPI input buffer for receive</td>
				<td>0x05</td>
				<td>0x06</td>
				<td>address</td>
				<td>size</td>
				<td>pointer</td>
				<td>overlap</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    I2C output buffer for send</td>
				<td>0x05</td>
				<td>0x07</td>
				<td>address</td>
				<td>size</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    I2C input buffer for receive</td>
				<td>0x05</td>
				<td>0x08</td>
				<td>address</td>
				<td>size</td>
				<td>pointer</td>
				<td>overlap</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    ADC input data buffer</td>
				<td>0x05</td>
				<td>0x09</td>
				<td>address</td>
				<td>size</td>
				<td>pointer</td>
				<td>overlap</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    Temperature sensor input buffer</td>
				<td>0x05</td>
				<td>0x0A</td>
				<td>high temperature excess</td>
				<td>nearby high temperature</td>
				<td>low temperature excess</td>
				<td>nearby low temperature</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    NOTIFY for UART send</td>
				<td>0x05</td>
				<td>0x1003</td>
				<td>enable flag</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    NOTIFY for UART receive</td>
				<td>0x05</td>
				<td>0x1004</td>
				<td>enable flag</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    NOTIFY for SPI send</td>
				<td>0x05</td>
				<td>0x1005</td>
				<td>enable flag</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    NOTIFY for SPI receive</td>
				<td>0x05</td>
				<td>0x1006</td>
				<td>enable flag</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    NOTIFY for I2C send</td>
				<td>0x05</td>
				<td>0x1007</td>
				<td>enable flag</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    NOTIFY for I2C receive</td>
				<td>0x05</td>
				<td>0x1008</td>
				<td>enable flag</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    NOTIFY for ADC</td>
				<td>0x05</td>
				<td>0x1009</td>
				<td>enable flag</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    NOTIFY for Temperature Sensor</td>
				<td>0x05</td>
				<td>0x100A</td>
				<td>enable flag</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>SET<br>
				    Calendar data</td>
				<td>0x05</td>
				<td>0x3001</td>
				<td>Second</td>
				<td>Minute</td>
				<td>Hour</td>
				<td>Day</td>
				<td>Day of week</td>
				<td>Month</td>
				<td>Year</td>
			</tr>
			<tr>
				<td>GET<br>
				    Temperature Sensor</td>
				<td>0x06</td>
				<td>0x0A</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>GET<br>
				    Calendar data</td>
				<td>0x06</td>
				<td>0x3001</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>START<br>
				    UART send</td>
				<td>0x07</td>
				<td>0x03</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>START<br>
				    UART receive</td>
				<td>0x07</td>
				<td>0x04</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>START<br>
				    SPI send</td>
				<td>0x07</td>
				<td>0x05</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>START<br>
				    SPI receive</td>
				<td>0x07</td>
				<td>0x06</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>START<br>
				    I2C send</td>
				<td>0x07</td>
				<td>0x07</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>START<br>
				    I2C receive</td>
				<td>0x07</td>
				<td>0x08</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>START<br>
				    ADC</td>
				<td>0x07</td>
				<td>0x09</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>START<br>
				    Temperature Sensor</td>
				<td>0x07</td>
				<td>0x0A</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>STOP<br>
				    UART receive</td>
				<td>0x08</td>
				<td>0x04</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>STOP<br>
				    SPI receive</td>
				<td>0x08</td>
				<td>0x06</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>STOP<br>
				    I2C receive</td>
				<td>0x08</td>
				<td>0x08</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>STOP<br>
				    ADC</td>
				<td>0x08</td>
				<td>0x09</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>STOP<br>
				    Temperature Sensor </td>
				<td>0x08</td>
				<td>0x0A</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>StateAterStandby</td>
				<td>0x0A</td>
				<td>State</td>
				<td>wait time</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
		</table><br>
	- Send command from CM0 to CA7
		<table>
			<tr>
				<th>command</th>
				<th>#0</th>
				<th>#1</th>
				<th>#2</th>
				<th>#3</th>
				<th>#4</th>
				<th>#5</th>
				<th>#6</th>
				<th>#7</th>
				<th>#8</th>
			</tr>
			<tr>
				<td>NOTIFY<br>
				    UART send</td>
				<td>0x03</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>NOTIFY<br>
				    UART receive</td>
				<td>0x04</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>NOTIFY<br>
				    SPI send</td>
				<td>0x05</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>NOTIFY<br>
				    SPI receive</td>
				<td>0x06</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>NOTIFY<br>
				    I2C send</td>
				<td>0x07</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>NOTIFY<br>
				    I2C receive</td>
				<td>0x08</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>NOTIFY<br>
				    ADC</td>
				<td>0x09</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
			<tr>
				<td>NOTIFY<br>
				    Temperature Sensor</td>
				<td>0x0A</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
				<td> -</td>
			</tr>
		</table><br>
		
@}*/	// fj_arm_overview

/** @weakgroup fj_arm_sequence
@{
	- IPCU(Inter-Processor Control Unit) Sequences for the communication between Linux and RTOS.
	-# @ref fj_ipcu_sequence_share_async_RTOS_to_Linux
	<br><br>
	-# @ref fj_ipcu_sequence_noshare_async_RTOS_to_Linux
	<br><br>
	-# @ref fj_ipcu_sequence_share_sync_RTOS_to_Linux
	<br><br>
	-# @ref fj_ipcu_sequence_noshare_sync_RTOS_to_Linux
	<br><br>
	-# @ref fj_ipcu_sequence_share_async_Linux_to_RTOS
	<br><br>
	-# @ref fj_ipcu_sequence_noshare_async_Linux_to_RTOS
	<br><br>
	-# @ref fj_ipcu_sequence_share_sync_Linux_to_RTOS
	<br><br>
	-# @ref fj_ipcu_sequence_noshare_sync_Linux_to_RTOS
	<br><br>
	<hr>
	
	@section fj_ipcu_sequence_share_async_RTOS_to_Linux IPCU Command Sending Sequence with shared memory and Asynchronous type function from RTOS to Linux.
	@image html fj_ipcu_sequence_share_async_RTOS_to_Linux.png
	<br>
	
	@section fj_ipcu_sequence_noshare_async_RTOS_to_Linux IPCU Command Sending Sequence with noshared memory and Asynchronous type function from RTOS to Linux.
	@image html fj_ipcu_sequence_noshare_async_RTOS_to_Linux.png
	<br>
	
	@section fj_ipcu_sequence_share_sync_RTOS_to_Linux IPCU Command Sending Sequence with shared memory and Synchronous type function from RTOS to Linux.
	@image html fj_ipcu_sequence_share_sync_RTOS_to_Linux.png
	<br>
	@section fj_ipcu_sequence_noshare_sync_RTOS_to_Linux IPCU Command Sending Sequence with noshared memory and Synchronous type function from RTOS to Linux.
	@image html fj_ipcu_sequence_noshare_sync_RTOS_to_Linux.png
	<br>
	
	@section fj_ipcu_sequence_share_async_Linux_to_RTOS IPCU Command Sending Sequence with shared memory and Asynchronous type function from Linux to RTOS.
	@image html fj_ipcu_sequence_share_async_Linux_to_RTOS.png
	<br>
	
	@section fj_ipcu_sequence_noshare_async_Linux_to_RTOS IPCU Command Sending Sequence with noshared memory and Asynchronous type function from Linux to RTOS.
	@image html fj_ipcu_sequence_noshare_async_Linux_to_RTOS.png
	<br>
	
	@section fj_ipcu_sequence_share_sync_Linux_to_RTOS IPCU Command Sending Sequence with shared memory and Synchronous type function from Linux to RTOS.
	@image html fj_ipcu_sequence_share_sync_Linux_to_RTOS.png
	<br>
	@section fj_ipcu_sequence_noshare_sync_Linux_to_RTOS IPCU Command Sending Sequence with noshared memory and Synchronous type function from Linux to RTOS.
	@image html fj_ipcu_sequence_noshare_sync_Linux_to_RTOS.png
	<br>

@}*/	// fj_arm_sequence

/** @weakgroup fj_arm_definition
@{*/
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define FJ_IPCU_MB_DATA_MAX				(9)		// number of data in a Mail Box

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Target CPU(core) */
typedef enum {
	E_FJ_GIC_TARGET_NO_TARGET = -1,					/**< No target. To change other parameters (enable or priority). */
	E_FJ_GIC_TARGET_OWN_CPU = 0,					/**< Own CPU */
	E_FJ_GIC_TARGET_CPU_ID0,						/**< CPU ID = 0 */
	E_FJ_GIC_TARGET_CPU_ID1,						/**< CPU ID = 1 */
	E_FJ_GIC_TARGET_CPU_ID2,						/**< CPU ID = 2 */
	E_FJ_GIC_TARGET_CPU_ID3							/**< CPU ID = 3 */
} E_FJ_GIC_TARGET_CPU;

/** IPCU's mailbox and User's definition command type */
typedef enum {
	E_FJ_IPCU_MAILBOX_TYPE_0 = 0,					/**< mailbox-0  and command type-0  */
	E_FJ_IPCU_MAILBOX_TYPE_1,						/**< mailbox-1  and command type-1  */
	E_FJ_IPCU_MAILBOX_TYPE_2,						/**< mailbox-2  and command type-2  */
	E_FJ_IPCU_MAILBOX_TYPE_3,						/**< mailbox-3  and command type-3  */
	E_FJ_IPCU_MAILBOX_TYPE_4,						/**< mailbox-4  and command type-4  */
	E_FJ_IPCU_MAILBOX_TYPE_5,						/**< mailbox-5  and command type-5  */
	E_FJ_IPCU_MAILBOX_TYPE_6,						/**< mailbox-6  and command type-6  */
	E_FJ_IPCU_MAILBOX_TYPE_7,						/**< mailbox-7  and command type-7  */
	E_FJ_IPCU_MAILBOX_TYPE_8,						/**< mailbox-8  and command type-8  */
	E_FJ_IPCU_MAILBOX_TYPE_9,						/**< mailbox-9  and command type-9  */
	E_FJ_IPCU_MAILBOX_TYPE_10,						/**< mailbox-10 and command type-10 */
	E_FJ_IPCU_MAILBOX_TYPE_11,						/**< mailbox-11 and command type-11 */
	E_FJ_IPCU_MAILBOX_TYPE_12,						/**< mailbox-12 and command type-12 */
	E_FJ_IPCU_MAILBOX_TYPE_13,						/**< mailbox-13 and command type-13 */
	E_FJ_IPCU_MAILBOX_TYPE_14,						/**< mailbox-14 and command type-14 */
	E_FJ_IPCU_MAILBOX_TYPE_15,						/**< mailbox-15 and command type-15 */
	E_FJ_IPCU_MAILBOX_TYPE_MAX
} E_FJ_IPCU_MAILBOX_TYPE;

/** Function type of buffer(CM0) */
typedef enum {
	E_FJ_IPCU_BUFF_UART_SEND = 3,					/**< UART output buffer for send  		*/
	E_FJ_IPCU_BUFF_UART_RECEIVE,					/**< UART input buffer for receive  	*/
	E_FJ_IPCU_BUFF_SPI_SEND,						/**< SPI output buffer for send  		*/
	E_FJ_IPCU_BUFF_SPI_RECEIVE,						/**< SPI input buffer for receive  		*/
	E_FJ_IPCU_BUFF_I2C_SEND,						/**< I2C output buffer for send 		*/
	E_FJ_IPCU_BUFF_I2C_RECEIVE,						/**< I2C input buffer for receive		*/
	E_FJ_IPCU_BUFF_ADC,								/**< ADC input data buffer 				*/
	E_FJ_IPCU_BUFF_TEMPERA,							/**< Temperature sensor input buffer 	*/
	E_FJ_IPCU_BUFF_MAX,
} E_FJ_IPCU_BUFF;

/** Function type of notice(CM0) */
typedef enum {
	E_FJ_IPCU_NOTIFY_UART_SEND = 0x1003,			/**< UART send  		*/
	E_FJ_IPCU_NOTIFY_UART_RECEIVE,					/**< UART receive  		*/
	E_FJ_IPCU_NOTIFY_SPI_SEND,						/**< SPI send  			*/
	E_FJ_IPCU_NOTIFY_SPI_RECEIVE,					/**< SPI receive  		*/
	E_FJ_IPCU_NOTIFY_I2C_SEND,						/**< I2C send			*/
	E_FJ_IPCU_NOTIFY_I2C_RECEIVE,					/**< I2C receive 		*/
	E_FJ_IPCU_NOTIFY_ADC,							/**< ADC 				*/
	E_FJ_IPCU_NOTIFY_TEMPERA,						/**< Temperature sensor */
	E_FJ_IPCU_NOTIFY_MAX,
} E_FJ_IPCU_NOTIFY;

/** Function type of setting(CM0) */
typedef enum {
	E_FJ_IPCU_SET_ADVFS_EN = 0x0B,					/**< Set Advfs enable/disable state */
	E_FJ_IPCU_SET_ADVFS = 0x0B1,					/**< Set Dvfs votage */
	E_FJ_IPCU_SET_PLL1_FRQ_BUF = 0x0B2,				/**< PLLFREQ1 input buffer 	*/
	E_FJ_IPCU_SET_WAKE_UP = 0x2001,					/**< Condition to go to wakeup state */
	E_FJ_IPCU_SET_CALENDAR = 0x3001,				/**< Calendar data */
	E_FJ_IPCU_SET_RTC_TIME = 0x3003,				/**< RTC time data */
	E_FJ_IPCU_SET_MAX
} E_FJ_IPCU_SET;

/** Function type of getting(CM0) */
typedef enum {
	E_FJ_IPCU_GET_TEMPERA	= 0x0A,					/**< Temperature sensor */
	E_FJ_IPCU_GET_ADVFS_EN	= 0x0B,					/**< Get Advfs enable/disable state */
	E_FJ_IPCU_GET_ADVFS		= 0x0B1,				/**< Get Dvfs votage */
	E_FJ_IPCU_GET_PLL1_FRQ_BUF = 0x0B2,				/**< PLLFREQ1 input buffer 	*/
	E_FJ_IPCU_GET_TEMPERA_BUF = 0x0B3,				/**< TEMPERA buffer 	*/
	E_FJ_IPCU_GET_CALENDAR	= 0x3001,				/**< Calendar data */
// --- REMOVE_SECURE BEGIN ---
	E_FJ_IPCU_GET_E_FUSE	= 0x3002,				/**< E-Fuse code */
// --- REMOVE_SECURE END ---
	E_FJ_IPCU_GET_MAX
} E_FJ_IPCU_COND;

/** Function type of starting(CM0) */
typedef enum {
	E_FJ_IPCU_START_UART_SEND = 0x03,				/**< UART send  		*/
	E_FJ_IPCU_START_UART_RECEIVE,					/**< UART receive  		*/
	E_FJ_IPCU_START_SPI_SEND,						/**< SPI send  			*/
	E_FJ_IPCU_START_SPI_RECEIVE,					/**< SPI receive  		*/
	E_FJ_IPCU_START_I2C_SEND,						/**< I2C send			*/
	E_FJ_IPCU_START_I2C_RECEIVE,					/**< I2C receive 		*/
	E_FJ_IPCU_START_ADC,							/**< ADC 				*/
	E_FJ_IPCU_START_TEMPERA,						/**< Temperature sensor */
	E_FJ_IPCU_START_MAX
} E_FJ_IPCU_START;

/** Function type of stopping(CM0) */
typedef enum {
	E_FJ_IPCU_STOP_UART_RECEIVE = 0x04,				/**< UART receive  		*/
	E_FJ_IPCU_STOP_SPI_RECEIVE = 0x06,				/**< SPI receive  		*/
	E_FJ_IPCU_STOP_I2C_RECEIVE = 0x08,				/**< I2C receive 		*/
	E_FJ_IPCU_STOP_ADC,								/**< ADC 				*/
	E_FJ_IPCU_STOP_TEMPERA,							/**< Temperature sensor */
	E_FJ_IPCU_STOP_MAX
} E_FJ_IPCU_STOP;

	/** State ater standby state(CM0) */
typedef enum {
	E_FJ_IPCU_STATE_AFTER_STANDY_WAIT = 0,	/**< keep standby state to wait for resume */
	E_FJ_IPCU_STATE_AFTER_STANDY_WAKEUP,	/**< wakeup state */
	E_FJ_IPCU_STATE_AFTER_STANDY_POWER_OFF,	/**< Power off state */
	E_FJ_IPCU_STATE_AFTER_STANDY_ADJUST_DVFS,	/**< adjustment DVFS */
	E_FJ_IPCU_STATE_AFTER_STANDY_DVFS_EN,	/**< En/Dis Auto DVFS */
	E_FJ_IPCU_STATE_AFTER_STANDY_MAX
} E_FJ_IPCU_STATE_AFTER_STANDY;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** Temperature data */
typedef struct {
	ULONG	ipcu_high_tempera;			/**< High temperature excess	*/
	ULONG	ipcu_near_by_high;			/**< Near by high temperature	*/
	ULONG	ipcu_near_by_low;			/**< Near by Low temperature 	*/
	ULONG	ipcu_low_tempera;			/**< Low temperature excess		*/
} T_FJ_ARM_IPCU_TEMPERA;

/** Calendar data */
typedef struct {
	ULONG	ipcu_second;				/**< Second(0-59)							*/
	ULONG	ipcu_minute;				/**< Minute(0-59)							*/
	ULONG	ipcu_hour;					/**< Hour(0-23)								*/
	ULONG	ipcu_day;					/**< Day(1-31)								*/
	ULONG	ipcu_day_of_week;			/**< Day of week(0(Sunday)-6(Saturaday))	*/
	ULONG	ipcu_month;					/**< Month(1(January)-12(December))			*/
	ULONG	ipcu_year;					/**< Year(0-99(Year20"00"-20"99"))			*/
} T_FJ_ARM_IPCU_CALENDAR;

typedef struct {
	ULONG	AVS;
	ULONG	DVFS;
} T_FJ_ARM_IPCU_ADVFS;

// --- REMOVE_SECURE BEGIN ---
/** Secure code data */
typedef struct {
	ULONG	ipcu_code_address;			/**< secure code start address	*/
	ULONG	ipcu_code_size;				/**< secure code size[byte]		*/
} T_FJ_ARM_IPCU_SECURE_CODE;
// --- REMOVE_SECURE END ---

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/* @} */	// fj_arm_definition

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/** @weakgroup fj_arm_api
@{*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
GIC (General Interrupt Control) Set Resource(enable, priority and target cpu), indicated by Interrupt ID number.
@param[in] intid		: Interrupt ID (0 to 223).<br>
							Refer to interrupt control section on LSI Specification document.<br><br>
@param[in] set_enable	: Interrupt Set-enable (0 to 1)
							<ul>
							<li>0:Disable<br>
							<li>1:Enable
							</ul>
@param[in] intrpt_pri	: Interrupt Priority Value (-1 to 248)<br>
							<ul>
							<li>-1:Not set
							<li>0, 8, 16, 24, ..., 248 (8 steps):Priority value
							</ul>
@param[in] cpu_target	: Target CPU(core). See @ref E_FJ_GIC_TARGET_CPU
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG		: Error
*/
FJ_ERR_CODE FJ_GIC_Ctrl(UINT32 intid, ULONG set_enable, LONG intrpt_pri, E_FJ_GIC_TARGET_CPU cpu_target);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Open mailbox and interrupt channel for IPCU (Inter-Processor Control Unit)
@param[in] ipctype: indicated mailbox and command type. See @ref E_FJ_IPCU_MAILBOX_TYPE
@param[out] id: instance ID.
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG		: Error
*/
FJ_ERR_CODE FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE ipctype, UCHAR *id);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Close mailbox and interrupt channel for IPCU (Inter-Processor Control Unit)
@param[in] id: instance ID.
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG		: Error
*/
FJ_ERR_CODE FJ_IPCU_Close(UCHAR id);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Regist request receive callback function for IPCU (Inter-Processor Control Unit)
@param[in] id: instance ID.
@param[in] req_fn_ptr: callback function for request interrupt.
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG		: Error
@remarks (*req_fn_ptr)() function will be called on a task, not on any interrupt.
*/
FJ_ERR_CODE FJ_IPCU_SetReceiverCB(UCHAR id, VOID (*req_fn_ptr)(UCHAR,UINT32,UINT32,UINT32,UINT32));

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Send data by IPCU (Inter-Processor Control Unit)
@param[in] id: instance ID.
@param[in] pdata: top address of the sending data table.
@param[in] length: byte size of data.
@param[in] sync: selection for sync and async type function<br>
				value 0 means Asynchronous type function.<br>
				value 1 means Synchronous type function.
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the reqest callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
@remarks This API checks the data table on shared or none-shared memory.<br>
	If none-shared memory, then this will copy data table to internal reserved shared memory. Its max size is 32kByte.
*/
FJ_ERR_CODE FJ_IPCU_Send(UCHAR id, UINT32 pdata, UINT32 length, UCHAR sync);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set state after standby state of CM0 by IPCU (Inter-Processor Control Unit)<br>
@param [in] next_state:	state after standby @ref E_FJ_IPCU_STATE_AFTER_STANDY
@param [in] wait_time:	(0):soon,  (other):waiting second
@retval FJ_ERR_OK		: Success
*/
FJ_ERR_CODE FJ_IPCU_SetStateAfterStandby_CM0( const E_FJ_IPCU_STATE_AFTER_STANDY next_state, const UINT32 wait_time, const UINT32 mode );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Go to sleep state of CM0 by IPCU (Inter-Processor Control Unit)<br>
See @ref fj_arm_overview - Communication command.
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Sleep_CM0( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Write to memory and memory-mapped register by a data.<br>
See @ref fj_arm_overview - Communication command.
@param[in] type: 0:word(32bit), 1:hword(16bit), 2:byte(8bit)
@param[in] address: start address 
@param[in] data: write data
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Write_CM0( ULONG type, ULONG address, ULONG data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Read a data from memory and memory-mapped register.<br>
See @ref fj_arm_overview - Communication command.
@param[in] type: 0:word(32bit), 1:hword(16bit), 2:byte(8bit)
@param[in] address: start address 
@param[out] read_data: Read data
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Read_CM0( ULONG type, ULONG address, ULONG *read_data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Read-modify-write a data from/to memory and memory-mapped register.<br>
See @ref fj_arm_overview - Communication command.
@param[in] type: 0:word(32bit), 1:hword(16bit), 2:byte(8bit)
@param[in] address: start address 
@param[in] data: write data
@param[in] mask: mask pattern(bit sequence). 0:no used, 1:update  
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Read_Modify_Write_CM0( ULONG type, ULONG address, ULONG data, ULONG mask );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set threshold temperature.<br>
See @ref fj_arm_overview - Communication command.
@param[in] tempera: threshold temperature setting
@remarks If it finds 'END' command, then it stops.
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Set_Tempera_Buff_CM0( T_FJ_ARM_IPCU_TEMPERA tempera );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set UART/SPI output data buffer for send.<br>
See @ref fj_arm_overview - Communication command.
@param[in] type: function buffer type
@param[in] address: start address of buffer 
@param[in] size: byte size. 0:no-buffer 
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Set_Send_Buff_CM0( E_FJ_IPCU_BUFF type, ULONG address, ULONG size );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set UART/SPI/I2C/ADC/Temperature input data buffer for recieve.<br>
See @ref fj_arm_overview - Communication command.
@param[in] type: function buffer type
@param[in] address: start address of buffer
@param[in] size: byte size. 0:no-buffer
@param[in] wpt: address of write pointer
@param[in] overlap: 0:no-overlap, 1:cyclic, 2:same position 
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Set_Receive_Buff_CM0( E_FJ_IPCU_BUFF type, ULONG address, ULONG size, ULONG wpt, ULONG overlap );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set I2C output data buffer for send.<br>
See @ref fj_arm_overview - Communication command.
@param[in] address: start address of buffer 
@param[in] size: byte size. 0:no-buffer 
@param[in] slave_address: I2C slave address
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Set_I2C_Send_Buff_CM0( ULONG address, ULONG size, ULONG slave_address );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set I2C input data buffer for recieve.<br>
See @ref fj_arm_overview - Communication command.
@param[in] address: start address of buffer
@param[in] size: byte size. 0:no-buffer
@param[in] wpt: address of write pointer
@param[in] overlap: 0:no-overlap, 1:cyclic, 2:same position
@param[in] slave_address: I2C slave address
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Set_I2C_Receive_Buff_CM0( ULONG address, ULONG size, ULONG wpt, ULONG overlap, ULONG slave_address );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set Notification is worked in normal state.<br>
See @ref fj_arm_overview - Communication command.
@param[in] func: notice type of IPCU's function
@param[in] enable: 0:no-notify, 1:notify by Cortex-M0 IPCU
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Set_Notify_CM0( E_FJ_IPCU_NOTIFY func, ULONG enable );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set condition to wake up.<br>
See @ref fj_arm_overview - Communication command.
@param[in] enable: 0:not go to wakeup state, 1:go to wakeup state 
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Set_Condition_CM0( ULONG enable );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set calendar data.<br>
See @ref fj_arm_overview - Communication command.
@param[in] calendar : Year, Month, Day of week, Day, Hour, Minute, Second
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Set_Calendar_CM0( T_FJ_ARM_IPCU_CALENDAR calendar );

FJ_ERR_CODE FJ_IPCU_Set_Advfs_CM0( T_FJ_ARM_IPCU_ADVFS advfs );
FJ_ERR_CODE FJ_IPCU_Set_Advfs_EN_CM0( ULONG enable );
FJ_ERR_CODE FJ_IPCU_Set_PLL1_Frq_Buf_CM0( ULONG frq_h, ULONG frq_l );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get Temperature data.<br>
See @ref fj_arm_overview - Communication command.
@param[out] temperature : Measured temperature data(-256.000(=0xFFFFF800)degree - 255.875(=0x000007FF)degree)
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Get_Temperature_CM0( ULONG *temperature );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get calendar data.<br>
See @ref fj_arm_overview - Communication command.
@param[out] calendar : Year, Month, Day of week, Day, Hour, Minute, Second
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Get_Calendar_CM0( T_FJ_ARM_IPCU_CALENDAR *calendar );

FJ_ERR_CODE FJ_IPCU_Get_Advfs_CM0( T_FJ_ARM_IPCU_ADVFS *advfs );
FJ_ERR_CODE FJ_IPCU_Get_Advfs_EN_CM0( ULONG *enable );
FJ_ERR_CODE FJ_IPCU_Get_PLL1_Frq_Buf_CM0( ULONG *frq_h, ULONG *frq_l );
FJ_ERR_CODE FJ_IPCU_Get_Tempera_Buf_CM0( T_FJ_ARM_IPCU_TEMPERA *tempera );

FJ_ERR_CODE FJ_IPCU_Get_CM0( ULONG id, ULONG *get_data );
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Start function in CM0.<br>
See @ref fj_arm_overview - Communication command.
@param[in] func: start function type
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Start_CM0( E_FJ_IPCU_START func );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Stop function in CM0.<br>
See @ref fj_arm_overview - Communication command.
@param[in] func: stop function type
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Stop_CM0( E_FJ_IPCU_STOP func );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set RTC Time data.<br>
See @ref fj_arm_overview - Communication command.
@param[in] calendar : Year, Month, Day of week, Day, Hour, Minute, Second
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Set_RTC_Time_CM0( T_FJ_ARM_IPCU_CALENDAR calendar );

// --- REMOVE_SECURE BEGIN ---
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Check Secure Code in CM0.<br>
See @ref fj_arm_overview - Communication command.
@param[in] digest_addr: start address for digest 256bit code.
@param[in] address: start address of target data array.
@param[in] count: number of target data array.
@param[out] result: check result.
					 0 : OK<br>
					-1 : Hash Calculate NG<br>
					-2 : Digest data unmatch<br>
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Check_Secure_Code_CM0( ULONG digest_addr, ULONG address, ULONG count, ULONG *result );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Read E-Fuse code in CM0.<br>
See @ref fj_arm_overview - Communication command.
@param[out] address: start address of read data storage destination.
@param[in] offset: E-Fuse code offset.
@param[in] size: E-Fuse code read size. Reads the code of the size from the position of the offset.
@param[out] result: E-Fuse read result.<br>
					 0 : OK<br>
					-1 : No written<br>
					-2 : NG<br>
@retval FJ_ERR_OK		: Success
@retval FJ_ERR_NG_CONDITION		: Error. Busy. This IPCU channel is used by other communication.
@retval FJ_ERR_NG_COMMUNICATON	: Error. Not ready for the request callback assigning on the instance ID.
@retval FJ_ERR_NG_INPUT_PARAM	: Error. Argument failure.
*/
FJ_ERR_CODE FJ_IPCU_Read_E_Fuse_CM0( ULONG address, ULONG offset, ULONG size, ULONG *result );
// --- REMOVE_SECURE END ---

// --- REMOVE_CAMERAIF BEGIN ---
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set status of finish boot of Linux FW (Inter-Processor Control Unit)
@param[in] boot_status: 1:Booted, 0:waiting for boot
*/
///FJ_ERR_CODE FJ_IPCU_Close(UCHAR id);
VOID FJ_IPCU_SET_FINISH_BOOT(ULONG boot_status);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get status of finish boot of Linux FW (Inter-Processor Control Unit)
@retval boot_status: 1:Booted, 0:waiting for boot
*/
ULONG FJ_IPCU_GET_FINISH_BOOT(VOID);
// --- REMOVE_CAMERAIF END ---

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
ARM Spin Lock and Interrut disable for exclusive control between cores.
@param[in] spin_id	: Lock Variable.
@retval Status Register
@remarks spin_id of argument, Please define the ".LOCK_SECTION" and 64-byte alignment.<br>
         Example : static UINT32 spin_id __attribute__((section(".LOCK_SECTION"), aligned(64)));
*/
UINT32 FJ_Critical_Section_Start(UINT32* spin_id);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
ARM Spin Lock and Interrut disable for exclusive control between cores.<br>
Spin Lock is polling.
@param[in] spin_id	: Lock Variable.
@retval Status Register
@remarks spin_id of argument, Please define the ".LOCK_SECTION" and 64-byte alignment.<br>
         Example : static UINT32 spin_id __attribute__((section(".LOCK_SECTION"), aligned(64)));
*/
UINT32 FJ_Critical_Section_Pol_Start(UINT32* spin_id);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
ARM Spin Unlock and Interrut enable for exclusive control between cores.
@param[in] spin_id	: Lock Variable.
@param[in] sr		: Status Register.
@remarks spin_id of argument, Please define the ".LOCK_SECTION" and 64-byte alignment.<br>
         Example : static UINT32 spin_id __attribute__((section(".LOCK_SECTION"), aligned(64)));
*/
VOID FJ_Critical_Section_End(UINT32* spin_id, UINT32 sr);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Get Timestamp Counter.
@retval Timestamp Counter value
@remarks This function is for debugging(e.g. MBA and measure-time)<br>
		If need of a time,  please divid this counter value by timer clock fequency.
*/
UINT64 FJ_TIMESTAMP_Get_Counter(VOID);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Get the state of Linux.
@retval state of Linux<br>
	0x56784321 = In suspend mode.
*/
UINT32 FJ_Linux_Get_State(VOID);

/* @} */	// fj_arm_api

#ifdef __cplusplus
}
#endif

#endif	// _FJ_ARM_H
