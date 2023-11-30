class MyClass:
    class_variable = "I am a class variable"

    def __init__(self, instance_variable):
        self.instance_variable = instance_variable

    @classmethod
    def class_method(cls):
        print(f"This is a class method. Class variable: {cls.class_variable}")

# Creating an instance of the class
obj = MyClass("I am an instance variable")

# Calling a class method on the class itself
MyClass.class_method()
