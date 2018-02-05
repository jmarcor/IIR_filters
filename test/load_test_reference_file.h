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

#ifndef LOAD_TEST_REFERENCE_FILE_H
#define LOAD_TEST_REFERENCE_FILE_H

#include <IIR_filters.h>

#ifdef IIR_USE_SIGNAL_TYPE_DOUBLE
    #define TEST_TOLERANCE 1e-7
    #define TEST_TOLERANCE_DIGITS_STR "7"
    #define TEST_IIR_SIGNAL_TYPE_CHAR 'd'
#else
    #define TEST_TOLERANCE 1e-5
    #define TEST_TOLERANCE_DIGITS_STR "5"
    #define TEST_IIR_SIGNAL_TYPE_CHAR 'f'
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct test_data_t {
    int n_coefs;
    int n_inputs;
    IIR_signal_t *a_coefs;
    IIR_signal_t *b_coefs;
    IIR_signal_t *inputs;
    IIR_signal_t *correct_outputs;
} test_data_t;
    
   
int read_comma_line( FILE *file, int read_size, IIR_signal_t *read_dest );
void dump_matlab_vector( char * name, int n_values, IIR_signal_t *values );
char *get_usage_str( char *exec_filename );
test_data_t* load_filter_test_data_fields( char* filename );
test_data_t* load_filter_test_data_fields_binary( char* filename );
int dump_to_binary_file( char* filename_no_ext, test_data_t* data, IIR_signal_t* outputs );

#ifdef __cplusplus
}
#endif

#endif /* LOAD_TEST_INPUTS_FILE_H */

