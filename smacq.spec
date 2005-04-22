Summary: SMACQ analysis tool for streaming data
Name: smacq
Version: 2.2
Release: 1
License: GPL
Group: Applications/Internet
URL: http://smacq.sourceforge.net/
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Requires: libgda, glib2, zlib, libpcap
BuildRequires: gcc-c++, libgda-devel, glib2-devel, zlib-devel

%description

The System for Modular Analysis and Continuous Queries (SMACQ) is a
modular platform for analyzing and querying large datasets, including
streaming network data, using features from databases, UNIX pipelines,
and modular intrusion detection systems. 

%prep
%setup -q


%build

%configure --with-docdir=/usr/share/doc/${name}-${version}.


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=${RPM_BUILD_ROOT}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%attr(0755,root,root) /usr/bin/*
%attr(0755,root,root) /usr/share/man/man1/*
%attr(0755,root,root) /usr/share/man/man3/*
%attr(0755,root,root) /usr/share/doc/smacq-${version}/*

#%doc AUTHORS README TODO doc/smacq.1.pdf doc/smacqq.1.pdf doc/gotsmacq.png doc/icon.png doc/index.html doc/api/refman.pdf 

%changelog
* Tue Mar 22 2005 Stephen J Smoogen <smoogen@smoogen3.lanl.gov> - 
- Initial build.

