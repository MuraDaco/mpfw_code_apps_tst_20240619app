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
 * tt_mg_SyncManager_3.cpp
 *
 *  Created on: Oct,  4 2022
 *      Author: Marco Dau
 */

// module
#include "tt_mg_SyncTbl_3.h"
#include "tt_mg_SyncManager_3.h"
#include "../ap/tt_ap_svSync_3.h"


namespace mpfw { namespace apps {


uint16_t tt_mg_SyncManager_3::g_cnt				[krThread_Num];
uint16_t tt_mg_SyncManager_3::g_cnt_no_match	[krThread_Num];
uint16_t tt_mg_SyncManager_3::g_SyncCnt			[krThread_Num];


void tt_mg_SyncManager_3::ThreadUnitLoop (krThreadId p_thread) {
	// init static arrays
	static bool l_init = false;
	if(!l_init) {
		l_init = true;
		for (auto i=0; i<krThread_Num; i++)	{
			g_cnt			[i] = 0;
			g_cnt_no_match	[i] = 0;
			g_SyncCnt		[i] = 0;			
		}
	}

	// SECTION - svState
	for (auto i=0; i<200; i++)
	{
		svState::state_t result;
		if (nullptr == tt_mg_SyncTbl_3::svStatePtrUnitArray[i]) break;

		result = tt_mg_SyncTbl_3::svStatePtrUnitArray[i]->StateRunStatus(p_thread);
		switch (result)	{
			case svState::k_SvState_THREAD_NOT_MATCH:
				g_cnt_no_match[p_thread]++;
			break;
			case svState::k_SvState_NO_ACTIVE:
				g_cnt_no_match[p_thread]++;
			break;
			default:
				g_SyncCnt[p_thread]++;
				tt_ap_svSync_3::g_SyncUpdateState.UpdateSyncResorces	(p_thread	,result	);
			break;
		}
	}
	g_cnt[p_thread]++;

	// SECTION - svSync
	tt_ap_svSync_3::g_SyncUpdateState.ResetSyncSignals	(p_thread	);

}

} }	// mpfw::apps
