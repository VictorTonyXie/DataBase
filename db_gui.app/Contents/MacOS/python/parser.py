import re
import sys

def getop(string):
	if string == "=":
		return "0 "
	elif string == "<":
		return "1 "
	elif string == ">":
		return "2 "
	elif string == "<=":
		return "3 "
	elif string == ">=":
		return "4 "
	elif string == "<>":
		return "5 "
	elif string == "like":
		return "6 "
	elif string == "null":
		return "7 "
		
def getAttr(string):
	names = string.split('.')
	if len(names) == 1:
		return "null " + names[0] + ' '
	else:
		return names[0] + ' ' + names[1] + ' '

def getagge(string):
	if string == "NONE":
		return "0 "
	elif string == "SUM":
		return "1 ";
	elif string == "AVG":
		return "2 "
	elif string == "MIN":
		return "3 "
	elif string == "MAX":
		return "4 "

	
def getvalue(string):
	if string[0] == '\'' and string[-1] == '\'':
		#print("string", string[1:-1])
		return "2 " + string[1:-1] + "\n"
	else:
		try:
			val = str(eval(string))
			if val.count('.') != 0:
				#print("float", val)
				return "1 " + val + '\n'
			else:
				#print("int", val)
				return "0 " + val + "\n"
		except:
			return "3 " + getAttr(string.strip()) + "\n"

def getlogic(string):
	if string == 'AND':
		return "2\n"
	else:
		return "3\n"

def remove(left, right):
	numleft = 0
	numright = 0
	while left[numleft] == '(':
		numleft += 1
	left = left[numleft:]
	
	if len(right) > 0:
		while right[-1] == ')' and right.count(')') > right.count('('):
			right = right[0:-1]
			numright += 1

	return left.strip(), right.strip(), numleft, numright

def whereclause(cols, f1):
	output = []
	error = 0
	f1.write('where\n')
	while cols != '':
		m1 = re.compile(r'(.+?)[\s]*(AND|OR)[\s]+(.+)').match(cols)
		if m1:
			m2 = re.compile(r'(.+?)(=|>|<|<=|>=|<>|like)([^><=](.+))').match(m1.group(1))
			tmpoutput = ""
			if m2:
				left = m2.group(1).strip()
				right = m2.group(3).strip()
				[left, right, numleft, numright] = remove(left, right)

				for i in range(numleft):  #左括号
					tmpoutput += "0\n"

				tmpoutput += '4 ' + getAttr(left.strip()) + getop(m2.group(2).strip())
				
				tmpoutput += getvalue(right)

				for i in range(numright): #右括号
					tmpoutput += "1\n"
			else:
				m2 = re.compile(r'(.+?)[\s]+is[\s]+null(.*)').match(m1.group(1))
				if m2:
					tmpoutput = ''
					left = m2.group(1).strip()
					right = m2.group(2).strip()
					[left, right, numleft, numright] = remove(left, right)
					for i in range(numleft):
						tmpoutput += '0\n'
					tmpoutput += '4 ' + getAttr(left) + getop("null") + "\n"
					for i in range(numright):
						tmpoutput += '1\n'
					print m2.group(1)
				else:
					error = 1
			tmpoutput += getlogic(m1.group(2)) #逻辑词
			output.append(tmpoutput)
			cols = m1.group(3).strip()
		else:
			m2 = re.compile(r'(.+?)(<=|>=|<>|=|>|<|like)([^><=](.+))').match(cols)
			if m2:
				tmpoutput = ""
				left = m2.group(1).strip()
				right = m2.group(3).strip()
				[left, right, numleft, numright] = remove(left, right)
				for i in range(numleft): #左括号
					tmpoutput += "0\n"

				tmpoutput += '4 ' + getAttr(left.strip()) + getop(m2.group(2).strip())
				
				tmpoutput += getvalue(right)

				for i in range(numright): #右括号
					tmpoutput += "1\n"
				output.append(tmpoutput)
			else:
				m2 = re.compile(r'(.+?)[\s]+is[\s]+null(.*)').match(cols)
				if m2:
					tmpoutput = ''
					left = m2.group(1).strip()
					right = m2.group(2).strip()
					[left, right, numleft, numright] = remove(left, right)
					for i in range(numleft):
						tmpoutput += '0\n'
					tmpoutput += '4 ' + getAttr(left) + getop("null") + "\n"
					for i in range(numright):
						tmpoutput += '1\n'
					print tmpoutput
					output.append(tmpoutput)
				else:
					error = 1
			cols = ''
	f1.write(str(len(output)) + "\n")
	for item in output:
		f1.write(item)
	f1.write('5\n')
	return error

