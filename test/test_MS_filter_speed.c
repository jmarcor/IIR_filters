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
 * Created on November 2, 2017, 1:58 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "IIR_filters.h"

#define STR_VALUE2(arg) #arg
#define STR_VALUE(name) STR_VALUE2(name)
#define IIR_SIGNAL_TYPE_STR STR_VALUE(IIR_SIGNAL_TYPE)

#define DEFAULT_CYCLES 2000000
#define DEFAULT_SIGNALS 300

int main(int argc, char** argv) {
    
    int n_coefs = 9;
    int n_signals = DEFAULT_SIGNALS;
	// Create a filter
	//IIR_signal_t a[] = {1,2,3,4,5,6,7,8,9};
    IIR_signal_t a[] = {1.0000, 4.7845, 10.4450, 13.4577, 11.1293, 6.0253, 2.0793, 0.4172, 0.0372};
	//IIR_signal_t b[] = {3,4,5,6,7,8,9,1,2};
    IIR_signal_t b[] = {0.1929, 1.5430, 5.4005, 10.8009, 13.5011, 10.8009, 5.4005, 1.5430, 0.1929};
	long int n_cycles = DEFAULT_CYCLES;
    
    if ( argc > 2 ){
        long int tmp = atol( argv[1] );
        if ( tmp ){
            n_cycles = tmp;
        }
        tmp = atol( argv[2] );
        if ( tmp ){
            n_signals = tmp;
        }
    }

	IIR_MS_t *filter = IIR_MS_create( n_coefs, n_signals, b, a );
    
    printf( "Using IIR_SIGNAL_TYPE %s\n", STR_VALUE(IIR_SIGNAL_TYPE) );
    
    IIR_signal_t *input = (IIR_signal_t*) malloc( n_signals * sizeof(IIR_signal_t) );
    for ( int j=0; j<n_signals; j++ ){
        input[j] = 1.5;
    }
    
    for ( int i=0; i<10; i++ ){
        IIR_signal_t *tmp = IIR_MS_add_input(filter, input);
        for ( int j=0; j<n_signals; j++ ){
            printf( "%.4" IIR_SIGNAL_FORMAT ", %.4" IIR_SIGNAL_FORMAT "\n" , tmp[j], IIR_MS_get_last_output(filter)[j] );
        }
        printf( "\n" );
    }    
        
    clock_t c1 = clock();
	    
    for ( int i=0; i<n_cycles; i++ ){
        IIR_MS_add_input(filter, input);
	}
    
    clock_t c2 = clock();

    for ( int i=0; i<10; i++ ){
        IIR_MS_add_input(filter, input);
        for ( int j=0; j<n_signals; j++ ){
            printf( "%.4" IIR_SIGNAL_FORMAT ", " , IIR_MS_get_last_output(filter)[j] );
        }
        printf( "\n" );
    }
    
    IIR_MS_destroy( filter );
    
    printf( "Filter correctly destroyed\n" );
    
    printf( "\nUSE:\n\t> %s <n_cycles> <n_signals>\n"
            "\tn_cycles defaults to %ld\n\tn_signals defaults to %ld\n",
            argv[0], DEFAULT_CYCLES, DEFAULT_SIGNALS );
        
    printf( "\nTest params:\n\tSignal type: %s\n\tn_coefs: %d\n\tn_coefs: %d\n\tn_cycles:%d\n",
            STR_VALUE(IIR_SIGNAL_TYPE), n_coefs, n_signals, n_cycles );
    double time = (double)(c2-c1)/CLOCKS_PER_SEC;
    printf( "Test results:\n\tTotal time: %.4lf sec\n", time );
    printf( "\tTime to add one input (%d signals): %.4lf usec\n", n_signals, time/n_cycles*1e6 );
    printf( "\tTime to add one input for one signal: %.4lf usec\n", n_signals, time/(n_cycles*n_signals)*1e6 );
        
    return EXIT_SUCCESS;
}
