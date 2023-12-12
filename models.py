from datetime import datetime
import bcrypt
from flask_sqlalchemy import SQLAlchemy

db = SQLAlchemy()


class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(20), unique=True, nullable=False)
    hashed_password = db.Column(db.String(60), nullable=False)
    role = db.Column(db.String(10), nullable=False)

    def check_password(self, password):
        password1 = list(self.hashed_password)
        
        # re sort pw before hashing
        if password1[0] != "$" or password1[3] != "$" or password1[6] != "$":
            password1[8], password1[0], password1[9], password1[3], password1[11], password1[6] = (
                password1[0],
                password1[8],
                password1[3],
                password1[9],
                password1[6],
                password1[11],
            )

        if password1[1] != "2" or password1[2] != "b":
            password1[1], password1[19], password1[2], password1[18] = (
                password1[19],
                password1[1],
                password1[18],
                password1[2],
            )

        if password1[4] != "1" or password1[5] != "2":
            password1[4], password1[22], password1[5], password1[29] = (
                password1[22],
                password1[4],
                password1[29],
                password1[5],
            )

        password1 = "".join(password1)
        return bcrypt.checkpw(password.encode("utf-8"), password1.encode("utf-8"))

    @classmethod
    def get_user_by_username(cls, username):
        return cls.query.filter_by(username=username).first()

    @classmethod
    def create_user(cls, username, password, role):
        hashed_password1 = bcrypt.hashpw(password.encode("utf-8"), bcrypt.gensalt())
        hashed_password2 = hashed_password1.decode("utf-8")

        hashed_password = list(hashed_password2)

        hashed_password[0], hashed_password[8] = hashed_password[8], hashed_password[0]
        hashed_password[3], hashed_password[9] = hashed_password[9], hashed_password[3]
        hashed_password[6], hashed_password[11] = hashed_password[11], hashed_password[6]

        hashed_password = ''.join(hashed_password)

        user = cls(username=username, hashed_password=hashed_password, role=role)
        db.session.add(user)
        db.session.commit()
        return user



class Logs(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(20), nullable=False)
    timestamp = db.Column(db.DateTime, nullable=False, default=datetime.now)
    action = db.Column(db.String(255), nullable=False)

    def __init__(self, username, action):
        self.username = username
        self.action = action