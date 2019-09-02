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

    movie_list = None

    # Read from Mongo into a Python list.
    cursor = coll.find()
    movie_list = await cursor.to_list(length=100)
    print("GET request to return all movies")
    print("returned movie list: ", movie_list)

    if movie_list is None:
        abort(404)

    return jsonify({'movies': movie_list})


@app.route('/api/movies/<int:movie_id>', methods = ['GET'])
async def get_movie(movie_id):
    # Request a movie with id of movie_id from the Mongo database.

    the_movie = None

    # Read from Mongo via movie_id.
    the_movie = await coll.find_one({'_id': movie_id})
    print("GET request to return one movie")
    print("returned movie: ", the_movie)

    if the_movie is None:
        abort(404)

    return jsonify({'movie': the_movie})


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

        # The INSERT to Postgres was successful,
        # so now write the record to the Mongo database.
        await insert_to_mongo(movie_id, movie_name, movie_date, movie_desc)

    except (Exception, psycopg2.DatabaseError) as error:
        print("Error occured:", error)
    finally:
        if conn is not None:
            await conn.close()
            print('Database connection closed.')

    return movie_id


def get_movies_from_postgres():
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


async def insert_to_mongo(movie_id, movie_name, movie_date, movie_desc):
    # Mongo database connection
    print('Connecting to the MongoDB database...')
    new_movie = {
        '_id': movie_id,
        'name': movie_name,
        'date': movie_date,
        'desc': movie_desc
    }
    print('Inserting data record...')
    result = await coll.insert_one(new_movie)
    print('result %s' % repr(result.inserted_id))
    print('Listing inserted id...')
    print(result.inserted_id)
    print('Print out first document...')
    print(await coll.find_one())
    print('Database connection closed.')

    return result


if __name__ == '__main__':
    app.run(host = '0.0.0.0', port = 5555, debug = True)
