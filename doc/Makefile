
PICS =                                                \
    hab-list-subscription-state-diagram-client.png    \
    hab-list-subscription-state-diagram-hab.png       \
    node-list-subscription-state-diagram-client.png   \
    node-list-subscription-state-diagram-hab.png      \
    resource-update-state-diagram-client.png          \
    resource-update-state-diagram-hab.png             \
    bionet-network-overview.png                       \
    bionet-network-with-acl-server.png                \
    bionet-network-tear-away.png                      \
    bionet-network-tear-away-two-acl-servers.png      \


all: ${PICS}

%.png: %.svg
	inkscape -D -e $@ $<

