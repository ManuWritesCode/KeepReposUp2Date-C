/*************************************************************************************************************
 * 
 * KEEP REPOS UP TO DATE (KeepReposUp2Date)
 * 
 * This file is part of the KeepReposUp2Date program (https://github.com/ManuWritesCode/KeepReposUp2Date-C).
 * Copyright (c) 2025 Emmanuel BOIDIN (@ManuWritesCode).
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *************************************************************************************************************/


#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main ( void )
{
	char *buffer = NULL;
	size_t size = 8;

	buffer = malloc( size );

    // Get the hostname of the local computer
	while( gethostname( buffer, size ) != 0 ) {
		if ( errno != ENAMETOOLONG ) {
			perror( "gethostname : Name too long" );
			return EXIT_FAILURE;
		}
		size += 8;
		buffer = realloc( buffer, size );
	}

	fprintf( stdout, "Hostname : %s\n", buffer );

    // If hostname is Sakura, defines the development path to xxxxxxx
    if ( strcmp( buffer, "Sakura" ) == 0 ) {
        fprintf ( stdout, "This is my Macbook Pro\n" );
    } else if ( strcmp( buffer, "acropolix-lab" ) == 0 ) {
        fprintf( stdout, "This is Acropolix-Lab Debian machine\n" );
    } else if ( strcmp( buffer, "Thanathos" ) == 0 ) {
        fprintf( stdout, "This is Thanatos machine\n" );
    }







	free( buffer );

	return EXIT_SUCCESS;
}