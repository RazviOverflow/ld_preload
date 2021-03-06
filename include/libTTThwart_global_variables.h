/*
	About: License

		Copyright 2019 Razvan Raducu and Ricardo J. Rodriguez

		This file is part of libTTThwart.so.

	    libTTThwart.so is free software: you can redistribute it and/or modify
	    it under the terms of the GNU General Public License as published by
	    the Free Software Foundation, either version 3 of the License, or
	    (at your option) any later version.

	    libTTThwart.so is distributed in the hope that it will be useful,
	    but WITHOUT ANY WARRANTY; without even the implied warranty of
	    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	    GNU General Public License for more details.

	    You should have received a copy of the GNU General Public License
	    along with libTTThwart.so.  If not, see <https://www.gnu.org/licenses/>.

	Authors:

		<Razvan Raducu: https://twitter.com/Razvieu>

	Academic coordinator:: 

		<Ricardo J. Rodríguez: https://webdiis.unizar.es/~ricardo/>

	Code: 
		You can find the source code associated with this header <here: https://github.com/RazviOverflow/ld.so.preload/blob/master/include/libTTThwart_global_variables.h>.

		
*/
#ifndef FILE_GLOBAL_VARIABLES_H_
#define FILE_GLOBAL_VARIABLES_H_

#include <stdbool.h>

/*	Constant: LIBARRY_FOLDER

	Global folder used to create all log files as well as temporal hard links.
*/
#define LIBRARY_FOLDER "/libTTThwart"

/*	
	Variable: file_objects_info

	Global <file_objects_info> array that's used to maintain all the information
	about file metadata.
*/
file_objects_info g_array;

/*	
	Bool: LIBRARY_ON

	Global boolean that indicates whether the libary must be used. Bear in mind
	that one of the premises of TOCTTOU vulnerabilities is the attacker has
	lesser privileges than the vulnerable program. This is, the program is
	a setuid-bit-on program. (EUID = UID); where UID = RUID
*/
bool LIBRARY_ON; 

/*
	Char: g_temp_dir

	Globlar variable that indcates the directory of temporal hard links, in case
	there are any. 
*/
char *g_temp_dir; 

#endif