# ac_module.py

from flask import render_template, redirect, url_for
from requests.exceptions import ConnectionError
from models import Logs
import requests

class AC:
    def __init__(self, arduino_url):
        self.arduino_url = arduino_url

    def update_ac_status(self):
        try:
            response_status = requests.get(f'{self.arduino_url}/get_AC_status')
            response_status.raise_for_status()  # Raise an exception for HTTP errors
            ac_status = response_status.text
            if ac_status == 'On':
                ac_info = 'AC ON'
            elif ac_status == 'Off':
                ac_info = 'AC OFF'
            else:
                ac_info = f'ERROR!, {ac_status}'
            return ac_info
        except ConnectionError as e:
            return 'unknown'  # Handle connection errors

    def update_ac_temp(self):
        try:
            response_status = requests.get(f'{self.arduino_url}/get_AC_temp')
            response_status.raise_for_status()  # Raise an exception for HTTP errors
            ac_status = response_status.text
            if ac_status:
                ac_info = ac_status
            else:
                ac_info = f'ERROR!, {ac_status}'
            return ac_info
        except ConnectionError as e:
            return 'unknown'  # Handle connection errors

    def toggle_ac(self, session, class_session_in_progress, db, socketio):
        if 'logged_in' in session:
            if class_session_in_progress:
                error = "Access to the AC is locked during class session."
                return render_template('loggedIn.html', error=error)

            response = requests.get(f'{self.arduino_url}/acControl')
            if response.text == "AC Is ON!":
                action = "AC ON"
                socketio.emit('update_AC_status', {'status': 'AC Nyala!'})
            else:
                action = "AC OFF"
                socketio.emit('update_AC_status', {'status': 'AC Mati!'})                

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
