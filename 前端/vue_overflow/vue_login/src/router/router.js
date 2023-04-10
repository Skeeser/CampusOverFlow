import Vue from 'vue'
import VueRouter from 'vue-router'
import Login from '@/views/Login'

Vue.use(VueRouter)

const routes = [
  {
    // 用户访问login地址的时候，使用动态组件找到login
    path: '/login',
    // meta: { title: '登录', noCache: true },
    // component: (resolve) => require(['@/views/login'], resolve)
    component: Login
    // hidden: true
  }
]

const router = new VueRouter({
  routes
})

export default router
