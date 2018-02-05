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

#include <IIR_filters.h>
#include "load_test_reference_file.h"

#define STR_VALUE2(arg) #arg
#define STR_VALUE(name) STR_VALUE2(name)
#define IIR_SIGNAL_TYPE_STR STR_VALUE(IIR_SIGNAL_TYPE)


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

        IIR_signal_t *my_outputs = (IIR_signal_t*) malloc( sizeof(IIR_signal_t) * n_inputs );
        
        // Read file ok: create the filters and perform filtering
        IIR_S_t *filter = IIR_S_create( n_coefs, b_coefs, a_coefs );
        
        for ( int i=0; i < n_inputs; i++ ){
            my_outputs[i] = IIR_S_add_input(filter, inputs[i]);
        }

        dump_matlab_vector("a_coefs", n_coefs, a_coefs);
        dump_matlab_vector("b_coefs", n_coefs, b_coefs);
        dump_matlab_vector("inputs", n_inputs, inputs);
        dump_matlab_vector("my_outputs", n_inputs, my_outputs);
        printf( "ml_out = filter(b_coefs, a_coefs, inputs); "
                "hold off; plot(inputs); hold on; plot(ml_out,'r'); plot(my_outputs,'g')\n" );
        
        // Dump to binary file
        char *filt_str = "_S_applyed";
        char out_fname[ strlen(argv[1]) + strlen(filt_str) ];
        strcpy( out_fname, argv[1] );
        argv[1][ strlen(out_fname)-strlen(".???") ] = '\0';
        sprintf( out_fname, "%s%s", argv[1], filt_str );

        dump_to_binary_file( out_fname, loaded_data, my_outputs );
        
        IIR_S_destroy(filter);
        
        printf( "\nSUCCESS: IIR_S: Binary reference file '%s' generated\n", out_fname );    
        printf( "Using IIR_SIGNAL_TYPE %s\n", STR_VALUE(IIR_SIGNAL_TYPE) );
               
    }else{
        printf( "Error loading the test data file.\n" );
        return EXIT_FAILURE;
    }
        
    printf( "\nSUCCESS: IIR_S: Filter applied and output binary file created\n" );
    return EXIT_SUCCESS;
}

