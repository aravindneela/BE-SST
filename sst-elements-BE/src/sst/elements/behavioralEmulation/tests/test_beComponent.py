# Automatically generated SST Python input
import sst
from simulator_build import Simulator

# Define SST core options
sst.setProgramOption("timebase", "1 ps")
#sst.setProgramOption("stopAtCycle", "25us")

# Obtain the simulation flags
output_file = ""
interpolation = ""
debug = ""
eventtrace = ""

try:
    execfile("flags.py")
except:
    print "Error with file flags.py"
    system_config = "Config file error"

sim = Simulator(system_config)

sim.build()

system_gids = sim.gids
system_kinds = sim.layout.kinds
system_edges = sim.layout.edges
system_cids = sim.layout.cids 
system_rordinals = sim.layout.rordinals
system_relations = sim.layout.relations
system_parents = sim.layout.parents
system_components = sim.designer.components
system_programs = sim.programs
system_operations = sim.operationDetails
system_properties = sim.designer.properties
system_tallies = sim.layout.tallies
system_indices = sim.layout.indices
system_mailboxes = sim.mailboxes
system_attributes = sim.attributes
system_netsizes = sim.layout.netsizes
system_netnames = sim.layout.netnames
 
root = sim.designer.system

layout = ""
"""
i=0
with open('layoutfile', 'r') as fin:
    for line in fin:
        i=i+1
        layout = layout+line
"""

# Define the simulation flags
flags = {"interpolation": interpolation, "output_file": output_file, "debug": debug, "eventtrace": eventtrace}

# Define the simulation components
system_comps = []

for gid in system_gids:

    comp = sst.Component(str(gid), "behavioralEmulation.beComponent")
    system_comps.append(comp)


ic_links = {}
other_links = {}
root_links = []
other_relations = {}
component_links = {}
operations = {}
relations = {}
clock = 1000000

cores = 0
links = 0
assists = 0
containers = 0


# Define the simulation links
for gid in system_gids:

    parent = system_parents[gid]
    operations[gid] = {}
    relations[gid] = {}

    if system_kinds[gid] in system_operations: operations[gid] = system_operations[system_kinds[gid]]

    if gid in system_relations:

        relations[gid] = system_relations[gid]

        for component in system_relations[gid]:
            component_id = system_relations[gid][component]
            if component_id != gid:
                link = sst.Link("link"+str(gid)+' '+str(component_id))
                system_comps[gid].addLink(link, "Link "+str(component_id), "1ps")
                system_comps[component_id].addLink(link, "Link "+str(gid), "1ps")
                if gid in component_links: component_links[gid].append(component_id)
                else: component_links.update({gid: [component_id]})
                if component_id in other_links: other_links[component_id].append(gid)
                else: other_links.update({component_id: [gid]})


    for edge in system_edges[gid]:
        if edge:
            ic_gid = edge[-1]
            link = sst.Link("link"+str(gid)+' '+str(ic_gid))
            system_comps[gid].addLink(link, "Link "+str(ic_gid), "1ps")
            system_comps[ic_gid].addLink(link, "Link "+str(gid), "1ps")
            if gid in component_links: component_links[gid].append(ic_gid)
            else: component_links.update({gid: [ic_gid]})
            if ic_gid in ic_links: ic_links[ic_gid].append(gid)
            else: ic_links.update({ic_gid: [gid]})

    
    if(gid in component_links and parent != "null" and parent != 0):
        link = sst.Link("link"+str(gid)+' '+str(parent))
        system_comps[gid].addLink(link, "Link "+str(parent), "1ps")
        system_comps[parent].addLink(link, "Link "+str(gid), "1ps")
        component_links[gid].append(parent)
        component_links[parent].append(gid)


def neighbourPos(dim, so, ne):

    fac = 1
    i = 0

    for co in dim[::-1]:
        fac = fac*co
        if abs(ne-so) < fac: return i
        i = i+1

    return -1

def wrapPos(dim, so, ne):

    i = 0
    wrap = 1

    for co in dim[::-1]:
        if (abs(ne - so) < (co-1)*wrap): return(0, -1)
        elif (ne - so == (co-1)*wrap): return (-1, i)
        elif (so - ne == (co-1)*wrap): return (1, i)
        i = i+1
        wrap = wrap*co

    return (0, -1)

