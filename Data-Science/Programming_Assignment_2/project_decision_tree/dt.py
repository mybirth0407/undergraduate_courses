#!/usr/bin/python3

"""
Data Science
Decision tree
@author: Yedarm Seong <mybirth0407@gmail.com>
"""

import sys
from math import log2
import operator


def main(argv):
  try:
    train_file = argv[1]
    test_file = argv[2]
    result_file = argv[3]
  except:
    print("Argument error")
    print("Usage: dt.py [training file] [test file] [result file]")

  labels, train_set, test_set = create_db(train_file, test_file)
  tree = create_tree(train_set, labels[:])
  results = [labels]

  for data in test_set:
    results.append(data + [classify(tree, labels, data)[0]])
  write_file(result_file, results)


# 훈련, 검사 파일로부터 데이터 집합을 생성한다.
def create_db(train_file, test_file):
  f_train = open(train_file, 'rt', encoding='utf-8')

  # 훈련 집합의 첫 행을 읽어 라벨을 얻어오고, 개행을 제거한다.
  first_line = f_train.readline()
  labels = first_line.replace('\n', '').split('\t')
  
  train_set = [line.replace('\n','').split('\t')
                 for line in f_train.readlines()]
  f_train.close()

  f_test = open(test_file, 'rt', encoding='utf-8')
  # 검사 집합의 첫 행은 라벨이므로 미리 읽어 제거한다.
  f_test.readline()
  test_set = [line.replace('\n','').split('\t')
                for line in f_test.readlines()]
  f_test.close()
  return labels, train_set, test_set

# 엔트로피를 계산한다.
def caculate_entropy(data_set):
  num_entry = len(data_set)
  label_counts = {}

  # 가능한 모든 분류 항목에 대한 사전 생성
  for feature_vector in data_set:
    current_label = feature_vector[-1]

    if current_label not in label_counts.keys():
      label_counts[current_label] = 0

    label_counts[current_label] += 1
  entropy = 0.0

  # 엔트로피 계산 공식 적용
  for key in label_counts:
    prob = float(label_counts[key]) / num_entry
    # 밑이 2인 로그
    entropy -= prob * log2(prob)
  return entropy

# 지니 계수를 계산한다.
def calculate_gini_index(data_set):
  num_entry = len(data_set)
  label_counts = {}

  # 가능한 모든 분류 항목에 대한 사전 생성
  for feature_vector in data_set:
    current_label = feature_vector[-1]

    if current_label not in label_counts.keys():
      label_counts[current_label] = 0

    label_counts[current_label] += 1

  gini_index = 1.0

  # 지니 계수 계산 공식 적용
  for key in label_counts:
    gini_index -= float(label_counts[key] / num_entry) ** 2
  return gini_index

# 데이터 집합의 한 엔트리에 대해서
# axis번째 원소의 값이 value인 list를 생성하여 반환한다.
def split_data_set(data_set, axis, value):
  splited_data_set = []

  for feature_vector in data_set:
    if feature_vector[axis] == value:
      # 분할한 속성 잘라내기
      reduced_feature_vector = feature_vector[: axis]
      reduced_feature_vector.extend(feature_vector[axis + 1: ])
      splited_data_set.append(reduced_feature_vector)
  return splited_data_set

