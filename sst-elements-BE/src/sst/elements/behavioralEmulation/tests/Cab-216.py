import input.cartesian

global cartesianData

cartesianData = input.cartesian.data216

# ----------------------------- CPU Core Setup ------------------------------ #

Component( "e5-2670-core" )
Program( "e5-2670-core", "nek-abstract.smc" )

Property( "e5-2670-core", "mpi.commRank", lambda gid, cid, cids, index: cid  )
Property( "e5-2670-core", "mpi.commSize", lambda gid, cid, cids, index: cids )

def cartX(g, rank, c, i): return cartesianData[rank]["index"][0]
def cartY(g, rank, c, i): return cartesianData[rank]["index"][1]
def cartZ(g, rank, c, i): return cartesianData[rank]["index"][2]

def cartXp(g, rank, c, i): return cartesianData[rank]["Xplus"]
def cartYp(g, rank, c, i): return cartesianData[rank]["Yplus"]
def cartZp(g, rank, c, i): return cartesianData[rank]["Zplus"]

def cartXm(g, rank, c, i): return cartesianData[rank]["Xminus"]
def cartYm(g, rank, c, i): return cartesianData[rank]["Yminus"]
def cartZm(g, rank, c, i): return cartesianData[rank]["Zminus"]

Property( "e5-2670-core", "mpi.cartesianX", cartX )
Property( "e5-2670-core", "mpi.cartesianY", cartY )
Property( "e5-2670-core", "mpi.cartesianZ", cartZ )

Property( "e5-2670-core", "mpi.cartesianXplus", cartXp )
Property( "e5-2670-core", "mpi.cartesianYplus", cartYp )
Property( "e5-2670-core", "mpi.cartesianZplus", cartZp )

Property( "e5-2670-core", "mpi.cartesianXminus", cartXm )
Property( "e5-2670-core", "mpi.cartesianYminus", cartYm )
Property( "e5-2670-core", "mpi.cartesianZminus", cartZm )


Ordinal( "e5-2670-core", "mpi.commRank" )

Relation( "e5-2670-core", "e5-2670-core", "cpu", "self" )

Attribute( "e5-2670-core", "usage", 0.0 )
Attribute( "e5-2670-core", "waiting", False )

Operation( "e5-2670-core", "wait", NoLookup,
           Modify( "waiting", True ),
           Loiter( "waiting", "==", False ) )

Operation( "e5-2670-core", "unwait", NoLookup,
           Loiter( "waiting", "==", True ),
           Modify( "waiting", False ))

Operation( "e5-2670-core", "mm", "cab-matmult-12.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( AnyOutput() ),
           Modify( "usage", 0.0 ) )

Mailbox( "e5-2670-core", "unwait", lambda source, target, size, tag: [],
         OnTarget ) # This mailbox is only applied to the last component.

# ----------------------- On-Chip Connection Setup -------------------------- #

Component( "on-chip-connection" )

Operation( "on-chip-connection", "transfer", "cab-transfer-occ.csv",
           Dawdle( AnyOutput() ) )

Mailbox( "on-chip-connection", "transfer",
         lambda source, target, size, tag: [size], OnAll )

Component( "bus" )

# ------------------------------- CPU Setup --------------------------------- #

Component( "xeon-e5-2670" )
Offspring( "xeon-e5-2670", Tree( ["on-chip-connection", "e5-2670-core"],
                                 ["bus"], [6] ) )

# ------------------------------- QPI Setup --------------------------------- #

Component( "qpi")
Operation( "qpi", "transfer", "cab-transfer-qpi.csv", Dawdle( AnyOutput() ) )
Mailbox( "qpi", "transfer", lambda source, target, size, tag: [size], OnAll )

# ---------------------------- Infiniband Setup ----------------------------- #

Component( "ib" )

# ------------------------------- Node Setup -------------------------------- #

Component( "node" )
Offspring( "node", Mesh("xeon-e5-2670", "qpi", [2]) ) # Or linear

# ------------------------------ System Setup ------------------------------- #

Component( "switch" )

Operation( "switch", "transfer", "cab-transfer-switch-B.csv", Dawdle( AnyOutput() ) )

Mailbox( "switch", "transfer", lambda source, target, size, tag: [size], OnAll )


Component( "system" )

Offspring( "system", Tree( ["switch", "node"], ["ib"], [18] ) )

# For the 4-node case study, all the 4-nodes are likely to be on the same
# switch.

Root("system")
