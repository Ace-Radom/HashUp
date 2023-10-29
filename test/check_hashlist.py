import os
import sys
import hashlib

def hash_file( path: str ) -> str:
    h = hashlib.md5()
    with open( path , 'rb' ) as rFile:
        while True:
            b = rFile.read( 1024 )
            if not b:
                break
            h.update( b )
    return h.hexdigest()

os.chdir( sys.argv[1] )
# change working dir to HashUp/test/test_dir (given in argv1)

with open( "test_dir.md5" , 'r' ) as rFile:
    while True:
        this_line = rFile.readline()
        if not this_line:
            break
        this_path = this_line[:this_line.rfind( " " )]
        this_hash = this_line[this_line.rfind( " " ) + 1:-1]
        if not hash_file( this_path ) == this_hash:
            print( hash_file( this_hash ) )
            exit( 1 )
