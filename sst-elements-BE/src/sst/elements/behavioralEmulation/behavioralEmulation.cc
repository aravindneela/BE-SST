
#include "sst_config.h"

#include "sst/core/element.h"
#include "BE_Component.h"

using namespace SST;
using namespace SST::BEComponent;


static Component* create_beComponent(SST::ComponentId_t id, SST::Params& params) 
{
    return new beComponent(id, params);
}

static const ElementInfoParam beComponent_params[] = {
    { "System Layout", "random", NULL},
    { "Flags", "list of flags enabled along with arguments", NULL},
    { "Component clock", "random", 0},
    { "Component kind", "random", NULL},
    { "Component gid", "random", "0"},
    { "Component cid", "random", "0"},
    { "Component ordinal", "random", "0"},
    { "Attributes", "List of attributes for that commponent", NULL},
    { "Operations", "List of operations and their respective lookup files", NULL},
    { "Relations", "List of relations to other components", NULL},
    { "Properties", "List of prperties for each component", NULL},
    { "Mailboxes", "List of message reception operations for each component", NULL},
    { "num_links", "Number of links to other components", "0"},
    { "Link list", "List of gids of linked components", NULL},
    { "Topology", "Network topology of the container object", NULL},
    { "Container dimensions", "Network dimensions", NULL},
    { "Plus links", "List of neighbours", NULL},
    { "Minus links", "List of neighbours", NULL},
    { "Parent", "hierarchical parent of component", "-1"},
    { "Children", "hierarchical children of component", NULL},
    { "Software Program", "Software Program for the component if any", NULL},
    { NULL, NULL, NULL}
};


static const char* beComponent_port_events[] = { "beComponent.beCommEvent", NULL };

/*static const ElementInfoPort beComponent_ports[] = {
    {"Link 0", "Link to the first component", beComponent_port_events},
    {"Link 1", "Link to the second component", beComponent_port_events},
    { NULL, NULL, NULL }
};*/


static const ElementInfoPort beComponent_ports[] = {
    {"Link %(num_links)d",  "Links which connect to other components.", beComponent_port_events},
    {NULL, NULL, NULL}
};


static const ElementInfoComponent behavioralEmulationComponents[] = {

    { "beComponent",                                 // Name
      "Simulated Component",                         // Description
      NULL,                                          // PrintHelp
      create_beComponent,                            // Allocator
      beComponent_params,                            // Parameters
      beComponent_ports,                             // Ports
      COMPONENT_CATEGORY_UNCATEGORIZED,              // Category
      NULL                                           // Statistics
    },

    { NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL}

};

extern "C" {
    ElementLibraryInfo behavioralEmulation_eli = {
        "behavioralEmulation",                           // Name
        "Behavioral Emulation for abstract components",  // Description
        behavioralEmulationComponents,                   // Components
        NULL,                                            // Events 
        NULL,                                            // Introspectors 
        NULL,                                            // Modules 
        NULL,                                            // Subcomponents 
        NULL,                                            // Partitioners
        NULL,                                            // Python Module Generator
        NULL                                             // Generators
    };
}


