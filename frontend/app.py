from quart import Quart, render_template, request, redirect, url_for
import json
import requests

app = Quart(__name__)

title   = "Movie Database"
heading = "Movie Database"

url_movies = 'http://trans-srv:5555/api/movies'

async def redirect_url():
    return (await request.args).get('next') or \
        request.referrer or \
        url_for('index')

@app.route("/")
@app.route("/list")
async def list():
    # List all Movies
    r = requests.get(url_movies)

    movies_l = r.json().get('movies')
    a1 = "active"

    return await render_template('index.html', a1 = a1, movies = movies_l, t = title, h = heading)

@app.route("/add", methods = ['POST'])
async def add():
    # Add a Movie
    name = (await request.values).get("name")
    date = (await request.values).get("date")
    desc = (await request.values).get("desc")
    if not desc:
        desc = None

    payload = {'name': name, 'date': date, 'desc': desc}
    r = requests.post(url_movies, json = payload)

    return redirect("/")

@app.route("/search", methods = ['GET'])
async def search():
    # Search for Movies
    refer = (await request.values).get("refer")
    key   = (await request.values).get("key")

    if (key == ""):
        r = requests.get(url_movies)
    else:
        url_search = url_movies + "?" + refer + "=" + key
        r = requests.get(url_search)

    movies_l = r.json().get('movies')

    return await render_template('search.html', movies = movies_l, t = title, h = heading)

if __name__ == "__main__":
    app.run(host = '0.0.0.0', port = 5050, debug = True)
