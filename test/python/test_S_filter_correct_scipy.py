#!/usr/bin/python

# Copyright (C) 2018 Jose Marco de la Rosa
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import numpy as np
from array import array
import sys
from scipy.signal import lfilter

def load_binary_test_file( filename ):
    with open(filename, "rb") as f:
        # Save n_coefs, n_inputs as ints
        initdata = array('i')
        initdata.fromfile(f, 2)
        n_coefs, n_inputs = initdata
        initdata = array('c')
        initdata.fromfile(f, 1)
        type_char = initdata[0]
        print '%d, %d, %c\n' % (n_coefs, n_inputs, type_char)
        a_coefs = array(type_char)
        a_coefs.fromfile(f, n_coefs)
        b_coefs = array(type_char)
        b_coefs.fromfile(f, n_coefs)
        inputs = array(type_char)
        inputs.fromfile(f, n_inputs)
        outputs = array(type_char)
        outputs.fromfile(f, n_inputs)

    return (type_char, a_coefs, b_coefs, inputs, outputs)

EXIT_FAILURE = 1
EXIT_SUCCESS = 0

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print 'Usage is:\n\t> %s <test_input_reference_bin_file>'% sys.argv[0]
        print '''The format of the binary file is\n
              int32 n_coefs;
              int32 n_inputs;
              char type;
              <The following values will be float32 if type == 'f' or double if type=='d'>
              float32/double a_coefs[n_coefs];
              float32/double b_coefs[n_coefs];
              float32/double inputs[n_inputs];
              float32/double c_outputs[n_inputs];
        '''
        exit( EXIT_FAILURE )
    filename = sys.argv[1]
    print filename

    # Read the file
    type_char, a_coefs, b_coefs, inputs, c_outputs = load_binary_test_file(filename)

    if type_char == 'f':
        a_coefs = np.array(a_coefs, dtype=np.float32)
        b_coefs = np.array(b_coefs, dtype=np.float32)
        inputs = np.array(inputs, dtype=np.float32)
        c_outputs = np.array(c_outputs, dtype=np.float32)
    else:
        a_coefs = np.array(a_coefs, dtype=np.double)
        b_coefs = np.array(b_coefs, dtype=np.double)
        inputs = np.array(inputs, dtype=np.double)
        c_outputs = np.array(c_outputs, dtype=np.double)

    a_coefs_str = ['%.10f' % v for v in a_coefs]
    a_coefs_str = ','.join(a_coefs_str)
    b_coefs_str = ['%.10f' % v for v in b_coefs]
    b_coefs_str = ','.join(b_coefs_str)

    ml_arr_str = {'d':'[%s]', 'f':'single([%s])'}[type_char]

    print 'a_coefs=%s' % (ml_arr_str % a_coefs_str)
    print 'b_coefs=%s' % (ml_arr_str % b_coefs_str)

    inputs_str = ['%.10f' % v for v in inputs]
    inputs_str = ','.join(inputs_str)
    print 'inputs=%s' % (ml_arr_str % inputs_str)

    c_outputs_str = ['%.10f' % v for v in c_outputs]
    c_outputs_str = ','.join(c_outputs_str)
    print 'c_out=%s' % (ml_arr_str % c_outputs_str)

    # a_coefs /= a_coefs[0]
    # b_coefs /= a_coefs[0]

    c_outputs = np.array(c_outputs, dtype=type_char)

    print '\nTested type is: %s'%({'d':'double','f':'float'}[type_char])
    outputs_scipy = np.array(lfilter(b_coefs, a_coefs, inputs), dtype=type_char)

    outputs_scipy_str = ['%.10f' % v for v in outputs_scipy]
    print 'scipy_out=%s' % (ml_arr_str % ','.join(outputs_scipy_str))

    print "ml_out = filter(b_coefs,a_coefs,inputs);%s hold off; plot(inputs,'g'); hold on; plot(c_out,'r'); plot(ml_out,'b')"\
          % {'d':'', 'f':' ml_out = single(ml_out);'}[type_char]
    print 'diff = c_out - ml_out\nsprintf(\'Maximum difference between c and matlab filtered signals: %e\', max(diff))'

    diff = c_outputs - outputs_scipy
    print 'sum(c_outputs - outputs_scipy)=', sum(diff)

    if sum(diff) == np.zeros(1)[0]:
        print 'SUCCESS: Test passed. C output values in the binary match bit by bit with python scipy lfilter implementation'
        exit(EXIT_SUCCESS)
    else:
        print 'ERROR: Test not passed. C output values in the binary do not match bit by bit with python base implementation'
        exit(EXIT_FAILURE)

