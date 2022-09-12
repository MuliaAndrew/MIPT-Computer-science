from fileinput import close
from io import SEEK_END


strs = [ "sdklssdf",
"kfsdlvmlcvxmvcxvl",
",vxclmosdfks; fkosd fsdf sd" ]

f = open( "test.txt", mode="w" )
for i in range( 100 ):
    print( strs[ i % 3 ], file=f )

print( "\n\n\n\n", file=f )
f.seek( 0, SEEK_END )
print( f.tell() )

f.close()