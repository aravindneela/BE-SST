# Example Details:
#   Nodes connected by a Torus. Each node has a memory and four CPUs. Each
#   CPU has four cores connected by a hypothetical "on-chip connection".

# ----------------------------- CPU Core Setup ------------------------------ #

ComponentN( "toddler.core", "Core", "xeon-e5-core" )
Program( "toddler.core", "toddler.smc" )

# A 'ddr4-32g' is 'aunt' and 'memory' (parent's sibling) of 'xeon-e5-core'
Relation( "toddler.core", "toddler.memory", "memory", "aunt" ) #Relation( "toddler.core", "toddler.memory", "aunt" )

# ----------------------- On-Chip Connection Setup -------------------------- #

ComponentN( "toddler.cpu.network", "occ")

# ------------------------------- CPU Setup --------------------------------- #

ComponentN( "toddler.cpu", "none" )

# Four cores per CPU.
Offspring( "toddler.cpu", Mesh("toddler.core", "toddler.cpu.network", [2, 2] ) )

# ----------------------------- Memory Setup -------------------------------- #

ComponentN( "toddler.memory", "ddr4-32g")

# ------------------------------- QPI Setup --------------------------------- #

ComponentN( "toddler.node.network", "qpi" )


# ---------------------------- Ethernet Setup ------------------------------- #

ComponentN( "toddler.system.network", "eth" )

# ------------------------------- Node Setup -------------------------------- #

ComponentN( "toddler.node", "none" )
Offspring( "toddler.node", Mesh("toddler.cpu", "toddler.node.network", [2, 2]) )
Offspring( "toddler.node", Single("toddler.memory") )

# ------------------------------ System Setup ------------------------------- #

ComponentN( "system", "none" )
Offspring( "system", Torus("toddler.node", "toddler.system.network", [4, 4]) ) # 2D-Torus.

Root("system")
