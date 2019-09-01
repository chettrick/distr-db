DROP TABLE IF EXISTS movies CASCADE;

CREATE TABLE movies (
	movie_id BIGSERIAL PRIMARY KEY,
	movie_name VARCHAR(255) NOT NULL,
	movie_date INTEGER NOT NULL,
	movie_desc TEXT,
	UNIQUE (movie_name, movie_date)
);
