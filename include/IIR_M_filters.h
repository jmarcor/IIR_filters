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

// Implementation of multiple input signal filters

#ifndef IIR_M_FILTERS_H
#define IIR_M_FILTERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "IIR_filters.h"

// Type for the structure holding the multiple input signal IIR filter state
// IIR_MS_t for Multiple input signals with Shared coefficients
// IIR_MD_t for Multiple input signals with Different coefficients
typedef struct {
    int n_signals;
    int n_coefs;
    IIR_signal_t *a;
    IIR_signal_t *b;
    IIR_signal_t *z;
    IIR_signal_t *last_output;
    int _element_byte_size;
    
} IIR_M_t, IIR_MS_t, IIR_MD_t;

/******************************************************
 * Functions common to both (Shared/Different coefs) Multi signal IIRs
 ******************************************************/

// A couple of forward declarations are needed
inline int IIR_MD_set_coefs_all_signals(IIR_MD_t* filter, int n_coefs, const IIR_signal_t *a_coefs, const IIR_signal_t *b_coefs);
inline int IIR_MS_set_coefs(IIR_MS_t* filter, int n_coefs, const IIR_signal_t *a_coefs, const IIR_signal_t *b_coefs);

/*
 * Create a new multi signal IIR filter with shared a and b coefficients
 * In this case, all the signals share the same a and b coefficients
 * Pars:
 *	n_coefs: number of a and b coefs. Should be <filter order>+1
 *	n_signals: number of signals to filter
 *	a_coefs: a coefficients (can be NULL for no initialization (leave as 0s)).
 *	b_coefs: b coefficients (can be NULL for no initialization (leave as 0s)).
 *	
 */

// Create a multiple input signal IIR filter.
// This function creates both MS and MD filters and is not supposed to be called
// directly by the user but to be used internally. It is very similar to the 
// MS and MD creation calls, though.
// Parameters:
//	n_coefs: number of coefficients (order+1)
//	n_signals: number of input signals
//	b_coefs, a_coefs: a and b coefficients arrays. They must hold
//	    n_coefs values each in the case of an MS filter or
//	    n_coefs * n_signals values each in the case of an MD filter
//	    For MD filters, coefs are stored contiguous for each signal. That is:
//		a_coefs: a0s0, a1s0,...,a<n_coefs-1>s0,
//			 a0s1, a1s1,...,a<n_coefs-1>s1,...,
//			 a0s<n_signals-1>, a1s<n_signals-1>,...,a<n_coefs-1>s<n_signals-1>
//		b_coefs: b0s0, b1s0,...,b<n_coefs-1>s0,
//			 b0s1, b1s1,...,b<n_coefs-1>s1,...,
//			 b0s<n_signals-1>, b1s<n_signals-1>,...,b<n_coefs-1>s<n_signals-1>
//	different_coefs: 0 for MS creation (shared coefs) or 1 for MD creation (different coefs)
// Returns:
//    A filter structure initialized to rest state or NULL upon error
//    (memory allocation problem)
inline IIR_M_t *_IIR_M_create(int n_coefs, int n_signals,
	const IIR_signal_t *b_coefs,
	const IIR_signal_t *a_coefs,
	int different_coefs) {

    // Check the number of coefs. Minimum is order 1, which means two coefs
    if (n_coefs <= 1) {
	fprintf(stderr,
		"IIR ERROR: trying to create a filter with not enough coefficients (%d). Min is 2.\n",
		n_coefs
		);
	return NULL;
    }
    
    // Check the number of signals
    if (n_signals <= 0) {
	fprintf(stderr,
		"IIR ERROR: trying to create a filter without or negative number of signals: %d.\n",
		n_signals
		);
	return NULL;
    }    

    IIR_M_t *filter = (IIR_M_t*) malloc(sizeof (IIR_M_t));
    if ( !filter ){
	fprintf( stderr, "IIR ERROR: Unable allocate memory for filter.\n" );
	return NULL;
    }    
    
    filter->n_coefs = n_coefs;
    filter->n_signals = n_signals;
   
    int coefs_size = sizeof (IIR_signal_t) * n_coefs;
    
    // Zs are always one n_coefs size vector for each signal
    filter->z = (IIR_signal_t*) malloc(coefs_size * n_signals);    
    if ( !filter->z ){
	free( filter );
	fprintf( stderr, "IIR ERROR: Unable allocate memory for filter z array.\n" );
	return NULL;
    }
    memset(filter->z, 0, coefs_size * n_signals);
    
    if (different_coefs) {
	coefs_size *= n_signals;
    }

    filter->a = (IIR_signal_t*) malloc(coefs_size);    
    filter->b = (IIR_signal_t*) malloc(coefs_size);

    if ( !filter->a || !filter->b ){
	free( filter->z );
	free( filter );
	fprintf( stderr, "IIR ERROR: Unable allocate memory for filter a or b.\n" );
	return NULL;
    }
    
    // Initialize the arrays
    memset(filter->a, 0, coefs_size);
    memset(filter->b, 0, coefs_size);    
    
    filter->_element_byte_size = sizeof (IIR_signal_t) * n_signals;
    filter->last_output = (IIR_signal_t*) malloc(filter->_element_byte_size);
    // Initialize the output (although not yet valid!)
    memset(filter->last_output, 0, filter->_element_byte_size);

    // Fill the coefficients if provided    
    if ( (a_coefs) && (b_coefs) ){
	if (different_coefs) {
	    IIR_MD_set_coefs_all_signals(filter, filter->n_coefs, b_coefs, a_coefs);
	} else {
	    IIR_MS_set_coefs(filter, filter->n_coefs, b_coefs, a_coefs);
	}
    }

    return filter;
}

