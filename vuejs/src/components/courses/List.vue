<template>
  <div>
    <!-- 面包屑导航区 -->
    <el-breadcrumb separator-class="el-icon-arrow-right">
      <el-breadcrumb-item :to="{ path: '/home' }">首页</el-breadcrumb-item>
      <el-breadcrumb-item>课程管理</el-breadcrumb-item>
      <el-breadcrumb-item>课程列表</el-breadcrumb-item>
    </el-breadcrumb>
    <!-- 卡片视图 -->
    <el-card>
      <!-- 搜索 添加 -->
      <el-row :gutter="20">
        <el-col :span="6">
          <el-input
            placeholder="请输入内容"
            v-model="queryInfo.query"
            clearable
            @clear="getCourseList"
          >
            <el-button
              slot="append"
              icon="el-icon-search"
              @click="getCourseList"
            ></el-button>
          </el-input>
        </el-col>
        <el-col :span="4">
          <el-button
            type="primary"
            @click="
              addDialogVisible = true
              getCollegeList()
            "
            >添加课程</el-button
          >
        </el-col>
      </el-row>
      <!-- 用户列表区域 -->
      <el-table
        :data="courselist"
        border
        @sort-change="handleSortChange"
        stripe
      >
        <!-- stripe: 斑马条纹
        border：边框-->
        <el-table-column type="index" label="#"></el-table-column>
        <el-table-column prop="coursename" label="课程名称"></el-table-column>
        <el-table-column prop="college" label="学院"></el-table-column>
        <el-table-column
          prop="coursenum"
          label="学分"
          :sortable="'custom'"
        ></el-table-column>
        <!-- 之后可以添加是否是必修还是选修 -->
        <el-table-column label="操作">
          <template slot-scope="scope">
            <el-button
              type="primary"
              icon="el-icon-edit"
              size="mini"
              @click="showEditDialog(scope.row.id)"
              >编辑</el-button
            >
            <el-button
              type="danger"
              icon="el-icon-delete"
              size="mini"
              @click="removeCourseById(scope.row.id)"
              >删除</el-button
            >
          </template>
        </el-table-column>
      </el-table>
      <!-- 分页区域 -->
      <el-pagination
        @size-change="handleSizeChange"
        @current-change="handleCurrentChange"
        :current-page="queryInfo.pagenum"
        :page-sizes="[2, 5, 10, 15]"
        :page-size="queryInfo.pagesize"
        layout="sizes, prev, pager, next, jumper"
        :total="totle"
      ></el-pagination>
    </el-card>

    <!-- 添加用户的对话框 -->
    <el-dialog
      title="添加用户"
      :visible.sync="addDialogVisible"
      width="50%"
      @close="addDialogClosed"
    >
      <!-- 内容主体 -->
      <el-form
        :model="addCourseForm"
        ref="addCourseFormRef"
        label-width="100px"
      >
        <el-form-item label="课程名称" prop="coursename">
          <el-input
            v-model="addCourseForm.coursename"
            style="width: 90%"
          ></el-input>
        </el-form-item>
        <el-form-item label="学院" prop="college">
          <el-select
            v-model="addCourseForm.collegeid"
            filterable
            allow-create
            default-first-option
            style="width: 90%"
          >
            <el-option
              v-for="item in collegeList"
              :key="item.id"
              :label="item.collegename"
              :value="item.id"
            ></el-option>
          </el-select>
        </el-form-item>
        <el-form-item label="学分" prop="coursenum">
          <el-input
            v-model="addCourseForm.coursenum"
            style="width: 90%"
          ></el-input>
        </el-form-item>
      </el-form>
      <span slot="footer" class="dialog-footer">
        <el-button @click="addDialogVisible = false">取 消</el-button>
        <el-button type="primary" @click="addCourse">确 定</el-button>
      </span>
    </el-dialog>

    <!-- 修改课程的对话框 -->
    <el-dialog
      title="修改课程信息"
      :visible.sync="editDialogVisible"
      width="50%"
      @close="editDialogClosed"
    >
      <!-- 内容主体 -->
      <el-form
        :model="editCourseForm"
        ref="editCourseFormRef"
        label-width="70px"
      >
        <el-form-item label="课程名称" prop="coursename">
          <el-input
            v-model="editCourseForm.coursename"
            style="width: 90%"
          ></el-input>
        </el-form-item>
        <el-form-item label="学院" prop="college">
          <el-select
            v-model="editCourseForm.collegeid"
            filterable
            allow-create
            default-first-option
            style="width: 90%"
          >
            <el-option
              v-for="item in collegeList"
              :key="item.id"
              :label="item.collegename"
              :value="item.id"
            ></el-option>
          </el-select>
        </el-form-item>
        <el-form-item label="学分" prop="coursenum">
          <el-input
            v-model="editCourseForm.coursenum"
            style="width: 90%"
          ></el-input>
        </el-form-item>
      </el-form>
      <span slot="footer" class="dialog-footer">
        <el-button @click="editDialogVisible = false">取 消</el-button>
        <el-button type="primary" @click="editCourse">确 定</el-button>
      </span>
    </el-dialog>
  </div>
