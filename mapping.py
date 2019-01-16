import sys

mapping = dict()
out = open(sys.argv[2],'w',encoding = 'big5-hkscs')
with open(sys.argv[1],encoding = 'big5-hkscs') as f:
	for line in f:
		ch,zhuyin = line.split(" ")
		zhuyin_arr = zhuyin.split("/")
		for z in zhuyin_arr:
			if z[0] not in mapping:
				mapping[z[0]] = [ch]
			else:
				mapping[z[0]].append(ch)
		mapping[ch] = [ch]
		
for key in sorted(mapping.keys()):
	out.write(key + '   ' + ' '.join(mapping[key]) + '\n')
out.close()
