f1 = open('output4.txt')
f2 = open('outputRsupport4.txt')

l1 = []
l2 = []

for line in f1.readlines():
  l1.append(line)

for line in f2.readlines():
  l2.append(line)

l3 = []
for i in l1:
    if i not in l2:
        l3.append(i)

print(l3)


