#!/usr/bin/env python3

# This file contains the runtime script to start advertising and registering services

import dbus
import dbus.exceptions
import dbus.mainloop.glib
import dbus.service
from threading import Thread
import time

import array
try:
  from gi.repository import GObject
except ImportError:
  import gobject as GObject
import sys

# get methods from other bluetooth implementation file
from bluez_bluetoothle_components import *

mainloop = None

class Fan1Rpm(Characteristic):
  FAN1_UUID = '00000000-0000-0000-1234-56789abc0000'

  def __init__(self, bus, index, service):
    Characteristic.__init__(self, bus, index, self.FAN1_UUID, ['read'], service)
    self.value = 0
    self.notifying = True

  def ReadValue(self, options):
    print('Characteristic was read\n')
    return [dbus.Byte(self.value)]

  def UpdateValue(self,v):
    # get new value from MMAP
    self.value = int(v*100)
    self.notify_value_changed()
    
  def notify_value_changed(self):
    if not self.notifying:
      return
    self.PopertiesChanged(GATT_CHRC_IFACE, {'Value':[dbus.Byte(self.value)]}, [])

  def StartNotify(self):
    if self.notifying:
      return # nothing to do
    self.notifying = True
    self.notify_value_changed()
    return

  def StopNotify(self):
    if not self.notifying:
      print('Not notifying, nothing to do')
      return
    self.notifying = False


class Fan2Rpm(Characteristic):
  FAN2_UUID = '00000000-0000-0000-1234-56789abc0001'

  def __init__(self, bus, index, service):
    Characteristic.__init__(self, bus, index, self.FAN2_UUID, ['read','notify'], service)
    self.value = 0
    self.notifying = True

  def ReadValue(self, options):
    print('Characteristic was read\n')
    return [dbus.Byte(self.value)]

  def UpdateValue(self,v):
    # get new value from MMAP
    self.value = int(v*100)
    self.notify_value_changed()
    
  def notify_value_changed(self):
    if not self.notifying:
      return
    self.PopertiesChanged(GATT_CHRC_IFACE, {'Value':[dbus.Byte(self.value)]}, [])

  def StartNotify(self):
    if self.notifying:
      return # nothing to do
    self.notifying = True
    self.notify_value_changed()
    return

  def StopNotify(self):
    if not self.notifying:
      print('Not notifying, nothing to do')
      return
    self.notifying = False

class BattVoltage(Characteristic):
  BATT_UUID = '00000000-0000-0000-1234-56789abc0002'

  def __init__(self, bus, index, service):
    Characteristic.__init__(self, bus, index, self.BATT_UUID, ['read','notify'], service)
    self.value = 0
    self.notifying = True

  def ReadValue(self, options):
    print('Characteristic was read\n')
    return [dbus.Byte(self.value)]

  def UpdateValue(self,v):
    # get new value from MMAP
    self.value = int(v*100)
    self.notify_value_changed()
    
  def notify_value_changed(self):
    if not self.notifying:
      return
    self.PopertiesChanged(GATT_CHRC_IFACE, {'Value':[dbus.Byte(self.value)]}, [])

  def StartNotify(self):
    if self.notifying:
      return # nothing to do
    self.notifying = True
    self.notify_value_changed()
    return

  def StopNotify(self):
    if not self.notifying:
      print('Not notifying, nothing to do')
      return
    self.notifying = False


class Temperature(Characteristic):
  TEMP_UUID = '00000000-0000-0000-1234-56789abc0003'

  def __init__(self, bus, index, service):
    Characteristic.__init__(self, bus, index, self.TEMP_UUID, ['read','notify'], service)
    self.value = 0
    self.notifying = True

  def ReadValue(self, options):
    print('Characteristic was read\n')
    return [dbus.Byte(self.value)]

  def UpdateValue(self,v):
    # get new value from MMAP
    self.value = int(v*100)
    self.notify_value_changed()
    
  def notify_value_changed(self):
    if not self.notifying:
      return
    self.PopertiesChanged(GATT_CHRC_IFACE, {'Value':[dbus.Byte(self.value)]}, [])

  def StartNotify(self):
    if self.notifying:
      return # nothing to do
    self.notifying = True
    self.notify_value_changed()
    return

  def StopNotify(self):
    if not self.notifying:
      print('Not notifying, nothing to do')
      return
    self.notifying = False


