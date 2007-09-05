Summary: imagine library
Name: imagine
Version: 1.0
Release: 1
License: FMI
Group: Development/Libraries
URL: http://www.weatherproof.fi
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}
Requires: newbase >= 1.0.1-1, freetype >= 2.1.4, gpc >= 1.0.1-1, libjpeg, libjpeg-devel, libpng-devel >= 1.2.2, libpng10 => 1.0, zlib >= 1.1.4, zlib-devel >= 1.1.4
Provides: imagine

%description
FMI imagine library

%prep
rm -rf $RPM_BUILD_ROOT
mkdir $RPM_BUILD_ROOT

%setup -q -n %{name}
 
%build
make clean
make depend
make %{_smp_mflags} 

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,www,0775)
%{_includedir}/imagine
%{_libdir}/libimagine.a


%changelog
* Thu Jun  7 2007 tervo <tervo@xodin.weatherproof.fi> - 
- Initial build.

