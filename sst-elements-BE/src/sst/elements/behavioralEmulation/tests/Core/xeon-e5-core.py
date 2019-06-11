name = sys.argv.pop()

# Make a new component and give it a program.
Component( name )

# Assign software properties.
Property( name, "mpi.commRank", lambda gid, cid, cids, index: cid  )
Property( name, "mpi.commSize", lambda gid, cid, cids, index: cids )

# The routing index is 'mpi.comm-rank'
Ordinal( name, "mpi.commRank" )

Relation( name, name, "cpu", "self" )

Attribute( name, "usage", 0.0 )
Attribute( name, "waiting", False )

Operation( name, "wait", NoLookup,
           Modify( "waiting", True ),
           Loiter( "waiting", "==", False ) )

Operation( name, "unwait", NoLookup,
           Loiter( "waiting", "==", True ),
           Modify( "waiting", False ))

Operation( name, "fft", "fft-dummy.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.005 ),
           Modify( "usage", 0.0 ) )

Mailbox( name, "unwait", lambda source, target, size, tag: [],
         OnTarget ) # This mailbox is only applied to the last component.
