Name:           HotShots
Version:        2.0.0
Release:        1%{?dist}
Summary:        HotShots is an application for capturing and annotating screenshots.

License:       GPL2
URL:            http://thehive.xbee.net/index.php?module=pages&func=display&pageid=31
Source0:        HotShots-2.0.0.tar.gz


BuildRequires: qt-devel >= 4.6
BuildRequires: cmake >= 2.4.3

%description


%prep
%setup -q


%build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$RPM_BUILD_ROOT/usr \
	  -DCMAKE_BUILD_TYPE=Release \
	  ./
make


%install
rm -rf $RPM_BUILD_ROOT
cd build
make install DESTDIR=$RPM_BUILD_ROOT
make install


%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_bindir}/*
%{_libdir}/%{name}/
%{_datadir}/applications/*
%{_datadir}/pixmaps/*
%{_datadir}/%{name}/
%{_mandir}/man1/*

%doc CREDITS.txt README.txt AUTHORS.txt Changelog.txt

%changelog
* Thu Jan 9 2014 xbee <xbee@xbee.net>
 - first package version



%changelog