// Return a copy of the filter output (or NULL if memory allocation problem)
// This function is the same for MS and MD, but is defined as a common one
// intended for internal use and aliased with a macro for each filter type
inline IIR_signal_t *_IIR_M_get_last_output_copy( IIR_M_t *filter ){
    
    IIR_signal_t *result;
    int nbytes = sizeof(IIR_signal_t)*filter->n_signals;    
    
    result = (IIR_signal_t *)malloc( nbytes );
    if ( !result ){
	fprintf( stderr, "IIR ERROR: Unable allocate memory for filter output copy.\n" );
	return NULL;
    }        
    
    memcpy( result, filter->last_output, nbytes );
    
    return result;
}

// Return a copy of the filter output (or NULL if memory allocation problem)
#define IIR_MS_get_last_output_copy(filter) _IIR_M_get_last_output_copy(filter)
#define IIR_MD_get_last_output_copy(filter) _IIR_M_get_last_output_copy(filter)

// Return the pointer to the filter output
#define IIR_MS_get_last_output(filter) (filter->last_output)
#define IIR_MD_get_last_output(filter) (filter->last_output)

// Reset the filter to the resting state
// Just return all previous states and last output to 0
// This one is a common function for both M filters used internally.
// It is aliased with two names, one for each MS and MD filters
inline void _IIR_M_reset(IIR_M_t *f) {
    memset( f->z, 0, sizeof(IIR_signal_t) * f->n_coefs * f->n_signals );
    memset( f->last_output, 0, sizeof(IIR_signal_t) * f->n_signals );
}

// Free all the memory allocated by the filter. Internal use, aliased later
// for both MS and MD
inline void _IIR_M_destroy(IIR_M_t *filter) {

    free(filter->a);
    free(filter->b);
    free(filter->last_output);
    free(filter->z);
    free(filter);
}

/******************************************************
 * Functions specific to Shared coefs Multi signal IIRs
 ******************************************************/

// Macros for indexing the a and b coefs
// They can be used both for reading and writing
#define IIR_MS_COEFS_A_INDEX(filter, i) filter->a[i]
#define IIR_MS_COEFS_B_INDEX(filter, i) filter->b[i]

