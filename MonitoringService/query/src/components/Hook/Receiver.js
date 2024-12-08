import React, { useEffect, useState } from 'react';
import { List } from 'antd';
import moment from 'moment'; // Ensure you have moment installed

const Receiver = ({ payload }) => {
  const [messages, setMessages] = useState([]);

  useEffect(() => {
    if (payload.topic) {
      const messageObject = JSON.parse(payload.message);
      if ("queryAns" in messageObject.data && messageObject.data.query !== "Routing Table GW") {
        const newMessage = {
          text: `${messageObject.data.addrSrc}: ${messageObject.data.queryAns} ${messageObject.data.query}`,
          timestamp: moment().format('YYYY-MM-DD HH:mm:ss')
        };
        setMessages(messages => {
          const updatedMessages = [newMessage, ...messages];
          return updatedMessages.slice(0, 5); // Keep only the last 5 messages
        });
      }
    }
  }, [payload]);

  const renderListItem = (item) => (
    <List.Item style={listItemStyle}>
      <List.Item.Meta
        title={<div style={titleStyle}>{item.text}</div>}
        description={<div style={descriptionStyle}>{item.timestamp}</div>}
      />
    </List.Item>
  );

  return (
    <>
      <h2 style={headerStyle}>ANSWER</h2>
      <div style={listContainerStyle}>
        <List
          size="small"
          bordered
          dataSource={messages}
          renderItem={renderListItem}
        />
      </div>
    </>
  );
};

// Styles
const listContainerStyle = {
  maxHeight: '100%',
  overflowY: 'scroll',
  padding: '20px',
  backgroundColor: 'transparent',
  borderRadius: '8px',
};

const listItemStyle = {
  padding: '10px',
  backgroundColor: '#ffffff',
  border: '1px solid #d9d9d9',
  borderRadius: '8px',
  marginBottom: '10px',
};

const titleStyle = {
  fontWeight: 'bold',
  fontSize: '16px',
};

const descriptionStyle = {
  color: '#595959',
  fontSize: '14px',
};

const headerStyle = {
  textAlign: 'left',
  marginBottom: '20px',
};

export default Receiver;
