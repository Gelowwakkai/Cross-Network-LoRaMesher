import React, { createContext, useEffect, useState, useCallback } from 'react';
import Connection from './Connection';
import Query from './Query';
import Receiver from './Receiver';
import VisNodes from './VisNodes';
import mqtt from 'mqtt';

export const QosOption = createContext([]);

const qosOption = [
  {
    label: '0',
    value: 0,
  },
  {
    label: '1',
    value: 1,
  },
  {
    label: '2',
    value: 2,
  },
];

const HookMqtt = () => {
  const [client, setClient] = useState(null);
  const [payload, setPayload] = useState({});
  const [connectStatus, setConnectStatus] = useState('Connect');

  const mqttConnect = (host, mqttOption) => {
    setConnectStatus('Connecting');
    setClient(mqtt.connect(host, mqttOption));
  };

  const mqttSub = useCallback((subscription) => {
    if (client) {
      const { subTopic, qos } = subscription;
      client.subscribe(subTopic, { qos }, (error) => {
        if (error) {
          console.log('Subscribe to topics error', error);
          return;
        }
        console.log(`Subscribe to topics: ${subTopic}`);
      });
    }
  }, [client]);

  useEffect(() => {
    if (client) {
      client.on('connect', () => {
        setConnectStatus('Connected');
        console.log('connection successful');
        mqttSub({ subTopic: "to-server/#", qos: 0 }); // Subscribe after connection
      });

      client.on('error', (err) => {
        console.error('Connection error: ', err);
        client.end();
      });

      client.on('reconnect', () => {
        setConnectStatus('Reconnecting');
      });

      client.on('message', (topic, message) => {
        const payload = { topic, message: message.toString() };
        setPayload(payload);
      });
    }
  }, [client, mqttSub]);

  const mqttDisconnect = () => {
    if (client) {
      try {
        client.end(false, () => {
          setConnectStatus('Connect');
          console.log('disconnected successfully');
        });
      } catch (error) {
        console.log('disconnect error:', error);
      }
    }
  };

  const mqttPublish = (context) => {
    if (client) {
      const { node, qos, payload, pubTopic } = context;
      const jsonPayload = JSON.stringify({
        data: {
          appPortDst: 41,
          appPortSrc: 41,
          addrDst: node,
          query: payload
        }
      });
      console.log(pubTopic, node, payload)
      client.publish(pubTopic, jsonPayload, { qos }, (error) => {
        if (error) {
          console.log('Publish error: ', error);
        }
      });
    }
  };

  return (
    <div style={containerStyle}>
      <div style={rowStyle}>
        <div style={componentStyle}>
          <Connection
            connect={mqttConnect}
            disconnect={mqttDisconnect}
            connectBtn={connectStatus}
            sub={mqttSub}
          />
        </div>
        <div style={componentStyle}>
          <VisNodes payload={payload} publish={mqttPublish}/>
        </div>
      </div>
      <div style={rowStyle}>
        <div style={componentStyle}>
          <QosOption.Provider value={qosOption}>
            <Query publish={mqttPublish} />
          </QosOption.Provider>
        </div>
        <div style={componentStyle}>
          <Receiver payload={payload} />
        </div>
      </div>
    </div>
  );
};

// Styles
const containerStyle = {
  display: 'flex',
  flexDirection: 'column',
  alignItems: 'center',
  width: '100%',
  padding: '20px',
};

const rowStyle = {
  display: 'flex',
  justifyContent: 'center',
  width: '100%',
  marginBottom: '20px',
  gap: '20px', // Horizontal separation between components
};

const componentStyle = {
  border: '1px solid #ccc',
  borderRadius: '4px',
  flex: '1',
  minWidth: '0', // Allow overflow for text
  padding: '10px',
  boxSizing: 'border-box',
};

export default HookMqtt;
