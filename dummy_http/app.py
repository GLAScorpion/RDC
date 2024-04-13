from flask import Flask,request
from werkzeug.serving import WSGIRequestHandler
app = Flask(__name__)

@app.route("/")
def hello_world():
    return "<p>Hello, World!</p>"

@app.route("/post",methods=["POST"])
def post():
    if request.method == 'POST':
        result = request.get_data(as_text=True)
        print(request.content_type)
        return result
    return "Not allowed"

WSGIRequestHandler.protocol_version = "HTTP/1.1"
if __name__ == "__main__":
    app.run()
