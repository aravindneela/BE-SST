name = sys.argv.pop()

Component( name )
Attribute( name, "usage", 0.0 )

Operation( name, "allocate", "memory-dummy.csv",
           Loiter( "usage", "<=", 2**24 ),               # Just a made-up size.
           Modify( "usage", Values("usage") + Inputs(0) ),
           Dawdle( 0.007 ) )

Operation( name, "deallocate", "memory-dummy.csv",
           Dawdle( 0.007 ),
           Modify( "usage", Values("usage") - Inputs(0) ) )
