import data
global rankdata

rankdata = data.lst
cores = 8

Component( "xeon-e5-core" )
Program( "xeon-e5-core", "lulesh.smc" )

Property( "xeon-e5-core", "app.elementsize", lambda gid, cid, cids, index: 10 )
Property( "xeon-e5-core", "app.procsperplane", lambda gid, cid, cids, index: 2 )
Property( "xeon-e5-core", "mpi.commsize", lambda gid, cid, cids, index: cids)
Property( "xeon-e5-core", "mpi.commRank", lambda gid, cid, cids, index: cid )
def rowmin(g, rank, c, i): return rankdata[rank]["rowmin"]
def rowmax(g, rank, c, i): return rankdata[rank]["rowmax"]
def colmin(g, rank, c, i): return rankdata[rank]["colmin"]
def colmax(g, rank, c, i): return rankdata[rank]["colmax"]
def planemin(g, rank, c, i): return rankdata[rank]["planemin"]
def planemax(g, rank, c, i): return rankdata[rank]["planemax"]
def Zminus(g, rank, c, i): return rankdata[rank]["Zminus"]
def Zplus(g, rank, c, i): return rankdata[rank]["Zplus"]
def Yminus(g, rank, c, i): return rankdata[rank]["Yminus"]
def Yplus(g,rank, c, i): return rankdata[rank]["Yplus"]
def Xminus(g, rank, c, i): return rankdata[rank]["Xminus"]
def Xplus(g, rank, c, i): return rankdata[rank]["Xplus"]
def recv1(g, rank, c, i): return rankdata[rank]["recv1"]
def recv2(g, rank, c, i): return rankdata[rank]["recv2"]
def recv3(g, rank, c, i): return rankdata[rank]["recv3"]
Property( "xeon-e5-core", "mpi.rowMin", rowmin )
Property( "xeon-e5-core", "mpi.rowMax", rowmax )
Property( "xeon-e5-core", "mpi.colMin", colmin )
Property( "xeon-e5-core", "mpi.colMax", colmax )
Property( "xeon-e5-core", "mpi.planeMin", planemin )
Property( "xeon-e5-core", "mpi.planeMax", planemax )
Property( "xeon-e5-core", "mpi.totalrecvs1", recv1 )
Property( "xeon-e5-core", "mpi.totalrecvs2", recv2 )
Property( "xeon-e5-core", "mpi.totalrecvs3", recv3 )
Property( "xeon-e5-core", "mpi.cartesianZminus", Zminus )
Property( "xeon-e5-core", "mpi.cartesianZplus", Zplus )
Property( "xeon-e5-core", "mpi.cartesianYminus", Yminus )
Property( "xeon-e5-core", "mpi.cartesianYplus", Yplus )
Property( "xeon-e5-core", "mpi.cartesianXminus", Xminus )
Property( "xeon-e5-core", "mpi.cartesianXplus", Xplus )
Attribute( "xeon-e5-core", "usage", 0.0 )
Attribute( "xeon-e5-core", "waiting", False )
Relation("xeon-e5-core", "xeon-e5-core", "cpu", "self")
#Operation( "xeon-e5-core", "wait", NoLookup,
 #          Modify( "waiting", True ) )
# Operation( "xeon-e5-core", "unwait", NoLookup,
           # Modify( "waiting", False ))
Operation( "xeon-e5-core", "ComputeA", "HiPGator2-ComputeA.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )
Operation( "xeon-e5-core", "barrier", "HiPGator2-MPIBarrier.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )		   
Operation( "xeon-e5-core", "ComputeB", "HiPGator2-ComputeB.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )
Operation( "xeon-e5-core", "ComputeC", "HiPGator2-ComputeC.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )
Operation( "xeon-e5-core", "ComputeD", "HiPGator2-ComputeD.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )
Operation( "xeon-e5-core", "ComputeE", "HiPGator2-ComputeE.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )
Operation( "xeon-e5-core", "ComputeF", "HiPGator2-ComputeF.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )
Operation( "xeon-e5-core", "ComputeG", "HiPGator2-ComputeG.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )
Operation( "xeon-e5-core", "ComputeH", "HiPGator2-ComputeH.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )
Operation( "xeon-e5-core", "ComputeI", "HiPGator2-ComputeI.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )

Operation( "xeon-e5-core", "wait", NoLookup,
           Modify( "waiting", True ),
           Loiter( "waiting", "==", False ) )

Operation( "xeon-e5-core", "unwait", NoLookup,
           Loiter( "waiting", "==", True ),
           Modify( "waiting", False ))

Ordinal("xeon-e5-core","mpi.commRank")		   
Component( "qpi" )
Attribute( "qpi", "usage", 0.0 )
Operation( "qpi", "transfer", "HiPGator2-TT-hack.csv",
           Loiter( "usage", "==", 0.0 ),
           Modify( "usage", 1.0 ),
           Dawdle( 0.003 ),
           Modify( "usage", 0.0 ) )
Mailbox( "qpi", "transfer", lambda source, target, size, tag: [size],
         OnAll )
Mailbox( "xeon-e5-core", "unwait", lambda source, target, size, tag: [],
         OnTarget )
Component( "BGQ-connection" )
Component( "system" )
Offspring( "system", Tree( ["qpi", "xeon-e5-core"], ["BGQ-connection"],
                           [ 8 ] ) )
Root("system")