filename = sys.argv[1]
f = open(filename)
f1 = open("tmp.gen", "w")
sqllist = f.read()
sqllist = re.split(';\r\n|;\n', sqllist)
res = []
res.append(re.compile(r'SHOW[\s]+TABLES[\s]*'))
res.append(re.compile(r'DROP[\s]+TABLE[\s]+([\w]+)'))
res.append(re.compile(r'DESC[\s]+([\w]+)'))
res.append(re.compile(r'CREATE[\s]+TABLE[\s]+([\w]+)[\s]*\((.+)\)'))
res.append(re.compile(r'INSERT[\s]+INTO[\s]+(.+)[\s]+VALUES[\s]*(.+)'))
res.append(re.compile(r'DELETE[\s]+FROM[\s]+([\w]+)[\s]*(.*)'))
res.append(re.compile(r'UPDATE[\s]+([\w]+)[\s]+SET[\s]+(.+)'))
res.append(re.compile(r'SELECT[\s]+(.+)FROM[\s]+(.+)'))
res.append(re.compile(r'SHOW[\s]+([\w]+)'))
res.append(re.compile(r'CREATE[\s]+DATABASE[\s]+([\w]+)'))
res.append(re.compile(r'DROP[\s]+DATABASE[\s]+([\w]+)'))
res.append(re.compile(r'USE[\s]+([\w]+)'))
res.append(re.compile(r'CREATE[\s]+INDEX[\s]+([\w]+)\(([\w]+)\)'))
res.append(re.compile(r'DROP[\s]+INDEX[\s]+([\w]+)\(([\w]+)\)'))

