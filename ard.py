from flask import Flask, render_template, request
import requests

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/turn_on_led', methods=['POST', 'GET'])
def turn_on_led():
    # Send a request to NodeMCU to turn on the LED
    response = requests.get('http://192.168.43.179/turn_on_led')  # Remove the extra "http://"
    return 'LED turned on'

if __name__ == '__main__':
    app.run(debug=True)
from flask import Flask, render_template, request
import requests

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/turn_on_led', methods=['POST', 'GET'])
def turn_on_led():
    # Send a request to NodeMCU to turn on the LED
    response = requests.get('http://192.168.43.179/turn_on_led')  # Remove the extra "http://"
    return 'LED turned on'

if __name__ == '__main__':
    app.run(debug=True)
