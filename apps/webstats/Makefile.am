
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

## GWT requires the compiled output, which is spread over a file named 
## 'webstats.nocache.js' and a handful of other files (1 per class) named
## things like "185670E9E926D33FD16B6A96C6861769.cache.html"
##
## It also requires (or looks better with) the standard GWT CSS files and
## browser-fix images, gwt/standard/*.css and gwt/standard/images/*.png
##
## These rules ensure automake installs those files while preserving the tree
webstatsdir=$(webdir)/webstats
webstats_DATA=war/webstats/*.cache.png \
 war/webstats/*.cache.html \
 war/webstats/*.cache.gif \
 war/webstats/webstats.nocache.js \
 war/webstats/hosted.html

gwtstandardwebdir=$(webstatsdir)/gwt/standard
gwtstandardweb_DATA=war/webstats/gwt/standard/standard.css \
 war/webstats/gwt/standard/standard_rtl.css

gwtstandardimageswebdir=$(gwtstandardwebdir)/images
gwtstandardimagesweb_DATA=war/webstats/gwt/standard/images/corner.png \
	war/webstats/gwt/standard/images/corner_ie6.png \
	war/webstats/gwt/standard/images/hborder.png \
	war/webstats/gwt/standard/images/hborder_ie6.png \
	war/webstats/gwt/standard/images/vborder.png \
	war/webstats/gwt/standard/images/vborder_ie6.png
