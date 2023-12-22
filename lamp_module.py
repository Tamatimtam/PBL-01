# lamp_module.py

from flask import render_template, redirect, url_for
from requests.exceptions import ConnectionError
from models import Logs
import requests

class Lamp:
    def __init__(self, arduino_url):
        self.arduino_url = arduino_url

    def get_led_status(self):
        try:
            response_status = requests.get(f'{self.arduino_url}/get_led_status')
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

    def turn_on_led(self, session, class_session_in_progress, db, socketio):
        if 'logged_in' in session:
            if class_session_in_progress:
                error = "Access to the lamp is locked during class session."
                return render_template('loggedIn.html', error=error)

            response = requests.get(f'{self.arduino_url}/turn_on_led')
            if response.text == "LED turned on":
                action = "Turn on LED"
                socketio.emit('update_led_status', {'status': 'Lampu Nyala!'})
            else:
                action = "Turn off LED"
                socketio.emit('update_led_status', {'status': 'Lampu Mati!'})

            log = Logs(username=session['username'], action=action)
            db.session.add(log)
            db.session.commit()
            return action
        else:
            return redirect(url_for('login'))
