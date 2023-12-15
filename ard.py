# region  SETUP / HEADER 


### Import necessary libraries
from flask import Flask, render_template, request, session, redirect, url_for
from flask_socketio import SocketIO
import requests
import bcrypt
import os
from requests.exceptions import ConnectionError
from models import User, db, Logs

###Create a Flask application
app = Flask(__name__)





### Configure SQLite connection and SocketIO
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///site.db'
db.init_app(app)  # assuming 'app' is your Flask app





###SocketIO
socketio = SocketIO(app)
  #Function for LED updated

  #DEBUGGING, add a log statement when an emit event happens
@socketio.on('update_led_status')
def handle_update_led_status(data):
    status = data['status']
    # Use status information to update the HTML dynamically on the client side
    print(f'LED status updated: {status}')







### Secret key for session management
app.secret_key = os.urandom(24) 



###Local Variables

class_session_in_progress = False       # Variable buat locking 
arduino_url = "http://192.168.87.6"     # Variable buat arduino URL
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
                error = 'Username already exists'
                return render_template('register.html', error=error)

            User.create_user(username, raw_password, role)

            success_message = 'User registered successfully'
            return render_template('register.html', success_message=success_message)

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
    if 'logged_in' in session:
        if class_session_in_progress and session['role'] == 'user':
            error = "Access to the AC is locked during class session."
            return render_template('loggedIn.html', error=error)
        return render_template('ac.html')
    else:
        return redirect(url_for('login'))





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
# Define route for LED Status
@app.route('/get_led_status', methods=['GET'])
def get_led_status():
    try:
        response_status = requests.get(f'{arduino_url}/get_led_status')
        response_status.raise_for_status()  # Raise an exception for HTTP errors
        led_status = response_status.text
        if led_status == 'On':
            led_info = 'Lampu Nyala!'
        elif led_status == 'Off':
            led_info = 'Lampu Mati!'
        else:
            led_info = f'ERROR!, {led_status}'
        return led_info
    except ConnectionError as e:
        return 'unknown'  # Handle connection errors
    
    # Define a route to turn on the LED
@app.route('/turn_on_led', methods=['POST', 'GET'])
def turn_on_led():
    # Check if the user is logged in
    if 'logged_in' in session:
        if class_session_in_progress:
            error = "Access to the lamp is locked during class session."
            return render_template('loggedIn.html', error=error)
        

        # Send a request to NodeMCU to turn on the LED
        response = requests.get(f'{arduino_url}/turn_on_led')  # NodeMCU IP
        if response.text == "LED turned on":
            action = "Turn on LED"
            socketio.emit('update_led_status', {'status': 'Lampu Nyala!'})
        else:
            action = "Turn off LED"
            socketio.emit('update_led_status', {'status': 'Lampu Mati!'})

        # Insert a record into the Logs table
        log = Logs(username=session['username'], action=action)
        db.session.add(log)
        db.session.commit()
        return action
    else:
        return redirect(url_for('login'))
# endregion








# region AC Functions
    # Define route for AC Update (POWER)
@app.route('/UpdateAC', methods=['GET'])
def UpdateAC():
    try:
        response_status = requests.get(f'{arduino_url}/get_AC_status')
        response_status.raise_for_status()  # Raise an exception for HTTP errors
        AC_status = response_status.text
        if AC_status == 'On':
            AC_info = 'AC ON'
        elif AC_status == 'Off':
            AC_info = 'AC OFF'
        else:
            AC_info = f'ERROR!, {AC_status}'
        return AC_info
    except ConnectionError as e:
        return 'unknown'  # Handle connection errors

            # Define route for AC Update (TEMPERATURE)
@app.route('/UpdateTemp', methods=['GET'])
def UpdateTemp():
    try:
        response_status = requests.get(f'{arduino_url}/get_AC_temp')
        response_status.raise_for_status()  # Raise an exception for HTTP errors
        AC_status = response_status.text
        if AC_status:
            AC_info = AC_status
        else:
            AC_info = f'ERROR!, {AC_status}'
        return AC_info
    except ConnectionError as e:
        return 'unknown'  # Handle connection errors

    # Define a route to Toggle the AC
@app.route('/ToggleAC', methods=['POST', 'GET'])
def ToggelAC():
    # Check if the user is logged in
    if 'logged_in' in session:
        if class_session_in_progress:
            error = "Access to the AC is locked during class session."
            return render_template('loggedIn.html', error=error)
        # Send a request to NodeMCU to turn on the LED
        response = requests.get(f'{arduino_url}/acControl')  # NodeMCU IP
        if response.text == "AC Is ON!":
            action = "AC ON"
        else:
            action = "AC OFF"

        # Insert a record into the Logs table
        log = Logs(username=session['username'], action=action)
        db.session.add(log)
        db.session.commit()
        return action
    else:
        return redirect(url_for('login'))


    # Define a route to add temp
@app.route('/ACUp', methods=['POST', 'GET'])
def ACUp():
    # Check if the user is logged in
    if 'logged_in' in session:
        if class_session_in_progress:
            error = "Access to the AC is locked during class session."
            return render_template('loggedIn.html', error=error)
        # Send a request to NodeMCU to turn on the LED
        response = requests.get(f'{arduino_url}/ACUp')  # NodeMCU IP
        action = "raised temp to " + response.text

        # Insert a record into the Logs table
        log = Logs(username=session['username'], action=action)
        db.session.add(log)
        db.session.commit()
        return action
    else:
        return redirect(url_for('login'))
    
     # Define a route to lower temp
@app.route('/ACDown', methods=['POST', 'GET'])
def ACDown():
    # Check if the user is logged in
    if 'logged_in' in session:
        if class_session_in_progress:
            error = "Access to the AC is locked during class session."
            return render_template('loggedIn.html', error=error)
        # Send a request to NodeMCU to turn on the LED
        response = requests.get(f'{arduino_url}/ACDown')  # NodeMCU IP
        action = "lowered temp to " + response.text

        # Insert a record into the Logs table
        log = Logs(username=session['username'], action=action)
        db.session.add(log)
        db.session.commit()
        return action
    else:
        return redirect(url_for('login'))
# endregion





# Run the Flask application
if __name__ == '__main__':
    with app.app_context():
        db.create_all()
    socketio.run(app, debug=True)

