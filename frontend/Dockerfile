FROM python:3.7

ADD requirements.txt /requirements.txt
RUN pip3 install -r requirements.txt

WORKDIR frontend
ADD . .

ENTRYPOINT ["python3", "app.py"]
