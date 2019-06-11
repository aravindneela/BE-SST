# @Author:	Nalini Kumar
# @Date:	09-12-16
# @Brief:	Hardware description file for Vulcan @ LLNL



# Highest level of BE simulation. Root is the "type" (not "name") of component.
# default:system, rack, node, processor
Root( "system" )

# Lowest level of BE simulation. Leaf is the "type" (not "name") of component.
#Leaf( "core" )		# default:core, processor, node



#------------- Define machine hierarchies --------------
# Component( "name", "hierarchy", "type", parameters)

# Containers:
Component( "vulcan", "system" )       
Component( "vulcan.rack" )
Component( "vulcan.midplane" )
Component( "vulcan.nodeboard" )

# Real things:
Component( "vulcan.node", "node", "bgq.node", "32" )
Component( "vulcan.cpu", "processor", "bgq.cpu", "1" )
Component( "vulcan.core", "core", "bgq.core", "16"  )

#Override lookup file defined for "bgq.core" for "computeA"
Operation( "vulcan.core", "computeA", "bgq.core.computeA.csv" )



#------------- Define machine networks -----------------
Component( "vulcan.system.network", "bgq.torus" )	#predefined type bgq.torus
Component( "vulcan.node.network", "pci-x16" )		#predefined type pci-x16
Component( "vulcan.processor.network", "links")

# Override lookup file for "vulcan-pci-x16 for "transfer"
Operation( "vulcan.network", "transfer", "vulcan-pci-x16-transfer.csv")



#------------- Describe the connectivity -----------
Connect( "vulcan.node.network", connect["vulcan.cpu", "vulcan.core" ] )


Offspring( "vulcan.rack", "vulcan.node", "vulcan.node.network")
Offspring( "vulcan.node", "vulcan.processor", "vulcan
