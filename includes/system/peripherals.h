/*
 * Copyright (C) 2015 Officine Robotiche
 * Author: Raffaello Bonghi
 * email:  raffaello.bonghi@officinerobotiche.it
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU Lesser General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

#ifndef PERIPHERALS_H
#define	PERIPHERALS_H

#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * I/O and Peripheral Initialization
     */
    void Peripherals_Init(void);
    /**
     * Initialization led blink
     */
    void InitLEDs(void);
    /**
     * Update frequency or type of blink
     * @param led to control
     * @param blink number of blinks
     */
    inline void UpdateBlink(short num, short blink);


#ifdef	__cplusplus
}
#endif

#endif	/* PERIPHERALS_H */

