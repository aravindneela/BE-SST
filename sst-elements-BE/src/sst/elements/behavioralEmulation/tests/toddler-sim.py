# Example Details:
#   Nodes connected by a Torus. Each node has a memory and four CPUs. Each
#   CPU has four cores connected by a hypothetical "on-chip connection".

# ----------------------------- CPU Core Setup ------------------------------ #

# Make a new component and give it a program.
Component( "xeon-e5-core" )
Program( "xeon-e5-core", "toddler-sw.txt" )

# Assign software properties.
Property( "xeon-e5-core", "mpi.commRank", lambda gid, cid, cids, index: cid  )
Property( "xeon-e5-core", "mpi.commSize", lambda gid, cid, cids, index: cids )

# The routing index is 'mpi.comm-rank'
Ordinal( "xeon-e5-core", "mpi.commRank" )

Relation( "xeon-e5-core", "xeon-e5-core", "cpu", "self" )

# A 'ddr4-32g' is 'aunt' and 'memory' (parent's sibling) of 'xeon-e5-core'
Relation( "xeon-e5-core", "ddr4-32g", "memory", "aunt" )

Attribute( "xeon-e5-core", "usage", 0.0 )
Attribute( "xeon-e5-core", "waiting", False )

Operation( "xeon-e5-core", "wait", NoLookup, None,
           Modify( "waiting", True ),
           Loiter( "waiting", "==", False ) )

Operation( "xeon-e5-core", "unwait", NoLookup, None,
           Loiter( "waiting", "==", True ),
           Modify( "waiting", False ))

Operation( "xeon-e5-core", "fft", "fft-dummy.csv", "linear",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.005 ),
           Modify( "usage", 0.0 ) )

Mailbox( "xeon-e5-core", "unwait", lambda source, target, size, tag: [],
         OnTarget ) # This mailbox is only applied to the last component.


# ----------------------- On-Chip Connection Setup -------------------------- #

Component( "occ" )
Attribute( "occ", "usage", 0.0 )

Operation( "occ", "transfer", "transfer-dummy-A.csv", "linear",
           Loiter( "usage", "==", 0.0 ),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )

Mailbox( "occ", "transfer", lambda source, target, size, tag: [size], OnAll )

# ------------------------------- CPU Setup --------------------------------- #

Component( "xeon-e5" )

# Four cores per CPU.
Offspring( "xeon-e5", Mesh("xeon-e5-core", "occ", [4, 4] ) )

# ----------------------------- Memory Setup -------------------------------- #

Component( "ddr4-32g" )
Attribute( "ddr4-32g", "usage", 0.0 )

Operation( "ddr4-32g", "allocate", "memory-dummy.csv", "linear",
           Loiter( "usage", "<=", 2**24 ),               # Just a made-up size.
           Modify( "usage", Values("usage") + Inputs(0) ),
           Dawdle( 0.019 ) )

Operation( "ddr4-32g", "deallocate", "memory-dummy.csv", "linear",
           Dawdle( 0.019 ),
           Modify( "usage", Values("usage") - Inputs(0) ) )

# ------------------------------- QPI Setup --------------------------------- #

Component( "qpi")
Attribute( "qpi", "usage", 0.0 )

Operation( "qpi", "transfer", "transfer-dummy-A.csv", "linear",
           Loiter( "usage", "==", 0.0 ),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )

Mailbox( "qpi", "transfer", lambda source, target, size, tag: [size], OnAll )


# ---------------------------- Ethernet Setup ------------------------------- #
Component( "eth" )
Attribute( "eth", "usage", 0.0 )

Operation( "eth", "transfer", "transfer-dummy-B.csv", "linear",
           Loiter( "usage", "==", 0.0 ),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )

Mailbox( "eth", "transfer", lambda source, target, size, tag: [size], OnAll )

# ------------------------------- Node Setup -------------------------------- #

Component( "node" )
Offspring( "node", Mesh("xeon-e5", "qpi", [2, 2]) )
Offspring( "node", Single("ddr4-32g") )

# ------------------------------ System Setup ------------------------------- #

Component( "system" )
Offspring( "system", Torus("node", "eth", [4, 4]) ) # 3D-Torus.

Root("system")
