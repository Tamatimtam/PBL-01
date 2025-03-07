# ac_module.py

from flask import render_template, redirect, url_for
from requests.exceptions import ConnectionError
from models import Logs
import requests

class AC:
    def __init__(self, arduino_url):
        self.arduino_url = arduino_url
        self.temp = 0
        self.status = "..."

    def update_ac_status(self):
        try:
            response_status = requests.get(f'{self.arduino_url}/acStatus')
            response_status.raise_for_status()  # Raise an exception for HTTP errors
            ac_status = response_status.text
            print (ac_status)
            return ac_status
        except ConnectionError as e:
            return 'unknown'  # Handle connection errors

    def update_ac_temp(self):
        try:
            response_status = requests.get(f'{self.arduino_url}/acTemp')
            response_status.raise_for_status()  # Raise an exception for HTTP errors
            ac_status = response_status.text
            if ac_status:
                self.temp = ac_status
            else:
                self.temp = f'ERROR!, {ac_status}'
            return self.temp
        except ConnectionError as e:
            return 'unknown'  # Handle connection errors

    def toggle_ac(self, session, class_session_in_progress, db, socketio, ac_id):
        if 'logged_in' in session:
            if class_session_in_progress:
                error = "Access to the AC is locked during class session."
                return render_template('loggedIn.html', error=error)

            response = requests.get(f'{self.arduino_url}/acControl{ac_id}')
            print(f'{self.arduino_url}/acControl{ac_id}')
            if response.text == "on":
                action = "AC On!"
                socketio.emit('update_AC_status', {'status': ['AC On!']})
            else:
                action = "AC OFF"
                socketio.emit('update_AC_status', {'status': ['AC Off!']})     
            print(response.text)           

            log = Logs(username=session['username'], action=action)
            db.session.add(log)
            db.session.commit()
            return action
        else:
            return redirect(url_for('login'))

    def adjust_temp(self, session, class_session_in_progress, db, command, socketio):
        if 'logged_in' in session:
            if class_session_in_progress:
                error = "Access to the AC is locked during class session."
                return render_template('loggedIn.html', error=error)

            response = requests.get(f'{self.arduino_url}/{command}')
            action = f"{command} temp to {response.text}"
            socketio.emit('update_AC_temp', {'status': action})

            log = Logs(username=session['username'], action=action)
            db.session.add(log)
            db.session.commit()
            return action
        else:
            return redirect(url_for('login'))
