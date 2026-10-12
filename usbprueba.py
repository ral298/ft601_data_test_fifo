#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import usb.core
import usb.util
import struct

# Buscar FT601
dev = usb.core.find(idVendor=0x0403, idProduct=0x601f)
if dev is None:
    raise RuntimeError("FT601 no encontrado")

# Configurar dispositivo
dev.set_configuration()
cfg = dev.get_active_configuration()
intf = cfg[(1, 0)]   # Interface FIFO

# Desconectar driver del kernel si existe (Linux)
if dev.is_kernel_driver_active(intf.bInterfaceNumber):
    dev.detach_kernel_driver(intf.bInterfaceNumber)

usb.util.claim_interface(dev, intf.bInterfaceNumber)

# Endpoints FT601 (interface 1)
ep_out = usb.util.find_descriptor(
    intf, bEndpointAddress=0x02
)
ep_in = usb.util.find_descriptor(
    intf, bEndpointAddress=0x82
)

# Datos a enviar (10 palabras de 32 bits)
tx = list(range(10))
tx_bytes = b''.join(struct.pack("<I", x) for x in tx)

print("TX:", tx)
ep_out.write(tx_bytes)

# Leer exactamente 40 bytes
rx_bytes = ep_in.read(4 * len(tx), timeout=2000)

rx = [
    struct.unpack("<I", rx_bytes[i:i+4])[0]
    for i in range(0, len(rx_bytes), 4)
]

print("RX:", rx)

usb.util.release_interface(dev, intf.bInterfaceNumber)
usb.util.dispose_resources(dev)
