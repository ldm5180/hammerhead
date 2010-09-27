%extend Value {
    Value(Resource * resource, int content) {
	Value * value = (Value *)malloc(sizeof(bionet_value_t));
	value = (Value *)bionet_value_new_binary((bionet_resource_t *)resource, content);
	return value;
    }

    ~Value() {
	bionet_datapoint_free($self);
    }
}