// Just a call to the general creation for multi signal IIR
// See the _IIR_MS_create function doc
inline IIR_MS_t *IIR_MS_create(int n_coefs, int n_signals,
	const IIR_signal_t *b_coefs,
	const IIR_signal_t *a_coefs) {

    return _IIR_M_create(n_coefs, n_signals, b_coefs, a_coefs, 0);
}

// Fills in the a and/or b coefficients of the filter
// returns 0 on fail (given number of coefs does not agree with filters coefs
// or any of coefs == NULL)
// Both a_coefs and b_coefs params should be arrays of size n_coefs
// The coefficients in the filter are normalized after being set
// (the contents of input parameter arrays are not normalized)
inline int IIR_MS_set_coefs(IIR_MS_t* filter,
			    int n_coefs,
			    const IIR_signal_t *b_coefs,
			    const IIR_signal_t *a_coefs) {

    int i;

    // Check that you are passing the appropriate number of coefs
    if ( (n_coefs != filter->n_coefs) || (!a_coefs) || (!b_coefs) ) {

	return 0;
    }

    int n_bytes_coefs = n_coefs * sizeof (IIR_signal_t);
    
    memcpy(filter->a, a_coefs, n_bytes_coefs);
    memcpy(filter->b, b_coefs, n_bytes_coefs);
    
    IIR_normalize_coefs( n_coefs, filter->b, filter->a );

    return 1;
}

// Add the next input (x) to the filter and return the corresponding output
// x is an array of size n_signals.
// HUGE gain by declaring this as inline!
inline IIR_signal_t *IIR_MS_add_input(IIR_MS_t *filter, const IIR_signal_t x[]) {
    
    int j, k;
    
    IIR_signal_t *y = filter->last_output;
    IIR_signal_t *z = filter->z;
    IIR_signal_t *a = filter->a;
    IIR_signal_t *b = filter->b;
    
    int n_coefs = filter->n_coefs;
    int n_signals = filter->n_signals;
    
    for (k=0; k<n_signals; k++){    
	y[k] = z[k*n_coefs] + b[0] * x[k];

	for (j = 1; j< n_coefs-1 ; j++){
	    z[k*n_coefs+(j-1)] = z[k*n_coefs+j] + x[k] * b[j] - y[k] * a[j];
	}
	z[k*n_coefs+(j-1)] = x[k] * b[j] - y[k] * a[j];
    }
    
    return y;
    
}

// Reset the filter to the resting state
#define IIR_MS_reset(filter) _IIR_M_reset(filter)

// Free all the memory allocated by the MS filter.
#define IIR_MS_destroy(filter) _IIR_M_destroy(filter)

/******************************************************
 * Functions specific to Different coefs Multi signal IIRs
 ******************************************************/

// Macros for indexing the a and b coefs
// They can be used both for reading and writing
// They use two indexes:
//	i = coef index
//	s = signal index
#define IIR_MD_COEFS_A_INDEX(filter, i, s) filter->a[filter->n_coefs*s + i]
#define IIR_MD_COEFS_B_INDEX(filter, i, s) filter->b[filter->n_coefs*s + i]

// Just a call to the general creation for multi signal IIR
// See the _IIR_MS_create function doc
inline IIR_MD_t *IIR_MD_create(int n_coefs, int n_signals,
	const IIR_signal_t *b_coefs,
	const IIR_signal_t *a_coefs) {

    return _IIR_M_create(n_coefs, n_signals, b_coefs, a_coefs, 1);
}

