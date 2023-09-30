from flask import Flask, render_template
import serial

app = Flask(__name__)

# Define the serial port for communication with Arduino
ser = serial.Serial('COM5', 9600)  # Update with your actual serial port and baud rate

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/button1')
def button1():
    # Send a request to NodeMCU (e.g., via serial)
    return 'Button 1 pressed'

@app.route('/arduino_status')
def arduino_status():
    # Check if the Arduino is connected
    if ser.is_open:
        status = 'Connected'
        return render_template('status.html', status=status)
    else:
        status = 'Not Connected'
        return render_template('index.html', status=status)
    
    

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)
