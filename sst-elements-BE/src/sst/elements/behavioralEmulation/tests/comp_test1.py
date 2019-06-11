Component( "BGQ-core" )
Program( "BGQ-core", "memory-test.txt" )

Property( "BGQ-core", "app.elementSize", lambda gid, cid, cids, index: 20 )
Property( "BGQ-core", "mpi.commRank", lambda gid, cid, cids, index: cid  )
Property( "BGQ-core", "mpi.commSize", lambda gid, cid, cids, index: cids )

Ordinal( "BGQ-core", "mpi.commRank" )
Relation( "BGQ-core", "BGQ-core", "cpu", "self" )
Attribute( "BGQ-core", "usage", 0.0 )

Component( "BGQ-network" )
Attribute( "BGQ-network", "usage", 0.0 )

Operation( "BGQ-core", "computeA", "computeA.csv", "linear",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( Outputs(0) ),
           Modify( "usage", 0.0 ) )
Operation( "BGQ-core", "computeB", "computeB.csv", "linear",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( Outputs(0) ),
           Modify( "usage", 0.0 ) )
Operation( "BGQ-core", "computeC", "computeC.csv", "linear",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( Outputs(0) ),
           Modify( "usage", 0.0 ) )

Component( "system" )
Offspring( "system", Mesh("BGQ-core", "BGQ-network", [32, 16, 16]) )
Root("system")

