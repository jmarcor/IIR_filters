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
 * Created on November 3, 2017, 1:58 PM
 */

// Implementation of Infinite Impulse Response (IIR) filters for one and multiple
// input signals.

#ifndef IIR_FILTERS_H
#define IIR_FILTERS_H

// Uncomment next line to use double for the signal type
//#define IIR_USE_SIGNAL_TYPE_DOUBLE

#ifdef IIR_USE_SIGNAL_TYPE_DOUBLE
    #define IIR_SIGNAL_TYPE double
    #define IIR_SIGNAL_FORMAT "lf"
#else
    #define IIR_SIGNAL_TYPE float
    #define IIR_SIGNAL_FORMAT "f"
#endif

#ifdef __cplusplus
extern "C" {
#endif  
      
// Type for the filter signals. Will be float (default) or double depending
// on the previous definition of IIR_SIGNAL_TYPE
typedef IIR_SIGNAL_TYPE IIR_signal_t;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Common operation to normalize coefficients a and b so a[0] == 1.0
// Operates directly on the arrays passed as parameters
inline int IIR_normalize_coefs( int n_coefs, IIR_signal_t *b_coefs, IIR_signal_t *a_coefs ){
    
    int i;

    if ( (!a_coefs) || (!b_coefs) ) {

	return 0;
    }    
    
    IIR_signal_t a0 = a_coefs[0];
    
    // Can't normalize if a[0] is 0
    if (a0 == 0){
	return 0;
    }
    
    // Initialize coefs and normalize
    for (i = 0; i < n_coefs; i++) {
	a_coefs[i] = a_coefs[i]/a0;
	b_coefs[i] = b_coefs[i]/a0;
    }
    
    return 1;
}

// One input signal filters
#include "IIR_S_filter.h"

// Multiple input signal filters
#include "IIR_M_filters.h"

#ifdef __cplusplus
}
#endif

#endif /* IIR_FILTERS_H */

