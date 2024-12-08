import React, { useContext } from 'react';
import { Form, Input, Row, Col, Button, Select } from 'antd';
import { QosOption } from './index';

const Query = ({ publish }) => {
  const [form] = Form.useForm();
  const qosOptions = useContext(QosOption);

  const handleButtonClick = (payload) => {
    form.setFieldsValue({ payload });
    form.submit();
  };

  const onFinish = (values) => {
    const { gateway_node, qos, node, payload } = values;
    const pubTopic = "from-server/" + gateway_node;
    console.log(pubTopic)
    const pubValues = { node, qos, payload, pubTopic };
    publish(pubValues);
  };

  const recordP = {
    gateway_node: '',
    node: '',
    qos: 0,
    payload: '', // This will be set by button clicks
  };

  const labelStyle = { color: 'white' }; // Style for labels
  const buttonStyle = { width: '100%' }; // Style for buttons

  const QueryForm = (
    <Form
      layout="vertical"
      name="basic"
      form={form}
      initialValues={recordP}
      onFinish={onFinish}
    >
      <Row gutter={20}>
        <Col span={24}>
          <Form.Item
            label={<span style={labelStyle}>Gateway Node</span>}
            name="gateway_node"
          >
            <Input />
          </Form.Item>
        </Col>
        <Col span={12}>
          <Form.Item
            label={<span style={labelStyle}>Node</span>}
            name="node"
          >
            <Input />
          </Form.Item>
        </Col>
        <Col span={12}>
          <Form.Item
            label={<span style={labelStyle}>QoS</span>}
            name="qos"
          >
            <Select options={qosOptions} />
          </Form.Item>
        </Col>
        <Col span={24}>
          <Form.Item
            name="payload"
            hidden
          >
            <Input type="number" />
          </Form.Item>
        </Col>
        <Col span={8}>
          <Form.Item>
            <Button type="primary" style={buttonStyle} onClick={() => handleButtonClick(0)}>
              Services
            </Button>
          </Form.Item>
        </Col>
        <Col span={8}>
          <Form.Item>
            <Button type="primary" style={buttonStyle} onClick={() => handleButtonClick(1)}>
              Routes
            </Button>
          </Form.Item>
        </Col>
        <Col span={8}>
          <Form.Item>
            <Button type="primary" style={buttonStyle} onClick={() => handleButtonClick(2)}>
              Status
            </Button>
          </Form.Item>
        </Col>
        <Col span={8}>
          <Form.Item>
            <Button type="primary" style={buttonStyle} onClick={() => handleButtonClick(3)}>
              Outgoing Messages
            </Button>
          </Form.Item>
        </Col>
        <Col span={8}>
          <Form.Item>
            <Button type="primary" style={buttonStyle} onClick={() => handleButtonClick(4)}>
              Incoming Messages
            </Button>
          </Form.Item>
        </Col>
        <Col span={8}>
          <Form.Item>
            <Button type="primary" style={buttonStyle} onClick={() => handleButtonClick(6)}>
              Sensor Value
            </Button>
          </Form.Item>
        </Col>
      </Row>
    </Form>
  );

  return (
    <>
      <h2>QUERY</h2>
      {QueryForm}
    </>
  );
};

export default Query;
