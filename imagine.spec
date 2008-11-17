%define LIBNAME imagine
Summary: imagine library
Name: libsmartmet-%{LIBNAME}
Version: 8.11.12
Release: 1.el5.fmi
License: FMI
Group: Development/Libraries
URL: http://www.weatherproof.fi
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: libsmartmet-newbase >= 8.9.29-1, freetype-devel, libjpeg-devel, libpng-devel, zlib-devel, cairomm-devel, boost-devel >= 1.36
Requires: freetype, libjpeg, libpng, zlib
Provides: %{LIBNAME}

%description
FMI imagine library

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n %{LIBNAME}
 
%build
make %{_smp_mflags}

%install
%makeinstall includedir=%{buildroot}%{_includedir}/smartmet

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,0775)
%{_includedir}/smartmet/%{LIBNAME}
%{_libdir}/libsmartmet_%{LIBNAME}.a

%changelog
* Mon Nov 12 2008 pkeranen <pekka.keranen@fmi.fi> - 8.11.12-1.el5.fmi
- More improved shapefile support
* Mon Nov 10 2008 pkeranen <pekka.keranen@fmi.fi> - 8.11.10-1.el5.fmi
- Improved shapefile support
* Mon Sep 29 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.9.29-1.el5.fmi
- Newbase header change forced recompile
* Mon Sep 22 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.9.22-3.el5.fmi
- Restored jpeg support to be on by default
* Mon Sep 22 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.9.22-2.el5.fmi
- Fixed typedef when Cairo is not in use
* Mon Sep 22 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.9.22-1.el5.fmi
- Updates from Asko and compiled with static boost 1.36
* Mon Sep 15 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.9.15-1.el5.fmi
- Compiled with boost 1.36 and latest newbase
* Tue Jul 15 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.7.15-1.el5.fmi
- Linked with newest newbase
* Mon May 19 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.5.19-1.el5.fmi
- Added partial support for line width in stroking paths
* Tue Apr 15 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.4.15-1.el5.fmi
- Linked with newbase 8.4.15-2 with Asko's mmap technology
* Tue Mar 11 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.3.11-1.el5.fmi
- Linked with newbase 8.3.11-1 with wind interpolation fixes
* Wed Jan 30 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.1.30-1.el5.fmi
- Linked with newbase 8.1.25-1 for getting new parameter names
* Thu Dec 27 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.7-1.el5.fmi
- Linked with newbase 1-0.7 with wind direction bugfixes
* Wed Dec 19 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.6-1.el5.fmi
- API fixes
* Fri Dec 14 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.5-1.el5.fmi
- Added new GetType method to NFmiEsriElement
* Fri Nov 30 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.4-1.el5.fmi
- Linked with newbase 1.0.5-1
* Mon Nov 19 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.3-1.el5.fmi
- Linked with newbase 1.0.4-1
* Thu Nov 15 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.2-1.el5.fmi
- Linked with newbase 1.0.3-1
- Fixed regression test build system                 
* Mon Sep 24 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.1-4.el5.fmi
- Fixed "make depend".
* Fri Sep 14 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.1-3.el5.fmi
- Added "make tag" feature.
* Thu Sep 13 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.1-2.el5.fmi
- Improved make system.
* Thu Jun  7 2007 tervo <tervo@xodin.weatherproof.fi> - 
- Initial build.

