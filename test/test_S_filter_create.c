/*
 * Copyright (C) 2018 Jose Marco de la Rosa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * Author: Jose Marco
 *
 * Created on January 20, 2018, 1:58 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include "IIR_filters.h"

#define N_SIGNALS 1

int main(int argc, char** argv) {
    
    IIR_signal_t a_coefs[20];
    IIR_signal_t b_coefs[20];
    
    int error = EXIT_SUCCESS;

    if ( IIR_S_create( 0, b_coefs, a_coefs ) ){
        error = EXIT_FAILURE;
        printf( "ERROR: IIR_S: Created filter with 0 coefficients!\n" );
    }else{
        printf( "SUCCESS: IIR_S: Filter with 0 coefficients is invalid\n" );
    }
    
    if ( IIR_S_create( 1, b_coefs, a_coefs ) ){
        error = EXIT_FAILURE;
        printf( "ERROR: IIR_S: Created filter with 1 coefficients!\n" );
    }else{
        printf( "SUCCESS: IIR_S: Filter with 1 coefficients is invalid\n" );
    }
    
    if ( IIR_S_create( -10, b_coefs, a_coefs ) ){
        error = EXIT_FAILURE;
        printf( "ERROR: IIR_S: Created filter with negative number of coefficients!\n" );
    }else{
        printf( "SUCCESS: IIR_S: Filter with negative number of coefficients is invalid\n" );
    }
    
    if ( error == EXIT_SUCCESS ){
        printf( "SUCCESS: IIR_S: all tests passed\n" );
    }else{
        printf( "ERROR: IIR_S: some test was not passed\n" );
    }
    
    return error;
}