import Vue from 'vue'
import App from './App.vue'
import router from './router'
import './plugins/element.js'

// 导入全局样式
import './assets/css/global.css'
// 导入字体图标
import './assets/fonts/iconfont.css'

// 导入表格树
import TreeTable from 'vue-table-with-tree-grid'
// 导入富文本编辑器
// import VueQuillEditor from 'vue-quill-editor'
// 导入富文本编辑器样式
// import 'quill/dist/quill.core.css'
// import 'quill/dist/quill.snow.css'
// import 'quill/dist/quill.bubble.css'

// 导入NProgress, 包对应的JS和CSS
import NProgress from 'nprogress'
// import 'nprogress/nprogress.css'

import axios from 'axios'
// 配置请求根路径
// 本机地址
axios.defaults.baseURL = 'http://127.0.0.1:9006/api/'
// axios.defaults.baseURL = 'http://192.168.195.164:9006/api/'

// 在request 拦截器中, 展示进度条 NProgress.start()
// 请求在到达服务器之前，先会调用use中的这个回调函数来添加请求头信息
axios.interceptors.request.use((config) => {
  NProgress.start()
  // console.log(config)
  // 为请求头对象，添加token验证的Authorization字段
  config.headers.Authorization = window.sessionStorage.getItem('token')
  // 在最后必须 return config
  return config
})

// response 拦截器中,  隐藏进度条NProgress.done()
axios.interceptors.response.use((config) => {
  NProgress.done()
  return config
})

Vue.config.productionTip = false

// 挂在到Vue实例，后面可通过this调用
Vue.prototype.$http = axios

// 组件全局注册 表格树
Vue.component('tree-table', TreeTable)

new Vue({
  router,
  render: (h) => h(App)
}).$mount('#app')
