#!/bin/bash

echo "Please modify the variables in this script"
#exit

#Please replace the following with a working directory of your choice
exampleRoot="/reg/neh/home5/agok/exafelSZ_example"

exp="cxic0515"
run="83"
peakSize="17"

#Read the data and generate the necessary files:
python roi_SZ_write_p17.py ${exp} ${run} ${peakSize} ${exampleRoot}
#The following  files are generated:
#  ${exampleRoot}/${exp}/r${run}/data.bin
#  ${exampleRoot}/${exp}/r${run}/peaks.bin

#Fix the peaks.bin file:
./fixPythonBinPeakFile ${exampleRoot}/${exp}/r${run}/peaks.bin ${exampleRoot}/${exp}/r${run}/peaks.fixed.bin
# There is a problem in the peaks.bin file due to the limited binary file support of Python 2. 
# This tool fixes that problem.

#Run exafelSZ_example. You can use either choose exafelSZ_test_sta or exafelSZ_test_dyn. The are functionally the same, the former being statically lined, the latter dynamically.
./exafelSZ_example_sta ${exampleRoot}/${exp}/r${run}/data.bin ${exampleRoot}/${exp}/r${run}/peaks.fixed.bin calibPanel.bin ${exampleRoot}/${exp}/r${run}/comp.bin ${exampleRoot}/${exp}/r${run}/decomp.bin
# ./exafelSZ_example_dyn ${exampleRoot}/${exp}/r${run}/data.bin ${exampleRoot}/${exp}/r${run}/peaks.fixed.bin calibPanel.bin ${exampleRoot}/${exp}/r${run}/comp.bin ${exampleRoot}/${exp}/r${run}/decomp.bin
# Please note that calibPanel.bin is an empty panel, which is provided in the example package.

#Convert the decompressed file into a cxi file.
python bin2cxi.py ${exampleRoot}/${exp}/r${run}/decomp.bin ${exampleRoot}/${exp}/r${run}/decomp.cxi

