from fileinput import close


strs = [ "sdklssdf\n",
"kfsdlvmlcvxmvcxvl\n",
",vxclmosdfks; fkosd fsdf sd\n" ]

f = open( "test.txt", mode="w" )
for i in range( 100 ):
    print( strs[ i % 3 ], file=f )

f.close()