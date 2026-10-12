#!/bin/bash
set -e

BITFILE="ft601.bit"
CFGDIR="$(pwd)/cfg"

echo "Iniciando programación FPGA"
echo "Archivo: $BITFILE"

sudo openocd \
  -s "$CFGDIR" \
  -f "$CFGDIR/ch347.cfg" \
  -f "$CFGDIR/xc7_75t.cfg" \
  -c "init; pld load  xc7.pld $BITFILE; exit"


echo "Programación finalizada correctamente"
