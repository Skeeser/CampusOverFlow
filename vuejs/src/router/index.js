import Vue from 'vue'
import VueRouter from 'vue-router'

Vue.use(VueRouter)

const routes = [
  { path: '/', redirect: '/login' },
  {
    path: '/login',
    meta: { title: '登录' },
    component: (resolve) => require(['@/components/Login'], resolve)
  },
  {
    path: '/home',
    meta: { title: '主页' },
    component: (resolve) => require(['@/components/Home'], resolve),
    redirect: '/Welcome',
    children: [
      {
        path: '/home',
        component: (resolve) => require(['@/components/Welcome'], resolve)
      },
      {
        path: '/students',
        component: (resolve) => require(['@/components/user/Students'], resolve)
      },

      {
        path: '/teachers',
        component: (resolve) => require(['@/components/user/Teachers'], resolve)
      },
      {
        path: '/rights',
        component: (resolve) => require(['@/components/power/Rights'], resolve)
      },
      {
        path: '/roles',
        component: (resolve) => require(['@/components/power/Roles'], resolve)
      },
      {
        path: '/score',
        component: (resolve) => require(['@/components/courses/Score'], resolve)
      },
      {
        path: '/courselist',
        component: (resolve) => require(['@/components/courses/List'], resolve)
      },
      {
        path: '/course',
        component: (resolve) => require(['@/components/class/Class'], resolve)
      },
      {
        path: '/class',
        component: (resolve) => require(['@/components/class/Class'], resolve)
      }
    ]
  }
]

const router = new VueRouter({
  base: '/admin/',
  routes
})

// 挂载路由导航守卫,to表示将要访问的路径，from表示从哪里来，next是下一个要做的操作 next('/login')强制跳转login
router.beforeEach((to, from, next) => {
  // 访问登录页，放行
  if (to.path === '/login') return next()
  // 获取token
  const tokenStr = window.sessionStorage.getItem('token')
  // 没有token, 强制跳转到登录页
  if (!tokenStr) return next('/login')
  next()
})

export default router
