<template>
  <!-- 通过类名，用css定义各种样式 -->
  <div class="login" :style="'background-image:url(' + Background + ');'">
    <!-- el-form element的组件 表单 -->
    <!-- model表示数据绑定，填写的数据都会自动同步到model对象上
        rules表示验证规则， prop指定具体的对应的规则
    -->
    <el-form
      ref="loginForm"
      :model="loginForm"
      :rules="loginRules"
      label-position="left"
      label-width="0px"
      class="login-form"
    >
      <h3 class="title">高校选课成绩管理系统</h3>
      <el-form-item prop="username">
        <!-- 组件的文本的输入框 -->
        <!-- v-model会绑定到model的某个具体属性中 -->
        <el-input
          v-model="loginForm.username"
          type="text"
          auto-complete="off"
          placeholder="账号"
          prefix-icon="iconfont icon-user"
        >
        </el-input>
      </el-form-item>
      <el-form-item prop="password">
        <!-- type password 表示密码形式，会帮你变成隐藏形式 -->
        <el-input
          v-model="loginForm.password"
          type="password"
          auto-complete="off"
          placeholder="密码"
          prefix-icon="iconfont icon-3702mima"
          @keyup.enter.native="handleLogin"
        >
        </el-input>
      </el-form-item>
      <el-form-item style="width: 100%">
        <el-button
          size="medium"
          type="primary"
          style="width: 100%"
          @click.native.prevent="handleLogin"
        >
          <span>登 录</span>
        </el-button>
      </el-form-item>
    </el-form>
  </div>
</template>

<script>
import Background from '@/assets/images/background_arknights01.jpg'

// 下面是具体的行为区
export default {
  name: 'Login',
  data() {
    // 返回数据对象
    return {
      Background: Background,
      // 数据绑定返回的的表单对象
      loginForm: {
        username: '',
        password: ''
      },
      // 具体的验证规则的定义
      // blur鼠标离开触发验证
      loginRules: {
        username: [
          { required: true, trigger: 'blur', message: '用户名不能为空' }
        ],
        password: [{ required: true, trigger: 'blur', message: '密码不能为空' }]
      }
    }
  },
  created() {
    document.title = '登录页面'
  },
  watch: {
    $route: {
      handler: function (route) {
        const data = route.query
        if (data && data.redirect) {
          this.redirect = data.redirect
          delete data.redirect
          if (JSON.stringify(data) !== '{}') {
            this.redirect =
              this.redirect + '&' + qs.stringify(data, { indices: false })
          }
        }
      },
      immediate: true
    }
  },
  methods: {
    handleLogin() {
      // 数据预验证
      // ref 表单引用对象  vaild验证结果，是一个布尔值
      this.$refs.loginForm.validate(async (valid) => {
        // 验证通过，发起请求
        if (valid) {
          // console.log('验证通过')
          // this.$http.post('login', this.loginForm): 返回值为promise
          // 返回值为promise，可加await简化操作 相应的也要加async
          const { data: res } = await this.$http.post('login', this.loginForm)
          console.log(res)
          if (res.meta.status !== 200) return this.$message.error('登录失败')
          this.$message.success('登录成功')
          // 1、将登陆成功之后的token, 保存到客户端的sessionStorage中; localStorage中是持久化的保存
          //   1.1 项目中出现了登录之外的其他API接口，必须在登陆之后才能访问
          //   1.2 token 只应在当前网站打开期间生效，所以将token保存在sessionStorage中
          window.sessionStorage.setItem('token', res.data.token)
          // 2、通过编程式导航跳转到后台主页, 路由地址为：/home
          this.$router.push('/home')
        } else {
          console.log('error submit!!')
          return false
        }
      })
    }
  }
}
</script>

<style rel="stylesheet/scss" lang="scss">
// .是类选择器
.login {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 100%;
  background-size: cover;
}
.title {
  margin: 0 auto 30px auto;
  text-align: center;
  color: #707070;
}

.login-form {
  border-radius: 6px;
  background: rgba(255, 255, 255, 0.8);
  width: 385px;
  padding: 25px 25px 5px 25px;
  .el-input {
    height: 38px;
    input {
      height: 38px;
    }
  }
  .input-icon {
    height: 39px;
    width: 14px;
    margin-left: 2px;
  }
}
.login-tip {
  font-size: 13px;
  text-align: center;
  color: #bfbfbf;
}
.login-code {
  width: 33%;
  display: inline-block;
  height: 38px;
  float: right;
  img {
    cursor: pointer;
    vertical-align: middle;
  }
}
</style>
