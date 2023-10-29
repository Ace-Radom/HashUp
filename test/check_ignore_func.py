import os
import sys

os.chdir( sys.argv[1] )
# change working dir to HashUp/test/test_dir (given in argv1)

with open( "test_dir.md5" , 'r' ) as rFile:
    counter = 10
    while True:
        this_line = rFile.readline()
        if not this_line:
            break
        this_path = this_line[:this_line.rfind( " " )]
        if len( this_path ) != 8:
            exit( 1 )
        this_num = this_path[6:]
        if int( this_num ) != counter:
            exit( 1 )
        counter += 1
