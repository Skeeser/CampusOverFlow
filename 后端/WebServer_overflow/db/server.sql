/*
 Navicat Premium Data Transfer

 Source Server         : wsl
 Source Server Type    : MySQL
 Source Server Version : 80033 (8.0.33-0ubuntu0.20.04.2)
 Source Host           : 192.168.114.189:3307
 Source Schema         : serversql

 Target Server Type    : MySQL
 Target Server Version : 80033 (8.0.33-0ubuntu0.20.04.2)
 File Encoding         : 65001

 Date: 15/06/2023 21:07:47
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for sp_class
-- ----------------------------
DROP TABLE IF EXISTS `sp_class`;
CREATE TABLE `sp_class`  (
  `class_id` smallint UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '课程id',
  `class_name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '' COMMENT '课程名称',
  `class_grade` smallint UNSIGNED NOT NULL DEFAULT 0 COMMENT '课程年级',
  `cge_id` int UNSIGNED NOT NULL DEFAULT 0 COMMENT '课程所属学院',
  `curs_ids` varchar(255) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT '' COMMENT '班级选的课程',
  PRIMARY KEY (`class_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 6 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of sp_class
-- ----------------------------
INSERT INTO `sp_class` VALUES (1, '计科1班', 20, 1, '1,2');
INSERT INTO `sp_class` VALUES (2, '计科2班', 21, 1, '1,3,6');
INSERT INTO `sp_class` VALUES (4, '计类1班', 22, 1, '1,2,3');
INSERT INTO `sp_class` VALUES (5, '信工1班', 21, 3, '7');

-- ----------------------------
-- Table structure for sp_college
-- ----------------------------
DROP TABLE IF EXISTS `sp_college`;
CREATE TABLE `sp_college`  (
  `cge_id` mediumint NOT NULL AUTO_INCREMENT COMMENT '学院id',
  `cge_name` varchar(255) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '' COMMENT '学院名称',
  PRIMARY KEY (`cge_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 5 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of sp_college
-- ----------------------------
INSERT INTO `sp_college` VALUES (1, '计算机学院');
INSERT INTO `sp_college` VALUES (2, '软件学院');
INSERT INTO `sp_college` VALUES (3, '电子与信息学院');
INSERT INTO `sp_college` VALUES (4, '自动化学院');

-- ----------------------------
-- Table structure for sp_course
-- ----------------------------
DROP TABLE IF EXISTS `sp_course`;
CREATE TABLE `sp_course`  (
  `curs_id` mediumint UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '课程id',
  `curs_name` varchar(255) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '' COMMENT '课程名称',
  `cge_id` int NOT NULL DEFAULT 0 COMMENT '学院id',
  `curs_num` double NOT NULL COMMENT '学分',
  PRIMARY KEY (`curs_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 8 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of sp_course
-- ----------------------------
INSERT INTO `sp_course` VALUES (1, '数据结构', 1, 4);
INSERT INTO `sp_course` VALUES (2, '计算机组成原理', 1, 3.5);
INSERT INTO `sp_course` VALUES (3, '计算机网络', 1, 3);
INSERT INTO `sp_course` VALUES (5, '自动化导论', 4, 2);
INSERT INTO `sp_course` VALUES (6, '大学物理', 1, 4);
INSERT INTO `sp_course` VALUES (7, '电路', 3, 3);

-- ----------------------------
-- Table structure for sp_express
-- ----------------------------
DROP TABLE IF EXISTS `sp_express`;
CREATE TABLE `sp_express`  (
  `express_id` int UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '主键id',
  `order_id` int UNSIGNED NOT NULL COMMENT '订单id',
  `express_com` varchar(32) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL COMMENT '订单快递公司名称',
  `express_nu` varchar(32) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL COMMENT '快递单编号',
  `create_time` int UNSIGNED NOT NULL COMMENT '记录生成时间',
  `update_time` int UNSIGNED NOT NULL COMMENT '记录修改时间',
  PRIMARY KEY (`express_id`) USING BTREE,
  INDEX `order_id`(`order_id` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 5 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci COMMENT = '快递表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of sp_express
-- ----------------------------
INSERT INTO `sp_express` VALUES (1, 54, 'huitongkuaidi', '70365716896101', 0, 0);
INSERT INTO `sp_express` VALUES (2, 53, 'yuantong', 'sdfsfdw22ew32ed23wd2e2e', 1508985920, 1508985920);
INSERT INTO `sp_express` VALUES (3, 48, 'yuantong', 'arfeesefsfsffw4ttefre', 1510025511, 1510025511);
INSERT INTO `sp_express` VALUES (4, 46, 'yuantong', 'sadsdsd', 1510835549, 1510835549);

-- ----------------------------
-- Table structure for sp_manager
-- ----------------------------
DROP TABLE IF EXISTS `sp_manager`;
CREATE TABLE `sp_manager`  (
  `mg_id` int NOT NULL AUTO_INCREMENT COMMENT '主键id',
  `mg_name` varchar(32) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL COMMENT '名称',
  `mg_pwd` char(64) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL COMMENT '密码',
  `mg_time` varchar(64) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '' COMMENT '注册时间',
  `role_id` tinyint NOT NULL DEFAULT 0 COMMENT '角色id',
  `class_id` int NULL DEFAULT NULL COMMENT '班级id',
  `mg_stuid` varchar(32) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL COMMENT '学号',
  `mg_college` varchar(255) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT '' COMMENT '学院名',
  `mg_mobile` varchar(32) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL COMMENT '电话号码',
  `mg_email` varchar(64) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL COMMENT '邮件',
  `mg_isstu` tinyint NULL DEFAULT 1 COMMENT '是否是学生',
  PRIMARY KEY (`mg_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 519 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci COMMENT = '管理员表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of sp_manager
-- ----------------------------
INSERT INTO `sp_manager` VALUES (500, 'admin', '123456', '1486720211', 12, NULL, '0', '计算机学院', '1234567', 'adsfad@qq.com', 0);
INSERT INTO `sp_manager` VALUES (502, 'linken', '123456', '1486720211', 1, 1, '202130241241', '计算机学院', '1213213123', '114514f@qq.com', 1);
INSERT INTO `sp_manager` VALUES (510, '银灰', '123456', '2023-06-04 12:26:22', 1, 2, '202130241242', '计算机学院', '202130241241', '1231313@sa.com', 1);
INSERT INTO `sp_manager` VALUES (513, 'sasj', '123456', '2023-06-07 16:01:37', 1, 2, '202130241231', '软件学院', '234556', '21313@qq.com', 1);
INSERT INTO `sp_manager` VALUES (517, '张娜娜', '123456', '2023-06-15 20:54:53', 11, NULL, '0', '自动化学院', '1244112414', '1241422431@qq.com', 0);
INSERT INTO `sp_manager` VALUES (518, '李思学', '123456', '2023-06-15 21:00:32', 11, NULL, '0', '计算机学院', '13124124124', '132141431@qq.com', 0);

-- ----------------------------
-- Table structure for sp_permission
-- ----------------------------
DROP TABLE IF EXISTS `sp_permission`;
CREATE TABLE `sp_permission`  (
  `ps_id` smallint UNSIGNED NOT NULL AUTO_INCREMENT,
  `ps_name` varchar(20) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL COMMENT '权限名称',
  `ps_pid` smallint UNSIGNED NOT NULL COMMENT '父id',
  `ps_c` varchar(32) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '' COMMENT '控制器',
  `ps_a` varchar(32) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '' COMMENT '操作方法',
  `ps_level` enum('0','2','1') CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '0' COMMENT '权限等级',
  PRIMARY KEY (`ps_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 163 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci COMMENT = '权限表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of sp_permission
-- ----------------------------
INSERT INTO `sp_permission` VALUES (101, '课程管理', 0, '', '', '0');
INSERT INTO `sp_permission` VALUES (102, '班级管理', 0, '', 'class', '0');
INSERT INTO `sp_permission` VALUES (103, '权限管理', 0, '', '', '0');
INSERT INTO `sp_permission` VALUES (104, '课程列表', 101, 'Goods', 'index', '1');
INSERT INTO `sp_permission` VALUES (105, '添加课程', 104, 'Goods', 'tianjia', '2');
INSERT INTO `sp_permission` VALUES (107, '班级列表', 102, 'Order', 'index', '1');
INSERT INTO `sp_permission` VALUES (109, '添加班级', 107, 'Order', 'tianjia', '2');
INSERT INTO `sp_permission` VALUES (111, '角色列表', 103, 'Role', 'index', '1');
INSERT INTO `sp_permission` VALUES (112, '权限列表', 103, 'Permission', 'index', '1');
INSERT INTO `sp_permission` VALUES (115, '选课管理', 101, 'Type', 'index', '1');
INSERT INTO `sp_permission` VALUES (116, '课程修改', 104, 'Goods', 'upd', '2');
INSERT INTO `sp_permission` VALUES (117, '课程删除', 104, 'Goods', 'del', '2');
INSERT INTO `sp_permission` VALUES (121, '成绩查询', 101, '', '', '1');
INSERT INTO `sp_permission` VALUES (122, '添加成绩', 121, '', '', '2');
INSERT INTO `sp_permission` VALUES (123, '删除成绩', 121, '', '', '2');
INSERT INTO `sp_permission` VALUES (125, '用户管理', 0, '', '', '0');
INSERT INTO `sp_permission` VALUES (129, '添加角色', 111, '', '', '2');
INSERT INTO `sp_permission` VALUES (130, '删除角色', 111, '', '', '2');
INSERT INTO `sp_permission` VALUES (131, '添加学生', 160, '', '', '2');
INSERT INTO `sp_permission` VALUES (132, '删除学生', 160, '', '', '2');
INSERT INTO `sp_permission` VALUES (133, '更新学生', 160, '', '', '2');
INSERT INTO `sp_permission` VALUES (134, '角色授权', 111, '', '', '2');
INSERT INTO `sp_permission` VALUES (135, '取消角色授权', 111, '', '', '2');
INSERT INTO `sp_permission` VALUES (136, '获取学生详情', 160, '', '', '2');
INSERT INTO `sp_permission` VALUES (137, '分配学生角色', 160, '', '', '2');
INSERT INTO `sp_permission` VALUES (138, '获取角色列表', 111, '', '', '2');
INSERT INTO `sp_permission` VALUES (139, '获取角色详情', 111, '', '', '2');
INSERT INTO `sp_permission` VALUES (140, '更新角色信息', 111, '', '', '2');
INSERT INTO `sp_permission` VALUES (141, '更新角色权限', 111, '', '', '2');
INSERT INTO `sp_permission` VALUES (142, '获取选课', 115, '', '', '2');
INSERT INTO `sp_permission` VALUES (143, '创建选课', 115, '', '', '2');
INSERT INTO `sp_permission` VALUES (144, '删除选课', 115, '', '', '2');
INSERT INTO `sp_permission` VALUES (145, '数据统计', 0, '', '', '0');
INSERT INTO `sp_permission` VALUES (146, '数据报表', 145, '', '', '1');
INSERT INTO `sp_permission` VALUES (147, '查看权限', 112, '', '', '2');
INSERT INTO `sp_permission` VALUES (148, '查看数据', 146, '', '', '2');
INSERT INTO `sp_permission` VALUES (149, '获取成绩', 121, '', '', '2');
INSERT INTO `sp_permission` VALUES (150, '更新课程', 104, '', '', '2');
INSERT INTO `sp_permission` VALUES (151, '更新课程属性', 104, '', '', '2');
INSERT INTO `sp_permission` VALUES (152, '更新课程状态', 104, '', '', '2');
INSERT INTO `sp_permission` VALUES (153, '获取课程详情', 104, '', '', '2');
INSERT INTO `sp_permission` VALUES (154, '班级更新', 107, '', '', '2');
INSERT INTO `sp_permission` VALUES (155, '获取班级详情', 107, '', '', '2');
INSERT INTO `sp_permission` VALUES (156, '课程列表添加', 104, '', '', '2');
INSERT INTO `sp_permission` VALUES (157, '课程列表删除', 104, '', '', '2');
INSERT INTO `sp_permission` VALUES (158, '课程列表详情', 104, '', '', '2');
INSERT INTO `sp_permission` VALUES (159, '设置管理状态', 160, '', '', '2');
INSERT INTO `sp_permission` VALUES (160, '学生列表', 125, '', '', '1');
INSERT INTO `sp_permission` VALUES (161, '教师列表', 125, '', '', '1');
INSERT INTO `sp_permission` VALUES (162, '查询具体班级', 107, '', '', '2');

-- ----------------------------
-- Table structure for sp_permission_api
-- ----------------------------
DROP TABLE IF EXISTS `sp_permission_api`;
CREATE TABLE `sp_permission_api`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `ps_id` int NOT NULL,
  `ps_api_service` varchar(255) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `ps_api_action` varchar(255) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `ps_api_path` varchar(255) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `ps_api_order` int NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `ps_id`(`ps_id` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 62 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of sp_permission_api
-- ----------------------------
INSERT INTO `sp_permission_api` VALUES (1, 101, NULL, NULL, 'course', 3);
INSERT INTO `sp_permission_api` VALUES (2, 102, NULL, NULL, 'class', 4);
INSERT INTO `sp_permission_api` VALUES (3, 103, NULL, NULL, 'rights', 2);
INSERT INTO `sp_permission_api` VALUES (4, 104, 'GoodService', 'getAllGoods', 'courselist', 1);
INSERT INTO `sp_permission_api` VALUES (5, 105, 'GoodService', 'createGood', 'course', NULL);
INSERT INTO `sp_permission_api` VALUES (6, 107, 'OrderService', 'getAllOrders', 'class', NULL);
INSERT INTO `sp_permission_api` VALUES (9, 109, 'OrderService', 'createOrder', 'class', NULL);
INSERT INTO `sp_permission_api` VALUES (11, 111, 'RoleService', 'getAllRoles', 'roles', NULL);
INSERT INTO `sp_permission_api` VALUES (12, 112, 'RightService', 'getAllRights', 'rights', NULL);
INSERT INTO `sp_permission_api` VALUES (15, 115, 'CategoryService', 'getAttributes', 'course', 2);
INSERT INTO `sp_permission_api` VALUES (16, 116, 'GoodService', 'updateGood', 'course', NULL);
INSERT INTO `sp_permission_api` VALUES (17, 117, 'GoodService', 'deleteGood', 'course', NULL);
INSERT INTO `sp_permission_api` VALUES (21, 121, 'CategoryService', 'getAllCategories', 'score', 3);
INSERT INTO `sp_permission_api` VALUES (22, 122, 'CategoryService', 'addCategory', 'categories', NULL);
INSERT INTO `sp_permission_api` VALUES (23, 123, 'CategoryService', 'deleteCategory', 'categories', NULL);
INSERT INTO `sp_permission_api` VALUES (25, 125, NULL, NULL, 'users', 1);
INSERT INTO `sp_permission_api` VALUES (29, 129, 'RoleService', 'createRole', 'roles', NULL);
INSERT INTO `sp_permission_api` VALUES (30, 130, 'RoleService', 'deleteRole', 'roles', NULL);
INSERT INTO `sp_permission_api` VALUES (31, 131, 'ManagerService', 'createManager', 'users', NULL);
INSERT INTO `sp_permission_api` VALUES (32, 132, 'ManagerService', 'deleteManager', 'users', NULL);
INSERT INTO `sp_permission_api` VALUES (33, 133, 'ManagerService', 'updateManager', 'users', NULL);
INSERT INTO `sp_permission_api` VALUES (34, 134, 'RoleService', 'updateRoleRight', 'roles', NULL);
INSERT INTO `sp_permission_api` VALUES (35, 135, 'RoleService', 'deleteRoleRight', 'roles', NULL);
INSERT INTO `sp_permission_api` VALUES (36, 136, 'ManagerService', 'getManager', 'users', NULL);
INSERT INTO `sp_permission_api` VALUES (37, 137, 'ManagerService', 'setRole', 'users', NULL);
INSERT INTO `sp_permission_api` VALUES (38, 138, 'RoleService', 'getAllRoles', 'roles', NULL);
INSERT INTO `sp_permission_api` VALUES (39, 139, 'RoleService', 'getRoleById', 'roles', NULL);
INSERT INTO `sp_permission_api` VALUES (40, 140, 'RoleService', 'updateRole', 'roles', NULL);
INSERT INTO `sp_permission_api` VALUES (41, 141, 'RoleService', 'updateRoleRight', 'roles', NULL);
INSERT INTO `sp_permission_api` VALUES (42, 142, 'AttributeService', 'getAttributes', 'categories', NULL);
INSERT INTO `sp_permission_api` VALUES (43, 143, 'AttributeService', 'createAttribute', 'categories', NULL);
INSERT INTO `sp_permission_api` VALUES (44, 144, 'AttributeService', 'deleteAttribute', 'categories', NULL);
INSERT INTO `sp_permission_api` VALUES (45, 145, NULL, NULL, 'reports', 5);
INSERT INTO `sp_permission_api` VALUES (46, 146, NULL, NULL, 'reports', NULL);
INSERT INTO `sp_permission_api` VALUES (47, 147, 'RightService', 'getAllRights', 'rights', NULL);
INSERT INTO `sp_permission_api` VALUES (48, 148, NULL, NULL, 'reports', NULL);
INSERT INTO `sp_permission_api` VALUES (49, 149, 'CategoryService', 'getCategoryById', 'categories', NULL);
INSERT INTO `sp_permission_api` VALUES (50, 150, 'GoodService', 'updateGoodPics', 'course', NULL);
INSERT INTO `sp_permission_api` VALUES (51, 151, 'GoodService', 'updateGoodAttributes', 'course', NULL);
INSERT INTO `sp_permission_api` VALUES (52, 152, 'GoodService', 'updateGoodsState', 'course', NULL);
INSERT INTO `sp_permission_api` VALUES (53, 153, 'GoodService', 'getGoodById', 'course', NULL);
INSERT INTO `sp_permission_api` VALUES (54, 154, 'OrderService', 'updateOrder', 'class', NULL);
INSERT INTO `sp_permission_api` VALUES (55, 155, 'OrderService', 'getOrder', 'class', NULL);
INSERT INTO `sp_permission_api` VALUES (56, 156, 'CategoryService', 'createAttribute', 'categories', NULL);
INSERT INTO `sp_permission_api` VALUES (57, 157, 'CategoryService', 'deleteAttribute', 'categories', NULL);
INSERT INTO `sp_permission_api` VALUES (58, 158, 'CategoryService', 'attributeById', 'categories', NULL);
INSERT INTO `sp_permission_api` VALUES (59, 159, 'ManagerService', 'updateMgrState', 'users', NULL);
INSERT INTO `sp_permission_api` VALUES (60, 160, 'ManagerService', 'getAllStudents', 'students', NULL);
INSERT INTO `sp_permission_api` VALUES (61, 161, 'ManagerService', 'getAllTeachers', 'teachers', NULL);

-- ----------------------------
-- Table structure for sp_role
-- ----------------------------
DROP TABLE IF EXISTS `sp_role`;
CREATE TABLE `sp_role`  (
  `role_id` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `role_name` varchar(20) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL COMMENT '角色名称',
  `ps_ids` varchar(512) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '' COMMENT '权限ids,1,2,5',
  `ps_ca` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL COMMENT '控制器-操作,控制器-操作,控制器-操作',
  `role_desc` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL,
  PRIMARY KEY (`role_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 156 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of sp_role
-- ----------------------------
INSERT INTO `sp_role` VALUES (1, '学生', '101,104,105,116,117,150,151,152,153,156,157,158,115,142,143,144,121,122,123,149,131,132,133,136,137,145,146,148,125,160', NULL, '大学生');
INSERT INTO `sp_role` VALUES (11, '老师', '101,102,103,104,116,115,142,143,144,121,122,123,149,102,107,109,103,111,129,130,134,135,138,139,140,141,112,147,125,131,132,133,136,137,145,146,148,160,125', NULL, '学科');
INSERT INTO `sp_role` VALUES (12, '教务老师', '101,102,103,104,116,115,142,143,144,121,122,123,149,102,107,109,103,111,129,130,134,135,138,139,140,141,112,147,125,131,132,133,136,137,145,146,148,160,125', NULL, '教务负责');

-- ----------------------------
-- Table structure for sp_score
-- ----------------------------
DROP TABLE IF EXISTS `sp_score`;
CREATE TABLE `sp_score`  (
  `sco_id` mediumint NOT NULL AUTO_INCREMENT COMMENT '成绩id',
  `curs_id` mediumint NOT NULL COMMENT '课程id',
  `mg_id` mediumint NOT NULL COMMENT '学生id',
  `sco_ispass` smallint NOT NULL COMMENT '是否及格',
  `sco_num` int UNSIGNED NOT NULL DEFAULT 0 COMMENT '具体成绩',
  PRIMARY KEY (`sco_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 4 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of sp_score
-- ----------------------------
INSERT INTO `sp_score` VALUES (1, 1, 502, 1, 75);
INSERT INTO `sp_score` VALUES (2, 2, 502, 1, 85);
INSERT INTO `sp_score` VALUES (3, 6, 502, 0, 56);

-- ----------------------------
-- Table structure for sp_user
-- ----------------------------
DROP TABLE IF EXISTS `sp_user`;
CREATE TABLE `sp_user`  (
  `user_id` int NOT NULL AUTO_INCREMENT COMMENT '自增id',
  `username` varchar(128) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '' COMMENT '登录名',
  `qq_open_id` char(32) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL COMMENT 'qq官方唯一编号信息',
  `password` char(64) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '' COMMENT '登录密码',
  `user_email` varchar(64) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '' COMMENT '邮箱',
  `user_email_code` char(13) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL COMMENT '新用户注册邮件激活唯一校验码',
  `is_active` enum('是','否') CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT '否' COMMENT '新用户是否已经通过邮箱激活帐号',
  `user_sex` enum('保密','女','男') CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '男' COMMENT '性别',
  `user_qq` varchar(32) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '' COMMENT 'qq',
  `user_tel` varchar(32) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '' COMMENT '手机',
  `user_xueli` enum('博士','硕士','本科','专科','高中','初中','小学') CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '本科' COMMENT '学历',
  `user_hobby` varchar(32) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '' COMMENT '爱好',
  `user_introduce` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL COMMENT '简介',
  `create_time` int NOT NULL COMMENT '创建时间',
  `update_time` int NOT NULL COMMENT '修改时间',
  PRIMARY KEY (`user_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 12 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci COMMENT = '会员表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of sp_user
-- ----------------------------
INSERT INTO `sp_user` VALUES (1, 'zce', NULL, '$2a$08$lV0Gr4AKx7xH7cCU4KCGCOikNzGPaWIpw9W7A9BONIxoJ2.hGC9qi', 'w@zce.me', '1242d9b5', '否', '男', '', '', '本科', '', NULL, 1512033129, 1512033129);
INSERT INTO `sp_user` VALUES (11, 'ww', NULL, '$2a$08$09nUxs.9czzXc4JZJTOdteeXSd/mxZVg96AhqciGbTMB6cfbGUWC2', 'i@zce.me', 'f9a9d0cc', '是', '女', '1231231211', '12313211', '博士', '123123', '123123123', 1512122098, 1512122098);

-- ----------------------------
-- Table structure for sp_user_cart
-- ----------------------------
DROP TABLE IF EXISTS `sp_user_cart`;
CREATE TABLE `sp_user_cart`  (
  `cart_id` int UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '主键',
  `user_id` int UNSIGNED NOT NULL COMMENT '学员id',
  `cart_info` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL COMMENT '购物车详情信息，二维数组序列化信息',
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL,
  `delete_time` timestamp NULL DEFAULT NULL,
  PRIMARY KEY (`cart_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 22 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of sp_user_cart
-- ----------------------------
INSERT INTO `sp_user_cart` VALUES (19, 5764, 'a:1:{s:32:\"84c7dfb6cb829817f6de9e7c9506d6f4\";a:10:{s:8:\"goods_id\";i:77;s:10:\"goods_name\";s:14:\"小米手机22\";s:11:\"goods_price\";s:6:\"111.00\";s:16:\"goods_small_logo\";s:67:\"./uploads/goods/20171017/small_9d5cccb340525d3f0652fd327cfb178b.jpg\";s:16:\"goods_buy_number\";i:1;s:14:\"goods_cart_uid\";s:32:\"84c7dfb6cb829817f6de9e7c9506d6f4\";s:7:\"user_id\";i:5764;s:4:\"time\";d:1509438617.630688;s:5:\"queue\";b:1;s:18:\"goods_price_xiaoji\";d:111;}}', NULL, NULL, NULL);
INSERT INTO `sp_user_cart` VALUES (20, 7505, 'a:1:{s:32:\"65927e4ef01cf6ab4b2764bea2f4ffba\";a:10:{s:8:\"goods_id\";i:76;s:10:\"goods_name\";s:14:\"华为闪耀33\";s:11:\"goods_price\";s:6:\"111.00\";s:16:\"goods_small_logo\";s:67:\"./uploads/goods/20171017/small_6a666c60fb4a9bd4fe462a04f4318019.jpg\";s:16:\"goods_buy_number\";i:1;s:14:\"goods_cart_uid\";s:32:\"65927e4ef01cf6ab4b2764bea2f4ffba\";s:7:\"user_id\";i:7505;s:4:\"time\";d:1509438621.5471151;s:5:\"queue\";b:1;s:18:\"goods_price_xiaoji\";d:111;}}', NULL, NULL, NULL);
INSERT INTO `sp_user_cart` VALUES (21, 1, '[{\"goods_id\":92,\"amount\":2},{\"goods_id\":94,\"amount\":2},{\"goods_id\":76,\"amount\":2},{\"goods_id\":75,\"amount\":1},{\"goods_id\":73,\"amount\":1}]', NULL, NULL, NULL);

SET FOREIGN_KEY_CHECKS = 1;
