#!/bin/bash

#bone-be parameters
#TIMESTEP=100
#ELEMENT_SIZE="5 9 13 17 21"
#EPP="2,2,2;4,4,2;4,4,4;8,4,4;8,8,4"
#CART="4,2,2;8,8,4;16,16,8;32,32,16;64,64,32"
#PHY_PARAM=5
#------------------------#

#5D Torus Diminesion for every CART value
#***Make sure that x0 = 2 always if 5D torus is {x4,x3,x2,x1,x0}***
#TORUS_5D="1,2,2,2,2;4,4,4,2,2;8,8,4,4,2;16,8,8,8,2;16,16,16,16,2"

#Test case!
TIMESTEP=1
ELEMENT_SIZE="5 6 9" # 13 16" #"8 9 10 11 12"
EPP="4,4,4" #"2,2,2;4,4,2;4,4,4;8,8,4"
CART="128,64,64" #"64,64,32" #"16,16,8;32,32,16"
TORUS_5D="32,32,16,16,2" #"16,16,16,16,2" #"8,8,4,4,2;16,8,8,8,2"
PHY_PARAM=5
#--------------------------#

#Creating array stack for EPP, CART and 5D_TRUS
TORUS_stack=($(echo $TORUS_5D | tr ";" "\n"))
CART_stack=($(echo $CART | tr ";" "\n"))
epp_stack=$(echo $EPP | tr ";" "\n")
#---------------------------#

