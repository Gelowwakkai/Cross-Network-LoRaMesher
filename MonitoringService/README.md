# Cross-Network-LoRaMesher

This section aims to setup the web monitoring services.

## Usage

To start the Monitoring Services, in the "MonitoringService" folder execute:

```bash
docker-compose up -d
```

EMQX broker: [http://localhost:18083](http://localhost:18083)
Credentials for your local emqx broker are user: "admin" password: "public"

InfluxDB: [http://localhost:8086](http://localhost:8086)
Credentials for your local influxDB server are user: "root" password: "monitoringservice"

Grafana: [http://localhost:3000](http://localhost:3000)
Credentials for your local grafana server are user: "admin" password: "admin"

Query web app: [http://localhost:3006](http://localhost:3006)


You can also start the monitoring services by running the .sh file:

```bash
./startMonitoring.sh
```

The services will open in your default browser as they start.
