# Import necessary libraries
from flask import Flask, render_template, request, session, redirect, url_for
from flask_mysqldb import MySQL
import requests
import bcrypt
import os
from requests.exceptions import ConnectionError

# Create a Flask application
app = Flask(__name__)

# Configure MySQL connection
app.config['MYSQL_HOST'] = 'localhost'
app.config['MYSQL_USER'] = 'root'
app.config['MYSQL_PASSWORD'] = ''
app.config['MYSQL_DB'] = 'user'

arduino_url = 'http://192.168.71.105'

# Initialize MySQL
mysql = MySQL(app)

# Secret key for session management
app.secret_key = os.urandom(24)

# Fitur buat locking
class_session_in_progress = True


#CLASS USER
# User classSSS
class User:
    def __init__(self, username, hashed_password, role):
        self.username = username
        self.hashed_password = hashed_password
        self.role = role

    def check_password(self, password):
        return bcrypt.checkpw(password.encode('utf-8'), self.hashed_password.encode('utf-8'))

    @classmethod
    def get_user_by_username(cls, username):
        cur = mysql.connection.cursor()
        cur.execute("SELECT * FROM login_table WHERE username = %s", (username,))
        user_data = cur.fetchone()
        cur.close()

        if user_data:
            return cls(username=user_data[1], hashed_password=user_data[2], role=user_data[3])
        return None

    @classmethod
    def create_user(cls, username, password, role):
        hashed_password = bcrypt.hashpw(password.encode("utf-8"), bcrypt.gensalt())
        cur = mysql.connection.cursor()
        cur.execute("INSERT INTO login_table (username, password, role) VALUES (%s, %s, %s)",
                    (username, hashed_password, role))
        mysql.connection.commit()
        cur.close()
        return cls(username=username, hashed_password=hashed_password, role=role)



# Define a route for the registration page (only accessible to admins)
@app.route('/register', methods=['GET', 'POST'])
def register():
    if 'logged_in' in session and session['role'] == 'admin':
        if request.method == 'POST':
            username = request.form['txt']
            raw_password = request.form['pswd']
            role = request.form['userType']

            existing_user = User.get_user_by_username(username)

            if existing_user:
                error = 'Username already exists'
                return render_template('register.html', error=error)

            User.create_user(username, raw_password, role)

            success_message = 'User registered successfully'
            return render_template('register.html', success_message=success_message)

        return render_template('register.html')

    error = 'Only Admins can register new users, please contact an admin!'
    return render_template('loggedOut.html', error=error)
        
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
        return render_template('loggedInAdmin.html', name=name, state = state)
    error = 'Please log in!'
    return render_template('loggedOut.html', error =error)


# Define a route for the login page
@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form['txt']
        password = request.form['pswd']

        user = User.get_user_by_username(username)

        if user and user.check_password(password):
            session['logged_in'] = True
            session['username'] = user.username
            session['role'] = user.role

            if user.role == 'admin':
                return redirect(url_for('menu_admin'))
            return redirect(url_for('menu'))

        error = 'Invalid login'
        return render_template('loggedOut.html', error=error)

    error = ''
    return render_template('loggedOut.html', error=error)

# Define a route for the home page
@app.route('/')
def index():
    return render_template('loggedOut.html')

# Define a route for the lamp page
@app.route('/lamp')
def lamp():
    if 'logged_in' in session :
        if class_session_in_progress == True and session['role'] == 'user':
            error = "Access to the lamp is locked during class session."
            return render_template('loggedIn.html', error = error)
        return render_template('lamp.html')
    else:
        return redirect(url_for('login'))

# Define a route for the logs page
@app.route('/logs', methods=['GET', 'POST'])
def view_logs():
#    if 'logged_in' in session:
        cur = mysql.connection.cursor()

        cur.execute("SELECT username, timestamp, action FROM lamp_usage_reports")
        logs = cur.fetchall()
        cur.close()

        logs_with_details = []

        for log in logs:
            username = log[0]
            timestamp = log[1]
            action = log[2]

            logs_with_details.append({
                'username': username,
                'timestamp': timestamp,
                'action': action
            })

        return render_template('logs.html', logs=logs_with_details)
    #else:
     #   return redirect(url_for('login'))

# Route for Locking Users
@app.route('/manage_session', methods=['POST', 'GET'])
def manage_session():
    global class_session_in_progress
    if 'logged_in' in   session:
            if class_session_in_progress == False:
                class_session_in_progress = True
                return "session was false now true"
            else :
                class_session_in_progress = False
                return "session was true now false"
    else:
        return redirect(url_for('login'))


# Define a route to log out
@app.route('/logout')
def logout():
    session.pop('logged_in', None)
    session.pop('username', None)
    session.pop('role', None)
    return redirect(url_for('login'))

# Define route for LED Status
@app.route('/get_led_status', methods=['GET'])
def get_led_status():
    try:
        response_status = requests.get(f'{arduino_url}/get_led_status')
        response_status.raise_for_status()  # Raise an exception for HTTP errors
        led_status = response_status.text
        if led_status == 'On':
            led_info = 'Lampu Nyala!'
        elif led_status == 'Off' :
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
    if 'logged_in' in session :
        if class_session_in_progress:
            error = "Access to the lamp is locked during class session."
            return render_template('loggedIn.html', error = error)
        # Send a request to NodeMCU to turn on the LED
        response = requests.get(f'{arduino_url}/turn_on_led')  # NodeMCU IP
        username = session['username']
        if response.text == "LED turned on":
            action = "Turn on LED"
        else:
            action = "Turn off LED"

        # Insert a record into the lamp_usage_reports table
        cur = mysql.connection.cursor()
        cur.execute("INSERT INTO lamp_usage_reports (username, timestamp, action) VALUES (%s, NOW(), %s)", (username, action))
        mysql.connection.commit()
        cur.close()
    else:
        return redirect(url_for('login'))




# Run the Flask application
if __name__ == '__main__':
    app.run(debug=True)