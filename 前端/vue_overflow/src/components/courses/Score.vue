<template>
  <div>
    <!-- 面包屑导航区 -->
    <el-breadcrumb separator-class="el-icon-arrow-right">
      <el-breadcrumb-item :to="{ path: '/home' }">首页</el-breadcrumb-item>
      <el-breadcrumb-item>课程管理</el-breadcrumb-item>
      <el-breadcrumb-item>选课管理</el-breadcrumb-item>
    </el-breadcrumb>
    <!-- 卡片视图 -->
    <el-card>
      <!-- 搜索 添加 -->
      <el-row :gutter="20">
        <el-col :span="6">
          <el-input
            placeholder="请输入学号"
            v-model="queryInfo.query"
            clearable
            @clear="getScoreList"
          >
            <el-button
              slot="append"
              icon="el-icon-search"
              @click="getScoreList"
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
            >添加成绩</el-button
          >
        </el-col>
      </el-row>
      <!-- 成绩列表区域 -->
      <el-table :data="scorelist" border @sort-change="handleSortChange" stripe>
        <!-- stripe: 斑马条纹
        border：边框-->
        <!-- 索引列 -->
        <el-table-column type="index" label="#"></el-table-column>
        <el-table-column prop="name" label="姓名"></el-table-column>
        <el-table-column prop="grade" label="年级"></el-table-column>
        <el-table-column prop="classname" label="班级"></el-table-column>
        <el-table-column prop="college" label="学院"></el-table-column>
        <el-table-column prop="course" label="课程"></el-table-column>
        <el-table-column
          prop="score"
          label="成绩"
          :sortable="'custom'"
        ></el-table-column>
        <el-table-column label="是否及格">
          <template slot-scope="scope">
            <el-tag type="success" size="mini" v-if="scope.row.ispass === '1'"
              >已及格</el-tag
            >
            <el-tag type="danger" size="mini" v-else>未及格</el-tag>
          </template>
        </el-table-column>
        <el-table-column label="操作">
          <template slot-scope="scope">
            <el-button
              type="primary"
              icon="el-icon-edit"
              size="mini"
              circle
              @click="showEditDialog(scope.row.id)"
            ></el-button>
            <el-button
              type="danger"
              icon="el-icon-delete"
              size="mini"
              circle
              @click="removeScoreById(scope.row.id)"
            ></el-button>
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

    <!-- 添加成绩的对话框 -->
    <el-dialog
      title="添加成绩"
      :visible.sync="addDialogVisible"
      width="50%"
      @close="addDialogClosed"
    >
      <!-- 内容主体 -->
      <el-form :model="addScoreForm" ref="addScoreFormRef" label-width="100px">
        <el-form-item label="学号" prop="stuid">
          <el-input
            v-model="addScoreForm.stuid"
            @change="handleStuidChange"
            style="width: 90%"
          ></el-input>
        </el-form-item>
        <el-form-item label="课程" prop="college">
          <el-select
            v-model="addScoreForm.courseid"
            filterable
            default-first-option
            style="width: 90%"
          >
            <el-option
              v-for="item in courseList"
              :key="item.id"
              :label="item.collegename"
              :value="item.id"
            ></el-option>
          </el-select>
        </el-form-item>
        <el-form-item label="成绩" prop="scorenum">
          <el-input
            v-model="addScoreForm.scorenum"
            style="width: 90%"
          ></el-input>
        </el-form-item>
      </el-form>
      <span slot="footer" class="dialog-footer">
        <el-button @click="addDialogVisible = false">取 消</el-button>
        <el-button type="primary" @click="addScore">确 定</el-button>
      </span>
    </el-dialog>

    <!-- 修改用户的对话框 -->
    <el-dialog
      title="修改用户信息"
      :visible.sync="editDialogVisible"
      width="50%"
      @close="editDialogClosed"
    >
      <!-- 内容主体 -->
      <el-form :model="editScoreForm" ref="editScoreFormRef" label-width="70px">
        <el-form-item label="姓名" prop="name">
          <el-input
            v-model="editScoreForm.name"
            style="width: 90%"
            disabled
          ></el-input>
        </el-form-item>
        <el-form-item label="课程" prop="course">
          <el-input
            v-model="editScoreForm.course"
            style="width: 90%"
            disabled
          ></el-input>
        </el-form-item>
        <el-form-item label="成绩" prop="score">
          <el-input v-model="editScoreForm.score" style="width: 90%"></el-input>
        </el-form-item>
      </el-form>
      <span slot="footer" class="dialog-footer">
        <el-button @click="editDialogVisible = false">取 消</el-button>
        <el-button type="primary" @click="editScore">确 定</el-button>
      </span>
    </el-dialog>
  </div>