# Wrapper to check cart and torus match in number and value
num_of_torus=${#TORUS_stack[*]}    #number of 5D torus points (per experimental setup)
num_of_cart=${#CART_stack[*]}      #number of cartesian points (per experimental setup)
eight=8

if [ $num_of_cart -ne $num_of_torus ]; then
  echo "x---x---x---x---x---x---x---x---x---x---x---x---x---x---x---x---x---x"
  echo "Number of Cartesian points=$num_of_cart & 5D torus points=$num_of_torus [>Do not match<]"
  echo "Make sure: number of cartesian points = number of 5D torus points"
  echo "~  !!* * * EXITTING * * *!! ~"
  echo "x---x---x---x---x---x---x---x---x---x---x---x---x---x---x---x---x---x"
  exit 0
fi

for index in ${!TORUS_stack[*]}
do
  x0=$(echo ${TORUS_stack[$index]} | tr "," " " | awk '{print $5}')
  x1=$(echo ${TORUS_stack[$index]} | tr "," " " | awk '{print $4}')
  x2=$(echo ${TORUS_stack[$index]} | tr "," " " | awk '{print $3}')
  x3=$(echo ${TORUS_stack[$index]} | tr "," " " | awk '{print $2}')
  x4=$(echo ${TORUS_stack[$index]} | tr "," " " | awk '{print $1}')
  NP=$((x0*x1*x2*x3*x4))

  CART_X=$(echo ${CART_stack[$index]} | tr "," " " | awk '{print $1}')
  CART_Y=$(echo ${CART_stack[$index]} | tr "," " " | awk '{print $2}')
  CART_Z=$(echo ${CART_stack[$index]} | tr "," " " | awk '{print $3}')
  CART_xyz=$((CART_X*CART_Y*CART_Z))
  if [ $NP -ne $CART_xyz ]; then
    one=1
    position=$((index+one))
    echo "x---x---x---x---x---x---x---x---x---x---x---x---x---x---x---x---x---x---x"
    echo "Cartesian points and 5D torus point do not match at entry $position; Check again!"
    echo "~ !!* * * EXITTING * * *!! ~"
    echo "x---x---x---x---x---x---x---x---x---x---x---x---x---x---x---x---x---x---x"
    exit 0
  fi
done
#---------------- End of wrapper ----------------#

#generating vulcan-proper.py file
for index in ${!TORUS_stack[*]}
do
  #Vulcan : 5D Torus Diminesions
  x0=$(echo ${TORUS_stack[$index]} | tr "," " " | awk '{print $5}')
  x1=$(echo ${TORUS_stack[$index]} | tr "," " " | awk '{print $4}')
  x2=$(echo ${TORUS_stack[$index]} | tr "," " " | awk '{print $3}')
  x3=$(echo ${TORUS_stack[$index]} | tr "," " " | awk '{print $2}')
  x4=$(echo ${TORUS_stack[$index]} | tr "," " " | awk '{print $1}')
  NP=$((x0*x1*x2*x3*x4))

  #bone-BE Cartesian coordinate
  CART_X=$(echo ${CART_stack[$index]} | tr "," " " | awk '{print $1}')
  CART_Y=$(echo ${CART_stack[$index]} | tr "," " " | awk '{print $2}')
  CART_Z=$(echo ${CART_stack[$index]} | tr "," " " | awk '{print $3}')

  for EC in $epp_stack
  do
    #Calculating EL_X, EL_Y,EL_Z from epp_stack
    EL_X=$(echo $EC | tr "," " " | awk '{print $1}')
    EL_Y=$(echo $EC | tr "," " " | awk '{print $2}')
    EL_Z=$(echo $EC | tr "," " " | awk '{print $3}')
    Exyz=$((EL_X*EL_Y*EL_Z))

    #Calculating ELEMENTS ON FACE (EOF) for X, Y, Z axis
    EOF_X=$((EL_Y*EL_Z))
    EOF_Y=$((EL_X*EL_Z))
    EOF_Z=$((EL_X*EL_Y))

    for ES in $ELEMENT_SIZE
    do
      FS=$((ES*ES))   #FACE_SIZE
      TRANSFER_SIZE_X=$((FS*PHY_PARAM*EOF_X*eight)) #Transfer size in bytes
      TRANSFER_SIZE_Y=$((FS*PHY_PARAM*EOF_Y*eight))
      TRANSFER_SIZE_Z=$((FS*PHY_PARAM*EOF_Z*eight))
      echo "from cartesianrank import CartesianGrid" > vul-prop.py
      echo "" >> vul-prop.py
      echo "global cartesianData" >> vul-prop.py
      echo "" >> vul-prop.py
      echo "cartesianData = CartesianGrid($CART_X, $CART_Y, $CART_Z)" >> vul-prop.py
      echo "" >> vul-prop.py
      echo 'Component( "BGQ-core" )' >> vul-prop.py
      echo 'Program( "BGQ-core", "cmt-bone-be.txt" )' >> vul-prop.py
      echo "" >> vul-prop.py
      echo 'Property( "BGQ-core", "app.elementSize", lambda gid, cid, cids, index: '$ES' )' >> vul-prop.py
      echo 'Property( "BGQ-core", "app.elementsPerProcess", lambda gid, cid, cids, index: '$Exyz' )' >> vul-prop.py
      echo 'Property( "BGQ-core", "app.transferSizeX", lambda gid, cid, cids, index: '$TRANSFER_SIZE_X' )' >> vul-prop.py
      echo 'Property( "BGQ-core", "app.transferSizeY", lambda gid, cid, cids, index: '$TRANSFER_SIZE_Y' )' >> vul-prop.py
      echo 'Property( "BGQ-core", "app.transferSizeZ", lambda gid, cid, cids, index: '$TRANSFER_SIZE_Z' )' >> vul-prop.py
      echo 'Property( "BGQ-core", "app.timesteps", lambda gid, cid, cids, index: '$TIMESTEP' )' >> vul-prop.py
      echo 'Property( "BGQ-core", "app.phyParam", lambda gid, cid, cids, index: '$PHY_PARAM' )' >> vul-prop.py
      echo "" >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.commRank", lambda gid, cid, cids, index: cid  )' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.commSize", lambda gid, cid, cids, index: cids )' >> vul-prop.py
      echo 'def cartX(g, rank, c, i): return cartesianData.myCoordinates(rank, "X")' >> vul-prop.py
      echo 'def cartY(g, rank, c, i): return cartesianData.myCoordinates(rank, "Y")' >> vul-prop.py
      echo 'def cartZ(g, rank, c, i): return cartesianData.myCoordinates(rank, "Z")' >> vul-prop.py
      echo 'def cartXp(g, rank, c, i): return cartesianData.neighbourRank(rank, "Xplus")' >> vul-prop.py
      echo 'def cartHasXp(g, rank, c, i): return cartesianData.neighbourRank(rank, "Xplus") >= 0' >> vul-prop.py
      echo 'def cartYp(g, rank, c, i): return cartesianData.neighbourRank(rank, "Yplus")' >> vul-prop.py
      echo 'def cartHasYp(g, rank, c, i): return cartesianData.neighbourRank(rank, "Yplus") >= 0' >> vul-prop.py
      echo 'def cartZp(g, rank, c, i): return cartesianData.neighbourRank(rank, "Zplus")' >> vul-prop.py
      echo 'def cartHasZp(g, rank, c, i): return cartesianData.neighbourRank(rank, "Zplus") >= 0' >> vul-prop.py
      echo 'def cartXm(g, rank, c, i): return cartesianData.neighbourRank(rank, "Xminus")' >> vul-prop.py
      echo 'def cartHasXm(g, rank, c, i): return cartesianData.neighbourRank(rank, "Xminus") >= 0' >> vul-prop.py
      echo 'def cartYm(g, rank, c, i): return cartesianData.neighbourRank(rank, "Yminus")' >> vul-prop.py
      echo 'def cartHasYm(g, rank, c, i): return cartesianData.neighbourRank(rank, "Yminus") >= 0' >> vul-prop.py
      echo 'def cartZm(g, rank, c, i): return cartesianData.neighbourRank(rank, "Zminus")' >> vul-prop.py
      echo 'def cartHasZm(g, rank, c, i): return cartesianData.neighbourRank(rank, "Zminus") >= 0' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.cartesianX", cartX )' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.cartesianY", cartY )' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.cartesianZ", cartZ )' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.cartesianXplus", cartXp )' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.cartesianHasXplus", cartHasXp )' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.cartesianYplus", cartYp )' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.cartesianHasYplus", cartHasYp )' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.cartesianZplus", cartZp )' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.cartesianHasZplus", cartHasZp )' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.cartesianXminus", cartXm )' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.cartesianHasXminus", cartHasXm )' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.cartesianYminus", cartYm )' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.cartesianHasYminus", cartHasYm )' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.cartesianZminus", cartZm )' >> vul-prop.py
      echo 'Property( "BGQ-core", "mpi.cartesianHasZminus", cartHasZm )' >> vul-prop.py
      echo 'Ordinal( "BGQ-core", "mpi.commRank" )' >> vul-prop.py
      echo 'Relation( "BGQ-core", "BGQ-core", "cpu", "self" )' >> vul-prop.py
      echo 'Attribute( "BGQ-core", "usage", 0.0 )' >> vul-prop.py
      echo " " >> vul-prop.py
      echo 'Operation( "BGQ-core", "wait", NoLookup, None,
           RecvWait(True))' >> vul-prop.py
      echo 'Operation( "BGQ-core", "unwait", NoLookup, None,
           Recv(True))' >> vul-prop.py
      echo 'Operation( "BGQ-core", "computeConv", "vulcan-compute-conv.csv", "polynomial-3",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( AnyOutput() ),
           Modify( "usage", 0.0 ) )' >> vul-prop.py
      echo 'Operation( "BGQ-core", "computedr", "vulcan-compute-dr.csv", "polynomial-4",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( AnyOutput() ),
           Modify( "usage", 0.0 ) )' >> vul-prop.py
      echo 'Operation( "BGQ-core", "computeds", "vulcan-compute-ds.csv", "polynomial-4",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( AnyOutput() ),
           Modify( "usage", 0.0 ) )' >> vul-prop.py
      echo 'Operation( "BGQ-core", "computedt", "vulcan-compute-dt.csv", "polynomial-4",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( AnyOutput() ),
           Modify( "usage", 0.0 ) )' >> vul-prop.py
      echo 'Operation( "BGQ-core", "computeSum", "vulcan-compute-sum.csv", "polynomial-3",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( AnyOutput() ),
           Modify( "usage", 0.0 ) )' >> vul-prop.py
      echo 'Operation( "BGQ-core", "computerk", "vulcan-compute-rk.csv", "polynomial-3",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( AnyOutput() ),
           Modify( "usage", 0.0 ) )' >> vul-prop.py
      echo 'Operation( "BGQ-core", "comminit", "vulcan-compute-comminit.csv", "linear",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( AnyOutput() ),
           Modify( "usage", 0.0 ) )' >> vul-prop.py
      echo 'Operation( "BGQ-core", "comminitaxis", "vulcan-compute-comminitaxis.csv", "linear",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( AnyOutput() ),
           Modify( "usage", 0.0 ) )' >> vul-prop.py
      echo 'Operation( "BGQ-core", "prepareFaces", "vulcan-compute-prepface.csv", "linear",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( AnyOutput() ),
           Modify( "usage", 0.0 ) )' >> vul-prop.py
      echo 'Operation( "BGQ-core", "cleanFaces", "vulcan-compute-cleanface.csv", "linear",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( AnyOutput() ),
           Modify( "usage", 0.0 ) )' >> vul-prop.py
      echo 'Mailbox( "BGQ-core", "unwait", lambda source, target, size, tag: [source],
         OnTarget )' >> vul-prop.py
      echo 'Component( "BGQ-network" )' >> vul-prop.py
      echo 'Attribute( "BGQ-network", "usage", 0.0 )' >> vul-prop.py
      echo 'Operation( "BGQ-network", "transfer", "vulcan-transfer.csv", "linear",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( AnyOutput() ), 
           Modify( "usage", 0.0 ) )' >> vul-prop.py
      echo 'Mailbox( "BGQ-network", "transfer", lambda source, target, size, tag: [size],
         OnAll )' >> vul-prop.py
      echo 'Component( "BGQ-connection" )' >> vul-prop.py
      echo 'Component( "system" )' >> vul-prop.py
      echo 'Offspring( "system", Torus( "BGQ-core", "BGQ-network", [ '$x4', '$x3', '$x2', '$x1', '$x0' ] ) )' >> vul-prop.py
      echo 'Root("system")' >> vul-prop.py
      mv vul-prop.py vulcan-proper'-es'$ES'ec'$Exyz'np'$NP'.py'
    done
  done
done
                     
