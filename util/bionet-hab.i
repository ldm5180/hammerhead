%extend Hab {
    Hab(const char * type, const char * id) {
	Hab * hab = (Hab *)bionet_hab_new(type, id);
	return hab;
    }
    Hab() {
	Hab * hab = (Hab *)bionet_hab_new(NULL, NULL);
	return hab;
    }
    ~Hab() {
	bionet_hab_free((bionet_hab_t *)$self);
    }
    const char * name() {
	return bionet_hab_get_name((bionet_hab_t *)$self);
    }
    const char * type() {
	return bionet_hab_get_type((bionet_hab_t *)$self);
    }
    const char * id() {
	return bionet_hab_get_id((bionet_hab_t *)$self);
    }
    Node * node(const char * node_id) {
	return (Node *)bionet_hab_get_node_by_id((bionet_hab_t *)$self, node_id);
    }
    Node * node(const char * node_id, const uint8_t node_uid[BDM_UUID_LEN]) {
	return (Node *)bionet_hab_get_node_by_id_and_uid((bionet_hab_t *)$self, node_id, node_uid);
    }
    int numNodes() {
	return bionet_hab_get_num_nodes((bionet_hab_t *)$self);
    }
    Node * node(unsigned int index) {
	return (Node *)bionet_hab_get_node_by_index((bionet_hab_t *)$self, index);
    }
    int add(const Node * node) {
	return bionet_hab_add_node((bionet_hab_t *)$self, (bionet_node_t *)node);
    }
    Node * remove(const char * node_id) {
	return (Node *)bionet_hab_remove_node_by_id((bionet_hab_t *)$self, node_id);
    }
    Node * remove(const char *node_id, const uint8_t node_uid[BDM_UUID_LEN]) {
	return (Node *)bionet_hab_remove_node_by_id_and_uid((bionet_hab_t *)$self, node_id, node_uid);
    }
    int removeAll() {
	return bionet_hab_remove_all_nodes((bionet_hab_t *)$self);
    }
    int matchesTypeAndId(const char * type, const char * id) {
	return bionet_hab_matches_type_and_id((bionet_hab_t *)$self, type, id);
    }
    void setUserData(const void * user_data) {
	bionet_hab_set_user_data((bionet_hab_t *)$self, user_data);
    }
    void * userData() {
	return bionet_hab_get_user_data((bionet_hab_t *)$self);
    }
    int isSecure() {
	return bionet_hab_is_secure((bionet_hab_t *)$self);
    }
    int numEvents() {
	return bionet_hab_get_num_events((bionet_hab_t *)$self);
    }
    bionet_event_t * eventByIndex(unsigned int index) {
	return bionet_hab_get_event_by_index((bionet_hab_t *)$self, index);
    } 
    int add(bionet_event_t * event) {
	return bionet_hab_add_event((bionet_hab_t *)$self, event);
    }
    int add(void (*destructor)(bionet_hab_t * hab, void * user_data),
		      void * user_data) {
	return bionet_hab_add_destructor((bionet_hab_t *)$self, destructor, user_data);
    }
}