def TorusNeighbours(neighbours, dim, cid):

    plus_links = [-1]*len(dim)
    minus_links = [-1]*len(dim)

    for ne in neighbours:

        ncid = system_cids[ne[0]]
        wpos = wrapPos(dim, cid, ncid)

        if wpos[0] < 0 and minus_links[wpos[1]] == -1:
            minus_links[wpos[1]] = ne[1]
           
        elif ncid > cid: plus_links[neighbourPos(dim, cid, ncid)] = ne[1]

        elif wpos[0] > 0 and plus_links[wpos[1]] == -1:
            plus_links[wpos[1]] = ne[1]

        elif ncid < cid: minus_links[neighbourPos(dim, cid, ncid)] = ne[1]

    return (plus_links, minus_links)

def MeshNeighbours(neighbours, dim, cid):

    plus_links = [-1]*len(dim)
    minus_links = [-1]*len(dim)

    for ne in neighbours:
           
        ncid = system_cids[ne[0]]

        if ncid > cid: plus_links[neighbourPos(dim, cid, ncid)] = ne[1]

        elif ncid < cid: minus_links[neighbourPos(dim, cid, ncid)] = ne[1]

    return (plus_links, minus_links)

#def TreeNeighbours(neighbours, dim, self_gid):

    
    


# Define the simulation parameters
for gid in system_gids:

    properties = {}
    attributes = []
    config_mailbox = []
    simulation_mailbox = []
    
    index =  (system_indices[gid]
              if gid in system_indices else None)
    
    for property_name in system_properties[system_kinds[gid]]:
        properties[property_name] = system_properties[system_kinds[gid]][property_name](gid, system_cids[gid], system_tallies[system_kinds[gid]], index)

    if system_kinds[gid] in system_attributes: attributes = system_attributes[system_kinds[gid]]  

    if system_kinds[gid] in system_mailboxes: config_mailbox = system_mailboxes[system_kinds[gid]]

    for entry in config_mailbox:
        simulation_mailbox.append((entry[0], entry[1]("source", "target", "size", "tag"), entry[2]))


    if system_kinds[gid] == root:
        pass

    elif gid not in component_links and gid in ic_links:

        links = links+1

        system_comps[gid].addParams({
          "System Layout"	 : """none""",
          "Component clock"	 : clock,
          "Flags"		 : flags,
          "Component kind"	 : system_kinds[gid],
          "Component gid"	 : gid,
          "Component cid"        : system_cids[gid],
          "Component ordinal"	 : """-1""",
          "Attributes"		 : str(attributes),
          "Operations"		 : str(operations[gid]),
          "Relations"		 : str(relations[gid]),
          "Properties"		 : str(properties),
          "Mailboxes"		 : str(simulation_mailbox),
          "num_links"		 : str(len(ic_links[gid])),
          "Link list"		 : str(ic_links[gid]),
          "Topology"             : str(system_netnames[gid]),
          "Container dimensions" : str(system_netsizes[gid]),
          "Plus links"           : """[]""",
          "Minus links"          : """[]""",
          "Parent"               : """-1""",
          "Children"             : """[]""",
          "Software Program"	 : """none"""
        })
        #print 'Links for ', gid, ': ', str(ic_links[gid])

    elif gid not in component_links:   # other_links will throw key error if relations are not defined

        assists = assists+1

        system_comps[gid].addParams({
          "System Layout"	 : """none""",
          "Component clock"	 : clock,
          "Flags"		 : flags,
          "Component kind"	 : system_kinds[gid],
          "Component gid"	 : gid,
          "Component cid"        : system_cids[gid],
          "Component ordinal"	 : """-1""",
          "Attributes"		 : str(attributes),
          "Operations"		 : str(operations[gid]),
          "Relations"		 : str(relations[gid]),
          "Properties"		 : str(properties),
          "Mailboxes"		 : str(simulation_mailbox),
          "num_links"		 : str(len(other_links[gid])),
          "Link list"		 : str(other_links[gid]),
          "Topology"             : """""",
          "Container dimensions" : """[]""",
          "Plus links"           : """[]""",
          "Minus links"          : """[]""",
          "Parent"               : """-1""",
          "Children"             : """[]""",
          "Software Program" 	 : """none"""
        })

    elif system_kinds[gid] not in system_programs:

        containers = containers + 1

        # This loop is used to construct children info for each container. Component_links contains list of gids connected by sst links. Once we reach the first gid in the list that is one greater
        # than the component gid, everything that follows is the gid of the container child. The list is constructed that way. Might not be a foolproof method!  
        i = 0
        c_children = []
        for link in component_links[gid]:
 
            if link == gid + 1: 
                c_children = component_links[gid][i:]
                break
           
            i = i+1

        topo = system_netnames[gid]
        dim = system_netsizes[gid]
        cid = system_cids[gid]

        neighbours = sorted(system_edges[gid], key=lambda x: x[0])

        if topo == "mesh": link_tuple = MeshNeighbours(neighbours, dim, cid)
        elif topo == "torus": link_tuple = TorusNeighbours(neighbours, dim, cid)
        #elif topo == "tree": link_tuple = TreeNeighbours(neighbours, dim, gid)  

        plus_links = link_tuple[0]
        minus_links = link_tuple[1]

        system_comps[gid].addParams({
          "System Layout"	 : """none""",
          "Component clock"	 : clock,
          "Flags"		 : flags,
          "Component kind"	 : system_kinds[gid],
          "Component cid"        : cid,
          "Component gid"	 : gid,
          "Component ordinal"	 : """-2""",
          "Attributes"		 : str(attributes),
          "Operations"		 : str(operations[gid]),
          "Relations"		 : str(relations[gid]),
          "Properties"		 : str(properties),
          "Mailboxes"		 : str(simulation_mailbox),
          "num_links"		 : str(len(component_links[gid])),
          "Link list"		 : str(component_links[gid]),
          "Topology"             : str(topo),
          "Container dimensions" : str(dim),
          "Plus links"           : str(plus_links[::-1]),
          "Minus links"          : str(minus_links[::-1]),
          "Parent"               : system_parents[gid],
          "Children"             : str(c_children),
          "Software Program"	 : """none"""
        })
   
    else:

        cores = cores+1

        topo = system_netnames[gid]
        dim = system_netsizes[gid]
        cid = system_cids[gid]

        neighbours = sorted(system_edges[gid], key=lambda x: x[0])

        if topo == "mesh": link_tuple = MeshNeighbours(neighbours, dim, cid)
        elif topo == "torus": link_tuple = TorusNeighbours(neighbours, dim, cid)
        #elif topo == "tree": link_tuple = TreeNeighbours(neighbours, dim, gid)  

        plus_links = link_tuple[0]
        minus_links = link_tuple[1]

        link = sst.Link("link"+str(gid)+" 0")
        system_comps[gid].addLink(link, "Link 0", "1ps")
        component_links[gid].append(0)

        system_comps[gid].addParams({
          "System Layout"	 : """none""",
          "Component clock"	 : clock,
          "Flags"		 : flags,
          "Component kind"	 : system_kinds[gid],
          "Component cid"        : cid,
          "Component gid"	 : gid,
          "Component ordinal"	 : system_rordinals[gid],
          "Attributes"		 : str(attributes),
          "Operations"		 : str(operations[gid]),
          "Relations"		 : str(relations[gid]),
          "Properties"		 : str(properties),
          "Mailboxes"		 : str(simulation_mailbox),
          "num_links"		 : str(len(component_links[gid])),
          "Link list"		 : str(component_links[gid]),
          "Topology"             : str(topo),
          "Container dimensions" : str(dim),
          "Plus links"           : str(plus_links[::-1]),
          "Minus links"          : str(minus_links[::-1]),
          "Parent"               : system_parents[gid],
          "Children"             : """[]""",
          "Software Program"	 : "Software/"+system_programs[system_kinds[gid]]
        })
        system_comps[0].addLink(link, "Link "+str(gid), "1ps")
        root_links.append(gid)
        #print 'Links for ', gid, ': ', str(component_links)


