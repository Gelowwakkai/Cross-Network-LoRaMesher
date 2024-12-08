#!/bin/sh

# Function to start Docker services
start_services() {
  echo "Starting Docker services..."
  docker-compose up -d
}

# Function to open the web browser to the specified URL
open_browser() {
  url=$1
  echo "Waiting for $url to be ready..."
  while ! curl --fail --silent --head "$url"; do
    sleep 1
  done
  if which xdg-open > /dev/null
  then
    xdg-open "$url"
  else
    echo "Could not detect the web browser to open. Please manually open $url"
  fi
}

# Function to stop Docker services
stop_services() {
  echo "Stopping Docker services..."
  docker-compose down
}

# Start Docker services
start_services

# Open the web interfaces
open_browser http://localhost:18083
open_browser http://localhost:8086
open_browser http://localhost:3000
open_browser http://localhost:3006

# Prompt user to press Enter to stop services
echo "Press Enter to stop the services..."
read -r _

# Stop Docker services
stop_services

