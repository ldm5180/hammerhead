
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.

include $(top_srcdir)/common.am

CLASSPATH=src:$(GWT_PATH)/gwt-dev-linux.jar:$(GWT_PATH)/gwt-user.jar
GWTCOMPILERCLASS=com.google.gwt.dev.Compiler

CLIENTPATH=src/com/bioserve/webstats/client

CLIENTFILES=$(CLIENTPATH)/Webstats.java \
    $(CLIENTPATH)/PlotWrapper.java \
	$(CLIENTPATH)/HttpGetWrapper.java \
	$(CLIENTPATH)/ParameterChooser.java \
	$(CLIENTPATH)/TimespanChooser.java \
	$(CLIENTPATH)/UpdateRateChooser.java \
	$(CLIENTPATH)/Refresher.java \
	$(CLIENTPATH)/Updateable.java \
	$(CLIENTPATH)/TimespanSettable.java

war/webstats/webstats.nocache.js: $(CLIENTFILES)
	$(JAVA) -cp $(CLASSPATH) $(GWTCOMPILERCLASS) com.bioserve.webstats.Webstats

all-local: war/webstats/webstats.nocache.js
