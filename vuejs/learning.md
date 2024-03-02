## 1.记录状态

不存在跨域问题，使用cookie和session记录状态

若存在，使用token记录状态



## 2.token

验证并生成用户的token值，并返回给客户端

客户端存储token， 后续所有请求携带token

接收到后验证token



## 3.样式表和一些图片资源

保存在/src/assets中

可以在/style/中找到



## 4.Vue.use是什么？

官方对 Vue.use() 方法的说明：通过全局方法 Vue.use() 使用插件，Vue.use 会自动阻止多次注册相同插件，它需要在你调用 new Vue() 启动应用之前完成，Vue.use() 方法至少传入一个参数，该参数类型必须是 Object 或 Function，如果是 Object 那么这个 Object 需要定义一个 install 方法，如果是 Function 那么这个函数就被当做 install 方法。在 Vue.use() 执行时 install 会默认执行，当 install 执行时第一个参数就是 Vue，其他参数是 Vue.use() 执行时传入的其他参数。就是说使用它之后调用的是该组件的install 方法。



## 5.常用图标路径

\eladmin-web\dist\static\css



## 6.登录相关函数

在\eladmin-web\src\store\modules\user.js



## 7.修改地址

eladmin-web\src\utils\request.js
