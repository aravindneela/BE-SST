import cartesianproper

global cartesianData

cartesianData = cartesianproper.data8192
cores = 8192

Component( "BGQ-core" )
Program( "BGQ-core", "cmt-bone-be.smc" )

Property( "BGQ-core", "app.elementSize", lambda gid, cid, cids, index: 9 )
Property( "BGQ-core", "app.elementsPerProcess", lambda gid, cid, cids, index: 8 )
Property( "BGQ-core", "app.transferSize", lambda gid, cid, cids, index: 125 )

Property( "BGQ-core", "mpi.commRank", lambda gid, cid, cids, index: cid  )
Property( "BGQ-core", "mpi.commSize", lambda gid, cid, cids, index: cids )
def cartX(g, rank, c, i): return cartesianData[rank]["index"][0]
def cartY(g, rank, c, i): return cartesianData[rank]["index"][1]
def cartZ(g, rank, c, i): return cartesianData[rank]["index"][2]
def cartXp(g, rank, c, i): return cartesianData[rank]["Xplus"]
def cartHasXp(g, rank, c, i): return cartesianData[rank]["hasXplus"]
def cartYp(g, rank, c, i): return cartesianData[rank]["Yplus"]
def cartHasYp(g, rank, c, i): return cartesianData[rank]["hasYplus"]
def cartZp(g, rank, c, i): return cartesianData[rank]["Zplus"]
def cartHasZp(g, rank, c, i): return cartesianData[rank]["hasZplus"]
def cartXm(g, rank, c, i): return cartesianData[rank]["Xminus"]
def cartHasXm(g, rank, c, i): return cartesianData[rank]["hasXminus"]
def cartYm(g, rank, c, i): return cartesianData[rank]["Yminus"]
def cartHasYm(g, rank, c, i): return cartesianData[rank]["hasYminus"]
def cartZm(g, rank, c, i): return cartesianData[rank]["Zminus"]
def cartHasZm(g, rank, c, i): return cartesianData[rank]["hasZminus"]
Property( "BGQ-core", "mpi.cartesianX", cartX )
Property( "BGQ-core", "mpi.cartesianY", cartY )
Property( "BGQ-core", "mpi.cartesianZ", cartZ )
Property( "BGQ-core", "mpi.cartesianXplus", cartXp )
Property( "BGQ-core", "mpi.cartesianHasXplus", cartHasXp )
Property( "BGQ-core", "mpi.cartesianYplus", cartYp )
Property( "BGQ-core", "mpi.cartesianHasYplus", cartHasYp )
Property( "BGQ-core", "mpi.cartesianZplus", cartZp )
Property( "BGQ-core", "mpi.cartesianHasZplus", cartHasZp )
Property( "BGQ-core", "mpi.cartesianXminus", cartXm )
Property( "BGQ-core", "mpi.cartesianHasXminus", cartHasXm )
Property( "BGQ-core", "mpi.cartesianYminus", cartYm )
Property( "BGQ-core", "mpi.cartesianHasYminus", cartHasYm )
Property( "BGQ-core", "mpi.cartesianZminus", cartZm )
Property( "BGQ-core", "mpi.cartesianHasZminus", cartHasZm )
Ordinal( "BGQ-core", "mpi.commRank" )
Relation( "BGQ-core", "BGQ-core", "cpu", "self" )
Attribute( "BGQ-core", "usage", 0.0 )
Attribute( "BGQ-core", "waiting", False )
Operation( "BGQ-core", "wait", NoLookup,
           Modify( "waiting", True ) )
Operation( "BGQ-core", "unwait", NoLookup,
           Modify( "waiting", False ))
Operation( "BGQ-core", "computeA", "vulcan-compute-a-proper.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )
Operation( "BGQ-core", "computeB", "vulcan-compute-b-proper.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )
Operation( "BGQ-core", "prepareFaces", "vulcan-compute-face-proper.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )
Mailbox( "BGQ-core", "unwait", lambda source, target, size, tag: [],
         OnTarget )
Component( "BGQ-network" )
Operation( "BGQ-network", "transfer", "vulcan-transfer-512c.csv",
           Dawdle( 0.003 ) )
Mailbox( "BGQ-network", "transfer", lambda source, target, size, tag: [size],
         OnAll )
#Component( "BGQ-connection" )
Component( "system" )
#Offspring( "system", Tree( ["BGQ-network", "BGQ-core"], ["BGQ-connection"], [ cores ] ) )
Offspring( "system", Torus("BGQ-core", "BGQ-network", [8, 8, 8, 4, 4]) )
Root("system")

