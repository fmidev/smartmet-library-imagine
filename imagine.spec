%define LIBNAME imagine
Summary: imagine library
Name: smartmet-%{LIBNAME}
Version: 1.0.1
Release: 1.el5.fmi
License: FMI
Group: Development/Libraries
URL: http://www.weatherproof.fi
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}
Requires: smartmet-newbase >= 1.0.1-1, freetype >= 2.1.4, smartmet-gpc >= 1.0.1-1, libjpeg, libjpeg-devel, libpng-devel >= 1.2.2, libpng10 => 1.0, zlib >= 1.1.4, zlib-devel >= 1.1.4
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

