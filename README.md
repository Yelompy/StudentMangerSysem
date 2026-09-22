# StudentManagerSystem

基于 Qt 6 与 SQLite 开发的桌面学生管理系统，涵盖学生信息、课程安排、财务统计、荣誉墙和系统设置。

## 功能

- 登录验证与本地登录信息缓存
- 学生信息的查看和维护
- 课程安排管理
- 财务信息统计与图表展示
- 荣誉墙展示
- SQLite 数据库存储

## 环境要求

- Qt 6.5 或更高版本
- CMake 3.19 或更高版本
- 支持 C++17 的编译器

项目依赖 Qt Widgets、Qt SQL 和 Qt Charts 模块。

## 构建运行

```bash
cmake -S . -B build
cmake --build build --config Release
```

运行生成的 `StudentManagerSystem` 程序即可。请从项目根目录启动程序，以便找到 `sqllite/axbTechManSys.db` 示例数据库。

首次登录账号：

- 用户名：`admin`
- 密码：`admin123`

程序会在数据库中自动创建初始用户。数据库文件位于 `sqllite/axbTechManSys.db`，运行时产生的 `config.ini` 会被 Git 忽略。

## 目录说明

```text
├─ ico/       图标资源
├─ style/     Qt 样式表
├─ sqllite/   示例 SQLite 数据库
├─ *.cpp/h    C++ 源码
├─ *.ui       Qt Designer 界面文件
└─ CMakeLists.txt
```

## 许可证

本项目仅用于学习和课程实践。
