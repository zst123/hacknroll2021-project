#include "mbed.h"

DigitalOut green_led(LED1);
DigitalOut yellow_led(LED2);
DigitalOut red_led(LED3);

UnbufferedSerial serial_port(USBTX, USBRX, 256000);

AnalogOut dacX(PA_4);
AnalogOut dacY(PA_5);

void DAC_SetPosition(float x, float y) {
    dacX = x;
    dacY = y;
}

#define IMAGE_SIZE_X (256)
#define IMAGE_SIZE_Y (256)
#define IMAGE_BYTES_X (256/8)
#define IMAGE_BYTES_Y (256)
char image[IMAGE_BYTES_Y][IMAGE_BYTES_X] = {0};

char currentSerialHex = 0;
int currentSerialIndex = 0;
void Serial_RX_Callback() {
    //char ch = serial_port.getc();
    char buf[1];
    serial_port.read(&buf[0],  1);
    
    char ch = buf[0];

    if (ch == 'R') {
        // Reset indexing and current buffer
        currentSerialHex = 0;
        currentSerialIndex = 0;
        
        red_led = !red_led;
    } else if (ch == '-') {
        // Delimiter, to commit the current byte in
        image[currentSerialIndex/IMAGE_BYTES_X][currentSerialIndex%IMAGE_BYTES_X] = currentSerialHex;
        
        // Reset buffer and increment index
        currentSerialIndex++;
        currentSerialHex = 0;
        
        // Reset index if it has reached the end
        if (currentSerialIndex >= (IMAGE_BYTES_Y * IMAGE_BYTES_X)) {
            currentSerialIndex = 0;
        }
    } else {
        if (('0' <= ch && ch <= '9')) { // if char is a digit
            currentSerialHex <<= 4; // shift up previous nibble
            currentSerialHex += (ch - '0'); // Convert ascii digit into integer
        } else if ('A' <= ch && ch <= 'F') { // if char is a hex digit
            currentSerialHex <<= 4; // shift up previous nibble
            currentSerialHex += (ch - 'A' + 0xA); // Convert hex digit into integer   
        }
    }
}


int main() {
    // Setup a serial interrupt function to receive data
    serial_port.attach(&Serial_RX_Callback, UnbufferedSerial::RxIrq);

    while(1) {
        for (int j = 0; j < IMAGE_SIZE_Y; j++) {
            for (int i = 0; i < IMAGE_SIZE_X; i++) {
                // Check if pixel is lighted up
                int x_pos = i % 8;
                int x_index = i / 8;
                int y_index = j;
                bool draw = (image[y_index][x_index] & (1 << x_pos)) != 0;
                
                // Move to that position to draw
                if (draw) {
                    // Invert y-axis so that coordinates (0, 0) will be at the top-left. (originally it is at the bottom-left)
                    DAC_SetPosition((i/256.0), 1 - (j/256.0));
                }
            }
        }
    }
}
