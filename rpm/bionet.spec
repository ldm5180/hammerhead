Name:		bionet
Version:	2.3.0
Release:	1%{?dist}
Summary:	Bionet Middleware

Group:		Other
License:	Non-free
URL:		http://bioserve.colorado.edu/
Source0:	%{name}-%{version}.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%description
The Bionet Middleware for space communications.

%package devel
Summary:        Development files for Bionet
Group:          Other
%description devel
Header files for doing development with the Bionet Middleware.

%package ignore
Summary:        Ignored files used for testing only
Group:          Other
%description ignore
Some files used for testing Bionet, but are not needed otherwise


%prep
%setup -q


%build
./configure
make 


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT


%clean
rm -rf $RPM_BUILD_ROOT


%files
   /usr/local/bin/alsa-hab
   /usr/local/bin/bdm-client
   /usr/local/bin/bionet-commander
   /usr/local/bin/bionet-data-manager
   /usr/local/bin/bionet-get
   /usr/local/bin/bionet-monitor
   /usr/local/bin/bionet-stream
   /usr/local/bin/bionet-watcher
   /usr/local/bin/ca-serve
   /usr/local/bin/cpod-hab
   /usr/local/bin/csa-cp-hab
   /usr/local/bin/mmod-hab
   /usr/local/bin/pal-650
   /usr/local/bin/parsec-hab
   /usr/local/bin/random-hab
   /usr/local/bin/speedway
   /usr/local/bin/stethoscope-hab
   /usr/local/bin/stream-manager
   /usr/local/bin/streamy-hab
   /usr/local/bin/syshealth-hab
   /usr/local/bin/test-asn-bdm-c2s
   /usr/local/bin/test-asn-bdm-s2c
   /usr/local/bin/test-asn-c2h
   /usr/local/bin/test-asn-h2c
   /usr/local/bin/test-pattern-hab
   /usr/local/bin/time-publisher
   /usr/local/bin/time-subscriber
   /usr/local/bin/uwb-hab
   /usr/local/lib/cgi-bin/bionet/bdmplot.py
   /usr/local/lib/cgi-bin/bionet/bdmplot.pyc
   /usr/local/lib/cgi-bin/bionet/bdmplot.pyo
   /usr/local/lib/cgi-bin/bionet/datapoints_to_dict.py
   /usr/local/lib/cgi-bin/bionet/datapoints_to_dict.pyc
   /usr/local/lib/cgi-bin/bionet/datapoints_to_dict.pyo
   /usr/local/lib/cgi-bin/bionet/timechooser.py
   /usr/local/lib/cgi-bin/bionet/timechooser.pyc
   /usr/local/lib/cgi-bin/bionet/timechooser.pyo
   /usr/local/lib/cgi-bin/bionet/timespan.py
   /usr/local/lib/cgi-bin/bionet/timespan.pyc
   /usr/local/lib/cgi-bin/bionet/timespan.pyo
   /usr/local/lib/libamedevice.a
   /usr/local/lib/libamedevice.la
   /usr/local/lib/libamedevice.so
   /usr/local/lib/libamedevice.so.0
   /usr/local/lib/libamedevice.so.0.1.3
   /usr/local/lib/libbionet-asn.a
   /usr/local/lib/libbionet-asn.la
   /usr/local/lib/libbionet-asn.so
   /usr/local/lib/libbionet-asn.so.0
   /usr/local/lib/libbionet-asn.so.0.1.3
   /usr/local/lib/libbionet-qt4.a
   /usr/local/lib/libbionet-qt4.la
   /usr/local/lib/libbionet-qt4.so
   /usr/local/lib/libbionet-qt4.so.0
   /usr/local/lib/libbionet-qt4.so.0.1.3
   /usr/local/lib/libbionet-util.a
   /usr/local/lib/libbionet-util.la
   /usr/local/lib/libbionet-util.so
   /usr/local/lib/libbionet-util.so.0
   /usr/local/lib/libbionet-util.so.0.1.3
   /usr/local/lib/libbionet.a
   /usr/local/lib/libbionet.la
   /usr/local/lib/libbionet.so
   /usr/local/lib/libbionet.so.0
   /usr/local/lib/libbionet.so.0.1.3
   /usr/local/lib/libcal-mdnssd-bip-client.a
   /usr/local/lib/libcal-mdnssd-bip-client.la
   /usr/local/lib/libcal-mdnssd-bip-client.so
   /usr/local/lib/libcal-mdnssd-bip-client.so.0
   /usr/local/lib/libcal-mdnssd-bip-client.so.0.1.3
   /usr/local/lib/libcal-mdnssd-bip-server.a
   /usr/local/lib/libcal-mdnssd-bip-server.la
   /usr/local/lib/libcal-mdnssd-bip-server.so
   /usr/local/lib/libcal-mdnssd-bip-server.so.0
   /usr/local/lib/libcal-mdnssd-bip-server.so.0.1.3
   /usr/local/lib/libcal-mdnssd-bip-shared.a
   /usr/local/lib/libcal-mdnssd-bip-shared.la
   /usr/local/lib/libcal-mdnssd-bip-shared.so
   /usr/local/lib/libcal-mdnssd-bip-shared.so.0
   /usr/local/lib/libcal-mdnssd-bip-shared.so.0.1.3
   /usr/local/lib/libevent.a
   /usr/local/lib/libevent.la
   /usr/local/lib/libevent.so
   /usr/local/lib/libevent.so.0
   /usr/local/lib/libevent.so.0.1.3
   /usr/local/lib/libhab.a
   /usr/local/lib/libhab.la
   /usr/local/lib/libhab.so
   /usr/local/lib/libhab.so.0
   /usr/local/lib/libhab.so.0.1.3
   /usr/local/lib/python2.6/site-packages/_bdm_client.a
   /usr/local/lib/python2.6/site-packages/_bdm_client.la
   /usr/local/lib/python2.6/site-packages/_bdm_client.so
   /usr/local/lib/python2.6/site-packages/_bdm_client.so.0
   /usr/local/lib/python2.6/site-packages/_bdm_client.so.0.1.3
   /usr/local/lib/python2.6/site-packages/_bionet.a
   /usr/local/lib/python2.6/site-packages/_bionet.la
   /usr/local/lib/python2.6/site-packages/_bionet.so
   /usr/local/lib/python2.6/site-packages/_bionet.so.0
   /usr/local/lib/python2.6/site-packages/_bionet.so.0.1.3
   /usr/local/lib/python2.6/site-packages/_hab.a
   /usr/local/lib/python2.6/site-packages/_hab.la
   /usr/local/lib/python2.6/site-packages/_hab.so
   /usr/local/lib/python2.6/site-packages/_hab.so.0
   /usr/local/lib/python2.6/site-packages/_hab.so.0.1.3
   /usr/local/lib/python2.6/site-packages/bdm_client.py
   /usr/local/lib/python2.6/site-packages/bdm_client.pyc
   /usr/local/lib/python2.6/site-packages/bdm_client.pyo
   /usr/local/lib/python2.6/site-packages/bionet.py
   /usr/local/lib/python2.6/site-packages/bionet.pyc
   /usr/local/lib/python2.6/site-packages/bionet.pyo
   /usr/local/lib/python2.6/site-packages/hab.py
   /usr/local/lib/python2.6/site-packages/hab.pyc
   /usr/local/lib/python2.6/site-packages/hab.pyo

%defattr(-,root,root,-)
%doc /usr/local/share/doc/bionet/index.html

%files devel
   /usr/local/include/bionet-datapoint.h
   /usr/local/include/bionet-hab.h
   /usr/local/include/bionet-node.h
   /usr/local/include/bionet-resource.h
   /usr/local/include/bionet-stream.h
   /usr/local/include/bionet-util.h
   /usr/local/include/bionet-value.h
   /usr/local/include/bionet-version.h
   /usr/local/include/bionet.h
   /usr/local/include/hardware-abstractor.h

%files ignore
/usr/local/lib/python2.6/site-packages/_bonjour.a
/usr/local/lib/python2.6/site-packages/_bonjour.la
/usr/local/lib/python2.6/site-packages/_bonjour.so
/usr/local/lib/python2.6/site-packages/_bonjour.so.0
/usr/local/lib/python2.6/site-packages/_bonjour.so.0.1.3

%changelog
