# RTU Game Net Protocol 1
是建立在稳定可靠连接基础之上的通信协议。
## 格式
* header 4字节 任意内容
* length 4字节 小端序整数
* data 长度为length 不超过65528 必须使用lz4压缩，压缩前的尺寸限制在65536。
* 如果收到不合规范的数据，客户端或服务器可以立即关闭连接。