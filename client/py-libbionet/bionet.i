%module bionet
%{
#include "bionet.h"
#include "bionet-util.h"
%}

%include "bionet.h"
%include "bionet-util.h"


%constant void cb_datapoint(bionet_datapoint_t *datapoint);
%constant void cb_lost_node(bionet_node_t *node);
%constant void cb_new_node(bionet_node_t *node);
%constant void cb_new_hab(bionet_hab_t *hab);
%constant void cb_lost_hab(bionet_hab_t *hab);


%inline %{
void cb_datapoint(bionet_datapoint_t *datapoint) {
    g_log(
        "",
        G_LOG_LEVEL_INFO,
        "%s.%s.%s:%s = %s %s %s @ %s",
        datapoint->resource->node->hab->type,
        datapoint->resource->node->hab->id,
        datapoint->resource->node->id,
        datapoint->resource->id,
        bionet_resource_data_type_to_string(datapoint->resource->data_type),
        bionet_resource_flavor_to_string(datapoint->resource->flavor),
        bionet_datapoint_value_to_string(datapoint),
        bionet_datapoint_timestamp_to_string(datapoint)
    );
}


 void cb_lost_node(bionet_node_t *node) {
    g_log("", G_LOG_LEVEL_INFO, "lost node: %s.%s.%s", node->hab->type, node->hab->id, node->id);
}

void cb_new_node(bionet_node_t *node) {
    GSList *i;

    g_log("", G_LOG_LEVEL_INFO, "new node: %s.%s.%s", node->hab->type, node->hab->id, node->id);

    if (node->resources) {
        g_log("", G_LOG_LEVEL_INFO, "    Resources:");

        for (i = node->resources; i != NULL; i = i->next) {
            bionet_resource_t *resource = i->data;
            bionet_datapoint_t *datapoint = bionet_resource_get_datapoint_by_index(resource, 0);

            if (datapoint == NULL) {
                g_log(
                    "", G_LOG_LEVEL_INFO,
                    "        %s %s %s (no known value)", 
                    bionet_resource_data_type_to_string(resource->data_type),
                    bionet_resource_flavor_to_string(resource->flavor),
                    resource->id
                );
            } else {
                g_log(
                    "", G_LOG_LEVEL_INFO,
                    "        %s %s %s = %s @ %s", 
                    bionet_resource_data_type_to_string(resource->data_type),
                    bionet_resource_flavor_to_string(resource->flavor),
                    resource->id,
                    bionet_datapoint_value_to_string(datapoint),
                    bionet_datapoint_timestamp_to_string(datapoint)
                );
            }

        }
    }

    if (node->streams) {
        g_log("", G_LOG_LEVEL_INFO, "    Streams:");

        for (i = node->streams; i != NULL; i = i->next) {
            bionet_stream_t *stream = i->data;
            g_log(
                "", G_LOG_LEVEL_INFO,
                "        %s %s %s", 
                stream->id,
                stream->type,
                bionet_stream_direction_to_string(stream->direction)
            );
        }
    }

}

void cb_lost_hab(bionet_hab_t *hab) {
    g_log("", G_LOG_LEVEL_INFO, "lost hab: %s.%s", hab->type, hab->id);
}


void cb_new_hab(bionet_hab_t *hab) {
    g_log("", G_LOG_LEVEL_INFO, "new hab: %s.%s", hab->type, hab->id);
}
%}

