#!/usr/bin/env python3
from PIL import Image
import serial
import sys


# Open up image
im = Image.open("image.png" if len(sys.argv) < 2 else sys.argv[1])
resize_x = 256
resize_y = 256


# Convert image to black & white
im = im.convert('1', dither=Image.NONE)
im = im.resize((resize_x, resize_y))

im_width, im_height = im.size
im_pixels = im.getdata()


# Convert image into array
array = []

for j in range(0, im_height):
    x_bytes = 0
    for i in range(0, im_width):
        current_pixel = im_pixels[j*im_width + i]
        # if current pixel is lit up, add it to the integer
        if current_pixel != 0: 
            x_bytes |= (1 << i)

    # convert integer to group of bytes
    x_bytes = x_bytes.to_bytes(resize_x // 8, byteorder='little')
    array.append(x_bytes)


# Convert to C-style code
data = ''
for i, line in enumerate(array):

    items = list(line)
    items = list(map(hex, items))

    joined_items = ', '.join(items) + ', '

    data += f'    {{ {joined_items} }}, /* line {i} */ \n'


template=f'''char image[] = {{
{data}
}};
'''

with open('output.h', 'w') as f:
    f.write(template)


# Send to microcontroller over serial
ser = serial.Serial(
    port=sys.argv[2],
    baudrate=256000,
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE)
print(f"Starting {ser.port} at {ser.baudrate}")

# reset index on MCU
ser.write(b'R')

# Send array
print(f"Sending")

for i, line in enumerate(array):
    payload = ''
    for item in list(line):
        payload += hex(item)[2:].upper()
        payload += '-'
    ser.write(payload.encode())
    print(f"Sent {i}")

# reset index on MCU
ser.write(b'R')

print(f"Done")


#print(array)
#print(len(array))
#print(len(array[0]))



