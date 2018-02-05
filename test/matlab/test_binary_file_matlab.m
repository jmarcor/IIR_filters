% Copyright (C) 2018 Jose Marco de la Rosa
% This program is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program.  If not, see <http://www.gnu.org/licenses/>.

fileID = fopen('../binary_ref_data_norm_coefs_double_scipy.bin');
read = fread(fileID,2,'uint')
n_coefs = read(1)
n_inputs = read(2)
type = fread(fileID,1,'uchar')

a_coefs = fread(fileID, n_coefs,'double')
b_coefs = fread(fileID, n_coefs,'double')
inputs = fread(fileID, n_inputs,'double')
c_outputs = fread(fileID, n_inputs,'double')

%a_coefs = fread(fileID, n_coefs,'single')
%b_coefs = fread(fileID, n_coefs,'single')
%inputs = fread(fileID, n_inputs,'single')
%c_outputs = fread(fileID, n_inputs,'single')

%a_coefs = single(a_coefs)
%b_coefs = single(b_coefs)
%inputs = single(inputs)
%c_outputs = single(c_outputs)

ml_outputs = filter(b_coefs, a_coefs, inputs)

%a_coefs = single(a_coefs)
%b_coefs = single(b_coefs)
%inputs = single(inputs)
%ml_outputs = single(filter(b_coefs, a_coefs, inputs))

hold off
plot( ml_outputs, 'r' )
hold on
plot( c_outputs, 'g' )
hold off

diff = (c_outputs - ml_outputs)

fclose(fileID);
