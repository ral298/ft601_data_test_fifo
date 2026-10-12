
#!/usr/bin/env python3
import time
import PyD3XX


def mostrar_estado(status):
    try:
        return PyD3XX.FT_STATUS_STR[status]
    except (KeyError, TypeError):
        return str(status)


# Detectar dispositivos FTDI
status, cantidad = PyD3XX.FT_CreateDeviceInfoList()

if status != PyD3XX.FT_OK:
    raise RuntimeError(
        f"No se pudo enumerar: {mostrar_estado(status)}"
    )

if cantidad == 0:
    print("No se detectaron dispositivos FTDI.")
    raise SystemExit(1)

print(f"\nDispositivos detectados: {cantidad}")

for i in range(cantidad):
    status, info = PyD3XX.FT_GetDeviceInfoDetail(i)

    if status == PyD3XX.FT_OK:
        print(
            f"[{i}] "
            f"Serie: {getattr(info, 'SerialNumber', '?')} | "
            f"Descripción: {getattr(info, 'Description', '?')}"
        )

indice = int(input("\nÍndice del FT601 que quieres reiniciar: "))

if not 0 <= indice < cantidad:
    raise SystemExit("Índice inválido.")

status, device = PyD3XX.FT_GetDeviceInfoDetail(indice)

if status != PyD3XX.FT_OK:
    raise RuntimeError("No se pudo obtener el dispositivo.")

status = PyD3XX.FT_Create(
    indice,
    PyD3XX.FT_OPEN_BY_INDEX,
    device
)

if status != PyD3XX.FT_OK:
    raise RuntimeError(
        f"No se pudo abrir: {mostrar_estado(status)}. "
        "Cierra antes los programas que estén usando el FT601."
    )

print("\nSelecciona la operación:")
print("1. Reiniciar el puerto USB")
print("2. Ciclo de alimentación del puerto USB")

opcion = input("Opción [1]: ").strip() or "1"

if opcion == "1":
    funcion = getattr(PyD3XX, "FT_ResetDevicePort", None)
    nombre = "FT_ResetDevicePort"
elif opcion == "2":
    funcion = getattr(PyD3XX, "FT_CycleDevicePort", None)
    nombre = "FT_CycleDevicePort"
else:
    PyD3XX.FT_Close(device)
    raise SystemExit("Opción inválida.")

if funcion is None:
    PyD3XX.FT_Close(device)
    raise SystemExit(
        f"Tu versión de PyD3XX no expone {nombre}. "
        "Comprueba la versión instalada."
    )

print(f"\nEjecutando {nombre}...")
status = funcion(device)

print("Resultado:", mostrar_estado(status))

print("Esperando a que vuelva a enumerarse...")
time.sleep(3)

status, cantidad = PyD3XX.FT_CreateDeviceInfoList()

if status == PyD3XX.FT_OK:
    print(f"Dispositivos detectados después del reinicio: {cantidad}")
    for i in range(cantidad):
        s, info = PyD3XX.FT_GetDeviceInfoDetail(i)
        if s == PyD3XX.FT_OK:
            print(
                f"[{i}] {getattr(info, 'SerialNumber', '?')} | "
                f"{getattr(info, 'Description', '?')}"
            )
else:
    print("No se pudo volver a enumerar el dispositivo.")

print(
    "\nEl identificador del dispositivo puede haber cambiado. "
    "Vuelve a abrirlo antes de realizar nuevas transferencias."
)
