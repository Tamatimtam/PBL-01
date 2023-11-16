# Smart Classroom Documentation

## Overview

Welcome to the Smart Classroom project! This application integrates Flask as the web backend and NodeMCU (ESP8266) as the hardware controller for LED lights. The project aims to create a smart classroom control system, utilizing Internet of Things (IoT) principles.

![Insert Logo Here](link_to_logo.png) <!-- Placeholder for Project Logo -->

## Installation

### Flask Environment Setup

1. Clone this repository to your local machine.
   ```bash
   git clone https://github.com/your-username/smart-classroom.git
   ```
2. Navigate to the project directory.

```bash

cd smart-classroom
```

3.Create a virtual environment (optional but recommended).

```bash

python -m venv venv
```

4. Activate the virtual environment.

```    Windows:
    bash

.\venv\Scripts\activate
```
```Linux/macOS:

bash

    source venv/bin/activate
```

5.Install dependencies.

bash

    pip install -r requirements.txt

NodeMCU Development Environment Setup

    Open the NodeMCU code (smart_classroom_nodeMCU.ino) in the Arduino IDE.

    Modify the ssid and password variables to match your WiFi network credentials.

    Upload the code to your NodeMCU.

Getting Started
Running Flask Locally

    Ensure the virtual environment is activated.

    Run the Flask application.

    bash

    flask run

    Access the application in your browser at http://localhost:5000.

Controlling LED

    On the web interface, log in or register as an admin.

    Navigate to the control page to toggle the LED.

Customization Options

    NodeMCU WiFi Configuration:
        Modify the ssid and password variables in the NodeMCU code to match your WiFi network.

External Dependencies

The project is written in Python, and dependencies are specified in the Flask application's import statements. A virtual environment is used for isolation.
Known Limitations

    This project is in early development, and certain features may not be fully optimized or implemented.
