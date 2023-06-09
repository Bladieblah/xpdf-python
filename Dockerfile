FROM python:3.10.12-bullseye

RUN apt-get update && apt-get install ffmpeg libsm6 libxext6  -y

ADD . /app
WORKDIR /app
