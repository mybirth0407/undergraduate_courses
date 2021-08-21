#!/usr/bin/python3
# -*- coding: utf-8 -*-

"""
Data Science
DBSCAN
@author: Yedarm Seong <mybirth0407@gmail.com>
"""

import sys
import math
import numpy as np


# 하나의 Point에 대한 클래스
class Point:
  def __init__(self, point_id, x, y):
    self.point_id = point_id
    self.x = x
    self.y = y
    # 방문 여부
    self.visited = False
    # None: 아직 클러스터에 속하지 않음
    # -1: 노이즈
    # >= 0: 어떤 클러스터에 속해 있다.
    self.cluster = None

  # 디버깅 용도, Point 클래스 정보를 출력한다.
  def print_info(self):
    print(str(self.point_id) + ', '
      + str(self.x) + ', '
      + str(self.y))

def main(argv):
  try:
    input_file = argv[1]
    n = int(argv[2])
    eps = int(argv[3])
    min_pts = int(argv[4])
  except:
    print('Argument error')
    print('Usage: clustering.py or clustering.exe'
      + ' [input file] [number of cluster] [eps] [min pts]')

  fr = open(input_file, 'rt', encoding='utf-8')

  points = []
  for line in fr.readlines():
    l = line.replace('\n', '').split('\t')
    # point id, x, y
    p = Point(int(l[0]), float(l[1]), float(l[2]))
    points.append(p)
  fr.close()
  dbscan(points, eps, min_pts)
  # remove_small_cluster(points)

  # 입력 파일명에서 input#을 추출한다.
  f_name = input_file[: input_file.find('.txt')]
  
  f = []
  for i in range(0, n):
    # 출력 파일 형식
    f.append(open(
      f_name + '_cluster_' + str(i) + '.txt', 'wt', encoding='utf-8'))

  # n개의 클러스터, 그 이후의 클러스터는 고려하지 않는다.
  for p in points:
    if p.cluster < n:
      f[p.cluster].write(str(p.point_id) + '\n')
    
  for i in range(0, n):
    f[i].close()


# 두 점 p, q의 거리를 계산한다.
def distantce(p, q):
  return np.sqrt(pow((q.x - p.x), 2) + pow((q.y - p.y), 2))

# 두 점 p, q가 이웃 반경 eps 이내이면 True를 반환한다.
def eps_neighbor(p, q, eps):
  return distantce(p, q) <= eps

# id에 해당하는 point에 대해 이웃 point를 찾아 반환한다.
def region_query(point_id, points, eps):
  neighbors = []
  for i in range(len(points)):
    if eps_neighbor(points[point_id], points[i], eps):
      neighbors.append(i)
  return neighbors

# 클러스터를 생성하고 확장한다.
def expand_cluster(point_id, points, neighbors, cluster_id, eps, min_pts):
  points[point_id].cluster = cluster_id

  for neighbor_id in neighbors:
    points[neighbor_id].cluster = cluster_id
    # 방문하지 않은 이웃들에 대해서만 검사한다.
    if points[neighbor_id].visited == False:
      points[neighbor_id].visited = True
      new_neighbors = region_query(neighbor_id, points, eps)

      # 새 이웃에 대해 eps 반경 내에 min pts 이상의 이웃 객체들이 있다면
      # 후보군에 추가한다.
      if len(new_neighbors) >= min_pts:
        neighbors.extend(new_neighbors)

    # 클러스터에 속하지 않았다면 클러스터에 추가한다.
    if points[neighbor_id].cluster == None:
      points[neighbor_id].cluster = cluster_id

# Do DBSCAN
def dbscan(points, eps, min_pts):
  cluster_id = 0
  # debug
  # points[0].print_info()

  # 전체 점을 순서대로 정의
  for i in range(len(points)):
    # 이미 방문했다면 고려할 필요 없다.
    if points[i].visited == True:
      continue

    points[i].visited = True
    neighbors = region_query(i, points, eps)

    # eps 반경 내에 min pts 이상의 이웃 객체들이 있다면 클러스터링
    if len(neighbors) >= min_pts:
      expand_cluster(i, points, neighbors, cluster_id, eps, min_pts)
      cluster_id += 1

    # 노이즈
    else:
      points[i].cluster = -1


if __name__ == '__main__':
  main(sys.argv)