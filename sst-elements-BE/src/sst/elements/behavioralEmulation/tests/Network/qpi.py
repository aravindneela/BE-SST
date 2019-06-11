name = sys.argv.pop()

Component( name )
Attribute( name, "usage", 0.0 )

Operation( name, "transfer", "transfer-dummy-A.csv",
           Loiter( "usage", "==", 0.0 ),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )

Mailbox( name, "transfer", lambda source, target, size, tag: [size], OnAll )
