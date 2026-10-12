#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jan 28 17:42:37 2026

@author: ral298
"""

 
import PyD3XX
VALUE_SIZE = 4

PyD3XX.SetPrintLevel(PyD3XX.PRINT_ALL)

Status, DeviceCount = PyD3XX.FT_CreateDeviceInfoList() # Create a device info list.
if Status != PyD3XX.FT_OK:
    print(PyD3XX.FT_STATUS_STR[Status] + " | FAILED TO CREATE DEVICE INFO LIST: ABORTING")
    exit()
print(str(DeviceCount) + " Devices detected.")
if (DeviceCount == 0):
    print("NO DEVICES DETECTED: ABORTING")
    exit()

Status, Devices = PyD3XX.FT_GetDeviceInfoList(DeviceCount) # This will create a list.
if Status != PyD3XX.FT_OK:
    print(PyD3XX.FT_STATUS_STR[Status] + " | FAILED TO CREATE DEVICE INFO LIST: ABORTING")
    exit()
for i in range(0, DeviceCount): # Get device info at each index from 0->(DeviceCount - 1).
    if(Devices[i].Flags & PyD3XX.FT_FLAGS_OPENED):
        print("Device " + str(i) + " is opened by another process!")
        continue # Device info not valid. Move onto next device.
    print("---| Device " + str(i) + " |---")

Status, Device = PyD3XX.FT_GetDeviceInfoDetail(0) # Get info of a device at index 0.
if Status != PyD3XX.FT_OK:
    print(PyD3XX.FT_STATUS_STR[Status] + " | WARNING: FAILED TO GET INFO FOR DEVICE 0")
else:
    if(Device.Flags & PyD3XX.FT_FLAGS_OPENED):
        print("Device 0 is opened by another process!")
    else:
        if(Device.Type == PyD3XX.FT_DEVICE_601):
            print("Device 0 is a FT601 device!")
        elif(Device.Type == PyD3XX.FT_DEVICE_600):
            print("Device 0 is a FT600 device!")
            
Status, Count = PyD3XX.FT_ListDevices(PyD3XX.NULL, PyD3XX.FT_LIST_NUMBER_ONLY)
print("Device Count: " + str(Count))
Status, SerialNumber = PyD3XX.FT_ListDevices(0, PyD3XX.FT_LIST_BY_INDEX |
PyD3XX.FT_OPEN_BY_SERIAL_NUMBER)
print("Device 0 Serial Number: " + str(SerialNumber))
Status, Description = PyD3XX.FT_ListDevices(0, PyD3XX.FT_LIST_BY_INDEX |
PyD3XX.FT_OPEN_BY_DESCRIPTION)
print("Device 0 Description: " + str(Description))
Status, SerialNumbers = PyD3XX.FT_ListDevices(Count, PyD3XX.FT_LIST_ALL |
PyD3XX.FT_OPEN_BY_SERIAL_NUMBER)
for i in range(Count):
    print("Device " + str(i) + " Serial Number: " + str(SerialNumbers[i]))
Status, Descriptions = PyD3XX.FT_ListDevices(Count, PyD3XX.FT_LIST_ALL |
PyD3XX.FT_OPEN_BY_DESCRIPTION)
for i in range(Count):
    print("Device " + str(i) + " Description: " + str(Descriptions[i]))
    
    
    
# Status, Device = PyD3XX.FT_GetDeviceInfoDetail(0) # Get info of a device at index 0.
if Status != PyD3XX.FT_OK:
    print(PyD3XX.FT_STATUS_STR[Status] + " | WARNING: FAILED TO GET INFO FOR DEVICE 0")
print("Device (index, serial, desc): 0, " + Device.SerialNumber + ", " + Device.Description);
Status = PyD3XX.FT_Create(0, PyD3XX.FT_OPEN_BY_INDEX, Device)
if Status != PyD3XX.FT_OK:
    print(PyD3XX.FT_STATUS_STR[Status] + " | FAILED TO OPEN DEVICE: ABORTING")
    exit()

print("Successfully opened and closed device by index!");
# Status = PyD3XX.FT_Create("44TXERgJa3h2IHD", PyD3XX.FT_OPEN_BY_SERIAL_NUMBER, Device)
# if Status != PyD3XX.FT_OK:
#     print(PyD3XX.FT_STATUS_STR[Status] + " | FAILED TO OPEN DEVICE: ABORTING")
#     exit()
# PyD3XX.FT_Close(Device)
# print("Successfully opened and closed device by serial number!");
# Status = PyD3XX.FT_Create("TestDevice", PyD3XX.FT_OPEN_BY_DESCRIPTION, Device)
# if Status != PyD3XX.FT_OK:
#     print(PyD3XX.FT_STATUS_STR[Status] + " | FAILED TO OPEN DEVICE: ABORTING")
#     exit()
# PyD3XX.FT_Close(Device)
# print("Successfully opened and closed device by description!");


Pipes = {}
for i in range(2): # Get in and out pipes for channel 1.
    Status, Pipes[i] = PyD3XX.FT_GetPipeInformation(Device, 1, i)
    if Status != PyD3XX.FT_OK:
        print("FAILED TO GET PIPE INFO OF [1," + str(i) +"]: ABORTING")
        exit()

tamano=1024

import random

data_bytes = bytearray()
for i in range(tamano):
    num = random.randint(0, 100)
    data_bytes += num.to_bytes(4, "little")
WriteBuffer = PyD3XX.FT_Buffer.from_bytes(bytes(data_bytes))

# WriteBuffer = PyD3XX.FT_Buffer.from_int(int.from_bytes(ReadBuffer.Value(), "little") + 1)
if(PyD3XX.Platform == "linux") or (PyD3XX.Platform == "darwin"):
    Status, BytesWrote = PyD3XX.FT_WritePipe(Device, int("0x02", 16), WriteBuffer, 4*tamano, 0)
else:
    Status, BytesWrote = PyD3XX.FT_WritePipe(Device, Pipes[0], WriteBuffer, 1, PyD3XX.NULL)


if(Status == PyD3XX.FT_OK):
    WriteValue = format(int.from_bytes(WriteBuffer.Value(), "little"), "x").zfill(1*2)
    print("Wrote 0x" + WriteValue + " to Channel 1!")
else:
    print(PyD3XX.FT_STATUS_STR[Status] + " | Failed to write data!: ABORTING")
    exit()

if(PyD3XX.Platform == "linux") or (PyD3XX.Platform == "darwin"):
    Status, ReadBuffer, BytesRead = PyD3XX.FT_ReadPipe(Device, int("0x82", 16), 4*tamano, 0)
else:
    Status, ReadBuffer, BytesRead = PyD3XX.FT_ReadPipe(Device, Pipes[1], 1, PyD3XX.NULL)

def GetValue(Buffer: PyD3XX.FT_Buffer, Index: int) -> int:
    Value = 0
    for i in range(VALUE_SIZE):
        Value += (Buffer.Value()[Index + i]) << (i*8)
    return Value

# ReadBuffer
# dato=(ReadBuffer).Value()


import numpy as np
arr = np.frombuffer(ReadBuffer.Value(), dtype=np.int32)
ori = np.frombuffer(WriteBuffer.Value(), dtype=np.int32)


print("array ori:",ori)
print("array:",arr)
PyD3XX.FT_Close(Device)

if(Status == PyD3XX.FT_OK):
    ReadValue = format(int.from_bytes(ReadBuffer.Value(), "little"), "x").zfill(1*2)
    print("Channel 1 = 0x" + ReadValue)
    print("WriteValue==ReadValue:",WriteValue==ReadValue)
else:
    print(PyD3XX.FT_STATUS_STR[Status] + " | Failed to read data!: ABORTING")
    exit()