</template>

<script>
export default {
  data() {
    return {
      // 获取用户列表查询参数对象
      queryInfo: {
        // 搜索的字符串
        query: '',
        // 当前页数
        pagenum: 1,
        // 每页显示多少数据
        pagesize: 5,
        // 学院id
        collegeid: '',
        // 排序参数
        sortprop: '',
        // 排序顺序
        sortorder: ''
      },
      courselist: [],
      totle: 0,
      // 添加用户对话框
      addDialogVisible: false,
      // 用户添加
      addCourseForm: {
        coursename: '',
        collegeid: '', // 学院id
        coursenum: ''
      },
      // 修改用户
      editDialogVisible: false,
      editCourseForm: {},
      // 所有角色数据列表
      rolesLsit: [],
      // 学院列表
      collegeList: [],
      // 已选中的角色Id值
      selectRoleId: ''
    }
  },
  created() {
    this.getCourseList()
  },
  methods: {
    async getCourseList() {
      const { data: res } = await this.$http.get('courses', {
        params: this.queryInfo
      })
      if (res.meta.status !== 200) {
        return this.$message.error('获取课程列表失败！')
      }
      this.courselist = res.data.courses
      this.totle = res.data.total
      // console.log(res)
    },
    // 监听排序改变的时间
    handleSortChange({ prop, order }) {
      // console.log({ prop, order })
      this.queryInfo.sortprop = prop
      this.queryInfo.sortorder = order === 'ascending' ? 'asc' : 'desc' // 排序顺序，可以根据需要进行适配
      this.getCourseList()
    },
    // 监听 pagesize改变的事件
    handleSizeChange(newSize) {
      // console.log(newSize)
      this.queryInfo.pagesize = newSize
      this.getCourseList()
    },
    // 监听 页码值 改变事件
    handleCurrentChange(newSize) {
      // console.log(newSize)
      this.queryInfo.pagenum = newSize
      this.getCourseList()
    },

    // 监听 添加用户对话框的关闭事件
    addDialogClosed() {
      this.$refs.addCourseFormRef.resetFields()
    },
    // 添加课程
    addCourse() {
      // 提交请求前，表单预验证
      this.$refs.addCourseFormRef.validate(async (valid) => {
        console.log(valid)
        // 表单预校验失败
        console.log(this.addCourseForm)
        if (!valid) return
        const { data: res } = await this.$http.post(
          'courses',
          this.addCourseForm
        )

        if (res.meta.status !== 201) {
          this.$message.error('添加课程失败！')
        }
        this.$message.success('添加课程成功！')
        // 隐藏添加用户对话框
        this.addDialogVisible = false
        this.getCourseList()
      })
    },
    // 编辑用户信息
    async showEditDialog(id) {
      this.getCollegeList()
      const { data: res } = await this.$http.get('courses/' + id)
      if (res.meta.status !== 200) {
        return this.$message.error('查询用户信息失败！')
      }
      this.editCourseForm = res.data
      this.editDialogVisible = true
    },
    // 监听修改用户对话框的关闭事件
    editDialogClosed() {
      this.$refs.editCourseFormRef.resetFields()
    },
    // 修改用户信息
    editCourse() {
      // 提交请求前，表单预验证
      this.$refs.editCourseFormRef.validate(async (valid) => {
        // console.log(valid)
        // 表单预校验失败
        if (!valid) return
        const { data: res } = await this.$http.put(
          'courses/' + this.editCourseForm.id,
          {
            college: this.editCourseForm.college,
            collegeid: this.editCourseForm.collegeid,
            coursename: this.editCourseForm.coursename,
            coursenum: this.editCourseForm.coursenum
          }
        )
        if (res.meta.status !== 200) {
          this.$message.error('更新课程信息失败！')
        }
        // 隐藏添加用户对话框
        this.editDialogVisible = false
        this.$message.success('更新课程信息成功！')
        this.getCourseList()
      })
    },
    // 删除用户
    async removeCourseById(id) {
      const confirmResult = await this.$confirm(
        '此操作将永久删除该用户, 是否继续?',
        '提示',
        {
          confirmButtonText: '确定',
          cancelButtonText: '取消',
          type: 'warning'
        }
      ).catch((err) => err)
      // 点击确定 返回值为：confirm
      // 点击取消 返回值为： cancel
      if (confirmResult !== 'confirm') {
        return this.$message.info('已取消删除')
      }
      const { data: res } = await this.$http.delete('courses/' + id)
      if (res.meta.status !== 200) return this.$message.error('删除用户失败！')
      this.$message.success('删除用户成功！')
      this.getCourseList()
    },
    // 获取学院列表
    async getCollegeList() {
      // this.classInfo = classinfo
      // 展示对话框之前，获取所有角色列表
      const { data: res } = await this.$http.get('college')
      if (res.meta.status !== 200) {
        return this.$message.error('获取学院列表失败！')
      }
      this.collegeList = res.data
    }
  }
}
</script>

<style lang="less" scoped></style>
