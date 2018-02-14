#!/usr/bin/python3

"""
Data Science
Apriori algorithm
@author: Yedarm Seong <mybirth0407@gmail.com>
"""

import sys


def main(argv):
  min_support = None
  input_file = None
  output_file = None
  try:
    min_support = int(argv[1])
    input_file = argv[2]
    output_file = argv[3]
    trx_db = create_db(input_file)
  except:
    print("Argument error")
    print("Usage: apriori.exe [minimum support] [input file] [output file]")
  l, supports = apriori(trx_db, min_support)
  rules = find_rules(l, supports, min_support)
  rules.sort()
  write_file(output_file, rules)


# 파일을 읽어 트랜잭션 리스트로 반환
def create_db(input_file):
  f = open(input_file, 'rt', encoding='utf-8')

  trxs = []
  for line in f.readlines():
    l = line.split('\t')
    # str to int, '\n' 제거를 위해 int로 형변환
    trx = [int(e) for e in l]
    trxs.append(trx)
  f.close()
    
  return trxs

# 트랜잭션 DB로부터 첫번째 후보 집합을 생성
def create_c1(trxs):
  c1 = []

  for trx in trxs:
    for item in trx:
      if not [item] in c1:
        c1.append([item])
  c1.sort()

  # frozenset: 수정 불가능한 집합
  return list(map(frozenset, c1))

# DB를 읽어 빈발 집합과 지지도 사전을 반환
def scan_tdb(db, ck, min_support):
  appearance_cnt = {}
  for trx in db:
    for candidate in ck:
      if candidate.issubset(trx):
        if candidate in appearance_cnt.keys():
          appearance_cnt[candidate] += 1
        else:
          appearance_cnt[candidate] = 1

  # 지지도 계산을 위한 전체 트랜잭션 수
  num_trxs = float(len(db))
  l = []
  supports = {}

  for key in appearance_cnt:
    support = 100 * (appearance_cnt[key] / num_trxs)

    # 지지도가 최소 지지도보다 높다면 빈발집합의 첫번째 위치에 삽입
    if support >= min_support:
      l.insert(0, key)
    # 현재 key에 대한 지지도를 지지도 사전에 저장
    supports[key] = support

  return l, supports

# k번째 후보 집합을 생성
def apriori_generate(lk, k):
  ck = []
  len_lk = len(lk)

  for i in range(len_lk):
    for j in range(i + 1, len_lk):
      l1 = list(lk[i])[:k - 2]
      l1.sort()
      l2 = list(lk[j])[:k - 2]
      l2.sort()

      if l1 == l2:
        ck.append(lk[i] | lk[j])
  return ck

# 트랜잭션과 지지도를 가지고 k번째 빈발 집합과 지지도를 반환
def apriori(trxs, min_support):
  # 초기 후보 집합을 생성
  c1 = create_c1(trxs)
  trx = list(map(set, trxs))

  # 길이가 1인 빈발 집합과 길이가 1인 아이템을 key로 갖는 지지도 사전 생성
  l1, supports = scan_tdb(trx, c1, min_support)
  l = [l1]
  c = [c1]
  k = 2

  # 길이가 k인 후보 집합과 빈발 집합, 지지도 사전을 생성하여 추가
  while len(l[k - 2]) > 0:
    ck = apriori_generate(l[k - 2], k)
    lk, supportk = scan_tdb(trx, ck, min_support)
    supports.update(supportk)
    l.append(lk)
    c.append(ck)
    k += 1

  return l, supports

# 연관 규칙을 생성
def find_rules(l, supports, min_support):
  rules = []
  
  # 길이가 i >= 1인 빈발 집합에 대해 검사
  for i in range(0, len(l)):
    for j in range(i + 1, len(l)):
      for e1 in l[i]:
        for e2 in l[j]:
          # 길이가 i인 어떤 빈발집합이
          # 길이가 i + 1인 어떤 빈발집합의 부분집합이라면 
          if e1.issubset(e2) and (e2 - e1) != frozenset({}):
            # 정렬하기 위해 frozenset 자료구조를 list로 변경
            l1 = list(e1)
            l2 = list(e2 - e1)
            l1.sort()
            l2.sort()
            # 모든 지지도의 분모는 트랜잭션 개수이므로 동일
            conf = 100 * supports[e2] / supports[e1]
            # 오차를 없애기 위해 과학적 반올림 표기법을 이용
            rules.append((l1, l2, round2(supports[e2], 2), round2(conf, 2)))

  return rules

# 과학적 반올림 표기법
def round2(f, n):
  return round(f + 0.00000000000001, n)

# 리스트를 과제 형식에 맞게 파일에 작성
def write_file(output_file, rules):
  f = open(output_file, 'wt', encoding='utf-8')

  for rule in rules:
    f.write('{')
    for i in range(len(rule[0]) -1):
      f.write(str(rule[0][i]) + ',')
    f.write(str(rule[0][len(rule[0]) - 1]))
    f.write('}\t')

    f.write('{')
    for i in range(len(rule[1]) -1):
      f.write(str(rule[1][i]) + ',')
    f.write(str(rule[1][len(rule[1]) - 1]))
    f.write('}\t')

    f.write('%0.2f\t' % rule[2])
    f.write('%0.2f\n' % rule[3])
  f.close()


if __name__ == '__main__':
  main(sys.argv)
