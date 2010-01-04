Name:		bionet
Version:	2.5.0
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
   /usr/local/bin/*
   /usr/local/lib/libamedevice*
   /usr/local/lib/libbionet*
   /usr/local/lib/libcal-mdnssd-bip*
   /usr/local/lib/libevent*
   /usr/local/lib/libhab*
   /usr/local/lib/python2.6/site-packages/*.py
   /usr/local/lib/python2.6/site-packages/_bionet*
   /usr/local/lib/python2.6/site-packages/_hab.*
   /usr/local/lib/libbdm*
   /usr/local/etc/mon-hab.d/*
   /usr/local/share/bionet2/schema

%defattr(-,root,root,-)
%doc /usr/local/share/doc/bionet/index.html

%files devel
   /usr/local/include/*.h

%files ignore
/usr/local/lib/python2.6/site-packages/_bonjour.a
/usr/local/lib/python2.6/site-packages/_bonjour.la
/usr/local/lib/python2.6/site-packages/_bonjour.so
/usr/local/lib/python2.6/site-packages/_bonjour.so.*
/usr/local/lib/python2.6/site-packages/*.pyc
/usr/local/lib/python2.6/site-packages/*.pyo

%changelog
