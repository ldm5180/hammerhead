%module bionet
%{
#include "bionet-2.1.h"
#include "bionet-util-2.1.h"
%}

%include "bionet-2.1.h"
%include "bionet-util-2.1.h"

%constant void cb_lost_hab(bionet_hab_t *hab);
%constant void cb_new_hab(bionet_hab_t *hab);
%constant void cb_new_node(bionet_node_t *node);
%constant void cb_lost_node(bionet_node_t *node);
%constant void cb_datapoint(bionet_datapoint_t *datapoint);

%inline
{
    void cb_lost_hab(bionet_hab_t *hab) {
	g_log("", G_LOG_LEVEL_INFO, "lost hab: %s.%s", bionet_hab_get_type(hab), bionet_hab_get_id(hab));
    }
 
 
    void cb_new_hab(bionet_hab_t *hab) {
	g_log("", G_LOG_LEVEL_INFO, "new hab: %s.%s", bionet_hab_get_type(hab), bionet_hab_get_id(hab));
    }

    void cb_new_node(bionet_node_t *node) {
	int i;
	bionet_hab_t *hab = bionet_node_get_hab(node);
	
	g_log("", G_LOG_LEVEL_INFO, "new node: %s.%s.%s", 
	      bionet_hab_get_type(hab), bionet_hab_get_id(hab), bionet_node_get_id(node));
	
	if (bionet_node_get_num_resources(node)) {
	    g_log("", G_LOG_LEVEL_INFO, "    Resources:");
	    
	    for (i = 0; i < bionet_node_get_num_resources(node); i++) {
		bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);
		bionet_datapoint_t *datapoint = bionet_resource_get_datapoint_by_index(resource, 0);
		
		if (datapoint == NULL) {
		    g_log("", G_LOG_LEVEL_INFO,
			  "        %s %s %s (no known value)", 
			  bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
			  bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
			  bionet_resource_get_id(resource));
		} else {
		    char * value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));
		    g_log("", G_LOG_LEVEL_INFO,
			  "        %s %s %s = %s @ %s", 
			  bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
			  bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
			  bionet_resource_get_id(resource),
			  value_str,
			  bionet_datapoint_timestamp_to_string(datapoint));
		    free(value_str);
		}
		
	    }
	}
#if 0
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
#endif
    }

    void cb_datapoint(bionet_datapoint_t *datapoint) {
	bionet_value_t * value = bionet_datapoint_get_value(datapoint);
	bionet_resource_t * resource = bionet_value_get_resource(value);
	bionet_node_t * node = bionet_resource_get_node(resource);
	bionet_hab_t * hab = bionet_node_get_hab(node);
	
	char * value_str = bionet_value_to_str(value);
	
	g_log(
	    "",
	    G_LOG_LEVEL_INFO,
	    "%s.%s.%s:%s = %s %s %s @ %s",
	    bionet_hab_get_type(hab),
	    bionet_hab_get_id(hab),
	    bionet_node_get_id(node),
	    bionet_resource_get_id(resource),
	    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
	    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
	    value_str,
	    bionet_datapoint_timestamp_to_string(datapoint)
	    );
	
	free(value_str);
    }
    
    
    void cb_lost_node(bionet_node_t *node) {
	bionet_hab_t *hab = bionet_node_get_hab(node);
	g_log("", G_LOG_LEVEL_INFO, "lost node: %s.%s.%s", 
	      bionet_hab_get_type(hab), 
	      bionet_hab_get_id(hab), 
	      bionet_node_get_id(node));
    }
} 
