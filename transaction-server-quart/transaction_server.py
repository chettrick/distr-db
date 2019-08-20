from quart import Quart

app = Quart(__name__)

@app.route('/')
async def hello():
    return 'hello world!!'

app.run(host='0.0.0.0')

## if __name__ == '__main__':
##     print("Hello, World!")
