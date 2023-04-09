## 1.记录状态

不存在跨域问题，使用cookie和session记录状态

若存在，使用token记录状态



## 2.token

验证并生成用户的token值，并返回给客户端

客户端存储token， 后续所有请求携带token

接收到后验证token

