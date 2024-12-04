# Cross-Network-LoRaMesher

This project aims to explore cross-network LoRaMesher applications.

## Installation

Install docker:

```bash
sudo apt update

sudo apt install -y ca-certificates curl gnupg lsb-release

sudo mkdir -p /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg

sudo echo  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu  $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
sudo apt update

sudo apt install -y docker-ce docker-ce-cli containerd.io docker-compose-plugin

sudo groupadd docker

sudo usermod -aG docker $USER
```

Install docker compose:

```bash
sudo curl -L "https://github.com/docker/compose/releases/latest/download/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose
```

## Usage


To start the Monitoring Services, in the "MonitoringService" folder execute:

```bash
docker-compose up -d
```
Credentials for your local emqx broker are user: "admin" password: "public"
Credentials for your local grafana server are user: "admin" password: "admin"

EMQX broker: [http://localhost:18083](http://localhost:18083)

InfluxDB: [http://localhost:8086](http://localhost:8086)

Grafana: [http://localhost:3000](http://localhost:3000)

Query web app: [http://localhost:3006](http://localhost:3006)
