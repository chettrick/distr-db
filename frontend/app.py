from quart import Quart, render_template, request, redirect, url_for
from bson import ObjectId
from pymongo import MongoClient

app = Quart(__name__)

title   = "Movie Database"
heading = "Movie Database"

client = MongoClient("mongodb://mongo:27017",
            username='docker', password='docker')
db     = client.docker
movies = db.movie

async def redirect_url():
    return (await request.args).get('next') or \
        request.referrer or \
        url_for('index')

@app.route("/")
@app.route("/list")
async def list():
    # List all Movies
    movies_l = movies.find()
    a1 = "active"
    return await render_template('index.html', a1 = a1, movies = movies_l, t = title, h = heading)

@app.route("/add", methods = ['POST'])
async def add():
    # Add a Movie
    name = (await request.values).get("name")
    desc = (await request.values).get("desc")
    date = (await request.values).get("date")
    movies.insert({"name":name, "desc":desc, "date":date})
    return redirect("/")

@app.route("/remove")
async def remove():
    # Delete a Movie
    key = (await request.values).get("_id")
    movies.remove({"_id":ObjectId(key)})
    return redirect("/")

@app.route("/edit")
async def edit():
    # Find Movie to edit
    id = (await request.values).get("_id")
    movie = movies.find({"_id":ObjectId(id)})
    return await render_template('edit.html', movies = movie, h = heading, t = title)

@app.route("/save", methods = ['POST'])
async def save():
    # Save edited Movie data
    name = (await request.values).get("name")
    desc = (await request.values).get("desc")
    date = (await request.values).get("date")
    id   = (await request.values).get("_id")
    movies.update({"_id":ObjectId(id)}, {'$set':{"name":name, "desc":desc, "date":date}})
    return redirect("/")

@app.route("/search", methods = ['GET'])
async def search():
    # Search for Movies
    key   = (await request.values).get("key")
    refer = (await request.values).get("refer")
    if (key == "_id"):
        movies_l = movies.find({refer:ObjectId(key)})
    elif (key == ""):
        movies_l = movies.find()
    else:
        movies_l = movies.find({refer:key})
    return await render_template('search.html', movies = movies_l, t = title, h = heading)

if __name__ == "__main__":
    app.run(host = '0.0.0.0', port=5050)
