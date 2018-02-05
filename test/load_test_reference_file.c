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
 * Created on January 17, 2018, 10:41 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "load_test_reference_file.h"

#define STR_VALUE2(arg) #arg
#define STR_VALUE(name) STR_VALUE2(name)
#define IIR_SIGNAL_TYPE_STR STR_VALUE(IIR_SIGNAL_TYPE)


int read_comma_line( FILE *file, int read_size, IIR_signal_t *read_dest ){
    
    int error = 0;
    
    int nread;
    
    printf( "Using IIR_SIGNAL_TYPE %s\n", STR_VALUE(IIR_SIGNAL_TYPE) );
    
    nread = fscanf( file, " %" IIR_SIGNAL_FORMAT, &read_dest[0] );
    if ( !nread ){
        printf("ERROR: File format error reading value (first in line)\n");
        error = 1;
    }
    printf( "%.10" IIR_SIGNAL_FORMAT, read_dest[0] );
    
    for ( int i=1; (i < read_size) && !error; i++ ){
        nread = fscanf( file, " , %" IIR_SIGNAL_FORMAT, &read_dest[i] );
        if ( !nread ){
            printf("ERROR: File format error reading values\n");
            error = 1;                
            break;
        }
        printf( ", %.10" IIR_SIGNAL_FORMAT, read_dest[i] );
    }
    printf( "\n" );

    return error;
}

void dump_matlab_vector( char * name, int n_values, IIR_signal_t *values ){
    
    printf( "%s=single([...\n", name );
    for ( int i=0; i < n_values-1; i++ ){        
        printf( "%." TEST_TOLERANCE_DIGITS_STR IIR_SIGNAL_FORMAT ", ", values[i] );
        if ( (i%10) == 9 )
            printf("...\n");
    }

    // Last value
    printf( "%." TEST_TOLERANCE_DIGITS_STR IIR_SIGNAL_FORMAT "...\n])\n", values[n_values-1] );
}

char *get_usage_str( char *exec_filename ){
    
    char *result;
    char *base_str = "Usage: %s <filename>\n"
                "Input file format is:\n"
                "<n_coefs n (%%d)>, <n_inputs_outputs m (%%d)>\\n\n"
                "<a_coef_0 (%%" IIR_SIGNAL_FORMAT ")>, ..., <a_coef_n-1 (%%" IIR_SIGNAL_FORMAT ")>\\n\n"
                "<b_coef_0 (%%" IIR_SIGNAL_FORMAT ")>, ..., <b_coef_n-1 (%%" IIR_SIGNAL_FORMAT ")>\\n\n"
                "<input_0 (%%" IIR_SIGNAL_FORMAT ")>, ..., <input_0_m-1 (%%" IIR_SIGNAL_FORMAT ")>\\n\n"
                "<output_0 (%%" IIR_SIGNAL_FORMAT ")>, ..., <output_0_m-1 (%%" IIR_SIGNAL_FORMAT ")>\\n\n";
    
    result = (char*)malloc( strlen(base_str)+strlen(exec_filename)+1 ); // Should be bigger (1 byte) than needed
    
    sprintf( result ,"Usage: %s <filename>\n"
                "Input file format is:\n"
                "<n_coefs n (%%d)>, <n_inputs_outputs m (%%d)>\\n\n"
                "<a_coef_0 (%%" IIR_SIGNAL_FORMAT ")>, ..., <a_coef_n-1 (%%" IIR_SIGNAL_FORMAT ")>\\n\n"
                "<b_coef_0 (%%" IIR_SIGNAL_FORMAT ")>, ..., <b_coef_n-1 (%%" IIR_SIGNAL_FORMAT ")>\\n\n"
                "<input_0 (%%" IIR_SIGNAL_FORMAT ")>, ..., <input_0_m-1 (%%" IIR_SIGNAL_FORMAT ")>\\n\n"
                "<output_0 (%%" IIR_SIGNAL_FORMAT ")>, ..., <output_0_m-1 (%%" IIR_SIGNAL_FORMAT ")>\\n\n",
                exec_filename
    );
    
    return result;
}

test_data_t* load_filter_test_data_fields( char* filename ){
    
    int nread;
    int n_coefs;
    int n_inputs;
    IIR_signal_t *a_coefs;
    IIR_signal_t *b_coefs;
    IIR_signal_t *inputs;
    //IIR_signal_t *correct_outputs;
    
    int error = 0;  
       
    // Load the file
    FILE * file;    
    file = fopen( filename, "r" );
        
    if (file){
        nread = fscanf( file, " %d , %d", &n_coefs, &n_inputs );
        // Check that the file has a coherent number of subaps
        if ( nread == 2 ){
            printf( "n_coefs %d, n_inputs %d\n", n_coefs, n_inputs );
            
            // Alloc mem for the coefs
            a_coefs = (IIR_signal_t*) malloc( sizeof(IIR_signal_t) * n_coefs );
            b_coefs = (IIR_signal_t*) malloc( sizeof(IIR_signal_t) * n_coefs );

            // Alloc mem for the inputs and outputs
            inputs = (IIR_signal_t*) malloc( sizeof(IIR_signal_t) * n_inputs );
//            correct_outputs = (IIR_signal_t*) malloc( sizeof(IIR_signal_t) * n_inputs );
            
//            char* read_names[] = {"a_coefs","b_coefs","inputs","outputs"};
//            int read_sizes[] = {n_coefs, n_coefs, n_inputs, n_inputs};
//            IIR_signal_t* read_dests[] = {a_coefs,b_coefs,inputs,correct_outputs};
//            int n_reads = 4;
            
            char* read_names[] = {"a_coefs","b_coefs","inputs"};
            int read_sizes[] = {n_coefs, n_coefs, n_inputs};
            IIR_signal_t* read_dests[] = {a_coefs,b_coefs,inputs};
            int n_reads = 3;

            for ( int i=0; (i < n_reads) && !error; i++ ){
                error = 0;
                printf( "Reading %s\n", read_names[i] );
                error = read_comma_line( file, read_sizes[i], read_dests[i] );
                if (error){
                    printf( "ERROR READING: %s\n", read_names[i] );
                    error = 1;
                    break;
                }else{
                    printf( "\n" );
                }
            }
        }else{
            printf("ERROR: File format error (%s)\n", filename);
            error = 1;
        }
        fclose(file);
    }else{
        printf("ERROR: Unable to open file (%s)\n", filename);
        error = 1;
    }
    
    if (!error){
        test_data_t *loaded_data = (test_data_t *)malloc( sizeof(test_data_t) );
        loaded_data->n_coefs = n_coefs;
        loaded_data->n_inputs = n_inputs;
        loaded_data->a_coefs = a_coefs;
        loaded_data->b_coefs = b_coefs;
        loaded_data->inputs = inputs;
        //loaded_data->correct_outputs = correct_outputs;
        loaded_data->correct_outputs = NULL;
        return loaded_data;
    }else{
        return NULL;
    }   
}