system_comps[0].addParams({
  "System Layout"	: layout,
  "Component clock"	: clock,
  "Flags"		: flags,
  "Component kind"	: system_kinds[0],
  "Component gid"	: """0""",
  "Component cid"       : """0""",
  "Component ordinal"	: """-1""",
  "Attributes"		: """[]""",
  "Operations"		: """{}""",
  "Relations"		: """{}""",
  "Properties"		: """{}""",
  "Mailboxes"		: """[]""",
  "num_links"		: str(len(root_links)),
  "Link list"		: str(root_links),
  "Topology"             : """""",
  "Container dimensions" : """[]""",
  "Plus links"           : """[]""",
  "Minus links"          : """[]""",
  "Parent"               : """-1""",
  "Children"             : """[]""",
  "Software Program"	 : """none"""
})    

print "The number of components are:- \n", "1) Cores: ", cores, "\n2) Links: ", links, "\n3) Containers: ", containers, "\n4) assists: ", assists

#link1 = sst.Link("link1")
#comp1.addLink(link1, "Link 1", "1ps")
#comp3.addLink(link1, "Link 1", "1ps")
#link1.connect( (comp1, "Link 1", "1ps"), (comp3, "Link 1", "1ps") )

#link2 = sst.Link("link2")
#comp2.addLink(link2, "Link 1", "1ps")
#comp3.addLink(link2, "Link 2", "1ps")
#link2.connect( (comp2, "Link 1", "1ps"), (comp3, "Link 2", "1ps") )


# End of generated output.
