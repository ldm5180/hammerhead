%extend Hab {
    Hab(const char * type, const char * id) {
	Hab * hab = (Hab *)malloc(sizeof(Hab));
	hab->this = bionet_hab_new(type, id);
	return hab;
    }
    Hab() {
	Hab * hab = (Hab *)malloc(sizeof(Hab));
	hab->this = bionet_hab_new(NULL, NULL);
	return hab;
    }
    ~Hab() {
	bionet_hab_free($self->this);
	free($self);
    }
    const char * name() {
	return bionet_hab_get_name($self->this);
    }
    const char * type() {
	return bionet_hab_get_type($self->this);
    }
    const char * id() {
	return bionet_hab_get_id($self->this);
    }
    bionet_node_t * node(const char * node_id) {
	return bionet_hab_get_node_by_id($self->this, node_id);
    }
    bionet_node_t * node(const char * node_id, const uint8_t node_uid[BDM_UUID_LEN]) {
	return bionet_hab_get_node_by_id_and_uid($self->this, node_id, node_uid);
    }
    int numNodes() {
	return bionet_hab_get_num_nodes($self->this);
    }
    bionet_node_t * node(unsigned int index) {
	return bionet_hab_get_node_by_index($self->this, index);
    }
    int addNode(const bionet_node_t * node) {
	return bionet_hab_add_node($self->this, node);
    }
    bionet_node_t * removeNode(const char * node_id) {
	return bionet_hab_remove_node_by_id($self->this, node_id);
    }
    bionet_node_t * removeNode(const char *node_id, const uint8_t node_uid[BDM_UUID_LEN]) {
	return bionet_hab_remove_node_by_id_and_uid($self->this, node_id, node_uid);
    }
    int removeAllNodes() {
	return bionet_hab_remove_all_nodes($self->this);
    }
    int matchesTypeAndId(const char * type, const char * id) {
	return bionet_hab_matches_type_and_id($self->this, type, id);
    }
    void setUserData(const void * user_data) {
	bionet_hab_set_user_data($self->this, user_data);
    }
    void * userData() {
	return bionet_hab_get_user_data($self->this);
    }
    int isSecure() {
	return bionet_hab_is_secure($self->this);
    }
    int numEvents() {
	return bionet_hab_get_num_events($self->this);
    }
    bionet_event_t * eventByIndex(unsigned int index) {
	return bionet_hab_get_event_by_index($self->this, index);
    } 
    int addEvent(bionet_event_t * event) {
	return bionet_hab_add_event($self->this, event);
    }
    int addDestructor(void (*destructor)(bionet_hab_t * hab, void * user_data),
		      void * user_data) {
	return bionet_hab_add_destructor($self->this, destructor, user_data);
    }
}
