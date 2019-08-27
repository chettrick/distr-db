from quart import Quart, request
import sys

app = Quart(__name__)

@app.route("/something", methods=['GET', 'POST'])
async def hello1():
    print("received hit to /something", file=sys.stderr)
#    async for data in request.body:
#        for element in data:
#            print(data)
#    results = request.args
    name = (await request.values).get("name")
    project = (await request.values).get("project")
    print('name:', name, '\nproject:', project)
    return 'hello something!!'
@app.route('/')
async def hello():
    print("received hit", file=sys.stderr)
    return 'hello world!!'

app.run(host='0.0.0.0')

## if __name__ == '__main__':
##     print("Hello, World!")
