怎么把sql 转成模型 封装
一.封装常见的数据库操作，比如
    1-获取所有的数据
        ＊参数conditions可以用json， 下面一样
        ｛
            “columns”: {
                "字段名" : "条件值"
            }，
            “only” : [""],
            等等
        ｝
        或者用string
        ＊参数modelname用表名
        ＊参数cb回调函数，可以处理返回相应的状态，存在meta数据中
    2-获取一条数据 conditions
    3-更新对象数据 id + 参数updateobj更新的数据
    4-通过主键ID获取对象 id
    5-通过主键ID删除对象 id
    6-通过条件判断数据是否存在 conditions
    
二.实现用户管理

三.合并Options请求(已弄好，未验证， 验证好了删多余代码)

四.修复登录的bug

五.返回的用户列表增加年级班级
