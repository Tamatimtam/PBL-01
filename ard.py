#region Import necessary libraries
from flask import Flask, render_template, request, session, redirect, url_for, jsonify  
from flask_mysqldb import MySQL
import requests
import json     
import bcrypt
import os
from requests.exceptions import ConnectionError  # Import ConnectionError
#endregion

# Create a Flask application
app = Flask(__name__)

#region Configure MySQL connection
app.config['MYSQL_HOST'] = 'localhost'  # MySQL server host
app.config['MYSQL_USER'] = 'root'   # MySQL username
app.config['MYSQL_PASSWORD'] = ''  # MySQL password
app.config['MYSQL_DB'] = 'user'  # MySQL database name
#endregion

arduino_url = 'http://192.168.37.179'  # Arduino IP#
# Initialize MySQL
mysql = MySQL(app)
# Secret key for session management
app.secret_key = os.urandom(24) 



# Define a route for the registration page (only accessible to admins)
@app.route('/register', methods=['GET', 'POST'])
def register():
    # Check if the user is an admin
    if 'logged_in' in session and session['role'] == 'admin':
        if request.method == 'POST':
            # Get user input from the registration form
            username = request.form['username']
            raw_password = request.form['password']
            role = request.form['role']  # You can set the role to 'admin' here if needed

            # Hash the password before storing it in the database
            hashed_password = bcrypt.hashpw(raw_password.encode('utf-8'), bcrypt.gensalt())

            # Connect to the MySQL database
            cur = mysql.connection.cursor()

            # Check if the username already exists
            cur.execute("SELECT * FROM users WHERE username = %s", (username,))
            existing_user = cur.fetchone()

            if existing_user:
                error = 'Username already exists'
                cur.close()
                return render_template('register.html', error=error)

            # If the username is unique, insert the new user into the database
            cur.execute("INSERT INTO users (username, password, role) VALUES (%s, %s, %s)",
                        (username, hashed_password, role))
            mysql.connection.commit()

            # Close the database connection
            cur.close()

            # Redirect to a success page or display a message
            success_message = 'User registered successfully'
            return render_template('register.html', success_message=success_message)

        return render_template('register.html')

    # If the user is not an admin, deny access
    return "Access denied. Only administrators can register new users."


# Define a route for the login page
@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        # Get user input from the login form
        username = request.form['username']
        password = request.form['password']

        # Connect to the MySQL database
        cur = mysql.connection.cursor()

        # Use a parameterized query to retrieve the user
        cur.execute("SELECT * FROM users WHERE username = %s", (username,))
        user = cur.fetchone()

        # Close the database connection
        cur.close()

        if user is not None:
            # Convert the stored hashed password from the database to bytes
            hashed_password_from_db = user[2].encode('utf-8')  # Assuming 'password' is the third column in the table
            
            # Check if the input password matches the hashed password from the database
            if bcrypt.checkpw(password.encode('utf-8'), hashed_password_from_db):
                # If the user exists and the password is correct, create a session
                session['logged_in'] = True
                session['username'] = username
                session['role'] = user[3]  # Assuming 'role' is the fourth column in the table
                return redirect(url_for('index'))  # Redirect to the home page

        # If the login is unsuccessful, show an error message
        error = 'Invalid login'
        return render_template('login.html', error=error)

    # If it's a GET request, render the login form
    return render_template('login.html')

# Define a route for the home page
@app.route('/')
def index():
    # Check if the user is logged in
    if 'logged_in' in session:
        response_status = requests.get(arduino_url)

        if response_status.status_code == 200:
            led_status = response_status.text
            return render_template('index.html', led_status=led_status)
        else:
            error_message = 'Failed to retrieve LED status'
            return render_template('index.html', error_message=error_message)
    else:
        return redirect(url_for('login'))  # Redirect to the login page if not logged in

# Define a route to log out
@app.route('/logout')
def logout():
    # Remove the session variables to log out
    session.pop('logged_in', None)
    session.pop('username', None)
    return redirect(url_for('login'))
    
# Define route for LED Status
@app.route('/get_led_status', methods=['GET'])
def get_led_status():
    try:
        response_status = requests.get(f'{arduino_url}/get_servo_position')
        response_status.raise_for_status()  # Raise an exception for HTTP errors
        servo_position = int(response_status.text)
        led_status = "ON" if servo_position == 180 else "OFF"
        return led_status
    except ConnectionError as e:
        return 'unknown'  # Handle connection errors

# Define a route to turn on the LED
@app.route('/turn_on_led', methods=['POST', 'GET'   ])
def turn_on_led():
    # Check if the user is logged in
    if 'logged_in' in session:
        # Send a request to NodeMCU to turn on the LED
        response_servo = requests.get(f'{arduino_url}/get_servo_position')  # NodeMCU IP
        
        if response_servo.status_code == 200:
            current_position = int(response_servo.text)  # Convert the response to an integer
            new_position = 180 if current_position == 0 else 0  # Toggle the position
            
            # Send a request to NodeMCU to set the servo position
            response_led = requests.get(f'{arduino_url}/rotate_servo?position={new_position}')
        
            if response_led.status_code == 200:
                return render_template('index.html', led_status='LED turned on' if new_position == 180 else 'LED turned off')
            else:
                return render_template('index.html', led_status='Failed to control LED')
        else:
            return render_template('index.html', led_status='Failed to retrieve servo position')
    else:
        return redirect(url_for('login'))  



# Run the Flask application
if __name__ == '__main__':
    app.run(debug=True)