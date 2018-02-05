#!/bin/bash

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

# function storing list of all files in variable files
#files="`ls ../build/apply*`"
files_text="`find ../build/ -type f -executable -not -regex '.*speed.*' -not -regex '.*binary.*' -not -regex '.*create.*'`"
files_bin="`find ../build/ -type f -executable -regex '.*binary.*'`"
files_no_input="`find ../build/ -type f -executable -regex '.*create.*'`"
text_test_input_files_pattern="tmp/text_ref_data_*.txt"
bin_test_input_files_prefix="tmp/binary_ref_data_*"

RED='\033[0;31m'
GREEN='\033[0;32m'
LIGHT_BLUE='\033[1;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

RESULTS=("${GREEN}SUCCESS${NC}" "${RED}FAIL${NC}")

# Create a tmp folder to hold all temporary results
mkdir -p tmp
cp *.txt tmp
cp *.bin tmp

# First run all no-input tests
count=0
for f in $files_no_input
do
	echo $f $test_input_file
	$f > /dev/null
	TEST_RESULTS[count]=$?
	count=$(( $count + 1 ))
done

# Second run all text-based tests (they do not depend on data type)
for f in $files_text
do
	for test_input_file in $text_test_input_files_pattern
	do
		echo $f $test_input_file
		$f $test_input_file > /dev/null
		TEST_RESULTS[count]=$?
		count=$(( $count + 1 ))
	done
done

# Third run binary-based tests (they do depend on data type)
for f in $files_bin
do	
	type=${f##*_}	
	for test_input_file in $bin_test_input_files_prefix${type}*scipy.bin
	do
		echo $f $test_input_file
		$f $test_input_file > /dev/null
		TEST_RESULTS[count]=$?
		count=$(( $count + 1 ))
	done
done

count=0
fails=0
echo ""

# Show results for text-based tests
for f in $files_no_input
do
	this_test_result=0
	if [ ${TEST_RESULTS[count]} != 0 ]
	then
		this_test_result=1
	fi
	
	printf -v var " - Executing: '${LIGHT_BLUE}${f}${NC}' ... %s\n" "${RESULTS[this_test_result]}"
	echo -e $var
	fails=$(( $fails + TEST_RESULTS[$count] ))
	count=$(( $count + 1 ))
done

# Show results for text-based tests
for f in $files_text
do
	for test_input_file in $text_test_input_files_pattern
	do
		this_test_result=0
		if [ ${TEST_RESULTS[count]} != 0 ]
		then
			this_test_result=1
		fi
		
		printf -v var " - Executing: '${LIGHT_BLUE}${f} ${CYAN}${test_input_file}${NC}' ... %s\n" "${RESULTS[this_test_result]}"
		echo -e $var
		fails=$(( $fails + TEST_RESULTS[$count] ))
		count=$(( $count + 1 ))
	done
done

# Show results for binary-based tests
for f in $files_bin
do	
	type=${f##*_}	
	for test_input_file in $bin_test_input_files_prefix${type}*scipy.bin
	do
		this_test_result=0
		if [ ${TEST_RESULTS[count]} != 0 ]
		then
			this_test_result=1
		fi
	
		printf -v var " - Executing: '${LIGHT_BLUE}${f} ${CYAN}${test_input_file}${NC}' ... %s\n" "${RESULTS[this_test_result]}"
		echo -e $var
		fails=$(( $fails + TEST_RESULTS[$count] ))
		count=$(( $count + 1 ))
	done
done

rm -fr tmp

if [ $fails -eq 0 ]
then
    echo -e "${GREEN}SUCCESS: All tests passed!${NC}"
else
    echo -e "${RED}FAIL: Some tests failed! Look at the failed tests above and check the code. All tests sources and input files are in the ./test directory${NC}"
    exit 1
fi
echo ""
