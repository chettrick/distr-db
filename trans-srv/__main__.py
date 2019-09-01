#!/usr/bin/python

import psycopg2
from config import config
from quart import Quart, request, jsonify, abort, make_response
from bson import ObjectId
import sys

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

@app.errorhandler(404)
def not_found(error):
    return make_response(jsonify({'error': 'Not found'}), 404)

@app.route('/api/movies', methods = ['GET'])
async def get_movies():
    # Request all movies directly from the Mongo database.

    # XXX Read from Mongo

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

    # Construct new movie in JSON.
    new_movie = {
        'id': movies[-1]['id'] + 1,
        'name': json_data['name'],
        'desc': json_data.get('desc', None),
        'date': json_data['date']
    }

    # Add the new movie to the database.
    movies.append(new_movie)

    # Return new movie to client with Created status code.
    return jsonify({'movie': new_movie}), 201


def insert_movie(movie_name, movie_date, movie_desc):
    """ insert a new movie into the movies table """
    sql = """
        INSERT INTO movies(movie_name, movie_date, movie_desc)
        VALUES(%s, %s, %s)
        RETURNING movie_id;
        """

    conn = None
    movie_id = None
    try:
        # read database configuration
        params = config()

        # connect to the PostgreSQL server
        print('Connecting to the PostgreSQL database...')
        conn = psycopg2.connect(**params)

        # create a new cursor
        cur = conn.cursor()

        # execute the INSERT statement
        cur.execute(sql, (movie_name, movie_date, movie_desc))
        print('Inserted record to the PostgreSQL database...')

        # get the generated id back
        movie_id = cur.fetchone()[0]

        # commit the changes to the database
        conn.commit()
        print('Committed changes to the PostgreSQL database...')

        # close communication with the database
        cur.close()
    except (Exception, psycopg2.DatabaseError) as error:
        print(error)
    finally:
        if conn is not None:
            conn.close()
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


if __name__ == '__main__':
    insert_movie("Vanilla Sky", "2001", "One daring young man lives forever.")
    insert_movie("Strawberry Sky", "2001", None)
    insert_movie("Chocolate Sky", "2001", "")
    get_movies()

    app.run(host = '0.0.0.0', port = 5555, debug = True)
#    insert_vendor_list([
#        ('AKM Semiconductor Inc.',),
#        ('Asahi Glass Co Ltd.',),
#        ('Daikin Industries Ltd.',),
#        ('Dynacast International Inc.',),
#        ('Foster Electric Co. Ltd.',),
#        ('Murata Manufacturing Co. Ltd.',)
