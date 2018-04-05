#!/bin/bash

# Script to simplify the names and directory tree of MiSeq/HiSeq output files.
# Destroy the nested folders in MiSeq folder, extract all archieves and then simplify filenames.
# Generate a single .tar.gz archieve containing all the data

# Raw data folder containing the input files, typically "somename-1234567890"
foldername=inputFolderName

# New and simpler name to rename the main folder
newfoldername=outputFolderName

mv ./$foldername/ ./$newfoldername
foldername=$newfoldername

# Gather all files under the main folder
mv ./$foldername/*/*/*.fastq.gz ./$foldername/

# Remove empty child folders
find ./$foldername/ -type d -empty
find ./$foldername/ -type d -empty -delete

# Uncompress
gzip -d ./$foldername/*.fastq.gz
rm ./$foldername/*.fastq.gz

# Simplify names of sample files
rename "s/\_S.*\_L001//g" $foldername/*.fastq
rename "s/\_001//g" $foldername/*.fastq

# Create an archive of the new whole folder
cd $foldername
tar -cvzf ../$foldername.tar.gz *.fastq
cd ..
