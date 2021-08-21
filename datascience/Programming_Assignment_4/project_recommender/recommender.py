#!/usr/bin/python3
# -*- coding: utf-8 -*-

"""
Data Science
Recommender system with Collaborative filtering(User based)
@author: Yedarm Seong <mybirth0407@gmail.com>
"""

import sys
import numpy as np


def main(argv):
  try:
    train_file = argv[1]
    test_file = argv[2]
  except:
    print("Argument error")
    print("Usage: recommender.exe [train file] [test file]")

  result_file = train_file + '_prediction.txt'

  # kNN을 위한 k
  k = 60

  # 데이터를 numpy 행렬로 변환한다.
  train_matrix = np.loadtxt(train_file, dtype='int64')
  test_matrix = np.loadtxt(test_file, dtype='int64')

  max_train_user_num = max(train_matrix[:, 0])
  max_train_item_num = max(train_matrix[:, 1])

  max_test_user_num = max(test_matrix[:, 0])
  max_test_item_num = max(test_matrix[:, 1])

  # 훈련, 실험 집합 중 가장 큰 사용자와 영화 번호를 저장한다.
  max_user_num = max(max_train_user_num, max_test_user_num)
  max_item_num = max(max_train_item_num, max_test_item_num)

  # user_ratings[사용자][영화] = 평가 점수
  # bool_matrix[사용자][영화] = True or False
  user_ratings, bool_matrix = user_ratings_dict(
    train_matrix, max_user_num, max_item_num)

  # 유저들의 경향을 알기 위한 평균 점수
  user_avg_ratings = user_average_ratings(train_matrix, max_user_num)

  # 사용자들 간 피어슨 상관계수 행렬
  pearson_matrix = user_pearson_matrix(user_ratings,
    max_user_num, user_avg_ratings)

  # 결과 파일 작성
  f = open(result_file, 'wt', encoding='utf-8')
  for test in test_matrix:
    prediction = recommend(user_ratings, user_avg_ratings,
      pearson_matrix, bool_matrix, test[0], test[1], 60)
    f.write(str(test[0]) + '\t' + str(test[1]) + '\t'
            + str(int(prediction)) + '\n')
  f.close()

# O(1)의 탐색을 위해 사전 형태를 사용한다.
def user_ratings_dict(data_set, max_user_num, max_item_num):
  user_ratings = [None] * (max_user_num + 1)
  bool_matrix = np.zeros((max_user_num + 1, max_item_num + 1), dtype=bool)

  for i in range(len(data_set)):
    bool_matrix[data_set[i][0]][data_set[i][1]] = True
    d = {data_set[i][1]: data_set[i][2]}
    if None == user_ratings[data_set[i][0]]:
      user_ratings[data_set[i][0]] = d
    else:
      user_ratings[data_set[i][0]].update(d)
  return user_ratings, bool_matrix

# 각 사용자들이 영화에 대해 평가한 점수의 평균을 반환한다.
def user_average_ratings(data_set, max_user_num):
  user_average_ratings = np.zeros(max_user_num + 1)
  user_rating_counts = np.zeros(max_user_num + 1)

  for i in range(len(data_set)):
    user_average_ratings[data_set[i][0]] += data_set[i][2]
    user_rating_counts[data_set[i][0]] += 1
  user_average_ratings /= user_rating_counts
  return user_average_ratings

# user1과 user2의 공통된 영화 목록을 반환한다.
def common_items(user1, user2, user_ratings):
  commons = []
  d1 = user_ratings[user1]
  d2 = user_ratings[user2]

  for item in d1.keys():
    if item in d2.keys():
      commons.append(item)
  return commons

# 각 사용자들간의 유사도를 측정하여 대칭행렬로 반환한다.
# 피어슨 상관계수를 사용하였다.
def user_pearson_matrix(user_ratings, max_user_num, user_avg_ratings):
  matrix = np.zeros((max_user_num + 1, max_user_num + 1))

  for user1 in range(1, max_user_num + 1):
    for user2 in range(1, user1):
      similarity = 0
      # 피어슨 상관계수의 분자 p
      p = 0
      # 피어슨 상관계수의 분모 s1 * s2
      s1 = 0
      s2 = 0

      user1_avg_rating = user_avg_ratings[user1]
      user2_avg_rating = user_avg_ratings[user2]

      commons = common_items(user1, user2, user_ratings)

      if 0 != len(commons):
        for item in commons:
          p += (user_ratings[user1][item] - user1_avg_rating)\
               * (user_ratings[user2][item] - user2_avg_rating)
          s1 += np.power(user_ratings[user1][item] - user1_avg_rating, 2)
          s2 += np.power(user_ratings[user2][item] - user2_avg_rating, 2)

        similarity = p
        similarity /= np.sqrt(s1 * s2)

      # 공통 item이 없다면 피어슨 상관계수는 0이다.
      if np.isnan(similarity) or np.isinf(similarity):
        matrix[user1][user2] = 0
        matrix[user2][user1] = 0
      else:
        matrix[user1][user2] = similarity
        matrix[user2][user1] = similarity
  return matrix

# user와의 유사도가 양수인 다른 사용자을 반환한다.
def get_similar_users(pearson_matrix, user):
  similars = []

  for i in range(1, len(pearson_matrix)):
    if pearson_matrix[i][user] > 0:
      similars.append(i)
  similars.sort()
  return similars

# 실제 item에 대한 추천 점수
def recommend(user_ratings, user_avg_ratings, pearson_matrix, bool_matrix,
              user, item, k):
  count = 0
  prediction = 0
  normalize_value = 0
  K = k
  weigthed_scaled_sum = 0

  similar_users = get_similar_users(pearson_matrix, user)

  for sim_user in similar_users:
    if True == bool_matrix[sim_user][item]:
      count += 1

      weight = pearson_matrix[user][sim_user]
      normalize_value += weight

      # 가중치와 정규화 상수를 이용한다.
      weigthed_scaled_sum += \
        weight * (user_ratings[sim_user][item] - user_avg_ratings[sim_user])

      K -= 1
      if K == 0:
        break

  # 비슷한 유저가 없다면 예상 점수는 0으로 처리한다.
  if 0 == count or 0 == normalize_value:
    prediction = 0
  else:
    prediction = user_avg_ratings[user]
    prediction += (weigthed_scaled_sum / normalize_value)
    
  if 5 < prediction:
    prediction = 5
  elif 1 > prediction:
    prediction = 1
  return round(prediction)


if __name__ == '__main__':
  main(sys.argv)
