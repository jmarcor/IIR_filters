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
 * Created on November 3, 2017, 12:22 PM
 */

// Implementation of one input signal filters

#ifndef IIR_S_FILTER_H
#define IIR_S_FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "IIR_filters.h"

// Type for the structure holding the one input signal IIR filter state
typedef struct {
    int n_coefs;
    IIR_signal_t *a;
    IIR_signal_t *b;
    IIR_signal_t *z;
    IIR_signal_t last_output;
} IIR_S_t;

// Create a single input signal IIR filter.
// Parameters:
//	n_coefs: number of coefficients (order+1)
//	n_signals: number of input signals
//	b_coefs, a_coefs: a and b coefficients arrays. They must hold
//	    n_coefs values each
// Returns:
//    A filter structure initialized to rest state or NULL upon error
//    (memory allocation problem)
inline IIR_S_t *IIR_S_create(int n_coefs,
			    const IIR_signal_t *b_coefs,
			    const IIR_signal_t *a_coefs) {

    int i;

    // Check the number of coefs. Minimum is order 1, which means two coefs
    if (n_coefs <= 1) {
	fprintf(stderr,
		"IIR ERROR: trying to create a filter with not enough coefficients (%d). Min is 2.\n",
		n_coefs
		);
	return NULL;
    }

    IIR_S_t *filter = (IIR_S_t*)malloc(sizeof (IIR_S_t));
    if ( !filter ){
	fprintf( stderr, "IIR ERROR: Unable allocate memory for filter.\n" );
	return NULL;
    }

    filter->n_coefs = n_coefs;
    int coefs_byte_size = sizeof (IIR_signal_t) * n_coefs;
    filter->a = (IIR_signal_t*)malloc( coefs_byte_size );
    filter->b = (IIR_signal_t*)malloc( coefs_byte_size );
    filter->z = (IIR_signal_t*)malloc( coefs_byte_size );
    if ( !filter->a || !filter->b || !filter->z ){
	free( filter );
	fprintf( stderr, "IIR ERROR: Unable allocate memory for filter a, b or z arrays.\n" );
	return NULL;
    }

    // Initialize the state vector
    memset( filter->z, 0, coefs_byte_size );
    
    memcpy( filter->a, a_coefs, coefs_byte_size );
    memcpy( filter->b, b_coefs, coefs_byte_size );
    
    IIR_normalize_coefs( n_coefs, filter->b, filter->a );
        
    // Not valid yet, but initialize!
    filter->last_output = 0;
    
    return filter;
}

// Returns the last output of the filter
#define IIR_S_get_last_output(filter) (filter->last_output)

// Add the next input (x) to the filter and return the corresponding output
// HUGE gain by declaring this as inline!
inline IIR_signal_t IIR_S_add_input(IIR_S_t *filter, IIR_signal_t x) {

    int j;
    
    IIR_signal_t *y = &filter->last_output;
    IIR_signal_t *z = filter->z;
    IIR_signal_t *a = filter->a;
    IIR_signal_t *b = filter->b;    
    int n_coefs = filter->n_coefs;

    *y = z[0] + b[0] * x;

    for (j = 1; j< n_coefs-1 ; j++){
	z[j-1] = z[j] + x * b[j] - (*y) * a[j];
    }
    z[j-1] = x * b[j] - (*y) * a[j];
    
    return *y;
    
}

// Reset the filter to the resting state
// Just return all previous states and last output to 0
inline void IIR_S_reset(IIR_S_t *filter) {
    memset( filter->z, 0, sizeof (IIR_signal_t) * filter->n_coefs );
    filter->last_output = 0;
}

// Free all the memory allocated by the filter
inline void IIR_S_destroy(IIR_S_t *filter) {

    free(filter->a);
    free(filter->b);
    free(filter->z);
    free(filter);
}

#ifdef __cplusplus
}
#endif

#endif /* IIR_S_FILTER_H */
