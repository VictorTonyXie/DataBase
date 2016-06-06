# Database Project

源代码编译与执行

	系统要求：OS X／Linux，c++ 4.2.1，Python 2.7.10
	编译命令：make
	执行命令：./test input.txt


图形界面使用

	1. 用Qt5.5打开Qt工程文件，构建工程。
	2. 将之前源代码编译得到的可执行文件test和python文件夹拷贝到GUI程序的相同路径下。
	3. 打开GUI程序，在输入框中输入SQL语句，按PushButton执行。
	注：如果使用Mac，且已经安装Qt5.5，可以直接打开生成好的db_gui.app，已经配置好。
	注：程序不记录当前数据库，因此每次执行的SQL语句都要先USE database选择数据库；orderDB数据库已经建好。
	