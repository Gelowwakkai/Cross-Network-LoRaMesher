import React from 'react'
import { Button, Form, Input, Row, Col, Select } from 'antd'

const Connection = ({ connect, disconnect, connectBtn, sub }) => {
  const [form] = Form.useForm()
  const initialConnectionOptions = {
    protocol: 'ws',
    host: 'localhost',
    clientId: 'searchWebApp',
    port: 8083,
    username: '',
    password: '',
  }

  const handleProtocolChange = (value) => {
    form.setFieldsValue({
      port: value === 'wss' ? 8084 : 8083,
    })
  }

  const onFinish = (values) => {
    const { protocol, host, clientId, port, username, password } = values
    const url = `${protocol}://${host}:${port}/mqtt`
    const options = {
      clientId,
      username,
      password,
      clean: true,
      reconnectPeriod: 1000,
      connectTimeout: 30 * 1000,
    }
    connect(url, options)
  }

  const handleConnect = () => {
    form.submit()
  }

  const handleDisconnect = () => {
    disconnect()
  }

  const labelStyle = { color: 'white' }; // Style for labels

  const ConnectionForm = (
    <Form
      layout="vertical"
      name="basic"
      form={form}
      initialValues={initialConnectionOptions}
      onFinish={onFinish}
    >
      <Row gutter={20}>
        <Col span={8}>
          <Form.Item label={<span style={labelStyle}>Protocol</span>} name="protocol">
            <Select onChange={handleProtocolChange}>
              <Select.Option value="ws">ws</Select.Option>
              <Select.Option value="wss">wss</Select.Option>
            </Select>
          </Form.Item>
        </Col>
        <Col span={8}>
          <Form.Item label={<span style={labelStyle}>Host</span>} name="host">
            <Input />
          </Form.Item>
        </Col>
        <Col span={8}>
          <Form.Item label={<span style={labelStyle}>Port</span>} name="port">
            <Input />
          </Form.Item>
        </Col>
        <Col span={8}>
          <Form.Item label={<span style={labelStyle}>Client ID</span>} name="clientId">
            <Input />
          </Form.Item>
        </Col>
        <Col span={8}>
          <Form.Item label={<span style={labelStyle}>Username</span>} name="username">
            <Input />
          </Form.Item>
        </Col>
        <Col span={8}>
          <Form.Item label={<span style={labelStyle}>Password</span>} name="password">
            <Input />
          </Form.Item>
        </Col>
      </Row>
    </Form>
  )

  return (
    <div style={{ display: 'flex', flexDirection: 'column', alignItems: 'stretch' }}>
      <h2>CONNECTION</h2>
      {ConnectionForm}
      <Button type="primary" onClick={handleConnect} style={{ width: '100%', marginBottom: '8px' }}>
        {connectBtn}
      </Button>
      <Button danger onClick={handleDisconnect} style={{ width: '100%' }}>
        Disconnect
      </Button>
    </div>
  )
}

export default Connection
