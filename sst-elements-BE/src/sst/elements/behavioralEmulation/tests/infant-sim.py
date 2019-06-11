# Example Details:
#   Two cores, one sender, one reciever, connected by QPI (a link).

# Make a new component and give it a program.
Component( "xeon-e5-core" )
Program( "xeon-e5-core", "infant.smc" )

# Assign attributes (hardware state things) to the component.
Attribute("xeon-e5-core", "usage", 0.0)
Attribute("xeon-e5-core", "waiting", False)

# "xeon-e5-core" is "self" of "xeon-e5-core" and is called its "cpu".
Relation( "xeon-e5-core", "xeon-e5-core", "cpu", "self" )

# A "property" of a component is sort of like a software-accessable attribute.
# In this case, "mpi.commRank", is equal to the CID (Component IDentifier).
# It could also be a function of other things, but don't mind those for now.
Property( "xeon-e5-core", "mpi.commRank", lambda gid, cid, cids, index: cid )

# An "ordinal" is something that must be assigned to anything which is
# the endpoint of a route. Because we want cores to talk to each other, they
# get an ordinal. The ordinal is one of the properties, and you might think
# of it as being a unique "routing number", or something to that effect.
Ordinal( "xeon-e5-core", "mpi.commRank" )

# Define some operations for the core:

Operation( "xeon-e5-core", "wait", NoLookup,
           Modify( "waiting", True ),
           Loiter( "waiting", "==", False ) )

Operation( "xeon-e5-core", "unwait", NoLookup,
           Loiter( "waiting", "==", True ),
           Modify( "waiting", False ) )

Operation( "xeon-e5-core", "fft", "fft-dummy.csv",
           Loiter( "usage", "==", 0.0),
           Modify( "usage", 1.0 ),
           Dawdle( 0.008 ),  # This is how we do Monte Carlo simulation.
           Modify( "usage", 0.0 ) )

# A Mailbox, or perhaps "Message Function Hook", is a function which gets
# triggered when a message passes through a component. Not every component
# needs a mailbox. Here, when a message reaches its destination: it calls
# unwait on it, with no parameters, if the target is a "xeon-e5-core".
Mailbox( "xeon-e5-core", "unwait", lambda source, target, size, tag: [],
         OnTarget )


# Here's an interconnect component. Nothing special yet.
Component( "qpi" )
Attribute( "qpi", "usage", 0.0 )
Operation( "qpi", "transfer", "transfer-dummy-A.csv",
           Loiter( "usage", "==", 0.0 ),
           Modify( "usage", 1.0 ),
           Dawdle( 0.005 ),
           Modify( "usage", 0.0 ) )

# When a message passses though a "qpi", "transfer" gets triggered,
# with the size of the message as an operand.
Mailbox( "qpi", "transfer", lambda source, target, size, tag: [size], OnAll )

# Make a top level component.
Component( "node" )

# Make a Core - QPI - Core network. (3 sub-components total).
Offspring( "node", Mesh("xeon-e5-core", "qpi", [2]) )

# Let the simulator know to start construction here, and work our way down.
Root( "node" )
