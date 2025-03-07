# region  SETUP / HEADER 



# region    Import necessary libraries
from flask import Flask, render_template, request, session, redirect, url_for
from flask_socketio import SocketIO, emit

from ac_module import AC        # Import the AC class
from lamp_module import Lamp    # Import the Lamp class

import paho.mqtt.client as mqtt
import threading
import requests                 #http request
import bcrypt                   #password security
import os                       
from requests.exceptions import ConnectionError
from models import User, db, Logs   #user and logs database
from flask_cors import CORS         #for socketio ig

from datetime import datetime, timedelta
# endregion


###Create a Flask application
app = Flask(__name__)
CORS(app)


# region    Configure SQLite connection and SocketIO
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///site.db'
db.init_app(app)
# endregion



# region    SocketIO
socketio = SocketIO(app) #initiate socketIO object




# MQTT setup
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe("your/AC/1")
    client.subscribe("your/AC/2")
    client.subscribe("your/AC/3")

def on_message(client, userdata, msg):
    if (msg.topic == "your/AC/1"):
        print(f"1 Message received: {msg.payload.decode()}")
        update_AC(msg.payload.decode())
    
    elif (msg.topic == "your/AC/2"):
        print(f"2 Message received: {msg.payload.decode()}")
        update_AC(msg.payload.decode())

    else:
        print ("wtf bro")
    

mqtt_client = mqtt.Client()
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

mqtt_client.connect("broker.emqx.io", 1883, 60)

def mqtt_thread():
    mqtt_client.loop_forever()






# endregion
acState = False
@socketio.on('connect')
def handle_connect():
    print("Successfull connect to SOCKET-IO")

@socketio.on('update_AC')
def recieve_AC(data):
    global acState
    print(f"Recieved updated ac data to {data}")
    emit('update_AC')


def update_AC(command):
    print("MQTT Message recieved!")
    print(command)
    global acState
    acState=command
    socketio.emit('update_AC', acState)

# region    Local Variables
class_session_in_progress = False       # Variable buat locking 
AC_url = "http://192.168.47.72"     # Variable buat arduino URL
lamp_url = "http://192.168.47.72"     # Variable buat arduino URL
app.secret_key = os.urandom(24) 
# endregion


# endregion



# region  LOGIN / LOGOUT / REGISTER
# Define a route for the registration page (only accessible to admins)
@app.route('/register', methods=['GET', 'POST'])
def register():
    if 'logged_in' in session and session['role'] == 'admin':
        if request.method == 'POST':
            username = request.form['txt']
            raw_password = request.form['pswd']
            role = request.form['userType']

            existing_user = User.query.filter_by(username=username).first()

            if existing_user:
                success_message = 'Username already exists'
                return render_template('register.html', success_message=success_message)

            User.create_user(username, raw_password, role)

            success_message = 'User registered successfully'
            return render_template('register.html'  , success_message=success_message)

        return render_template('register.html')

    error = 'Only Admins can register new users, please contact an admin!'
    return render_template('loggedOut.html', error=error)

# Define a route for the login page
@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form['txt']
        password = request.form['pswd']

        user = User.query.filter_by(username=username).first()

        if user and user.check_password(password):
            session['logged_in'] = True
            session['username'] = user.username
            session['role'] = user.role

            if user.role == 'admin':
                return redirect(url_for('menu_admin'))
            return redirect(url_for('menu'))

        error = 'Invalid login'
        return render_template('loggedOut.html', error=error)

    return render_template('loggedOut.html')


# Define a route to log out
@app.route('/logout')
def logout():
    session.pop('logged_in', None)
    session.pop('username', None)
    session.pop('role', None)
    return redirect(url_for('login'))

# endregion




# region INDEX, MENU AND RENDER MENU
# Define a route for the home page
@app.route('/')
def index():
    return render_template('loggedOut.html')

@app.route('/graph')
def graph():
    return render_template('graph.html')




# Define a route for the redirect to lamp or ac page
@app.route('/menu')
def menu():
    if 'logged_in' in session:
        name = session['username']
        return render_template('loggedIn.html', name=name)
    error = 'Please log in!'
    return render_template('loggedOut.html', error=error)

