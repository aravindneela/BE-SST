# Vulcan system with 3 level config hierarchy

# ----------------------------- CPU Core Setup ------------------------------ #

ComponentLoad( "vulcan.core", "Core", "BGQ-core" )
Program( "vulcan.core", "cmt-bone-be.txt" )

# ---------------------------- Ethernet Setup ------------------------------- #

ComponentLoad( "vulcan.system.network", "Network", "BGQ-network" )

# ------------------------------ System Setup ------------------------------- #

Component( "system") #Component will prevent hierarchical lookup
Offspring( "system", Torus("vulcan.core", "vulcan.system.network", [2, 2, 2, 2, 2]) )

Root("system")

# ------------------------------ Operations specifics ------------------------------- #

OperationLoad( "vulcan.core", "computeConv", "computedr", "computeds", "wait", "unwait")

OperationLookupOvr("vulcan.core", "computedr", "vulcan-compute-dr.csv") #Lookup file override optional

OperationInterpOvr("vulcan.core", "computedr", "polynomial-4") #Interpolation scheme override optional

OperationLoad( "vulcan.system.network", "transfer")
