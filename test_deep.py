#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jan 28 17:36:29 2026

@author: ral298
"""

import PyD3XX

CHANNEL_COUNT = 1  # Número de canales (FT601 soporta hasta 4)
STREAM_SIZE = 1024  # Múltiplo de 1024 para WinUSB (256 valores de 32 bits)
VALUE_SIZE = 4  # 32 bits = 4 bytes
FIXED_TRANSFER_SIZE = True  # Habilitado porque STREAM_SIZE es múltiplo de 1024
LOOP_COUNT = 10  # Número de iteraciones
CHECK_INCREMENT = False  # Deshabilitado para simplificar
PRINT_COUNT = False

def GetValue(Buffer: PyD3XX.FT_Buffer, Index: int) -> int:
    """Extrae un valor de 32 bits del buffer."""
    Value = 0
    for i in range(VALUE_SIZE):
        Value += (Buffer.Value()[Index + i]) << (i * 8)
    return Value

# 1. Inicialización y detección del dispositivo
Status, DeviceCount = PyD3XX.FT_CreateDeviceInfoList()
if Status != PyD3XX.FT_OK:
    print(PyD3XX.FT_STATUS_STR[Status] + " | FAILED TO CREATE DEVICE INFO LIST: ABORTING")
    exit()
print(f"{DeviceCount} dispositivo(s) detectado(s).")
if DeviceCount == 0:
    print("NO DEVICES DETECTED: ABORTING")
    exit()

# 2. Obtener información del dispositivo y abrirlo
Status, Device = PyD3XX.FT_GetDeviceInfoDetail(0)
Status = PyD3XX.FT_Create(0, PyD3XX.FT_OPEN_BY_INDEX, Device)
if Status != PyD3XX.FT_OK:
    print(PyD3XX.FT_STATUS_STR[Status] + " | FAILED TO OPEN DEVICE: ABORTING")
    exit()

# 3. Obtener información de los pipes (canal 1)
Pipes = {}
for i in range(2):  # i=0: pipe de escritura, i=1: pipe de lectura
    Status, Pipes[i] = PyD3XX.FT_GetPipeInformation(Device, 1, i)
    if Status != PyD3XX.FT_OK:
        print(f"FAILED TO GET PIPE INFO OF [1,{i}]: ABORTING")
        exit()
    print(f"Pipe {i}: ID={hex(Pipes[i].PipeID)}, Type={Pipes[i].PipeType}")

# 4. Configurar el stream pipe (tamaño fijo de transferencia)
if FIXED_TRANSFER_SIZE:
    Status = PyD3XX.FT_SetStreamPipe(Device, True, True, PyD3XX.NULL, STREAM_SIZE)
    if Status != PyD3XX.FT_OK:
        print("FAILED TO SET STREAM SIZE FOR ALL PIPES.")
        exit()
    else:
        print(f"Stream size set to {STREAM_SIZE} bytes.")

# 5. Preparar datos de prueba (10 palabras de 32 bits = 40 bytes)
data_bytes = bytearray()
for i in range(10):
    data_bytes += i.to_bytes(4, "little")
WriteBuffer = PyD3XX.FT_Buffer.from_bytes(bytes(data_bytes))

# 6. Enviar datos al FT601 (pipe OUT)
Status, BytesWrote = PyD3XX.FT_WritePipeEx(
    Device,
    Pipes[0].PipeID,   # Pipe OUT (escritura)
    WriteBuffer,
    len(data_bytes),   # 40 bytes
    0                  # Timeout sincrónico
)
if Status == PyD3XX.FT_OK:
    print(f"Enviadas {BytesWrote} bytes (10 palabras de 32 bits)")
else:
    print("Error al escribir en el pipe:", PyD3XX.FT_STATUS_STR[Status])

# # 7. Leer datos del FT601 (pipe IN) - si el FPGA responde
# if LOOP_COUNT > 0:
#     for i in range(LOOP_COUNT):
#         Status, ReadBuffer, BytesRead = PyD3XX.FT_ReadPipeEx(
#             Device,
#             Pipes[1].PipeID,   # Pipe IN (lectura)
#             STREAM_SIZE,
#             0
#         )
#         if Status == PyD3XX.FT_OK:
#             print(f"Iteración {i+1}: Leídos {BytesRead} bytes")
#             # Procesar los valores de 32 bits
#             for j in range(0, BytesRead, VALUE_SIZE):
#                 value = GetValue(ReadBuffer, j)
#                 if PRINT_COUNT:
#                     print(f"  Valor {j//VALUE_SIZE}: {value}")
#         else:
#             print("Error al leer del pipe:", PyD3XX.FT_STATUS_STR[Status])
#             break

# # 8. Limpiar y cerrar
# Status = PyD3XX.FT_ClearStreamPipe(Device, True, True, PyD3XX.NULL)
# if Status != PyD3XX.FT_OK:
#     print("WARNING: Failed to clear stream pipe.")













PyD3XX.FT_Close(Device)