# Define a route for the redirect to lamp or ac page ADMIN VERSION
@app.route('/menu_admin')
def menu_admin():
    if 'logged_in' in session and session['role'] == 'admin':
        name = session['username']
        state = class_session_in_progress
        return render_template('loggedInAdmin.html', name=name, state=state)
    error = 'Please log in!'
    return render_template('loggedOut.html', error=error)





# Define a route for the lamp page
@app.route('/lamp')
def lamp(): 
    if 'logged_in' in session:
        if class_session_in_progress and session['role'] == 'user':
            error = "Access to the lamp is locked during class session."
            return render_template('loggedIn.html', error=error)
        return render_template('lamp.html')
    else:
        return redirect(url_for('login'))

# Define a route for the AC page
@app.route('/ac')
def ac(): 
    if True:
        if class_session_in_progress and session['role'] == 'user':
            error = "Access to the AC is locked during class session."
            return render_template('loggedIn.html', error=error)
        return render_template('ac.html')
    else:
        return redirect(url_for('login'))
    
@socketio.on('event')
def handle_message(message):
    print('Message from ESP8266:', message)
    # Handle the message as needed





# Define a route for the logs page
@app.route('/logs', methods=['GET', 'POST'])
def view_logs():
    if 'logged_in' in session and session['role'] == 'admin':
        logs = Logs.query.all()

        logs_with_details = []

        for log in logs:
            username = log.username
            timestamp = log.timestamp
            action = log.action

            logs_with_details.append({
                'username': username,
                'timestamp': timestamp,
                'action': action
            })

        return render_template('logs.html', logs=logs_with_details)
    else:
        return redirect(url_for('login'))





# Route for Locking Users
@app.route('/manage_session', methods=['POST', 'GET'])
def manage_session():
    global class_session_in_progress
    print(class_session_in_progress)
    if 'logged_in' in session:
        if class_session_in_progress:
            class_session_in_progress = False
            return "session was true now false"
        else:
            class_session_in_progress = True
            return "session was false now true"
    else:
        return redirect(url_for('login'))
# endregion



# region LAMP FUNCTIONS
lamp_handler = Lamp(lamp_url)

@app.route('/get_led_status', methods=['GET'])
def get_led_status():
    led_info = lamp_handler.get_led_status()
    return led_info

# Define route for turning on the LED
@app.route('/turn_on_led', methods=['POST', 'GET'])
def turn_on_led():
    ac_id = request.args.get('acId')
    action = lamp_handler.turn_on_led(session, class_session_in_progress, db, socketio, ac_id)
    return action
# endregion



# region AC Functions

ac_handler = AC(AC_url)



# Define route for updating AC status
@app.route('/UpdateAC', methods=['GET'])
def update_ac():
    ac_info = ac_handler.update_ac_status()
    return ac_info

# Define route for updating AC temperature
@app.route('/UpdateTemp', methods=['GET'])
def update_temp():
    ac_info = ac_handler.update_ac_temp()
    return ac_info

# Define route for toggling AC
@app.route('/ToggleAC', methods=['POST', 'GET'])
def toggle_ac():
    ac_id = request.args.get('acId')
    action = ac_handler.toggle_ac(session, class_session_in_progress, db, socketio, ac_id)
    return action

# Define route for adjusting AC temperature up
@app.route('/ACUp', methods=['POST', 'GET'])
def ac_up():
    action = ac_handler.adjust_temp(session, class_session_in_progress, db, 'ACUp', socketio)
    return action

# Define route for adjusting AC temperature down
@app.route('/ACDown', methods=['POST', 'GET'])
def ac_down():
    action = ac_handler.adjust_temp(session, class_session_in_progress, db, 'ACDown', socketio)
    return action

# endregion



# Run the Flask application
if __name__ == '__main__':
    print("Test")
    threading.Thread(target=mqtt_thread,).start()
    with app.app_context():
        db.create_all()
    socketio.run(app, debug=False, ssl_context=('ssl-certificate/laragon.crt', 'ssl-certificate/laragon.key'))
    print("WALAWEEEEEE")