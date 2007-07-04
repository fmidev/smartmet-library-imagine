Summary: imagine library
Name: imagine
Version: 1.0
Release: 1
License: FMI
Group: Development/Libraries
URL: http://www.weatherproof.fi
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}
Requires: newbase >= 1.0-1, freetype >= 2.1.4, freetype-devel >= 2.1.4, gpc >= 1-0-1, libjpeg, libjpeg-devel, libpng10 >= 1.0, libpng-devel >= 1.2.2, zlib >= 1.1.4, zlib-devel >= 1.1.4
Provides: imagine

%description
FMI imagine library

%prep
rm -rf $RPM_BUILD_ROOT
mkdir $RPM_BUILD_ROOT

%setup -q -n %{name}
 
%build
make %{_smp_mflags} 

%install
make install prefix="${RPM_BUILD_ROOT}"
mkdir -p ${RPM_BUILD_ROOT}/smartmet/src/c++/lib/imagine 
cp -r . ${RPM_BUILD_ROOT}/smartmet/src/c++/lib/imagine 

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,www,www,0775)
/usr/include/imagine
/usr/lib/libimagine.a
/smartmet/src/c++/lib/imagine 


%changelog
* Thu Jun  7 2007 tervo <tervo@xodin.weatherproof.fi> - 
- Initial build.

