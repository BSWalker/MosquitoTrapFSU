#!/usr/bin/env python3

# This file contains the runtime script to start advertising and registering services

import dbus
import dbus.exceptions
import dbus.mainloop.glib
import dbus.service

import array
try:
  from gi.repository import GObject
except ImportError:
  import gobject as GObject
import sys

# get methods from other bluetooth implementation file
from bluez_gatt_service import *
