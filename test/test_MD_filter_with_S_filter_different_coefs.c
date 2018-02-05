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
#include "load_test_reference_file.h"


#define STR_VALUE2(arg) #arg
#define STR_VALUE(name) STR_VALUE2(name)
#define IIR_SIGNAL_TYPE_STR STR_VALUE(IIR_SIGNAL_TYPE)

#define N_SIGNALS 5
#define SIGNAL_NOISE_RANGE 40
#define COEF_STEP_RANGE 0.01


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
        IIR_MD_t *MD_filter = IIR_MD_create( n_coefs, N_SIGNALS, b_coefs, a_coefs );

//        for ( int j=0; j < N_SIGNALS; j++ ){
//            printf( "Coefs set: %d\na\n", j );
//            for ( int k=0; k < n_coefs; k++ ){
//                printf( "%.4f, ", IIR_MD_COEFS_A_INDEX( MD_filter, k, j) );
//            }        
//            printf( "\nb\n" );
//            for ( int k=0; k < n_coefs; k++ ){
//                printf( "%.4f, ", IIR_MD_COEFS_B_INDEX( MD_filter, k, j) );
//            }        
//            printf( "\n" );
//        }        
        
        
        // Now, create N_SIGNALS different filters to compare
        // With small variations in some coefs (not in a[0])        
        IIR_S_t *S_filters[N_SIGNALS];
        for ( int j=0; j < N_SIGNALS; j++ ){
            for ( int i=1; i < n_coefs; i++ ){
                b_coefs[i] += (i%2) * COEF_STEP_RANGE * i/2;
                a_coefs[i] -= (i%2) * COEF_STEP_RANGE * i/2;
            }
            // Force small changes in a[0] to force normalization
            if ( !(j%2) ){
                a_coefs[0] += COEF_STEP_RANGE;
            }
            S_filters[j] = IIR_S_create( n_coefs, b_coefs, a_coefs );
            IIR_MD_set_coefs_one_signal( MD_filter, n_coefs, b_coefs, a_coefs, j);
        }            
        
        IIR_signal_t multi_signal_input[N_SIGNALS];
        IIR_signal_t IIR_S_outputs[N_SIGNALS];
        IIR_signal_t *this_output;                
        srand(time(NULL));
        for ( int i=0; i < n_inputs; i++ ){
            // Inputs are single values: turn into array
            for ( int j=0; j < N_SIGNALS; j++ ){
                float noise = ((((float)rand())/RAND_MAX) * SIGNAL_NOISE_RANGE)-(SIGNAL_NOISE_RANGE/2);
                //printf("Noise: %f\n", noise);
                multi_signal_input[j] = inputs[i] + noise;
                //multi_signal_input[j] = inputs[i];
                // Compute the simple filters outputs
                IIR_S_outputs[j] = IIR_S_add_input(S_filters[j], multi_signal_input[j]);
                //printf( "%f, ", multi_signal_input[j] );
            }
            // Check that outputs of the MS match the outputs from the IIR_Ss
            this_output = IIR_MD_add_input(MD_filter, multi_signal_input);
            
//            //this_output = IIR_MS_add_input(MS_filter, multi_signal_input);
//
//            printf("multi_signal_input\n" );
//            for ( int j=0; j < N_SIGNALS; j++ ){
//                printf("%f, ", multi_signal_input[j] );
//            }            
//            
//            printf("\nIIR_S_outputs\n" );
//            for ( int j=0; j < N_SIGNALS; j++ ){
//                printf("%f, ", IIR_S_outputs[j] );
//            }
//            printf("Thisoutput\n" );
//            for ( int j=0; j < N_SIGNALS; j++ ){
//                printf("%f, ", this_output[j] );
//            }
//            printf("\n" );
//            getchar();

            for ( int j=0; j < N_SIGNALS; j++ ){
                if (this_output[j] != IIR_S_outputs[j]){
                    printf("ERROR: Different outputs for the IIR_S and IIR_MD signals (%f, %f)!!", IIR_S_outputs[j], this_output[j] );
                    printf("ERROR: (i=%d, j=%d)!!", i, j );
                    return EXIT_FAILURE;
                }
            }            
        }

        // Dump the a_coefs and b_coefs to check they are different
        printf( "\nCOEFS:\n" );
        for ( int j=0; j < N_SIGNALS; j++ ){
            printf( "A_COEFS[%d]: ", j );
            for ( int i=0; i < n_coefs; i++ ){
                printf( ",% .4f", (float)IIR_MD_COEFS_A_INDEX(MD_filter, i, j) );
            }
            printf( "\nB_COEFS[%d]: ", j );
            for ( int i=0; i < n_coefs; i++ ){
                printf( ",% .4f", (float)IIR_MD_COEFS_B_INDEX(MD_filter, i, j) );
            }
            printf( "\n" );
        }
        printf( "\n" );

        IIR_MD_destroy(MD_filter);
        for ( int j=0; j < N_SIGNALS; j++ ){
            IIR_S_destroy(S_filters[j]);
        }                    
    
        //    printf( "Using IIR_SIGNAL_TYPE %s\n", STR_VALUE(IIR_SIGNAL_TYPE) );
    }else{
        printf( "Error loading the test data file.\n" );
        return EXIT_FAILURE;
    }
    
    printf( "Using IIR_SIGNAL_TYPE %s\n", STR_VALUE(IIR_SIGNAL_TYPE) );
    printf( "SUCCESS: IIR_MD: Test passed: outputs in the IIR_MD with several signals filter ALL WITH DIFFERENT COEFS do match those "
            "of several IIR_S filters, one for each signal\n" );
    return EXIT_SUCCESS;
}