error = 0
for sql in sqllist:
	sql = sql.replace('\r', '').strip()
	sql = sql.replace('\n', '').strip()
	if sql == '':
		continue
	if sql[-1] == ';':
		sql = sql[0:-1]
	print sql
	m = []
	sqltype = -1
	for i in range(len(res)):
		m = res[i].match(sql)
		if m:
			sqltype = i
			break

	print sqltype

	if sqltype == 0:  #show
		print("cmd show tables")
		f1.write("showtables\n")
	elif sqltype == 1: #drop
		print("cmd drop ", m.group(1))
		f1.write("drop\n" + m.group(1) + "\n")
	elif sqltype == 2: #desc
		print("cmd desc ", m.group(1))
		f1.write("desc\n" + m.group(1) + "\n")
	
	elif sqltype == 3: #create
		print("cmd create table")
		f1.write("createtable\n")
		f1.write(m.group(1) + "\n")
		print("Table name ", m.group(1))
		temp = m.group(2).split(',')
		cols = []
		col = ''
		for i in range(len(temp)):
			col += temp[i]
			if col.count('(') != col.count(')'):
				col += ','
				continue
			cols.append(col.strip())
			col = ''

		m1 = re.compile(r'PRIMARY[\s]+KEY[\s]*\(([\w]+)\)').match(cols[-1])
		cnt = len(cols)
		if m1:
			print("Primary key ", m1.group(1))
			f1.write(m1.group(1) + "\n")
			cnt = cnt - 1
		else:
			print("Primary key ", "null")
			f1.write("null\n")
		print cols[cnt-1]
		m1 = re.compile(r'CHECK[\s]*\([\s]*([\w]+)[\s]+in[\s]*\((.+)\)[\s]*\)').match(cols[cnt-1])
		if m1:
			f1.write("check " + m1.group(1) + '\n')
			res2 = m1.group(2).split(',')
			f1.write(str(len(res2)) + '\n')
			for item in res2:
				f1.write(getvalue(item.strip()))
			cnt = cnt - 1
		else:
			f1.write("nocheck\n")

		f1.write(str(cnt) + "\n")
		for i in range(cnt):
			if len(cols[i].split(' ')) == 2:
				m2 = re.compile(r'([\w]+)[\s]+([\w]+)\(([0-9]+)\)').match(cols[i])
				if m2:
					print("Col name ", m2.group(1))
					print("Col type ", m2.group(2))
					print("Col leng ", m2.group(3))
					print("Col null ", "0") #0表示没有not null的限制
					f1.write(m2.group(1) + ' ')
					if m2.group(2) == "varchar":#2表示varchar 0表示int
						f1.write("2 ")
					elif m2.group(2) == "int":
						f1.write("0 ")
					elif m2.group(2) == "float":
						f1.write("1 ")
					else:
						error = 1
					f1.write(m2.group(3) + " 0\n")
				else:
					error = 1
			elif len(cols[i].split(' ')) == 4:
				m2 = re.compile(r'([\w]+)[\s]+([\w]+)\(([0-9]+)\)[\s]+NOT[\s]+NULL').match(cols[i])
				if m2:
					print("Col name ", m2.group(1))
					print("Col type ", m2.group(2))
					print("Col leng ", m2.group(3))
					print("Col null ", "1")
					f1.write(m2.group(1) + ' ')
					if m2.group(2) == "varchar":#2表示varchar 0表示int
						f1.write("2 ")
					elif m2.group(2) == "int":
						f1.write("0 ")
					elif m2.group(2) == "float":
						f1.write("1 ")
					else:
						error = 1
					f1.write(m2.group(3) + " 1\n")
				else:
					error = 1
			else:
				error = 1
	
	elif sqltype == 4: #insert
		print("cmd insert")
		cols = m.group(1).strip()
		m1 = re.compile(r'([\w]+)[\s]*\((.+?)\)').match(cols)
		if m1:
			f1.write("insert\n" + m1.group(1) + " 1\n")
			print m1.group(2).strip().split(',')
			f1.write(str(len(m1.group(2).strip().split(','))) + "\n")
			for item in m1.group(2).split(','):
				print("Table name ", item.strip())
				f1.write(item.strip() + "\n")
		else:
			f1.write("insert\n" + cols + " 0\n")
		print("Table name ", m.group(1))

		cols = m.group(2).strip()
		output = []

		insert_list = re.split('(\)[\s]*,[\s]*\()', cols)

		listnum = len(insert_list)
		col = ''
		for i in range(0, listnum, 2):
			col += insert_list[i]
			if col.count('\'') % 2 != 0:
				col += insert_list[i + 1]
				continue;

			if col[0] == '(':
				col = col[1:]

			if col[-1] == ')':
				col = col[0:-1]

			value_list = col.split(',')
			valuenum = len(value_list)
			value = ''
			tmpoutput = ''
			numv = 0
			for j in range(valuenum):
				value += value_list[j]
				if value.count('\'') % 2 != 0:
					value += ','
					continue

				tmpoutput += getvalue(value.strip())
				numv += 1
				value = ''

			tmpoutput = str(numv) + '\n' + tmpoutput
			output.append(tmpoutput)

			col = ''
		f1.write(str(len(output)) + '\n')
		for item in output:
			f1.write(item)
		

	elif sqltype == 5: #delete
		print("cmd delete")
		f1.write("delete\n" + m.group(1) + "\n")
		print("Table name ", m.group(1))

		m1 = re.compile(r'WHERE[\s]+(.+)').match(m.group(2).strip())
		if m1:
			error = (error or whereclause(m1.group(1).strip(), f1))

		else:
			f1.write("enddelete\n")

	elif sqltype == 6: #update
		print("cmd update")
		f1.write("update\n" + m.group(1) + "\n")
		print("Table name ", m.group(1))
		m1 = re.compile(r'(.+)WHERE[\s]+(.+)').match(m.group(2))
		if m1:
			cols = m1.group(1).split(',')
			for item in cols:
				m2 = re.compile(r'(.+)=(.+)').match(item)
				if m2:
					f1.write(getAttr(m2.group(1).strip()) + getvalue(m2.group(2).strip()))
				else:
					error = 1

			cols = m1.group(2).strip()
			error = (error or whereclause(cols, f1))

		else:
			cols = m.group(2).split(',')
			for item in cols:
				m2 = re.compile(r'(.+)=(.+)').match(item)
				if m2:
					print m2.group(1).strip().split('.')
					f1.write(getAttr(m2.group(1).strip()) + getvalue(m2.group(2).strip()))
					print m2.group(2).strip()
				else:
					error = 1
			f1.write("endupdate\n")

	elif sqltype == 7: #select
		print("cmd select")
		f1.write("select\n")
		cols = m.group(1).strip().split(',')
		f1.write(str(len(cols)) + "\n")
		for item in cols:
			m1 = re.compile(r'(AVG|SUM|MIN|MAX)\((.+)\)').match(item.strip())
			if m1:
				f1.write(getagge(m1.group(1)) + getAttr(m1.group(2).strip()) + "\n")
				print m1.group(1), m1.group(2)
			else:
				print item
				f1.write(getagge("NONE") + getAttr(item.strip()) + "\n")

		m1 = re.compile(r'(.+)WHERE[\s]+(.+)').match(m.group(2))
		if m1:
			f1.write(str(len(m1.group(1).split(','))) + "\n")
			for item in m1.group(1).split(','):
				print("Table name ", item.strip())
				f1.write(item.strip() + "\n")
			cols = m1.group(2).strip()
			error = (error or whereclause(cols, f1))
		else:
			m1 = re.compile(r'(.+)GROUP[\s]+BY[\s]+(.+)').match(m.group(2))
			if m1:
				f1.write(str(len(m1.group(1).split(','))) + "\n")
				for item in m1.group(1).split(','):
					print("Table name ", item.strip())
					f1.write(item.strip() + "\n")
				f1.write("group\n" + getAttr(m1.group(2)) + "\n")
			else:
				print m.group(2).strip().split(',')
				f1.write(str(len(m.group(2).strip().split(','))) + "\n")
				for item in m.group(2).split(','):
					print("Table name ", item.strip())
					f1.write(item.strip() + "\n")
				f1.write("endselect\n")
	elif sqltype == 8: #show
		f1.write("showtable\n")
		f1.write(m.group(1).strip() + '\n')
	elif sqltype == 9: #create database
		f1.write("createdatabase\n")
		f1.write(m.group(1).strip() + '\n')
	elif sqltype == 10: #drop database
		f1.write("dropdatabase\n")
		f1.write(m.group(1).strip() + '\n')
	elif sqltype == 11: #USE
		f1.write("use\n")
		f1.write(m.group(1).strip() + '\n')
	elif sqltype == 12: #create index
		f1.write("createindex\n")
		f1.write(m.group(1).strip() + '\n' + m.group(2).strip() + '\n')
	elif sqltype == 13: #drop index
		f1.write("dropindex\n")
		f1.write(m.group(1).strip() + '\n' + m.group(2).strip() + '\n')
	else:
		print("Syntax error")
	#f1.write(data)

f1.close()
if error:
	f1 = open("tmp.gen", "w")
	f1.write("Syntax error\n")
	f1.close()
f.close()
