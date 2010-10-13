%extend Bdm {
    Bdm() {
	Bdm * bdm = (Bdm *)calloc(1, sizeof(Bdm));
	if (NULL == bdm) {
	    g_warning("bionet-bdm.i: Failed to allocate memory for Bdm");
	    return NULL;
	}
	bdm->this = bionet_bdm_new(NULL);
	bionet_bdm_set_user_data(bdm->this, bdm);
	return bdm;
    }

    Bdm(const char * id) {
	Bdm * bdm = (Bdm *)calloc(1, sizeof(Bdm));
	if (NULL == bdm) {
	    g_warning("bionet-bdm.i: Failed to allocate memory for Bdm");
	    return NULL;
	}
	bdm->this = bionet_bdm_new(id);
	bionet_bdm_set_user_data(bdm->this, bdm);
	return bdm;
    }

    ~Bdm() {
	int free_me = 0;
	if (0 == bionet_bdm_get_ref_count($self->this)) {
	    free_me = 1;
	}
	bionet_bdm_free($self->this);
	if (free_me) {
	    free($self);
	}
    }
}
