// #include "pch.h"
#include "CommonConnectionPool.h"
#include "public.h"

// 现了一个线程安全的连接池，用于管理数据库连接。
// 连接池的主要功能是通过复用已经创建的连接来提高性能，并在需要时创建新的连接，同时定期回收长时间未使用的连接

// 线程安全的懒汉单例函数接口
ConnectionPool* ConnectionPool::getConnectionPool() {
	static ConnectionPool pool; // lock和unlock  通过static确保ConnectionPool只被实例化一次，并在首次调用时创建
	return &pool;
}

// 从配置文件中加载配置项 从mysql.ini文件中读取数据库配置项并存储在成员变量中
bool ConnectionPool::loadConfigFile() {
	// FILE *pf = fopen("mysql.ini", "r");
	FILE *pf = fopen("/home/nowcoder/Linux_nc/ChatServer/myChatServer/src/server/db/mysql.ini", "r");
	if (pf == nullptr) {
		LOG("mysql.ini file is not exist!");
		return false;
	}

	while (!feof(pf)) {
		char line[1024] = { 0 };
		fgets(line, 1024, pf);
		string str = line;
		int idx = str.find('=', 0);
		if (idx == -1) { // 无效的配置项
			continue;
		}

		// password=123456\n
		int endidx = str.find('\n', idx);
		string key = str.substr(0, idx);
		string value = str.substr(idx + 1, endidx - idx - 1);

		if (key == "ip") {
			_ip = value;
		} else if (key == "port") {
			_port = atoi(value.c_str());
		} else if (key == "username") {
			_username = value;
		} else if (key == "password") {
			_password = value;
		} else if (key == "dbname") {
			_dbname = value;
		} else if (key == "initSize") {
			_initSize = atoi(value.c_str());
		} else if (key == "maxSize") {
			_maxSize = atoi(value.c_str());
		} else if (key == "maxIdleTime") {
			_maxIdleTime = atoi(value.c_str());
		} else if (key == "connectionTimeOut") {
			_connectionTimeout = atoi(value.c_str());
		}
	}
	return true;
}

// 连接池的构造
// 加载配置文件并初始化连接池，创建初始数量的数据库连接。
// 然后启动两个独立线程，一个负责生产新连接，一个定期扫描并回收超过闲置时间的连接
ConnectionPool::ConnectionPool() {
	// 加载配置项了
	if (!loadConfigFile()) {
		LOG("configure file error!");
		return;
	}

	// 创建初始数量的连接
	for (int i = 0; i < _initSize; ++i) {
		Connection *p = new Connection();

		// p->connect(_ip, _port, _username, _password, _dbname); // 原本的连接方式，没有报错提醒
		// if (p->connect(_ip, _port, _username, _password, _dbname) == false) { // 读取文件的参数不对，不清楚是哪的问题
		if (p->connect("127.0.0.1", 3306, "zsq", "*Zz123456", "chat") == false) {
            cout << "connect mysql error!" << endl;
        } else cout << "connect mysql ok" << endl;

		p->refreshAliveTime(); // 刷新一下连接开始空闲的起始时间
		_connectionQue.push(p);
		_connectionCnt++;
	}

	// 启动一个新的线程，作为连接的生产者 linux thread => pthread_create
	thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
	produce.detach();

	// 启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，进行对于的连接回收
	thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();
}

// 运行在独立的线程中，专门负责生产新连接  
// 该方法在独立线程中运行，当连接池为空时，生产新连接并加入队列中
void ConnectionPool::produceConnectionTask() {
	for (;;) {
		unique_lock<mutex> lock(_queueMutex);
		while (!_connectionQue.empty()) {
			cv.wait(lock); // 队列不空，此处生产线程进入等待状态
		}

		// 连接数量没有到达上限，继续创建新的连接
		if (_connectionCnt < _maxSize) {
			Connection *p = new Connection();
			
			// p->connect(_ip, _port, _username, _password, _dbname);
			// if (p->connect(_ip, _port, _username, _password, _dbname) == false) {
			if (p->connect("127.0.0.1", 3306, "zsq", "*Zz123456", "chat") == false) {
				cout << "connect mysql error!" << endl;
			} else cout << "connect mysql ok" << endl;

			p->refreshAliveTime(); // 刷新一下开始空闲的起始时间
			_connectionQue.push(p);
			_connectionCnt++;
		}

		// 通知消费者线程，可以消费连接了，通知cv.wait_for，运行到这行代码的时候只有一个cv.wait_for阻塞着，不通知他通知谁
		cv.notify_all();
	}
}

// 扫描超过maxIdleTime时间的空闲连接，进行对于连接的回收
// 该方法在独立线程中运行，定期扫描连接池，回收超过闲置时间的连接
void ConnectionPool::scannerConnectionTask() {
	for (;;) {
		// 通过sleep模拟定时效果
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));

		// 扫描整个队列，释放多余的连接
		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize) {
			Connection *p = _connectionQue.front();
			if (p->getAliveeTime() >= (_maxIdleTime * 1000)) {
				_connectionQue.pop();
				_connectionCnt--;
				delete p; // 调用~Connection()释放连接
				LOG("connect delete");
				cout << "Idle connection deleted." << endl;
			} else {
				break; // 队头的连接没有超过_maxIdleTime，其它连接肯定没有，因此直接break，不用扫描后面的
			} // 这个else break;到底是不是真的需要呢  真的需要，不然会一直循环占用资源
		}
	}
}

// 给外部提供接口，从连接池中获取一个可用的空闲连接
// 从连接池中获取一个可用的连接，如果连接池为空，则等待一段时间。
// 返回的 shared_ptr 包含自定义析构函数，将连接归还到连接池。
shared_ptr<Connection> ConnectionPool::getConnection() {
	unique_lock<mutex> lock(_queueMutex);
	while (_connectionQue.empty()) { // 当可连接数量为0时
		// sleep
		if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout))) {
			if (_connectionQue.empty()) {
				LOG("获取空闲连接超时了...获取连接失败!");
				return nullptr;
			}
		}
	}

	/*
	shared_ptr智能指针析构时，会把connection资源直接delete掉，相当于
	调用connection的析构函数，connection就被close掉了。
	因此这里需要自定义shared_ptr的释放资源的方式，把connection直接归还到queue当中
	*/
	shared_ptr<Connection> sp(_connectionQue.front(), [&](Connection *pcon) {
		// 这里是在服务器应用线程中调用的，所以一定要考虑队列的线程安全操作
		unique_lock<mutex> lock(_queueMutex);
		pcon->refreshAliveTime(); // 刷新一下开始空闲的起始时间
		_connectionQue.push(pcon); // 将连接重新放回队列中，以便将来再次使用
	});

	_connectionQue.pop(); // 从队列中移除第一个连接对象，因为它已经被 shared_ptr 持有
	
	cv.notify_all();  // 消费完连接以后，通知生产者线程检查一下，如果队列为空了，赶紧生产连接
	// 运行到此行代码，就只有一个cv.wait(lock)被阻塞着，不通知他通知谁
	
	// cv.notify_all();要不要改成下面这行这样呢
	// if (_connectionQue.empty()) cv.notify_all(); // 没有可用连接了，通知生产线程去生产

	return sp;
}