class LinePressure(Characteristic):
  LINE_UUID = '00000000-0000-0000-1234-56789abc0004'

  def __init__(self, bus, index, service):
    Characteristic.__init__(self, bus, index, self.LINE_UUID, ['read','notify'], service)
    self.value = 0
    self.notifying = True

  def ReadValue(self, options):
    print('Characteristic was read\n')
    return [dbus.Byte(self.value)]

  def UpdateValue(self,v):
    # get new value from MMAP
    self.value = int(v*100)
    self.notify_value_changed()
    
  def notify_value_changed(self):
    if not self.notifying:
      return
    self.PopertiesChanged(GATT_CHRC_IFACE, {'Value':[dbus.Byte(self.value)]}, [])

  def StartNotify(self):
    if self.notifying:
      return # nothing to do
    self.notifying = True
    self.notify_value_changed()
    return

  def StopNotify(self):
    if not self.notifying:
      print('Not notifying, nothing to do')
      return
    self.notifying = False

# provides a control point to start and stop the test
class TestCtlPoint(Characteristic):
  TEST_UUID = '00000000-0000-0000-1234-56789abc0005'
  #action = None

  def __init__(self, bus, index, service):
    Characteristic.__init__(self, bus, index, self.TEST_UUID, ['read','write','notify'], service)
    self.value = 0
    self.notifying = True
    self.handle = None
    #self.action = callback

  def AttachService(self, service):
    self.handle = service

  def ReadValue(self, options):
    print('Characteristic was read\n')
    return [dbus.Byte(self.value)]

  def WriteValue(self, value, options):
    print('control point value was written')
    #self.handle.StartTest.__func__
    self.UpdateValue(value)
    #self.action.__func__

  def UpdateValue(self,v):
    # get new value from MMAP
    self.value = v
    self.notify_value_changed()
    
  def notify_value_changed(self):
    if not self.notifying:
      return
    self.PopertiesChanged(GATT_CHRC_IFACE, {'Value':[dbus.Byte(self.value)]}, [])

  def StartNotify(self):
    if self.notifying:
      return # nothing to do
    self.notifying = True
    self.notify_value_changed()
    return

  def StopNotify(self):
    if not self.notifying:
      print('Not notifying, nothing to do')
      return
    self.notifying = False

# Service provides remote testing for functionality
class TestService(Service):
  TEST_SVC_UUID = '00000000-0000-0000-1234-56789abc0100'

  def __init__(self, bus, index):
    Service.__init__(self, bus, index, self.TEST_SVC_UUID,True)
    self.add_characteristic(Fan1Rpm(bus, 0, self))
    self.add_characteristic(Fan2Rpm(bus, 1, self))
    self.add_characteristic(BattVoltage(bus, 2, self))
    self.add_characteristic(Temperature(bus, 3, self))
    self.add_characteristic(LinePressure(bus, 4, self))
    self.add_characteristic(TestCtlPoint(bus, 5, self))
    
    #self.characteristics[5].AttachService(self)

  def WatchForTestRequest(self):
    print('watch thread started')
    while self.characteristics[5].value is 0:
      print('still watching')
      time.sleep(1)
    self.StartTest()
    
  def StartTest(self):
    print('made it to StartTest')
    t = Thread(target=self.RunTest())
    t.start()
    
  def RunTest(self):
    print('made it to RunTest')
    for x in range (60): # iterate 60 times over 30 seconds
      sensor_data = []
      file = open('/home/pi/mosquito_log/sensor_data.txt','r')
      for y in file.read().split('\n'):
        if y.isdigit():
          sensor_data.append(float(y))
      print(sensor_data)
      self.characteristics[0].UpdateValue(sensor_data[0])
      self.characteristics[1].UpdateValue(sensor_data[1])
      self.characteristics[2].UpdateValue(sensor_data[2])
      self.characteristics[3].UpdateValue(sensor_data[3])
      self.characteristics[4].UpdateValue(sensor_data[4])
      print('made it through iteration')
      time.sleep(0.5)
    self.characteristics[5].UpdateValue(0)
    w = Thread(target=self.WatchForTestRequest())
    w.start()
    
      

class TestApplication(Application):
  def __init__(self, bus):
    Application.__init__(self, bus)
    self.add_service(TestService(bus, 0))

  def StartWatchForTest(self):
    w = Thread(target=self.services[0].WatchForTestRequest())
    w.start()
    

class TestAdvertisement(Advertisement):
  def __init__(self, bus, index):
    Advertisement.__init__(self, bus, index, 'peripheral')
    self.add_service_uuid(TestService.TEST_SVC_UUID)
    self.include_tx_power = False
    self.add_local_name('MosquitoTrap')

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

  adapter = find_adapter(bus)
  if not adapter:
    print('LEAdvertisingManager1 interface not found')
    return

  adapter_props = dbus.Interface(bus.get_object(BLUEZ_SERVICE_NAME,adapter),
                                 "org.freedesktop.DBus.Properties")

  adapter_props.Set("org.bluez.Adapter1","Powered", dbus.Boolean(1))

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

  app.StartWatchForTest()

  try:
    mainloop.run()
  except KeyboardInterrupt:
    display.clear()
    display.write_display()

if __name__ == '__main__':
  main()




























  

                     
