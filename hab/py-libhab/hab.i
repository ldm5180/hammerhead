%module hab
%{
#include "hardware-abstractor.h"
#include "bionet-util.h"
#include "bionet-hab.h"
#include "bionet-node.h"
#include "bionet-resource.h"
#include "bionet-datapoint.h"
#include "bionet-value.h"
%}

%include "hardware-abstractor.h"
%include "bionet-util.h"
%include "bionet-hab.h"
%include "bionet-node.h"
%include "bionet-resource.h"
%include "bionet-datapoint.h"
%include "bionet-value.h"

%include "typemaps.i"
%import "stdint.i"
%import "inttypes.i"

%constant void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value);


%inline %{
void cb_set_resource(bionet_resource_t *resource, 
		     bionet_value_t *value) {
    printf(
        "callback: should set %s:%s to '%s'\n",
        bionet_node_get_id(bionet_resource_get_node(resource)),
        bionet_resource_get_id(resource),
        bionet_value_to_str(value)
    );
}
%}

