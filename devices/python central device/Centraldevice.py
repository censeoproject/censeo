import asyncio
from bleak import BleakClient
import struct

def handle_rotation_change(sender, data):
    rotation = struct.unpack('<L', data)

    print(rotation[0])

async def main():
    async with BleakClient("58:BF:25:9C:4E:C6") as client:
        await client.start_notify("19b10001-e8f2-537e-4f6c-d104768a1214", handle_rotation_change)
        print("connected")
        # Continuously run the loop
        while True:
            await asyncio.sleep(0.001)

# Using asyncio.run() is important to ensure that device disconnects on
# KeyboardInterrupt or other unhandled exception.
asyncio.run(main())