// Fills in the a and/or b coefficients of the filter but only for one given
// input signal. This function should only be used with MD filters.
// Returns 0 on fail (given number of coefs does not agree with filters coefs
// or any of coefs == NULL or signal index out of bounds)
// Both a_coefs and b_coefs params should be arrays of size n_coefs
// The coefficients in the filter are normalized after being set
// (the contents of input parameter arrays are not normalized)
inline int IIR_MD_set_coefs_one_signal(IIR_MD_t* filter, 
					int n_coefs,
					const IIR_signal_t *b_coefs,
					const IIR_signal_t *a_coefs,
					int signal_index) {
    
    int i;

    // Check that you are passing the appropriate number of coefs and is multi_coefs filter
    if ( (n_coefs != filter->n_coefs) || (!a_coefs) || (!b_coefs) ) {

	return 0;
    }

    // Check that the index is correct
    if ( (signal_index >= filter->n_signals) || (signal_index < 0) ) {

	return 0;
    }

    int n_bytes_coefs = n_coefs * sizeof (IIR_signal_t);
    
    IIR_signal_t *a_base = filter->a + n_coefs*signal_index;
    IIR_signal_t *b_base = filter->b + n_coefs*signal_index;
    
    memcpy(a_base, a_coefs, n_bytes_coefs);
    memcpy(b_base, b_coefs, n_bytes_coefs);
        
    IIR_normalize_coefs( n_coefs, b_base, a_base );

    return 1;
}

// Fills in the a and/or b coefficients of the filter
// returns 0 on fail (given number of coefs does not agree with filters coefs)


// Fills in the a and/or b coefficients of the filter for all the input signal.
// This function should only be used with MD filters.
// Returns 0 on fail (given number of coefs does not agree with filters coefs
// or any of coefs == NULL or signal index out of bounds)
// Both a_coefs and b_coefs params should be arrays of size n_coefs
// The coefficients in the filter are normalized after being set
// (the contents of input parameter arrays are not normalized)
inline int IIR_MD_set_coefs_all_signals(IIR_MD_t* filter,
					int n_coefs,
					const IIR_signal_t *b_coefs,
					const IIR_signal_t *a_coefs) {

    int i;
    
    // Check that you are passing the appropriate number of coefs
    if ( (n_coefs != filter->n_coefs) || (!a_coefs) || (!b_coefs) ) {

	return 0;
    }

    int n_bytes_coefs = n_coefs * sizeof (IIR_signal_t);
    int n_signals = filter->n_signals;

    // Make pointers to the normalized coefs
    IIR_signal_t *a_norm = filter->a;
    IIR_signal_t *b_norm = filter->b;
        
    // Prepare the first coefs copy and normalize
    memcpy(a_norm, a_coefs, n_bytes_coefs);
    memcpy(b_norm, b_coefs, n_bytes_coefs);
    
    IIR_normalize_coefs( n_coefs, b_norm, a_norm );

    IIR_signal_t *a_base = filter->a + n_coefs;
    IIR_signal_t *b_base = filter->b + n_coefs;
        
    for ( i=1; i< n_signals; i++){
	memcpy(a_base, a_norm, n_bytes_coefs);	
	memcpy(b_base, b_norm, n_bytes_coefs);
	a_base += n_coefs;
	b_base += n_coefs;
    }
    
    return 1;
}

// Add the next input (x) to the filter and return the corresponding output
// x is an array of size n_signals.
// HUGE gain by declaring this as inline!
inline IIR_signal_t *IIR_MD_add_input(IIR_MD_t *filter, const IIR_signal_t x[]) {
    
    int j, k;
    
    IIR_signal_t *y = filter->last_output;
    IIR_signal_t *z = filter->z;
    IIR_signal_t *a = filter->a;
    IIR_signal_t *b = filter->b;
    
    int n_coefs = filter->n_coefs;
    int n_signals = filter->n_signals;
    
    for (k=0; k<n_signals; k++){    
	y[k] = z[k*n_coefs] + b[k*n_coefs] * x[k];

	for (j = 1; j< n_coefs-1 ; j++){
	    z[k*n_coefs+(j-1)] = z[k*n_coefs+j] + x[k] * b[k*n_coefs+j] - y[k] * a[k*n_coefs+j];
	}
	z[k*n_coefs+(j-1)] = x[k] * b[k*n_coefs+j] - y[k] * a[k*n_coefs+j];
    }    
    
    return y;    
    
}

// Reset the filter to the resting state
#define IIR_MD_reset(filter) _IIR_M_reset(filter)

// Free all the memory allocated by the MD filter.
#define IIR_MD_destroy(filter) _IIR_M_destroy(filter)

#ifdef __cplusplus
}
#endif

#endif /* IIR_M_FILTERS_H */
