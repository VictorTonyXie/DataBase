import re
import sys

f = open('../vague', 'r+')
con = f.read()
f.close()
con = con.split('\n')
s1 = con[0]
s2 = con[1]

s2 = s2.replace('?', '(.?)')
s2 = s2.replace('*', '(.+)')
s2 = s2.replace('%', '(.*)')
f = open('../vague', 'w')
m = re.compile(s2).match(s1)
if m:
	if m.group(0) == s1:
		f.write('1\n')
	else:
		f.write('0\n')
else:
	f.write('0\n')
f.close()
