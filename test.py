from flask import Flask, render_template
from flask_socketio import SocketIO, emit
import paho.mqtt.client as mqtt
import threading

app = Flask(__name__)
socketio = SocketIO(app)

# Global variable
variable = "Initial Value"

@app.route('/')
def index():
    return render_template('ws.html', variable=variable)

@socketio.on('connect')
def handle_connect():
    emit('update', variable)

def update_variable(new_value):
    global variable
    variable = new_value
    socketio.emit('update', variable)

# MQTT setup
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe("your/topic")

def on_message(client, userdata, msg):
    print(f"Message received: {msg.payload.decode()}")
    update_variable(msg.payload.decode())

mqtt_client = mqtt.Client()
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

mqtt_client.connect("broker.emqx.io", 1883, 60)

def mqtt_thread():
    mqtt_client.loop_forever()

if __name__ == '__main__':
    threading.Thread(target=mqtt_thread).start()
    socketio.run(app)
