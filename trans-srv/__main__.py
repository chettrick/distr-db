#!/usr/bin/python

import psycopg2
from config import config
from quart import Quart, request, jsonify, abort, make_response
from bson import ObjectId
import sys
import asyncio
import asyncpg
import motor.motor_asyncio

app = Quart(__name__)
app.config['JSONIFY_PRETTYPRINT_REGULAR'] = True

# Sample database for testing API.
movies = [
    {
        'id': 1,
        'name': u'Top Gun',
        'desc': u'One daring young pilot flies jets.',
        'date': u'1986'
    },
    {
        'id': 2,
        'name': u'Days of Thunder',
        'desc': u'One daring young driver drives cars.',
        'date': u'1990'
    }
]

motorclient = None  # Motor (MongoDB) client
db = None           # MongoDB database
coll = None         # Movies collection of the database

@app.before_serving
async def init():
    global motorclient
    global db
    global coll
    # Create a Motor (MongoDB) client
    motorclient = motor.motor_asyncio.AsyncIOMotorClient("mongodb://mongo:27017/",
        username='docker', password='docker', io_loop=asyncio.get_event_loop())
    db = motorclient.docker # Get a MongoDB database
    coll = db.movies        # Get the movies collection of the database


@app.errorhandler(404)
def not_found(error):
    return make_response(jsonify({'error': 'Not found'}), 404)


@app.route('/api/movies', methods = ['GET'])
async def get_movies():
    # Request all movies directly from the Mongo database.

    # XXX Read from Mongo
    await do_insert()

    # Mongo database connection
    print('Connecting to the MongoDB database...')
    data = {
        "_id": 59,
        "name": "The Matrix 59",
        "date": "2059",
        "desc": "Neo, in his old age, practices Tai Chi"
    }
    print('Inserting data document...')
    result = await coll.insert_one(data)
    print('result %s' % repr(result.inserted_id))
    print('Listing database names...')
    print(await motorclient.list_database_names())
    print('Listing inserted id...')
    print(result.inserted_id)
    print('Find document by _id and print it out...')
    print(await coll.find_one({'_id': 59}))
    print('Database connection closed.')

    return jsonify({'movies': movies})


@app.route('/api/movies/<int:movie_id>', methods = ['GET'])
async def get_movie(movie_id):
    # Request a movie with id of movie_id from the Mongo database.

    # XXX Read from Mongo
    movie = [movie for movie in movies if movie['id'] == movie_id]
    if len(movie) == 0:
        abort(404)

    return jsonify({'movie': movie[0]})


@app.route('/api/movies', methods = ['POST'])
async def create_movie():
    json_data = await request.get_json()
    print("json data:", json_data, file=sys.stderr)

    # Slight validation on POSTed JSON. Abort on Bad Request.
    if not json_data or \
       not 'name' in json_data or \
       not 'date' in json_data:
        abort(400)

    movie_name = json_data['name']
    movie_date = json_data['date']
    movie_desc = json_data.get('desc', None)

    # Add the new movie to the database.
    movie_id = await insert_movie(movie_name, movie_date, movie_desc)

    # XXX Check return value of movie_id.

    # Construct new movie in JSON.
    new_movie = {
        'id': movie_id,
        'name': movie_name,
        'date': movie_date,
        'desc': movie_desc
    }

    # Return new movie to client with Created status code.
    return jsonify({'movie': new_movie}), 201


async def insert_movie(movie_name, movie_date, movie_desc):
    """ insert a new movie into the movies table """
    sql = """
        INSERT INTO "movies" ("movie_name", "movie_date", "movie_desc")
        VALUES($1, $2, $3)
        RETURNING movie_id;
        """

    conn = None
    movie_id = None
    try:
        # read database configuration
        params = config()

        # connect to the PostgreSQL server
        print('Connecting to the PostgreSQL database...')
        conn = await asyncpg.connect(**params)

        # execute the INSERT statement and get the generated id back
        movie_id = await conn.fetchval(sql, movie_name, movie_date, movie_desc)
        print('Inserted record to the PostgreSQL database...')
        print("movie id: " + str(movie_id))

        # commit the changes to the database
        print('Committed changes to the PostgreSQL database...')

        # close communication with the database
        await conn.close()

        print("Movie written to db, no error occured")
    except (Exception, psycopg2.DatabaseError) as error:
        print("Error occured:", error)
    finally:
        if conn is not None:
            await conn.close()
            print('Database connection closed.')

    return movie_id


def get_movies():
    """ query data from the movies table """
    conn = None
    try:
        params = config()
        conn = psycopg2.connect(**params)
        cur = conn.cursor()
        cur.execute("SELECT * FROM movies ORDER BY movie_name")
        print("The number of movies: ", cur.rowcount)
        row = cur.fetchone()

        while row is not None:
            print(row)
            row = cur.fetchone()

        cur.close()
    except (Exception, psycopg2.DatabaseError) as error:
        print(error)
    finally:
        if conn is not None:
            conn.close()


async def do_insert():
    # Mongo database connection
    print('Connecting to the MongoDB database...')
    data = {
        "_id": 42,
        "name": "The Hitchhikers Get Lost",
        "date": "20042",
        "desc": "The Meaning of Life is Survival"
    }
    print('Inserting data record...')
    result = await coll.insert_one(data)
    print('result %s' % repr(result.inserted_id))
    print('Listing database names...')
    print(await motorclient.list_database_names())
    print('Listing inserted id...')
    print(result.inserted_id)
    print('Print out first document...')
    print(await coll.find_one())
    print('Database connection closed.')


if __name__ == '__main__':
    app.run(host = '0.0.0.0', port = 5555, debug = True)
