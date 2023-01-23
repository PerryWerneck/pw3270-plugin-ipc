#
# spec file for package pw3270-plugin-ipc
#
# Copyright (c) 2015 SUSE LINUX GmbH, Nuernberg, Germany.
# Copyright (C) <2008> <Banco do Brasil S.A.>
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

%define product %(pkg-config --variable=product_name lib3270)
%define plugindir %(pkg-config --variable=plugin_path lib3270)

%if "%{plugindir}" == ""
	%define plugindir /usr/lib64/pw3270-plugins
%endif

Summary:		IPC plugin for %{product} 
Name:			pw3270-plugin-ipc
Version:		5.5
Release:		0
License:		LGPL-3.0
Source:			%{name}-%{version}.tar.xz

URL:			https://github.com/PerryWerneck/pw3270-plugin-ipc.git

Group:			Development/Libraries/C and C++
BuildRoot:		/var/tmp/%{name}-%{version}

BuildRequires:	autoconf >= 2.61
BuildRequires:	automake
BuildRequires:	libtool
BuildRequires:	binutils
BuildRequires:	coreutils
BuildRequires:	gcc-c++
BuildRequires:	gettext-devel
BuildRequires:	m4

BuildRequires:	pkgconfig(lib3270) >= 5.4
BuildRequires:	pkgconfig(libv3270) >= 5.4
BuildRequires:	pkgconfig(dbus-1)
BuildRequires:	pkgconfig(dbus-glib-1)
BuildRequires:	pkgconfig(gtk+-3.0)

Requires:		%{product} >= 5.4
Enhances:		%{product}

%description

PW3270 plugin exporting D-Bus objects for every tn3270 session, used by libipc3270 and other applications to remote control or screen scraping.

For more details, see https://github.com/PerryWerneck/libipc3270 .

#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%setup

NOCONFIGURE=1 \
	./autogen.sh

%configure

%build
make all

%install
%make_install

%files
%defattr(-,root,root)

# https://en.opensuse.org/openSUSE:Packaging_for_Leap#RPM_Distro_Version_Macros
%doc AUTHORS README.md
%license LICENSE
%{plugindir}/*.so

%changelog

