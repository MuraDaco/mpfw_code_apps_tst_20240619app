//  *******************************************************************************
//  
//  mpfw / fw2 - Multi Platform FirmWare FrameWork 
//      library that contains the application code that is platform independent
//  Copyright (C) (2023) Marco Dau
//  
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//  
//  You can contact me by the following email address
//  marco <d o t> ing <d o t> dau <a t> gmail <d o t> com
//  
//  *******************************************************************************
/*
 * ap_krThread02.cpp
 *
 *  Created on: Jan,  5 2023
 *      Author: Marco Dau
 */

#include "ap_krThread02.h"

// system
#include <cstring>

#include <krThread.h>
using namespace fw2::wrapper::core;
#include <uyProtocol2.h>
using namespace fw2::core::core;
#include <rsSerial.h>
using namespace fw2::wrapper::resources;

namespace mpfw { namespace apps {

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define THREAD_1_ID  0
#define THREAD_2_ID  1

volatile uint8_t ap_krThread02::thread_sync = THREAD_1_ID;
uint8_t ap_krThread02::rx_buffer[100];
uint8_t* ap_krThread02::rx_buffer_bkp;


bool ap_krThread02::rx_parsing	(uint8_t* p_buffer)		{
	if(uyProtocol2::AddsDstMatch((char *) p_buffer)) {
		uint8_t l_adds;
		uint8_t l_cmd = uyProtocol2::CommandGet((char *) p_buffer);
		switch (l_cmd)	{
			case UY_PROTOCOL2__CMD__SET_MASTER:
				uyProtocol2::Set_Master();
				rsSerial::Tx("01u|Performed command SET_MASTER|"); rsSerial::Tx(rsSerial::endl);
				break;
			case UY_PROTOCOL2__CMD__SET_SLAVE:
				rsSerial::Tx("01u|Performing command SET_SLAVE|"); rsSerial::Tx(rsSerial::endl);
				uyProtocol2::Set_Slave();
				break;
			case UY_PROTOCOL2__CMD__SET_ADDS:
				// 01u|913-2| --> set the address to '2'
				// 01u|923-4| --> set the address to '4'
				// 01u|943-3| --> set the address to '3'
				rsSerial::Tx("01u|Performing ADDRESS CHANGING|"); rsSerial::Tx(rsSerial::endl);
				l_adds = uyProtocol2::CommandGetData((char *) p_buffer);
				uyProtocol2::AddsSet (l_adds);
				break;
			default:
				rsSerial::Tx("01u|No command recognized|"); rsSerial::Tx(rsSerial::endl);
				break;
		}
		return true;
	}
	return false;
}

void ap_krThread02::TaskTest0	(krThreadId p_ThreadId) {
	rx_buffer_bkp = rx_buffer;
    if(krThread_Zero == p_ThreadId) {
    	// lock rsSerial resources
    	while(THREAD_1_ID != thread_sync){wp_sleep_for(100);}
		if(uyProtocol2::AmI_Master())	{
	    	rsSerial::Tx("Start thread 01 ..."); rsSerial::Tx(rsSerial::endl);
		}
    	// release rsSerial resources
    	thread_sync = THREAD_2_ID;

    	bool g_ask_for_an_input = true;

    	for (;;) {
    		wp_sleep_for(100);
    		// lock rsSerial resources
    		if(THREAD_1_ID == thread_sync) {
				if(uyProtocol2::AmI_Master())	{
					// device behaviour is MASTER

	    			if(g_ask_for_an_input)	{
	    				g_ask_for_an_input = false;
	    				//           01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
	    				//           0    -    1    -    2    -    3    -    4    -    5    -    6    -    7    -    8    -    9    -    0
	    				rsSerial::Tx("th1-Input the sequence \"01<xxx>\" or \"01u<terminating character>\" then the body of packet..."); rsSerial::Tx(rsSerial::endl);
	    			}

	    			if(rsSerial::RxCheck())	{
	    				g_ask_for_an_input = true;
	    				rsSerial::Tx("Thread 1 - The following characters have been received:"); rsSerial::Tx(rsSerial::endl);
	    				rsSerial::Tx("01uxQuesta è un altra provax"); rsSerial::Tx(rsSerial::endl);
						if(!rx_parsing(rx_buffer)) {
		    				rsSerial::Tx("01u|The received message has not been sent to me:-(|"); rsSerial::Tx(rsSerial::endl);
						}
	    				rsSerial::Tx(rx_buffer); rsSerial::Tx(rsSerial::endl);
	    				// release rsSerial resources
	    				thread_sync = THREAD_2_ID;
						// uyProtocol2::Set_Slave();
	    			} else rsSerial::RxStart(rx_buffer, sizeof(rx_buffer));
				} else {
					// device behaviour is SLAVE
	    			if(rsSerial::RxCheck())	{
	    				g_ask_for_an_input = true;
						//rsSerial::Tx("01u|No command recognized|"); rsSerial::Tx(rsSerial::endl);
						rx_parsing(rx_buffer);
						//if(rx_parsing(rx_buffer)) {
		    			//	rsSerial::Tx(rx_buffer); rsSerial::Tx(rsSerial::endl);
						//}

	    			} else rsSerial::RxStart((char *) rx_buffer, sizeof(rx_buffer));
				}
	    		if(rsSerial::CheckService()) {
					rsSerial::RxStop();
					rsSerial::Tx("01u|Thread 01 is exiting ... bye, byte ...|");
					thread_sync = THREAD_2_ID;
	    			return;
	    		}
    		}
    	}
    }
}

void ap_krThread02::TaskTest1	(krThreadId p_ThreadId) {
    if(krThread_One == p_ThreadId) {
    	// lock rsSerial resources
    	while(THREAD_2_ID != thread_sync){wp_sleep_for(100);}
		if(uyProtocol2::AmI_Master())	{
	    	rsSerial::Tx("Start thread 02 ..."); rsSerial::Tx(rsSerial::endl);
		}
    	// release rsSerial resources
    	thread_sync = THREAD_1_ID;

    	bool g_ask_for_an_input = true;

    	for (;;) {
    		wp_sleep_for(100);

    		// lock rsSerial resources
    		if(THREAD_2_ID == thread_sync) {
				if(uyProtocol2::AmI_Master())	{
					// device behaviour is MASTER
	    			if(g_ask_for_an_input)	{
	    				g_ask_for_an_input = false;
	    				//           01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
	    				//           0    -    1    -    2    -    3    -    4    -    5    -    6    -    7    -    8    -    9    -    0
	    				rsSerial::Tx("th2-Input the sequence \"01<xxx>\" or \"01u<terminating character>\" then the body of packet..."); rsSerial::Tx(rsSerial::endl);
	    			}

	    			if(rsSerial::RxCheck())	{
	    				g_ask_for_an_input = true;
	    				//           01234567890123456789012345678901234567890123456789012345678901234567890
	    				//           0    -    1    -    2    -    3    -    4    -    5    -    6    -    7
	    				rsSerial::Tx("Thread 2 - The following characters have been received:"); rsSerial::Tx(rsSerial::endl);
	    				rsSerial::Tx("01041Questa_e_un altra prova.nVediamo se funziona!n----....."); rsSerial::Tx(rsSerial::endl);
						if(!rx_parsing(rx_buffer)) {
		    				rsSerial::Tx("01u|The received message has not been sent to me:-(|"); rsSerial::Tx(rsSerial::endl);
						}
	    				rsSerial::Tx(rx_buffer); rsSerial::Tx(rsSerial::endl);
	
	    				// release rsSerial resources
						// uyProtocol2::Set_Slave();
	    				thread_sync = THREAD_1_ID;
	    			} else rsSerial::RxStart((char *) rx_buffer, sizeof(rx_buffer));
				} else {
					// device behaviour is SLAVE

	    			if(rsSerial::RxCheck())	{
	    				g_ask_for_an_input = true;
						//if(rx_parsing(rx_buffer)) {
		    			//	// rsSerial::Tx(rx_buffer); rsSerial::Tx(rsSerial::endl);
		    			//	rsSerial::Tx(rx_buffer); rsSerial::Tx(rsSerial::endl);
						//}
	    			} else rsSerial::RxStart((char *) rx_buffer, sizeof(rx_buffer));
				}
	    		if(rsSerial::CheckService()) {
					rsSerial::RxStop();
					rsSerial::Tx("01u|Thread 02 is exiting ... bye, byte ...|");
					thread_sync = THREAD_1_ID;
	    			return;
	    		}
    		}
    	}
    }
}

} }	// mpfw::apps
