Summary: SMACQ analysis tool for streaming data
Name: smacq
Version: 2.1
Release: 1
License: GPL
Group: Applications/Internet
URL: http://smacq.sourceforge.net/
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Requires: libgda

%description

The System for Modular Analysis and Continuous Queries (SMACQ) is a
modular platform for analyzing and querying large datasets, including
streaming network data, using features from databases, UNIX pipelines,
and modular intrusion detection systems. 

%prep
%setup -q


%build

%configure


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=${RPM_BUILD_ROOT}

## 
## smacq's makefile is written for Debian which seems to have
## different man policies. Need to do the following for RHEL.
rm -rf ${RPM_BUILD_ROOT}/usr/share/man/manh/
rm -rf  ${RPM_BUILD_ROOT}/usr/share/man/manp/
mkdir -p ${RPM_BUILD_ROOT}/usr/share/man/man1
cp -p doc/smacq.1 ${RPM_BUILD_ROOT}/usr/share/man/man1
cp -p doc/smacqq.1 ${RPM_BUILD_ROOT}/usr/share/man/man1
mkdir -p ${RPM_BUILD_ROOT}/usr/share/man/man3
cp -p doc/api/man3/*.3 ${RPM_BUILD_ROOT}/usr/share/man/man3

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%attr(0755,root,root) /usr/bin/*
%attr(0755,root,root) /usr/share/man/man1/*
%attr(0755,root,root) /usr/share/man/man3/*

%doc AUTHORS README TODO doc/smacq.1.pdf doc/smacqq.1.pdf doc/gotsmacq.png doc/icon.png doc/index.html doc/api/refman.pdf 


%changelog
* Tue Mar 22 2005 Stephen J Smoogen <smoogen@smoogen3.lanl.gov> - 
- Initial build.

