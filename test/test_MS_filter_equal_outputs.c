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
#include <math.h>

#include "IIR_filters.h"
#include "load_test_reference_file.h"


#define STR_VALUE2(arg) #arg
#define STR_VALUE(name) STR_VALUE2(name)
#define IIR_SIGNAL_TYPE_STR STR_VALUE(IIR_SIGNAL_TYPE)

#define N_SIGNALS 10

int main(int argc, char** argv) {
    if (argc != 2){
        printf( get_usage_str( argv[0] ) );
        return EXIT_FAILURE;
    }
    
    test_data_t *loaded_data = load_filter_test_data_fields( argv[1] );
        
    if ( loaded_data ){
        
        int n_coefs = loaded_data->n_coefs;
        int n_inputs = loaded_data->n_inputs;
        IIR_signal_t *a_coefs = loaded_data->a_coefs;
        IIR_signal_t *b_coefs = loaded_data->b_coefs;
        IIR_signal_t *inputs = loaded_data->inputs;

        // Read file ok: create the filters and perform filtering
        IIR_MS_t *filter = IIR_MS_create( n_coefs, N_SIGNALS, b_coefs, a_coefs );

        IIR_signal_t multi_signal_input[N_SIGNALS];
        IIR_signal_t *this_output;
        for ( int i=0; i < n_inputs; i++ ){
            // Inputs are single values: turn into array
            for ( int j=0; j < N_SIGNALS; j++ ){
                multi_signal_input[j] = inputs[i];
            }            
            // All outputs should be equal (all inputs are)!
            this_output = IIR_MS_add_input(filter, multi_signal_input);
            for ( int j=1; j < N_SIGNALS; j++ ){
                if (this_output[j] != this_output[j-1]){
                    printf("ERROR: Different outputs for different signals!!");
                    return EXIT_FAILURE;
                }
            }            
        }
        
        IIR_MS_destroy(filter);
        
    }else{
        printf( "Error loading the test data file.\n" );
        return EXIT_FAILURE;
    }

    printf( "Using IIR_SIGNAL_TYPE %s\n", STR_VALUE(IIR_SIGNAL_TYPE) );
    printf( "SUCCESS: IIR_MS: Test passed: using IIR_MS filter with several signals.\n"
        "If all input signals are equal, all outputs (one for each signal) are equal too.\n");
    
    return EXIT_SUCCESS;
}

