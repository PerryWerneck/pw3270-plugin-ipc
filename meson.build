# Maintainer: Perry Werneck <perry.werneck@gmail.com>

# SPDX-License-Identifier: LGPL-3.0-or-later 

# Copyright (C) 2025 Banco do Brasil S. A.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

project(
	'ipcplugin', 
	['c'],
	version: '5.5.0',
	license: 'GPL-3.0-or-later',
)

project_description = 'IPC plugin for pw3270'

#
# Compiler flags
#
c = meson.get_compiler('c')

enable_debug = get_option('debug') or get_option('buildtype').contains('debug')

time_epoch = run_command('sh', '-c', 'echo "$SOURCE_DATE_EPOCH"', check : true).stdout().strip()
if time_epoch == ''
  time_epoch = run_command('date','+%Y%m%d', check: true).stdout().strip()
  datecmd = 'date'
else
  datecmd = 'date -u -d@' + time_epoch
endif

add_project_arguments(c.get_supported_arguments([
	'-ggdb3',
	'-DHAVE_CONFIG_H=1',
	'-DBUILD_DATE='+time_epoch,
	get_option('buildtype').contains('debug') ? '-DDEBUG=1' : '-DNDEBUG=1',
]), language: 'c')

#
# Versioning
#
pkg_version = meson.project_version()
version_array = pkg_version.split('.')
pkg_major_version = version_array[0].to_int()
pkg_minor_version = version_array[1].to_int()
pkg_micro_version = version_array[2].to_int()

libversion = '@0@.@1@'.format(pkg_major_version,pkg_minor_version)

#
# i18n
#
subdir('po')

#
# Dependencies
#
dbus = dependency('dbus-1', required : false)
lib3270 = dependency('lib3270', fallback: [ 'lib3270', 'dynamic_library' ])
deplist = [
	dependency('iconv'),
	lib3270,
	dependency('libv3270', fallback: [ 'libv3270', 'dynamic_library' ]),
	dbus,
	dependency('dbus-glib-1'),
	dependency('gtk+-3.0'),
	dependency('glib-2.0'),
	dependency('gmodule-2.0'),
]

#
# Configuration
#
product_id = lib3270.get_variable('product_id')
app_conf = configuration_data()
app_conf.set_quoted('PACKAGE_NAME', meson.project_name())
app_conf.set_quoted('PACKAGE_VERSION', meson.project_version())
app_conf.set_quoted('PACKAGE_DESCRIPTION', project_description)
app_conf.set('PACKAGE_VERSION_MAJOR', pkg_major_version)
app_conf.set('PACKAGE_VERSION_MINOR', pkg_minor_version)
app_conf.set('PACKAGE_VERSION_MICRO', pkg_micro_version)
app_conf.set('PLUGIN_PATH',lib3270.get_variable('plugin_path'))

app_conf.set('PRODUCT_ID', lib3270.get_variable('product_id'))
app_conf.set('PRODUCT_NAME', lib3270.get_variable('product_name'))
app_conf.set('PRODUCT_VERSION',lib3270.get_variable('sdk_version'))

if dbus.found()
	app_conf.set('HAVE_DBUS', 1)

	# FIXME: use product_id from lib3270
	dbus_object_path=product_id.replace('.','/')
	app_conf.set_quoted('DBUS_OBJECT_PATH',dbus_object_path)

	app_conf.set_quoted(
		'PW3270_IPC_SESSION_BUS_NAME',
		'@0@.terminal.%c'.format(product_id)
	)

	app_conf.set_quoted(
		'PW3270_IPC_SESSION_BUS_PATH',
		'@0@/%c'.format(dbus_object_path)
	)

	app_conf.set_quoted(
		'PW3270_IPC_SESSION_INTERFACE_NAME',
		'@0@.terminal.session'.format(product_id)
	)
endif

if host_machine.system() == 'windows'
	app_conf.set_quoted(
		'PW3270_IPC_SESSION_BUS_NAME',
		'\\\\.\\pipe\\%s\\%c'
	)
endif

app_conf.set('HAVE_LIBINTL', 1)
app_conf.set('HAVE_GNUC_VISIBILITY', 1)

