name = sys.argv.pop()

Component( name )

Attribute( name, "usage", 0.0 )

Mailbox( name, "transfer", lambda source, target, size, tag: [size],
         OnAll )

