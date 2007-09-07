%define LIBNAME imagine
Summary: imagine library
Name: libsmartmet-%{LIBNAME}
Version: 1.0.1
Release: 1.el5.fmi
License: FMI
Group: Development/Libraries
URL: http://www.weatherproof.fi
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: libsmartmet-newbase, freetype-devel, libjpeg-devel, libpng-devel, zlib-devel
Provides: %{LIBNAME}

%description
FMI imagine library

%prep
rm -rf $RPM_BUILD_ROOT

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