if c.compiles('#include <unistd.h>', name : 'unistd.h')
	app_conf.set('HAVE_UNISTD_H', 1)
endif

#
# Sources
#
includes_dir = include_directories(
    'src/include',
)

lib_src = [
	'src/core/constants.c',
	'src/core/getproperties.c',
	'src/core/response.c',
	'src/core/setproperties.c',
	'src/core/tools.c',
	'src/core/' + host_machine.system() + '/gobject.c',
	'src/core/' + host_machine.system() + '/start.c',
	'src/core/' + host_machine.system() + '/stop.c',
	'src/core/methods/action.c',  
	'src/core/methods/field.c',
	'src/core/methods/methods.c',
	'src/core/methods/set.c',
	'src/core/methods/cursor.c',  
	'src/core/methods/get.c',
	'src/core/methods/network.c',  
	'src/core/methods/wait.c',
	'src/plugin/plugin.c',
]

#
# Targets
#
config_src = [
  configure_file(
      output : 'config.h', 
      configuration : app_conf
  )
]

if host_machine.system() == 'windows'

	# https://mesonbuild.com/Windows-module.html
	windows = import('windows')
	resources = windows.compile_resources(
		configure_file(
			input : 'src/plugin/windows/resources.rc.in',
			output : 'resources.rc',
			configuration : app_conf
		)
	)

	dynamic = shared_library(
		meson.project_name(), 
		lib_src + resources,
		install: true,
		gnu_symbol_visibility: 'hidden',
		dependencies: deplist,
		link_args: [
			'-lshlwapi', 
			'-lshell32', 
			'-lole32', 
			'-luuid', 
			'-liphlpapi',
			'-lws2_32',
		],
		include_directories: includes_dir,
		install_dir: lib3270.get_variable('plugin_path'),
	)

	static_library = declare_dependency(
		link_with : static_library(
		meson.project_name(), 
			lib_src,
			pic: true,
			c_args: [ '-DSTATIC_LIBRARY' ],
			install: false,
			include_directories: includes_dir,
			dependencies: deplist,
		),
		link_args: [
			'-lshlwapi', 
			'-lshell32', 
			'-lole32', 
			'-luuid', 
			'-liphlpapi',
			'-lws2_32',
		],
		include_directories : includes_dir,
	)

elif host_machine.system() == 'darwin'

	dynamic = shared_library(
		meson.project_name(), 
		lib_src,
		install: true,
		c_args: [ 
			'-fPIC' 
		],
		gnu_symbol_visibility: 'hidden',
		dependencies: deplist,
		install_dir: lib3270.get_variable('plugin_path'),
		include_directories: includes_dir
	)

	static_library = declare_dependency(
		link_with : static_library(
			meson.project_name(), 
			lib_src,
			pic: true,
			c_args: [ '-DSTATIC_LIBRARY' ],
			install: false,
			include_directories: includes_dir,
			dependencies: deplist,
		),
		include_directories : includes_dir,
	)

else

	dynamic = shared_library(
		meson.project_name(), 
		lib_src,
		install: true,
		version : libversion,
		cpp_args: [ '-ffat-lto-objects' ],
		name_prefix: [],
		soversion: '',
		gnu_symbol_visibility: 'hidden',
		dependencies: deplist,
		install_dir: lib3270.get_variable('plugin_path'),
		include_directories: includes_dir
	)

	static_library = declare_dependency(
		link_with : static_library(
			meson.project_name(), 
			lib_src,
			pic: true,
			cpp_args: [ '-DSTATIC_LIBRARY', '-fno-lto' ],
			install: false,
			include_directories: includes_dir,
			dependencies: deplist,
		),
		include_directories : includes_dir,
	)
 
	configure_file(
		input : 'branding/metainfo.xml.in',
		output : lib3270.get_variable('product_id') + '.ipcplugin.metainfo.xml',
		install_dir: get_option('datadir') / 'metainfo',
		configuration : app_conf
	)

endif

dynamic_library = declare_dependency(
	link_with : dynamic,
	include_directories : includes_dir,
) 

executable(
	meson.project_name(),
	[ 
		'src/testprogram/testprogram.c',
	],
	install: false,
	link_with : [ dynamic ],
	dependencies: deplist,
	include_directories: includes_dir
)
