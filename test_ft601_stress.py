#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Prueba de estrés FT601 <-> FPGA.
Cada interacción envía 40 palabras de 32 bits (160 bytes), lee la respuesta
y compara palabra por palabra.

Uso:
    python3 test_ft601_stress.py
    python3 test_ft601_stress.py --batches 10000
    python3 test_ft601_stress.py --batches 1000 --range 0 100

Por defecto se prueban palabras aleatorias de 32 bits completos.
Usa "--range 0 100" para conservar el rango del script original.
"""

import argparse
import random
import sys
import time

import PyD3XX

WORDS_PER_BATCH =1024
BYTES_PER_WORD = 4
BYTES_PER_BATCH = WORDS_PER_BATCH * BYTES_PER_WORD


def status_text(status):
    try:
        return str(PyD3XX.FT_STATUS_STR[status])
    except (KeyError, IndexError, TypeError):
        return str(status)


def check_status(status, action):
    if status != PyD3XX.FT_OK:
        print(f"ERROR: {action}: {status_text(status)}")
        return False
    return True


def open_ft601():
    status, device_count = PyD3XX.FT_CreateDeviceInfoList()
    if not check_status(status, "crear lista de dispositivos"):
        return None, None

    print(f"Dispositivos detectados: {device_count}")
    if device_count == 0:
        print("No se detectó ningún FT601. Revisa USB, alimentación y controlador.")
        return None, None

    status, device = PyD3XX.FT_GetDeviceInfoDetail(0)
    if not check_status(status, "obtener información del dispositivo 0"):
        return None, None

    if device.Flags & PyD3XX.FT_FLAGS_OPENED:
        print("ERROR: el dispositivo 0 ya está abierto por otro proceso.")
        return None, None

    if getattr(device, "Type", None) == getattr(PyD3XX, "FT_DEVICE_601", object()):
        print("Dispositivo 0 identificado como FT601.")
    elif getattr(device, "Type", None) == getattr(PyD3XX, "FT_DEVICE_600", object()):
        print("AVISO: el dispositivo 0 aparece como FT600, no FT601.")

    print(f"Serie: {getattr(device, 'SerialNumber', 'desconocida')}")
    print(f"Descripción: {getattr(device, 'Description', 'desconocida')}")

    status = PyD3XX.FT_Create(0, PyD3XX.FT_OPEN_BY_INDEX, device)
    if not check_status(status, "abrir dispositivo"):
        return None, None

    pipes = {}
    if PyD3XX.Platform not in ("linux", "darwin"):
        for i in range(2):
            status, pipe = PyD3XX.FT_GetPipeInformation(device, 1, i)
            if not check_status(status, f"obtener información del pipe {i}"):
                PyD3XX.FT_Close(device)
                return None, None
            pipes[i] = pipe

    return device, pipes


def write_batch(device, pipes, raw_bytes):
    write_buffer = PyD3XX.FT_Buffer.from_bytes(raw_bytes)

    if PyD3XX.Platform in ("linux", "darwin"):
        status, bytes_written = PyD3XX.FT_WritePipe(
            device, 0x02, write_buffer, len(raw_bytes), 0
        )
    else:
        # Se conserva la forma de llamada usada en el script original.
        status, bytes_written = PyD3XX.FT_WritePipe(
            device, pipes[0], write_buffer, 1, PyD3XX.NULL
        )

    return status, bytes_written


def read_batch(device, pipes):
    if PyD3XX.Platform in ("linux", "darwin"):
        status, read_buffer, bytes_read = PyD3XX.FT_ReadPipe(
            device, 0x82, BYTES_PER_BATCH, 0
        )
    else:
        status, read_buffer, bytes_read = PyD3XX.FT_ReadPipe(
            device, pipes[1], 1, PyD3XX.NULL
        )

    if status != PyD3XX.FT_OK:
        return status, b"", bytes_read if "bytes_read" in locals() else 0

    raw = bytes(read_buffer.Value())
    # bytes_read es el número informado por el driver; no asumir que siempre
    # coincide con la longitud del objeto Buffer.
    usable = min(max(int(bytes_read), 0), len(raw))
    return status, raw[:usable], int(bytes_read)


def to_bytes(words):
    return b"".join(word.to_bytes(BYTES_PER_WORD, "little") for word in words)


def main():
    parser = argparse.ArgumentParser(
        description="Prueba de estrés FT601: 40 palabras por transacción."
    )
    parser.add_argument(
        "--batches", type=int, default=None,
        help="cantidad de transacciones de 40 palabras (predeterminado: preguntar)"
    )
    parser.add_argument(
        "--range", dest="value_range", nargs=2, type=lambda x: int(x, 0),
        metavar=("MIN", "MAX"), default=(0, 0xFFFFFFFF),
        help="rango inclusivo de valores aleatorios; acepta decimal o 0x... "
             "(predeterminado: 0 a 0xFFFFFFFF)"
    )
    parser.add_argument(
        "--seed", type=int, default=None,
        help="semilla aleatoria para reproducir una prueba"
    )
    parser.add_argument(
        "--progress-every", type=int, default=10,
        help="mostrar avance cada N transacciones (predeterminado: 10)"
    )
    args = parser.parse_args()

    if args.batches is None:
        answer = input("¿Cuántas transacciones de 40 datos quieres ejecutar? [1000]: ").strip()
        args.batches = int(answer) if answer else 1000

    if args.batches <= 0:
        print("La cantidad de transacciones debe ser mayor que cero.")
        return 2

    minimum, maximum = args.value_range
    if minimum < 0 or maximum > 0xFFFFFFFF or minimum > maximum:
        print("Rango inválido. Debe cumplir 0 <= MIN <= MAX <= 0xFFFFFFFF.")
        return 2

    if args.progress_every <= 0:
        args.progress_every = 1

    if args.seed is not None:
        random.seed(args.seed)

    # Reduce el ruido de la biblioteca durante pruebas largas, si la versión
    # instalada expone PRINT_ERRORS. En caso contrario conserva PRINT_ALL.
    print_level = getattr(PyD3XX, "PRINT_ERRORS", getattr(PyD3XX, "PRINT_ALL", None))
    if print_level is not None:
        PyD3XX.SetPrintLevel(print_level)

    print("\n=== Prueba de estrés FT601 ===")
    print(f"Transacciones planeadas: {args.batches:,}")
    print(f"Palabras por transacción: {WORDS_PER_BATCH}")
    print(f"Bytes por transacción: {BYTES_PER_BATCH}")
    print(f"Palabras planeadas: {args.batches * WORDS_PER_BATCH:,}")
    print(f"Rango aleatorio: {minimum} .. {maximum} (0x{minimum:08X} .. 0x{maximum:08X})")
    print("Se espera que la FPGA devuelva exactamente los mismos datos.\n")

    device, pipes = open_ft601()
    if device is None:
        return 1

    batches_attempted = 0
    batches_good = 0
    batches_bad = 0
    words_tested = 0
    words_good = 0
    words_bad = 0
    failures = []
    start_time = time.monotonic()
    abort_reason = None

    try:
        for batch_number in range(1, args.batches + 1):
            expected_words = [
                random.randint(minimum, maximum) for _ in range(WORDS_PER_BATCH)
            ]
            raw_write = to_bytes(expected_words)
            batches_attempted += 1
            batch_failures_before = len(failures)

            # Escribir el bloque completo.
            status, bytes_written = write_batch(device, pipes, raw_write)
            if status != PyD3XX.FT_OK or int(bytes_written) != BYTES_PER_BATCH:
                words_tested += WORDS_PER_BATCH
                words_bad += WORDS_PER_BATCH
                batches_bad += 1
                description = status_text(status)
                reason = (
                    f"falló la escritura ({description}); "
                    f"bytes escritos={bytes_written}, esperados={BYTES_PER_BATCH}"
                )
                for i, expected in enumerate(expected_words, start=1):
                    failures.append({
                        "batch": batch_number, "index": i,
                        "expected": expected, "actual": None, "reason": reason
                    })
                abort_reason = (
                    "La escritura no completó los 160 bytes. Se detiene la prueba "
                    "para evitar desalinear las siguientes lecturas."
                )
                break

            # Leer la respuesta del bloque.
            status, raw_read, bytes_read = read_batch(device, pipes)
            if status != PyD3XX.FT_OK:
                words_tested += WORDS_PER_BATCH
                words_bad += WORDS_PER_BATCH
                batches_bad += 1
                description = status_text(status)
                reason = f"falló la lectura ({description})"
                for i, expected in enumerate(expected_words, start=1):
                    failures.append({
                        "batch": batch_number, "index": i,
                        "expected": expected, "actual": None, "reason": reason
                    })
                abort_reason = "Falló la lectura; se detiene la prueba."
                break

            # Comparar palabra por palabra, respetando little-endian.
            batch_good_words = 0
            for i, expected in enumerate(expected_words):
                offset = i * BYTES_PER_WORD
                if offset + BYTES_PER_WORD <= len(raw_read):
                    actual = int.from_bytes(
                        raw_read[offset:offset + BYTES_PER_WORD], "little"
                    )
                    words_tested += 1
                    if actual == expected:
                        words_good += 1
                        batch_good_words += 1
                    else:
                        words_bad += 1
                        failures.append({
                            "batch": batch_number, "index": i + 1,
                            "expected": expected, "actual": actual,
                            "reason": "valor recibido distinto al enviado"
                        })
                else:
                    words_tested += 1
                    words_bad += 1
                    failures.append({
                        "batch": batch_number, "index": i + 1,
                        "expected": expected, "actual": None,
                        "reason": (
                            f"palabra ausente: lectura de {bytes_read} bytes "
                            f"(se necesitan {BYTES_PER_BATCH})"
                        )
                    })

            if len(failures) == batch_failures_before:
                batches_good += 1
            else:
                batches_bad += 1

            # Si el tamaño no fue exactamente el esperado, parar para no correr
            # el riesgo de comparar palabras de transacciones diferentes.
            if bytes_read != BYTES_PER_BATCH or len(raw_read) != BYTES_PER_BATCH:
                abort_reason = (
                    f"Lectura incompleta/inesperada en transacción {batch_number}: "
                    f"{bytes_read} bytes informados y {len(raw_read)} disponibles; "
                    f"se esperaban {BYTES_PER_BATCH}. Se detiene la prueba."
                )

            if (
                batch_number == 1
                or batch_number % args.progress_every == 0
                or batch_number == args.batches
            ):
                elapsed = time.monotonic() - start_time
                print(
                    f"Transacción {batch_number:,}/{args.batches:,} | "
                    f"datos OK: {words_good:,} | malos: {words_bad:,} | "
                    f"tiempo: {elapsed:.1f} s"
                )

            if abort_reason:
                break

    except KeyboardInterrupt:
        abort_reason = "Prueba interrumpida por el usuario (Ctrl+C)."
    except Exception as exc:
        abort_reason = f"Excepción inesperada: {type(exc).__name__}: {exc}"
    finally:
        try:
            PyD3XX.FT_Close(device)
        except Exception as exc:
            print(f"AVISO: no se pudo cerrar limpiamente el dispositivo: {exc}")

    elapsed = time.monotonic() - start_time
    print("\n" + "=" * 58)
    print("RESUMEN DE LA PRUEBA")
    print("=" * 58)
    print(f"Transacciones intentadas: {batches_attempted:,}")
    print(f"Transacciones buenas:     {batches_good:,}")
    print(f"Transacciones malas:      {batches_bad:,}")
    print(f"Palabras comprobadas:     {words_tested:,}")
    print(f"Palabras buenas:          {words_good:,}")
    print(f"Palabras malas:           {words_bad:,}")
    if words_tested:
        print(f"Porcentaje correcto:      {100.0 * words_good / words_tested:.6f}%")
        print(f"Porcentaje de error:      {100.0 * words_bad / words_tested:.6f}%")
    print(f"Tiempo total:             {elapsed:.2f} s")
    if elapsed > 0:
        print(f"Rendimiento comprobado:   {words_tested / elapsed:.2f} palabras/s")
        print(f"Rendimiento comprobado [MB/s]:   {((words_tested / elapsed)*32/8/(10**6)):.2f} MB/s")

    if abort_reason:
        print(f"\nMOTIVO DE PARADA: {abort_reason}")

    if failures:
        print(f"\nERRORES DETECTADOS ({len(failures):,} palabras):")
        for failure in failures:
            expected = f"0x{failure['expected']:08X}"
            actual = (
                f"0x{failure['actual']:08X}"
                if failure["actual"] is not None else "SIN DATO"
            )
            print(
                f"  Transacción {failure['batch']}, dato {failure['index']:02d}: "
                f"enviado={expected}, recibido={actual} | {failure['reason']}"
            )
    else:
        print("\n¡Sin errores! Todos los datos comprobados coincidieron.")

    print("\nPrueba finalizada.")
    return 0 if words_bad == 0 and abort_reason is None else 1


if __name__ == "__main__":
    sys.exit(main())
