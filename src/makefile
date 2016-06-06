all:test
test:qlmanager/parse_manager.cpp qlmanager/test_parse.cpp recordmanager/rm_record.cpp recordmanager/rm_filehandle.cpp systemmanager/sm_manager.cpp
	g++ -o test qlmanager/parse_manager.cpp qlmanager/test_parse.cpp recordmanager/rm_record.cpp recordmanager/rm_filehandle.cpp systemmanager/sm_manager.cpp indexmanager/IX.cpp
clean:
	rm test
