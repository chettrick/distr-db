#!/usr/bin/python

import psycopg2
from config import config

def create_tables():
    """ Create tables in the PostgreSQL database"""
    commands = (
        """
        CREATE TABLE vendors (
            vendor_id SERIAL PRIMARY KEY,
            vendor_name VARCHAR(255) NOT NULL
        )
        """,
        """
        CREATE TABLE parts (
            part_id SERIAL PRIMARY KEY,
            part_name VARCHAR(255) NOT NULL
        )
        """,
        """
        CREATE TABLE part_drawings (
            part_id INTEGER PRIMARY KEY,
            file_extension VARCHAR(5) NOT NULL,
            drawing_data BYTEA NOT NULL,
            FOREIGN KEY (part_id)
                REFERENCES parts (part_id)
                ON UPDATE CASCADE ON DELETE CASCADE
        )
        """,
        """
        CREATE TABLE vendor_parts (
            vendor_id INTEGER NOT NULL,
            part_id INTEGER NOT NULL,
            PRIMARY KEY (vendor_id , part_id),
            FOREIGN KEY (vendor_id)
                REFERENCES vendors (vendor_id)
                ON UPDATE CASCADE ON DELETE CASCADE,
            FOREIGN KEY (part_id)
                REFERENCES parts (part_id)
                ON UPDATE CASCADE ON DELETE CASCADE
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


if __name__ == '__main__':
#    create_tables()
    insert_vendor("3M Co.")
    get_vendors()
#    insert_vendor_list([
#        ('AKM Semiconductor Inc.',),
#        ('Asahi Glass Co Ltd.',),
#        ('Daikin Industries Ltd.',),
#        ('Dynacast International Inc.',),
#        ('Foster Electric Co. Ltd.',),
#        ('Murata Manufacturing Co. Ltd.',)
