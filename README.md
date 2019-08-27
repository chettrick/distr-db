# distr-db
database engine implementing CQRS via separate reads and writes

## API
The API uses JSON throughout.
A movie JSON object has four fields:

Field | Purpose | Type | Required Field
---|---|---|---
id   | A unique identifier for movies. | Numeric | Auto-generated by Transaction Server
name | Name of movie.                  | String  | Required
desc | Description of movie.           | String  | Optional
date | Release date of movie.          | String  | Required

Currently, there are only three HTTP requests that will get a response.

* Retrieve list of all movies from the database:
  * GET request to endpoint http://localhost:5555/api/movies
  * Example cURL command:
    * $ curl -i http://localhost:5555/api/movies
  * Example response from transaction server:
```
HTTP/1.1 200
content-type: application/json
content-length: 289
date: Mon, 26 Aug 2019 23:10:39 GMT
server: hypercorn-h11

{
  "movies": [
    {
      "date": "1986",
      "desc": "One daring young pilot flies jets.",
      "id": 1,
      "name": "Top Gun"
    },
    {
      "date": "1990",
      "desc": "One daring young driver drives cars.",
      "id": 2,
      "name": "Days of Thunder"
    }
  ]
}
```

* Retrieve one movie from the database based on its id.
  * GET request to endpoint http://localhost:5555/api/movies/[id]
  * Example cURL command:
    * $ curl -i http://localhost:5555/api/movies/1
  * Example response from transaction server:
```
HTTP/1.1 200
content-type: application/json
content-length: 128
date: Mon, 26 Aug 2019 23:20:13 GMT
server: hypercorn-h11

{
  "movie": {
    "date": "1986",
    "desc": "One daring young pilot flies jets.",
    "id": 1,
    "name": "Top Gun"
  }
}
```

* Create a new movie and add it to the database:
  * POST to endpoint http://localhost:5555/api/movies
  * Currently "name" and "date" fields are required in POST request.
  * Example cURL command:
    * $ curl -i -H "Content-Type: application/json" -X POST -d '{"name": "Mission Impossible", "date": "1996", "desc": "One daring young man is a secret agent."}' http://localhost:5555/api/movies
  * Example response from transaction server:
```
HTTP/1.1 201
content-type: application/json
content-length: 145
date: Mon, 26 Aug 2019 23:22:34 GMT
server: hypercorn-h11

{
  "movie": {
    "date": "1996",
    "desc": "One daring young man is a secret agent.",
    "id": 3,
    "name": "Mission Impossible"
  }
}
```
