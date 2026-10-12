#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jan 22 14:15:15 2026

@author: ral298
"""

import usb.core

dev = usb.core.find(idVendor=0x0403, idProduct=0x601f)
print(dev)