</template>

<script>
export default {
  data() {
    return {
      // 获取课程列表查询参数对象
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
      scorelist: [],
      totle: 0,
      // 添加用户对话框
      addDialogVisible: false,
      // 班级添加
      addScoreForm: {
        stuid: '',
        courseid: '',
        scorenum: ''
      },
      // 修改用户
      editDialogVisible: false,
      editScoreForm: {},
      // 分配角色对话框
      setRoleDialogVisible: false,
      // 当前需要被角色的班级
      classInfo: {},
      // 所有角色数据列表
      rolesLsit: [],
      // 学院列表
      collegeList: [],
      // 课程列表
      courseList: [],
      // 已选中的角色Id值
      selectRoleId: ''
    }
  },
  created() {
    // this.getScoreList()
  },
  methods: {
    async getScoreList() {
      const { data: res } = await this.$http.get('score', {
        params: this.queryInfo
      })
      if (res.meta.status !== 200) {
        return this.$message.error('查找不到相关的学号')
      }
      this.scorelist = res.data.score
      this.totle = res.data.total
      // console.log(res)
    },
    // 监听排序改变的时间
    handleSortChange({ prop, order }) {
      this.queryInfo.sortprop = prop
      this.queryInfo.sortorder = order === 'ascending' ? 'asc' : 'desc' // 排序顺序，可以根据需要进行适配
      this.getScoreList()
    },
    // 监听 pagesize改变的事件
    handleSizeChange(newSize) {
      // console.log(newSize)
      this.queryInfo.pagesize = newSize
      this.getScoreList()
    },
    // 监听 页码值 改变事件
    handleCurrentChange(newSize) {
      // console.log(newSize)
      this.queryInfo.pagenum = newSize
      this.getScoreList()
    },
    // 监听 添加用户对话框的关闭事件
    addDialogClosed() {
      this.$refs.addScoreFormRef.resetFields()
    },
    // 添加用户
    addScore() {
      // 提交请求前，表单预验证
      this.$refs.addScoreFormRef.validate(async (valid) => {
        console.log(valid)
        // 表单预校验失败
        console.log(this.addScoreForm)
        if (!valid) return
        const { data: res } = await this.$http.post('score', this.addScoreForm)

        if (res.meta.status !== 201) {
          return this.$message.error('已有该成绩或添加成绩失败！')
        }
        this.$message.success('添加成绩成功！')
        // 隐藏添加用户对话框
        this.addDialogVisible = false
        this.queryInfo.query = this.addScoreForm.stuid
        this.getScoreList()
      })
    },
    // 编辑用户信息
    async showEditDialog(id) {
      const { data: res } = await this.$http.get('score/' + id)
      if (res.meta.status !== 200) {
        return this.$message.error('查询课程信息失败！')
      }
      this.editScoreForm = res.data
      this.editDialogVisible = true
    },
    // 监听修改用户对话框的关闭事件
    editDialogClosed() {
      this.$refs.editScoreFormRef.resetFields()
    },
    // 修改用户信息
    editScore() {
      // 提交请求前，表单预验证
      this.$refs.editScoreFormRef.validate(async (valid) => {
        // console.log(valid)
        // 表单预校验失败
        if (!valid) return
        const { data: res } = await this.$http.put(
          'score/' + this.editScoreForm.id,
          {
            // name: this.editScoreForm.name,
            // course: this.editScoreForm.course,
            score: this.editScoreForm.score
          }
        )
        if (res.meta.status !== 200) {
          this.$message.error('更新用户信息失败！')
        }
        // 隐藏添加用户对话框
        this.editDialogVisible = false
        this.$message.success('更新用户信息成功！')
        this.getScoreList()
      })
    },
    // 删除用户
    async removeScoreById(id) {
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
      const { data: res } = await this.$http.delete('score/' + id)
      if (res.meta.status !== 200) return this.$message.error('删除用户失败！')
      this.$message.success('删除用户成功！')
      this.getScoreList()
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
    },
    // 根据学号获取学院和课程信息
    async handleStuidChange(inputdata) {
      const { data: res } = await this.$http.get(`courses/${inputdata}/stuid`)
      if (res.meta.status !== 200) {
        return this.$message.error('获取课程列表失败！')
      }
      this.courseList = res.data
    }
  }
}
</script>

<style lang="less" scoped></style>
