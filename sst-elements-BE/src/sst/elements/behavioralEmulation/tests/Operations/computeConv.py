name = sys.argv.pop()

Operation( name, "computeConv", "vulcan-compute-conv.csv", "linear",
           Loiter( "usage", "==", 0.0),				
           Modify( "usage", 1.0 ),
           Dawdle( Outputs(0) ),
           Modify( "usage", 0.0 ) )
