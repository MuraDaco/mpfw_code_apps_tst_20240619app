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
 * svStateMachineTbl.h
 *
 *  Created on: Sep, 19 2022
 *      Author: Marco Dau
 */

#ifndef MPFW_APPS_SV_STATE_MACHINE_TBL_H_
#define MPFW_APPS_SV_STATE_MACHINE_TBL_H_


#include <svStateMachine.h>

namespace mpfw { namespace apps {

class svStateMachineTbl {
public:

 
	static fw2::core::core::svStateMachine* UnitsArray[];

};

} }	// mpfw::apps

#endif /* MPFW_APPS_SV_STATE_MACHINE_TBL_H_ */
