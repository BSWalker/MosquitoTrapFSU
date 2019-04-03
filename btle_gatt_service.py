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
from bluez_bluetoothle_components import *

mainloop = None

class TestChr(Characteristic):
  TEST_UUID = '00000000-0000-0000-1234-56789abc0000'

  def __init__(self, bus, index, service):
    Characteristic.__init__(self, bus, index, self.TEST_UUID, ['read','write'], service)
    self.value = 1;

  def ReadValue(self, options):
    print('Characteristic was read\n')
    return self.value

  def WriteValue(self, value, options):
    print('Characteristic was written\n')
    self.value = value

class TestService(Service):
  TEST_SVC_UUID = '00000000-0000-0000-1234-56789abc0100'

  def __init__(self, bus, index):
    Service.__init__(self, bus, index, self.TEST_SVC_UUID,True)
    self.add_characteristic(TestChr(bus, 0, self))

class TestApplication(Application):
  def __init__(self, bus):
    Application.__init__(self, bus)
    self.add_service(TestService(bus, 0))

class TestAdvertisement(Advertisement):
  def __init__(self, bus, index):
    Advertisement.__init__(self, bus, index, 'peripheral')
    self.add_service_uuid(TestService.TEST_SVC_UUID)
    self.include_tx_power = False

def register_ad_cb():
  # callback if registering advertisement was successful
  print('Advertisement registered')

def register_ad_error_cb(error):
  # callback if registering advertisement failed
  print('Advertisement failed to register: ' + str(error))
  mainloop.quit()

def register_app_cb():
  # callback if registering advertisement was successful
  print('Application registered')

def register_app_error_cb(error):
  # callback if registering advertisement failed
  print('Application failed to register: ' + str(error))
  mainloop.quit()



def main():
  global mainloop

  dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

  bus = dbus.SystemBus()

  # Get service manager and advertising manager
  service_manager = get_service_manager(bus)
  ad_manager = get_ad_manager(bus)

  # create gatt services
  app = TestApplication(bus)

  # create advertisement
  advertisement = TestAdvertisement(bus, 0)

  mainloop = GObject.MainLoop()

  # register gatt services
  service_manager.RegisterApplication(app.get_path(), {},
                                      reply_handler=register_app_cb,
                                      error_handler=register_app_error_cb)

  # register advertisement
  ad_manager.RegisterAdvertisement(advertisement.get_path(), {},
                                   reply_handler=register_ad_cb,
                                   error_handler=register_ad_error_cb)

  try:
    mainloop.run()
  except KeyboardInterrupt:
    display.clear()
    display.write_display()

if __name__ == '__main__':
  main()




























  

                     
