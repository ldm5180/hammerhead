%module hab
%{
#include "hardware-abstractor.h"
#include "bionet-util.h"
#include "bionet-hab.h"
#include "bionet-node.h"
#include "bionet-resource.h"
%}

%include "hardware-abstractor.h"
%include "bionet-util.h"
%include "bionet-hab.h"
%include "bionet-node.h"
%include "bionet-resource.h"

%constant void cb_set_resource(bionet_resource_t *resource, const bionet_datapoint_value_t *value);


%inline %{
void cb_set_resource(bionet_resource_t *resource, 
     const bionet_datapoint_value_t *value) {
    printf(
        "callback: should set %s:%s to '%s'\n",
        resource->node->id,
        resource->id,
        bionet_datapoint_value_to_string_isolated(resource->data_type, value)
    );
}
%}

