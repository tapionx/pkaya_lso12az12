/*****************************************************************************
 * Copyright 2004, 2005 Michael Goldweber, Davide Brini.                     *
 * Modified 2012 by Marco Di Felice                                          *
 *                                                                           *
 * This file is part of pkaya2012.                                           *
 *                                                                           *
 * pkaya is free software; you can redistribute it and/or modify it under the*
 * terms of the GNU General Public License as published by the Free Software *
 * Foundation; either version 2 of the License, or (at your option) any      *
 * later version.                                                            *
 * This program is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General *
 * Public License for more details.                                          *
 * You should have received a copy of the GNU General Public License along   *
 * with this program; if not, write to the Free Software Foundation, Inc.,   *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.                  *
 *****************************************************************************/

/*********************************myp2test.C*******************************
 *
 *	Test program for the modules ASL and PCB (phase 2).
 *
 *	Produces progress messages on terminal 0 in addition 
 *		to the array ``okbuf[]''
 *		Error messages will also appear on terminal 0 in 
 *		addition to the array ``errbuf[]''.
 *
 *		Aborts as soon as an error is detected.
 *
 *    
 */

#include "const.h"
#include "uMPStypes.h"
#include "listx.h"
#include "types11.h"

#include "pcb.e"
#include "asl.e"

#include "utils.h"

#define	MAXSEM	MAXPROC

#define MAX_PCB_PRIORITY		10
#define MIN_PCB_PRIORITY		0
#define DEFAULT_PCB_PRIORITY		5


#define TRANSMITTED	5
#define TRANSTATUS    2
#define ACK	1
#define PRINTCHR	2
#define CHAROFFSET	8
#define STATUSMASK	0xFF
#define	TERM0ADDR	0x10000250
#define DEVREGSIZE 16       
#define READY     1
#define DEVREGLEN   4
#define TRANCOMMAND   3
#define BUSY      3


/******************* MAIN TEST **********************/

int main(void){
	/* Enable all the CPUs */
	addokbuf("Enabling all the CPUs!\n");
	
	
}
