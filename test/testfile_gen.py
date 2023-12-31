#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os
import random
import sys

random_list = [ '0' , '1' , '2' , '3' , '4' , '5' , '6' , '7' , '8' , '9' , 'a' , 'b' , 'c' , 'd' , 'e' , 'f' ]

def get_random_line( len: int ) -> str:
    out = ""
    for i in range( 0 , len ):
        out += random.choice( random_list )
    return out

os.chdir( sys.argv[1] )
# change working dir to HashUp/test (given in argv1)

if os.path.exists( "test_dir" ) and os.path.isdir( "test_dir" ):
    for root, dirs, files in os.walk( "test_dir" , topdown = False ):
        for name in files:
            os.remove( os.path.join( root , name ) )
        for name in dirs:
            os.rmdir( os.path.join( root , name ) )
else:
    os.makedirs( "test_dir" )

for i in range( 0 , 64 ):
    with open( f"test_dir/test{ i }" , 'w' ) as wFile:
        for j in range( 0 , 32 ):
            wFile.write( get_random_line( 64 ) + "\n" )

for i in range( 0 , 16 ):
    with open( f"test_dir/{ i }.testf" , 'w' ) as wFile:
        for j in range( 0 , 32 ):
            wFile.write( get_random_line( 64 ) + "\n" )

os.makedirs( "test_dir/test_dir_second" )

for i in range( 0 , 16 ):
    with open( f"test_dir/test_dir_second/test{ i }" , 'w' ) as wFile:
        for j in range( 0 , 32 ):
            wFile.write( get_random_line( 64 ) + "\n" )

with open( "test_dir/.hashupignore" , 'w' ) as wFile:
    wFile.writelines(
        [
            "# test ignore file\n",
            "# ignore all files under test_dir_second\n",
            "test_dir_second/\n"
            "# ignore all test files num 0~9\n",
            "test?\n",
            "# ignore files which extension are '.testf'\n",
            "*.testf\n",
            "# ignore ignore file itself\n",
            ".hashupignore\n"
        ]
    )
