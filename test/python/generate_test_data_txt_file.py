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

import sys
import numpy as np
from scipy.signal import butter, lfilter

EXIT_FAILURE = 1
EXIT_SUCCESS = 0

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print 'Usage is:\n\t> %s <n_coefs> <butterworth_param> <output_filename_w/o_ext>'% sys.argv[0]
        print '''
        '''
        exit( EXIT_FAILURE )
    filename = sys.argv[1]
    print filename

    n_coefs = int(sys.argv[1])-1
    b_par = float(sys.argv[2])
    filename = sys.argv[3]

    print 'n_coefs = %d\nb_par = %f\nfilename = %s' \
            %(n_coefs, b_par, filename)

    # Create the inputs: sin followed by random followed by sin
    x = np.arange(0, 4 * 2 * np.pi, 0.075)
    inputs = np.concatenate((
        np.array( 1500*np.sin(x), dtype='f' ),
        np.array(1000 * np.random.rand(200), dtype='f'),
        np.array(500 * (np.random.rand(200)-.5), dtype='f'),
        np.array(1000 * (np.random.rand(200)-1), dtype='f'),
        np.array(1500 * np.sin(x), dtype='f'),
    ))

    # Create filter and perform filtering for both float an double
    f_init = butter(n_coefs, b_par)

    f = ( np.array(f_init[0], dtype='f'), np.array(f_init[1], dtype='f') )
    outputs = lfilter( f[0], f[1], inputs )

    print len(inputs)

    n_coefs = len(f[0])
    n_inputs = len(inputs)
    a_coefs = f[1]
    b_coefs = f[0]

    # Write two versions of the file
    for n_fact, n_suf in {1: '_norm_coefs.txt', 1.45: '_not_norm_coefs.txt'}.iteritems():
        # Open the file
        with open(filename+n_suf, 'w') as file:
            str = '%d, %d' % (n_coefs, n_inputs)
            file.write(str+'\n')
            print str

            str = ', '.join(['%.10f'%v for v in a_coefs*n_fact])
            file.write(str+'\n')
            print str

            str = ', '.join(['%.10f'%v for v in b_coefs*n_fact])
            file.write(str+'\n')
            print str

            str = ', '.join(['%.10f'%v for v in inputs])
            file.write(str+'\n')
            print str
            str = ', '.join(['%.10f'%v for v in outputs])
            file.write(str+'\n')
            print str

    exit(EXIT_SUCCESS)

    import matplotlib.pyplot as plt
    plt.plot(inputs)
    plt.plot(outputs, 'r')
    plt.show()
