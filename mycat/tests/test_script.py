from fileinput import close
from io import SEEK_END
from venv import create


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

for i in range( 10 ):
    fname = "test" + chr( i + ord( '0' ) ) + ".txt"
    f = open( fname, "w" )
    int_a = ord( 'a' )
    text = chr( i + int_a ) + "---" + chr( i + 5 + int_a ) + "---" + chr( i + 3 + int_a )
    buffer = ""
    for k in range( 50 ):
        buffer += text + text
    print( buffer, file=f )
    f.close()