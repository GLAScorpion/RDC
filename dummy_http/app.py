from flask import Flask,request

app = Flask(__name__)

@app.route("/")
def hello_world():
    return "<p>Hello, World!</p>"

@app.route("/post",methods=["POST"])
def post():
    if request.method == 'POST':
        result = request.get_data(as_text=True)
        print(result)
        return result
    return "Not allowed"
