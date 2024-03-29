## IPC (interprocess communication) plugin for pw3270.

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
![CodeQL](https://github.com/PerryWerneck/pw3270-plugin-ipc/workflows/CodeQL/badge.svg?branch=master)
[![build result](https://build.opensuse.org/projects/home:PerryWerneck:pw3270/packages/pw3270-plugin-ipc/badge.svg?type=percent)](https://build.opensuse.org/package/show/home:PerryWerneck:pw3270/pw3270-plugin-ipc)

## Installation

### Linux

You can download installation package for supported distributions in Open Build Service.

[<img src="https://raw.githubusercontent.com/PerryWerneck/pw3270/master/branding/obs-badge-en.svg" alt="Download from open build service" height="80px">](https://software.opensuse.org/download.html?project=home%3APerryWerneck%3Apw3270&package=pw3270-plugin-ipc)

## Sample scripts

Getting instrospection

```bash
#!/bin/bash

PRODUCT_NAME=$(pkg-config --variable=product_name lib3270)
PRODUCT_ID=$(pkg-config --variable=product_id lib3270)

DBUS_DEST=${PRODUCT_ID}.terminal.a
DBUS_PATH="/${PRODUCT_ID//.//}/terminal/a"
DBUS_INTERFACE="${PRODUCT_ID}.terminal.session"

gdbus \
	introspect \
	--session \
	--dest=${DBUS_DEST} \
	--object-path="${DBUS_PATH}"

```

