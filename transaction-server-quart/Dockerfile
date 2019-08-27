FROM python:3.7

RUN apt-get install libpq-dev

ADD requirements.txt /requirements.txt
RUN pip3 install -r requirements.txt

## EXPOSE 5000:5000
WORKDIR transaction-server
ADD . .

ENTRYPOINT ["python3", "-u", "transaction_server.py"]
