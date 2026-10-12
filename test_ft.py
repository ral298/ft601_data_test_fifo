import PyD3XX

CHANNEL_COUNT = 1 # How many channels we're streaming.
STREAM_SIZE = 100*4 # How many bytes each read pipe call is.
VALUE_SIZE = 4 # How many bytes each counter value is.
# Total counter values = STREAM_SIZE / VALUE_SIZE
FIXED_TRANSFER_SIZE = False # Fix the transfer size. DO NOT enable unless STREAM_SIZE is a multiple of MaxPacketSize.
LOOP_COUNT = 1 # Set to True to loop forever. Set to an integer X value to loop X times.
CHECK_INCREMENT = True # Stop streaming if a counter value is not greater than the previous by 1.
PRINT_COUNT = True # Set to true if you want to print every single value.

def GetValue(Buffer: PyD3XX.FT_Buffer, Index: int) -> int:
    Value = 0
    for i in range(VALUE_SIZE):
        Value += (Buffer.Value()[Index + i]) << (i*8)
    return Value

if(CHECK_INCREMENT) and ((STREAM_SIZE < (VALUE_SIZE*2)) or ((STREAM_SIZE % VALUE_SIZE) != 0)):
    print("ERROR: STREAM_SIZE must be divisible by " + str(VALUE_SIZE) + " and " + str(VALUE_SIZE*2) + " or greater.")
    exit()
if(CHANNEL_COUNT > 4):
    print("ERROR: CHANNEL_COUNT must be 4 or less.")
    exit()

Status, DeviceCount = PyD3XX.FT_CreateDeviceInfoList() # Create a device info list.
if Status != PyD3XX.FT_OK:
    print(PyD3XX.FT_STATUS_STR[Status] + " | FAILED TO CREATE DEVICE INFO LIST: ABORTING")
    exit()
print(str(DeviceCount) + " Devices detected.")
if (DeviceCount == 0):
    print("NO DEVICES DETECTED: ABORTING")
    exit()

Status, Device = PyD3XX.FT_GetDeviceInfoDetail(0) # Get info of a device at index 0.
Status = PyD3XX.FT_Create(0, PyD3XX.FT_OPEN_BY_INDEX, Device) # Open the device we're using.
if Status != PyD3XX.FT_OK:
    print(PyD3XX.FT_STATUS_STR[Status] + " | FAILED TO OPEN DEVICE: ABORTING")
    exit()

Pipes = {}
for i in range(2): # Get in and out pipes for channel 1.
    Status, Pipes[i] = PyD3XX.FT_GetPipeInformation(Device, 1, i)
    if Status != PyD3XX.FT_OK:
        print("FAILED TO GET PIPE INFO OF [1," + str(i) +"]: ABORTING")
        exit()
Status = PyD3XX.FT_SetStreamPipe(Device, True, True, PyD3XX.NULL, 1024)
if Status != PyD3XX.FT_OK:
    print("FAILED TO SET STREAM SIZE FOR ALL PIPES.")
    exit()
else:
    print("Stream size for all pipes set to 1024 byte!")
# Status = PyD3XX.FT_ClearStreamPipe(Device, False, False, Pipes[1])
# if Status != PyD3XX.FT_OK:
#     print("FAILED TO CLEAR STREAM SIZE FOR CH1 READ PIPE.")
#     exit()
# else:
#     print("Cleared stream size for CH1 read pipe!")
    
# data_bytes = bytearray()

# for i in range(10):
#     data_bytes += i.to_bytes(4, byteorder="little")

# data_bytes tiene exactamente 40 bytes
data_bytes = (0xAABBCCDD).to_bytes(4, "little")

WriteBuffer = PyD3XX.FT_Buffer.from_bytes(bytes(data_bytes))

# =========================================
# 5) Enviar al FT601 (PIPE OUT)
# =========================================
Status, BytesWrote = PyD3XX.FT_WritePipeEx(
    Device,
    Pipes[0].PipeID,              # Pipe OUT (write)
    WriteBuffer,
    20*len(data_bytes),       # 40 bytes
    0
)

if Status == PyD3XX.FT_OK:
    print("Enviadas 10 palabras de 32 bits")
    print("Bytes escritos:", BytesWrote)
else:
    print("Error al escribir en el pipe")
    
print("PIPE OUT ID:", Pipes[0].PipeID)
print("PIPE OUT TYPE:", Pipes[0].PipeType)

