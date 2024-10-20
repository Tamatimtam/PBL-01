# Classify  [![LinkedIn Badge](https://img.shields.io/badge/-Tamatimtam🍫-brown?logo=linkedin&style=flat-square&link=https%3A%2F%2Fwww.linkedin.com%2Fin%2Fpratamasiregarpnj)](https://www.linkedin.com/in/pratamasiregarpnj) ![GitHub commit activity](https://img.shields.io/github/commit-activity/t/Tamatimtam/PBL-01)

<img src="https://github.com/Tamatimtam/PBL-01/assets/121283167/a4ca314e-d2f7-4b5d-bbb4-84fcddd86f84" alt="Classify Logo" width="230" align="right"> </img>
Ever wished your classroom could keep its cool as effortlessly as you do? Enter **Classify** – A Flask based Smart Classroom Control System integrating IoT to remotely control your AC and lighting.

## Features
- AC Control: Let Classify handle the temperature drama. Hot? Cold? Just right? It's like having a personal climate concierge.

- Lighting Control: Say goodbye to fumbling for the switch. Let Classify light up your path.


- Web-o-Matic Interface: A sleek web interface that's so user-friendly even your grandmother could use it. (Probably)

## Why Classify?
Because turning a mundane classroom into a smart, controlled environment is just plain cool. But beyond that, it's a great project to learn about how REST APIs work, how to play around with IoT libraries, learning about how to secure an SQL database, and just making technology do something that will make our lives easier! If you're looking for a good beginner IoT project to get to learn all of these things, We'd wholeheartedly recommend you fork this repo and try to recreate what we made. You can also contact us via linked in if you have any questions!

## Technologies Used
- HTML & CSS (Obviously.)
- Flask
- RESTful APIs
- IoT (ESP 8266)
- Javascript
- Websockets
- SQLite
- SQLAlchemy
- Bcrypt

## Installation
1. Clone the repo
2. Install the libraries using 'pip install -r requirements.txt'
3. Navigate to the 'arduino' folder and upload the appropriate .ino file to your ESP8266 (AC.ino / lamp.ino)
4. Update the SSID and Password on the respective .ino files
5. Setup your esp according to our schematics!
6. Update the IP address for your esp in the ard.py file
7. run using python3 ard.py

## Disclaimer
May cause jealousy among neighboring classrooms. Use responsibly. 

## License
MIT
**Cause Free Software is Awesome!**

