%define LIBNAME imagine
Summary: imagine library
Name: libsmartmet-%{LIBNAME}
Version: 1.0.1
Release: 1.el5.fmi
License: FMI
Group: Development/Libraries
URL: http://www.weatherproof.fi
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}
#How about libpng10?
BuildPrereq: smartmet-newbase >= 1.0.1-1,  smartmet-gpc >= 1.0.1-1, freetype, libjpeg, libjpeg-devel, libpng-devel, zlib, zlib-devel
Provides: %{LIBNAME}

%description
FMI imagine library

%prep
rm -rf $RPM_BUILD_ROOT
mkdir $RPM_BUILD_ROOT

%setup -q -n %{LIBNAME}
 
%build
make clean
make depend
make %{_smp_mflags} release

%install
%makeinstall includedir=%{buildroot}%{_includedir}/smartmet

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,0775)
%{_includedir}/smartmet/%{LIBNAME}
%{_libdir}/libsmartmet_%{LIBNAME}.a


%changelog
* Thu Jun  7 2007 tervo <tervo@xodin.weatherproof.fi> - 
- Initial build.