int dump_to_binary_file( char* filename_no_ext, test_data_t* data, IIR_signal_t* outputs ){

    int error_code = EXIT_SUCCESS;
    char filename[1000];
    
    sprintf( filename, "%s_%s.bin", filename_no_ext, IIR_SIGNAL_TYPE_STR);
    
    FILE *f_out = fopen( filename, "wb" );
    
    if ( f_out ){
        fwrite( &data->n_coefs, sizeof(data->n_coefs), 1, f_out );
        fwrite( &data->n_inputs, sizeof(data->n_inputs), 1, f_out );
        char type_char = TEST_IIR_SIGNAL_TYPE_CHAR;
        fwrite( &type_char, sizeof(type_char), 1, f_out );
        fwrite( data->a_coefs, sizeof(IIR_signal_t), data->n_coefs, f_out );
        fwrite( data->b_coefs, sizeof(IIR_signal_t), data->n_coefs, f_out );
        fwrite( data->inputs, sizeof(IIR_signal_t), data->n_inputs, f_out );
        fwrite( outputs, sizeof(IIR_signal_t), data->n_inputs, f_out );
        fclose( f_out );
    }else{
        error_code = EXIT_FAILURE;
    }
    
    return error_code;
}

test_data_t* load_filter_test_data_fields_binary( char* filename ){
    
    int nwrite;
    int n_coefs;
    int n_inputs;
    char dtype;
    IIR_signal_t *a_coefs;
    IIR_signal_t *b_coefs;
    IIR_signal_t *inputs;
    IIR_signal_t *correct_outputs;
    
    test_data_t *result = NULL;
    
    int error = 0;  
       
    // Load the file
    FILE * file;    
    file = fopen( filename, "rb" );
        
    if (file){
        nwrite = fread( &n_coefs, sizeof(n_coefs), 1, file );
        nwrite += fread( &n_inputs, sizeof(n_inputs), 1, file );
        nwrite += fread( &dtype, sizeof(dtype), 1, file );
        
        if ( nwrite != 3 ){
            fprintf( stderr, "ERROR: bad file format reading n_coefs, n_inputs, dtype\nFile: %s\n", filename);
            return NULL;
        }
        if ( dtype != TEST_IIR_SIGNAL_TYPE_CHAR ){
            fprintf( stderr, "ERROR: datatype mismatch: IIR_signal_t if type '%c' and file is type '%c'\n",
                    TEST_IIR_SIGNAL_TYPE_CHAR, dtype
                    );
            return NULL;
        }        
        
        printf( "n_coefs: %d, n_inputs: %d\ndtype: '%c'\n", n_coefs, n_inputs, dtype );
        
        int coefs_bytesize = sizeof(IIR_signal_t) * n_coefs;
        a_coefs = (IIR_signal_t*)malloc(coefs_bytesize);
        b_coefs = (IIR_signal_t*)malloc(coefs_bytesize);
        nwrite = fread( a_coefs, coefs_bytesize, 1, file );
        nwrite += fread( b_coefs, coefs_bytesize, 1, file );
        if ( nwrite != 2 ){
            fprintf( stderr, "ERROR: bad file format reading a and b coefs\nFile: %s\n", filename);
            return NULL;
        }       
        
        int inputs_bytesize = sizeof(IIR_signal_t) * n_inputs;
        inputs = (IIR_signal_t*)malloc(inputs_bytesize);
        correct_outputs = (IIR_signal_t*)malloc(inputs_bytesize);
        nwrite = fread( inputs, inputs_bytesize, 1, file );
        nwrite += fread( correct_outputs, inputs_bytesize, 1, file );
        if ( nwrite != 2 ){
            fprintf( stderr, "ERROR: bad file format reading inputs and outputs\nFile: %s\n", filename);
            return NULL;
        }       

        fclose(file);
        
    }else{
        fprintf(stderr, "ERROR: Unable to open file (%s)\n", filename);
        return NULL;
    }
    
    test_data_t *loaded_data = (test_data_t *)malloc( sizeof(test_data_t) );
    loaded_data->n_coefs = n_coefs;
    loaded_data->n_inputs = n_inputs;
    loaded_data->a_coefs = a_coefs;
    loaded_data->b_coefs = b_coefs;
    loaded_data->inputs = inputs;
    loaded_data->correct_outputs = correct_outputs;
    return loaded_data;
}
