%extend Node {
    Node(Hab * hab, const char * id) {
	Node * node = (Node *)malloc(sizeof(Node));
	node->this = bionet_node_new(hab->this, id);
	return node;
    }
    ~Node() {
	bionet_node_free($self->this);
	free($self);
    }
    const char * name() { return bionet_node_get_name($self->this); }
    const char * id() {	return bionet_node_get_id($self->this); }
    const uint8_t * uid() { return bionet_node_get_uid($self->this); }
    bionet_hab_t * hab() { return bionet_node_get_hab($self->this); }
    int addResource(bionet_resource_t * resource) { return bionet_node_add_resource($self->this, resource); }
    int numResources() { return bionet_node_get_num_resources($self->this); }
    bionet_resource_t * resource(unsigned int index) { return bionet_node_get_resource_by_index($self->this, index); }
    bionet_resource_t * resource(const char * id) { return bionet_node_get_resource_by_id($self->this, id); }
    int addStream(bionet_stream_t * stream) { return bionet_node_add_stream($self->this, stream); }
    int numStreams() { return bionet_node_get_num_streams($self->this); }
    bionet_stream_t * stream(unsigned int index) { return bionet_node_get_stream_by_index($self->this, index); }
    bionet_stream_t * stream(const char * id) { return bionet_node_get_stream_by_id($self->this, id); }
    int matchesId(const char * id) { return bionet_node_matches_id($self->this, id); }
    int matchesHabTypeHabIdNodeId(const char * hab_type,
				  const char * hab_id, 
				  const char * node_id) { 
	return bionet_node_matches_habtype_habid_nodeid($self->this, hab_type, hab_id, node_id);
    }
    void setUserData(const void * user_data) { bionet_node_set_user_data($self->this, user_data); }
    void * userData() { return bionet_node_get_user_data($self->this); }
    int numEvents() { return bionet_node_get_num_events($self->this); }
    bionet_event_t * event(unsigned int index) { return bionet_node_get_event_by_index($self->this, index); }
    int addEvent(const bionet_event_t * event) { return bionet_node_add_event($self->this, event); }
    int addDestructor(void (*destructor)(bionet_node_t * node, void * user_data),
		      void * user_data) {
	return bionet_node_add_destructor($self->this, destructor, user_data);
    }
 }
