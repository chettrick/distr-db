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
        'desc': json_data.get('desc', null),
        'date': json_data['date']
    }

    # Add the new movie to the database.
    movies.append(new_movie)

    # Return new movie to client with Created status code.
    return jsonify({'movie': new_movie}), 201


def insert_vendor(vendor_name):
    """ insert a new vendor into the vendors table """
    sql = """
        INSERT INTO vendors(vendor_name)
        VALUES(%s) RETURNING vendor_id;
        """

    conn = None
    vendor_id = None
    try:
        # read database configuration
        params = config()

        # connect to the PostgreSQL server
        print('Connecting to the PostgreSQL database...')
        conn = psycopg2.connect(**params)

        # create a new cursor
        cur = conn.cursor()

        # execute the INSERT statement
        cur.execute(sql, (vendor_name,))

        # get the generated id back
        vendor_id = cur.fetchone()[0]

        # commit the changes to the database
        conn.commit()

        # close communication with the database
        cur.close()
    except (Exception, psycopg2.DatabaseError) as error:
        print(error)
    finally:
        if conn is not None:
            conn.close()
            print('Database connection closed.')

    return vendor_id


def get_vendors():
    """ query data from the vendors table """
    conn = None
    try:
        params = config()
        conn = psycopg2.connect(**params)
        cur = conn.cursor()
        cur.execute("SELECT vendor_id, vendor_name FROM vendors ORDER BY vendor_name")
        print("The number of parts: ", cur.rowcount)
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
    app.run(host = '0.0.0.0', port = 5555, debug = True)
#    insert_vendor("3M Co.")
#    get_vendors()
#    insert_vendor_list([
#        ('AKM Semiconductor Inc.',),
#        ('Asahi Glass Co Ltd.',),
#        ('Daikin Industries Ltd.',),
#        ('Dynacast International Inc.',),
#        ('Foster Electric Co. Ltd.',),
#        ('Murata Manufacturing Co. Ltd.',)
