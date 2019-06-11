
# -- Should be specified in AppBEO. Not here. Have it here for now
from cartesianrank import CartesianGrid 

global cartesianData

cartesianData = CartesianGrid(4, 4, 2) #Should match with the system config Topology dimensions. Must talk about this

def cartX(g, rank, c, i): return cartesianData.myCoordinates(rank, "X")
def cartY(g, rank, c, i): return cartesianData.myCoordinates(rank, "Y")
def cartZ(g, rank, c, i): return cartesianData.myCoordinates(rank, "Z")
def cartXp(g, rank, c, i): return cartesianData.neighbourRank(rank, "Xplus")
def cartHasXp(g, rank, c, i): return cartesianData.neighbourRank(rank, "Xplus") >= 0
def cartYp(g, rank, c, i): return cartesianData.neighbourRank(rank, "Yplus")
def cartHasYp(g, rank, c, i): return cartesianData.neighbourRank(rank, "Yplus") >= 0
def cartZp(g, rank, c, i): return cartesianData.neighbourRank(rank, "Zplus")
def cartHasZp(g, rank, c, i): return cartesianData.neighbourRank(rank, "Zplus") >= 0
def cartXm(g, rank, c, i): return cartesianData.neighbourRank(rank, "Xminus")
def cartHasXm(g, rank, c, i): return cartesianData.neighbourRank(rank, "Xminus") >= 0
def cartYm(g, rank, c, i): return cartesianData.neighbourRank(rank, "Yminus")
def cartHasYm(g, rank, c, i): return cartesianData.neighbourRank(rank, "Yminus") >= 0
def cartZm(g, rank, c, i): return cartesianData.neighbourRank(rank, "Zminus")
def cartHasZm(g, rank, c, i): return cartesianData.neighbourRank(rank, "Zminus") >= 0

# component config
name = sys.argv.pop()

# Make a new component and give it a program.
Component( name )

# Assign software properties. -- Should be specified in AppBEO. Not here. Have it here for now
Property( name, "app.elementSize", lambda gid, cid, cids, index: 20 )
Property( name, "app.elementsPerProcess", lambda gid, cid, cids, index: 100 )
Property( name, "app.transferSizeX", lambda gid, cid, cids, index: 32768 ) 
Property( name, "app.transferSizeY", lambda gid, cid, cids, index: 32768 )
Property( name, "app.transferSizeZ", lambda gid, cid, cids, index: 65536 )
Property( name, "app.timesteps", lambda gid, cid, cids, index: 1 )
Property( name, "app.phyParam", lambda gid, cid, cids, index: 5 )

Property( name, "mpi.commRank", lambda gid, cid, cids, index: cid  )
Property( name, "mpi.commSize", lambda gid, cid, cids, index: cids )

Property( name, "mpi.cartesianX", cartX )
Property( name, "mpi.cartesianY", cartY )
Property( name, "mpi.cartesianZ", cartZ )
Property( name, "mpi.cartesianXplus", cartXp )
Property( name, "mpi.cartesianHasXplus", cartHasXp )
Property( name, "mpi.cartesianYplus", cartYp )
Property( name, "mpi.cartesianHasYplus", cartHasYp )
Property( name, "mpi.cartesianZplus", cartZp )
Property( name, "mpi.cartesianHasZplus", cartHasZp )
Property( name, "mpi.cartesianXminus", cartXm )
Property( name, "mpi.cartesianHasXminus", cartHasXm )
Property( name, "mpi.cartesianYminus", cartYm )
Property( name, "mpi.cartesianHasYminus", cartHasYm )
Property( name, "mpi.cartesianZminus", cartZm )
Property( name, "mpi.cartesianHasZminus", cartHasZm )

# The routing index is 'mpi.comm-rank'
Ordinal( name, "mpi.commRank" )

Relation( name, name, "cpu", "self" )

Attribute( name, "usage", 0.0 )

Mailbox( name, "unwait", lambda source, target, size, tag: [source],
         OnTarget ) # This mailbox is only applied to the last component.