# 데이터 집합에서 가장 좋은 특성을 선택한다.
def choose_best_feature(data_set):
  # 마지막 열은 분류 항목 표시에 사용되므로 제외한다.
  num_feature = len(data_set[0]) - 1 
  # 전체 데이터 집합에 대한 엔트로피를 계산한다.
  base_entropy = caculate_entropy(data_set)
  best_information_gain = 0.0
  best_feature = -1

  # 모든 속성에 대해 계산한다.
  for i in range(num_feature):
    # 이 속성에 대한 모든 예시 리스트를 생성한다.
    feature_list = [example[i] for example in data_set]
    # 중복을 제거한 특성 집합을 얻는다.
    value_set = set(feature_list)
    new_entropy = 0.0

    for value in value_set:
      # data_set에서 i번째 원소가 value인 원소일 때
      # i번째 원소를 제외한 값의 리스트
      sub_data_set = split_data_set(data_set, i, value)
      prob = float(len(sub_data_set) / (len(data_set)))
      new_entropy += (prob * caculate_entropy(sub_data_set))
    # 정보 이득을 계산한다.
    information_gain = base_entropy - new_entropy
    # 최대 정보 이득과 현재 정보 이득을 비교하여
    # 현재 정보 이득이 최대 정보 이득보다 크다면, 최대로 지정한다. 
    if information_gain > best_information_gain:
      best_information_gain = information_gain
      best_feature = i
  # 최대 속성을 정수로 반환
  return best_feature

# 발생 빈도가 가장 큰 item을 반환한다.
def majority_count(class_list):
  class_count = {}
  for vote in class_list:
    # class_count 사전에 vote가 없다면 사전에 추가한다.
    if vote in class_count.keys():
      class_count[vote] += 1
    else:
      class_count[vote] = 0

  # itemgetter(1): 사전의 값을 기준으로 정렬한다.
  sorted_class_count = sorted(
    class_count.items(), key=operator.itemgetter(1), reverse=True
  )
  return sorted_class_count[0][0]

# 주어진 데이터와 라벨을 이용하여 재귀적으로 트리를 생성한다.
def create_tree(data_set, labels):
  # 예) data_set = [[1, 1, 'yes'], [1, 1, 'yes'], [1, 0, 'no']]
  # class_list = ['yes, 'yes', 'no']
  class_list = [example[-1] for example in data_set]

  # 모든 분류 항목이 같은 경우 분할을 중단한다.
  if class_list.count(class_list[0]) == len(class_list):
    return class_list[0], len(class_list)

  # 데이터 집합에 더 이상 속성이 없는 경우 분할을 중단한다.
  if len(data_set[0]) == 1:
    return majority_count(class_list)

  best_feature = choose_best_feature(data_set)
  best_feature_label = labels[best_feature]
  tree = {best_feature_label: {}}
  del labels[best_feature]
  feature_vector = [example[best_feature] for example in data_set]
  unique_feature_set = set(feature_vector)

  for value in unique_feature_set:
    # 모든 분류 항목을 복사한다.
    sub_labels = labels[:]
    tree[best_feature_label][value] = create_tree(
      split_data_set(data_set, best_feature, value), sub_labels
    )
  return tree, len(class_list)

# 의사결정 트리를 위한 분류 함수
def classify(input_tree, feature_label, test_vector):
  first_key = list(input_tree[0].keys())[0]
  sub_tree = input_tree[0][first_key]
  # 분류 항목 표시를 문자열로 반환한다.
  feature_index = feature_label.index(first_key)
  key = test_vector[feature_index]

  try:
    value_of_feature = sub_tree[key]
  # 키가 없다면 해당 노드를 기준으로 하위 노드의 값 중 가장 많은 값을 반환한다.
  # 가장 많은 값이 여러개라면 랜덤하게 선택한다.
  except:
    l = []
    majority = 0
    value_of_feature = None

    for k in list(sub_tree.keys()):
      if majority <= sub_tree[k][1]:
        value_of_feature = sub_tree[k]

  # 특성의 값이 사전이라면 재귀적으로 검사한다. 
  if isinstance(value_of_feature[0], dict):
    class_label = classify(value_of_feature, feature_label, test_vector)
  # 특성의 값이 잎새 노드라면 해당 값을 라벨로 지정하여 반환한다.
  else:
    class_label = value_of_feature
  return class_label

# 결과 파일의 양식에 맞춰 작성한다.
def write_file(output_file, results):
  f = open(output_file, 'wt', encoding='utf-8')
  for result in results:
    for i in range(len(result) - 1):
      f.write(result[i] + '\t')
    f.write(result[len(result) - 1] + '\n')
  f.close()


if __name__ == '__main__':
  main(sys.argv)
  