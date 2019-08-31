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
        'desc': json_data.get('desc', ""),
        'date': json_data['date']
    }

    # Add the new movie to the in-memory dictionary.
    # The dictionary is a temporary stop-gap used until the databases
    # are functioning, at which point the dictionary will be removed
    movies.append(new_movie)

    # Add the new movie to the postgreSQL database
    add_movie(json_data['name'], json_data.get('desc', ""), json_data['date'])

    # Return new movie to client with Created status code.
    return jsonify({'movie': new_movie}), 201


def create_tables():
    """ Create tables in the PostgreSQL database"""
#    commands = (
#        """
#        DROP TABLE IF EXISTS vendors CASCADE
#        """,
#        """
#        DROP TABLE IF EXISTS parts CASCADE
#        """,
#        """
#        DROP TABLE IF EXISTS part_drawings CASCADE
#        """,
#        """
#        DROP TABLE IF EXISTS vendor_parts CASCADE
#        """,
#        """
#        CREATE TABLE vendors (
#            vendor_id SERIAL PRIMARY KEY,
#            vendor_name VARCHAR(255) NOT NULL
#        )
#        """,
#        """
#        CREATE TABLE parts (
#            part_id SERIAL PRIMARY KEY,
#            part_name VARCHAR(255) NOT NULL
#        )
#        """,
#        """
#        CREATE TABLE part_drawings (
#            part_id INTEGER PRIMARY KEY,
#            file_extension VARCHAR(5) NOT NULL,
#            drawing_data BYTEA NOT NULL,
#            FOREIGN KEY (part_id)
#                REFERENCES parts (part_id)
#                ON UPDATE CASCADE ON DELETE CASCADE
#        )
#        """,
#        """
#        CREATE TABLE vendor_parts (
#            vendor_id INTEGER NOT NULL,
#            part_id INTEGER NOT NULL,
#            PRIMARY KEY (vendor_id , part_id),
#            FOREIGN KEY (vendor_id)
#                REFERENCES vendors (vendor_id)
#                ON UPDATE CASCADE ON DELETE CASCADE,
#            FOREIGN KEY (part_id)
#                REFERENCES parts (part_id)
#                ON UPDATE CASCADE ON DELETE CASCADE
#        )
#        """
#    )

    commands = (
        """
        DROP TABLE IF EXISTS movies CASCADE
        """,
        """
        CREATE TABLE movies (
            name VARCHAR(255) NOT NULL,
            description VARCHAR(255),
            date VARCHAR(255) NOT NULL,
            PRIMARY KEY (name, date)
        )
        """    
    )

    conn = None
    try:
        # read connection parameters
        params = config()

        # connect to the PostgreSQL server
        print('Connecting to the PostgreSQL database...')
        conn = psycopg2.connect(**params)

        # create a cursor
        cur = conn.cursor()

        # create table one by one
        for command in commands:
            cur.execute(command)

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

async def add_movie(name_arg, descr_arg, date_arg):
    """ insert a new movie into the movies table """
    sql = """
        INSERT INTO movies(name, description, date)
        VALUES(%s, %s, %s)
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
        await cur.execute(sql, (name_arg,descr_arg,date_arg))
#        cur.execute(sql)

        # get the generated id back (used when a unique id is generated via 'SERIAL')
#        vendor_id = cur.fetchone()[0]

        # commit the changes to the database
        conn.commit()

        # close communication with the database
        cur.close()

        print("Movie written to db, no error occured")
    except (Exception, psycopg2.DatabaseError) as error:
        print("Error occured:", error)
    finally:
        if conn is not None:
            conn.close()
            print('Database connection closed.')

#    return vendor_id


if __name__ == '__main__':
    create_tables()
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
