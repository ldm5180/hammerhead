
typedef struct {
    char *name;
} bionet_hab_t;


int join(bionet_hab_t *me);
int leave(bionet_hab_t *me);


int subscribe_hab_list(
    void (*cb_new_hab)(bionet_hab_t *new_hab),
    void (*cb_lost_hab)(bionet_hab_t *lost_hab)
);

