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
            placeholder="请输入内容"
            v-model="queryInfo.query"
            clearable
            @clear="getClassList"
          >
            <el-button
              slot="append"
              icon="el-icon-search"
              @click="getClassList"
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
            >添加班级</el-button
          >
        </el-col>
      </el-row>
      <!-- 班级列表区域 -->
      <el-table :data="classlist" border @sort-change="handleSortChange" stripe>
        <!-- stripe: 斑马条纹
        border：边框-->
        <!-- 索引列 -->
        <el-table-column type="index" label="#"></el-table-column>
        <el-table-column
          prop="classname"
          label="班级"
          :sortable="'custom'"
        ></el-table-column>
        <el-table-column
          prop="grade"
          label="年级"
          :sortable="'custom'"
        ></el-table-column>
        <el-table-column prop="college" label="学院"></el-table-column>
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
              @click="removeClassById(scope.row.id)"
            ></el-button>
            <!-- <el-tooltip
              class="item"
              effect="dark"
              content="角色分配"
              :enterable="false"
              placement="top"
            >
              <el-button
                type="warning"
                icon="el-icon-setting"
                size="mini"
                circle
                @click="showSetRole(scope.row)"
              ></el-button>
            </el-tooltip> -->
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
      <el-form :model="addClassForm" ref="addClassFormRef" label-width="100px">
        <el-form-item label="班级名称" prop="classname">
          <el-input
            v-model="addClassForm.classname"
            style="width: 90%"
          ></el-input>
        </el-form-item>
        <el-form-item label="年级" prop="grade">
          <el-input v-model="addClassForm.grade" style="width: 90%"></el-input>
        </el-form-item>
        <el-form-item label="学院" prop="college">
          <el-select
            v-model="addClassForm.collegeid"
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
      </el-form>
      <span slot="footer" class="dialog-footer">
        <el-button @click="addDialogVisible = false">取 消</el-button>
        <el-button type="primary" @click="addClass">确 定</el-button>
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
      <el-form :model="editClassForm" ref="editClassFormRef" label-width="70px">
        <el-form-item label="班级名称">
          <el-input
            v-model="editClassForm.classname"
            style="width: 90%"
          ></el-input>
        </el-form-item>
        <el-form-item label="年级" prop="grade">
          <el-input v-model="editClassForm.grade" style="width: 90%"></el-input>
        </el-form-item>
        <el-form-item label="学院" prop="college">
          <el-select
            v-model="editClassForm.college"
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
      </el-form>
      <span slot="footer" class="dialog-footer">
        <el-button @click="editDialogVisible = false">取 消</el-button>
        <el-button type="primary" @click="editClass">确 定</el-button>
      </span>
    </el-dialog>

    <!-- 分配角色对话框 -->
    <el-dialog
      title="分配角色"
      :visible.sync="setRoleDialogVisible"
      width="50%"
      @close="setRoleDialogClosed"
    >
      <div>
        <p>当前用户：{{ classInfo.classname }}</p>
        <p>当前角色：{{ classInfo.role_name }}</p>
        <p>
          分配角色：
          <el-select
            v-model="selectRoleId"
            filterable
            allow-create
            default-first-option
            placeholder="请选择文章标签"
          >
            <el-option
              v-for="item in rolesLsit"
              :key="item.id"
              :label="item.roleName"
              :value="item.id"
            ></el-option>
          </el-select>
        </p>
      </div>
      <span slot="footer" class="dialog-footer">
        <el-button @click="setRoleDialogVisible = false">取 消</el-button>
        <el-button type="primary" @click="saveRoleInfo">确 定</el-button>
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
      classlist: [],
      totle: 0,
      // 添加用户对话框
      addDialogVisible: false,
      // 班级添加
      addClassForm: {
        classname: '',
        grade: '',
        collegeid: '' // 学院id
      },
      // 修改用户
      editDialogVisible: false,
      editClassForm: {},
      // 分配角色对话框
      setRoleDialogVisible: false,
      // 当前需要被角色的班级
      classInfo: {},
      // 所有角色数据列表
      rolesLsit: [],
      // 学院列表
      collegeList: [],
      // 已选中的角色Id值
      selectRoleId: ''
    }
  },
  created() {
    this.getClassList()
  },
  methods: {
    async getClassList() {
      const { data: res } = await this.$http.get('class', {
        params: this.queryInfo
      })
      if (res.meta.status !== 200) {
        return this.$message.error('获取班级列表失败！')
      }
      this.classlist = res.data.class
      this.totle = res.data.total
      // console.log(res)
    },
    // 监听排序改变的时间
    handleSortChange({ prop, order }) {
      this.queryInfo.sortprop = prop
      this.queryInfo.sortorder = order === 'ascending' ? 'asc' : 'desc' // 排序顺序，可以根据需要进行适配
      this.getClassList()
    },
    // 监听 pagesize改变的事件
    handleSizeChange(newSize) {
      // console.log(newSize)
      this.queryInfo.pagesize = newSize
      this.getClassList()
    },
    // 监听 页码值 改变事件
    handleCurrentChange(newSize) {
      // console.log(newSize)
      this.queryInfo.pagenum = newSize
      this.getClassList()
    },
    // 监听 添加用户对话框的关闭事件
    addDialogClosed() {
      this.$refs.addClassFormRef.resetFields()
    },
    // 添加用户
    addClass() {
      // 提交请求前，表单预验证
      this.$refs.addClassFormRef.validate(async (valid) => {
        console.log(valid)
        // 表单预校验失败
        console.log(this.addClassForm)
        if (!valid) return
        const { data: res } = await this.$http.post('class', this.addClassForm)

        if (res.meta.status !== 201) {
          this.$message.error('添加班级失败！')
        }
        this.$message.success('添加班级成功！')
        // 隐藏添加用户对话框
        this.addDialogVisible = false
        this.getClassList()
      })
    },
    // 编辑用户信息
    async showEditDialog(id) {
      this.getCollegeList()
      const { data: res } = await this.$http.get('class/' + id)
      if (res.meta.status !== 200) {
        return this.$message.error('查询班级信息失败！')
      }
      this.editClassForm = res.data
      this.editDialogVisible = true
    },
    // 监听修改用户对话框的关闭事件
    editDialogClosed() {
      this.$refs.editClassFormRef.resetFields()
    },
    // 修改用户信息
    editClass() {
      // 提交请求前，表单预验证
      this.$refs.editClassFormRef.validate(async (valid) => {
        // console.log(valid)
        // 表单预校验失败
        if (!valid) return
        const { data: res } = await this.$http.put(
          'class/' + this.editClassForm.id,
          {
            classname: this.editClassForm.classname,
            grade: this.editClassForm.grade,
            collegeid: this.editClassForm.collegeid,
            college: this.editClassForm.college
          }
        )
        if (res.meta.status !== 200) {
          this.$message.error('更新用户信息失败！')
        }
        // 隐藏添加用户对话框
        this.editDialogVisible = false
        this.$message.success('更新用户信息成功！')
        this.getClassList()
      })
    },
    // 删除用户
    async removeClassById(id) {
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
      const { data: res } = await this.$http.delete('class/' + id)
      if (res.meta.status !== 200) return this.$message.error('删除用户失败！')
      this.$message.success('删除用户成功！')
      this.getClassList()
    },
    // 展示分配角色的对话框
    async showSetRole(role) {
      this.classInfo = role
      // 展示对话框之前，获取所有角色列表
      const { data: res } = await this.$http.get('roles')
      if (res.meta.status !== 200) {
        return this.$message.error('获取角色列表失败！')
      }
      this.rolesLsit = res.data
      this.setRoleDialogVisible = true
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
    // 分配角色
    async saveRoleInfo() {
      if (!this.selectRoleId) {
        return this.$message.error('请选择要分配的角色')
      }
      const { data: res } = await this.$http.put(
        `class/${this.classInfo.id}/role`,
        { rid: this.selectRoleId }
      )
      if (res.meta.status !== 200) {
        return this.$message.error('更新用户角色失败！')
      }
      this.$message.success('更新角色成功！')
      this.getClassList()
      this.setRoleDialogVisible = false
    },
    // 分配角色对话框关闭事件
    setRoleDialogClosed() {
      this.selectRoleId = ''
      this.classInfo = {}
    }
  }
}
</script>

<style lang="less" scoped></style>
