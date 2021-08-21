import java.io.BufferedWriter;
import java.io.FileWriter;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

/**
 * N-여왕 문제를 다음과 같은 세 가지 방법으로 해결하고
 * 각각의 알고리즘 수행 시간을 확인한다.
 * 깊이 우선 탐색, 너비 우선 탐색, 반복적 깊이 증가 우선 탐색
 *
 * @author Yedarm Seong
 * @email mybirth0407@gmail.com
 */

public class NQueens {
    public static void main(String[] args) {
        // n * n 체스판을 만들 n의 크기를 인자로 받음
        int n;
        String path;
        try {
            n = Integer.parseInt(args[0]);
            path = args[1];
        } catch (Exception e) {
            System.out.println("인자가 없거나 잘못되었습니다.");
            System.out.println("사용법: [java] [-jar] [2013011424.jar] "
                             + "[퀸의 개수] [출력 파일의 절대 경로]");
            e.printStackTrace();
            return;
        }

        String noSol = "No solution";

        // DFS
        System.out.println("> dfs");
        // 시간을 재기 위해 DFS를 실행하기 전의 시각을 저장한다.
        long start = System.currentTimeMillis();
        // DFS를 진행할 체스판, board[i] = j는 i행, j열에 퀸이 있음을 의미한다.
        int[] dfsBoard = dfs(0, new int[n], n);

        if (null != dfsBoard) {
        // 정답을 찾았다면 해당 상태 board를 출력한다.
            printBoard(dfsBoard);
        } else {
            // 정답이 없다면 No solution을 출력
            System.out.println(noSol);
        }

        // DFS를 완료한 후 정답을 찾았다면, 현재 시각에서 이전 시각을 출력
        double dfsTime = (System.currentTimeMillis() - start) / 1000.000;
        System.out.println(dfsTime);

        // BFS
        System.out.println("> bfs");
        start = System.currentTimeMillis();
        // 가장 처음 정답을 찾은 상태 board를 반환한다.
        // 정답이 없다면 null을 반환한다.
        int [] bfsBoard = bfs(1, n);

        if (null != bfsBoard) {
           printBoard(bfsBoard);
        } else {
            System.out.println(noSol);
        }

        double bfsTime = (System.currentTimeMillis() - start) / 1000.000;
        System.out.println(bfsTime);

        // DFID
        System.out.println("> dfid");
        start = System.currentTimeMillis();
        // 가장 처음 정답을 찾은 상태 board를 반환한다.
        // 정답이 없다면 null을 반환한다.
        int[] dfidBoard = dfid(n);

        if (null != dfidBoard) {
           printBoard(dfidBoard);
        } else {
            System.out.println(noSol);
        }

        double dfidTime = (System.currentTimeMillis() - start) / 1000.000;
        System.out.println(dfidTime);

        // 결과를 파일에 작성한다.
        writeFile(path, "result" + n + ".txt",
                  dfsBoard, bfsBoard, dfidBoard,
                  dfsTime, bfsTime, dfidTime);
    }

    /**
     * 인자로 받은 정수형 배열에 담긴 퀸들의 위치를 가지고,
     * 현재 퀸들의 배치가 적합한지 확인한다.
     */
    static boolean promising(int[] board) {
        if (board.length <= 1) {
            return false;
        }

        for (int i = 0; i < board.length; i++) {
            for (int j = i + 1; j < board.length; j++) {
                // 같은 열 검사
                if (board[i] == board[j]) {
                    return false;
                } else if (j - i == Math.abs(board[j] - board[i])) {
                // 대각선 검사
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * 깊이 우선 탐색으로,
     * 깊이가 최대가(n) 될 때까지 하나씩 증가시키며 dfs 메서드를 호출한다.
     * 깊이가 n의 크기와 같다면, 정답 후보 중 하나이므로
     * 퀸의 배치가 정답인지 검사한다.
     */
    static int[] dfs(int level, int[] board, int n) {
        // 깊이가 n에 도달했다면, 정답인지 검사
        if (level >= n) {
            if (promising(board)) {
                return board;
            }
            return null;
        }

        for (int i = 0; i < n; i++) {
            board[level] = i;
            // 깊이를 하나 증가시켜 재귀 호출
            if (null != dfs(level + 1, board, n)) {
                return board;
            }
        }
        return null;
    }

    /**
     * 너비 우선 탐색으로,
     * 어떤 한 깊이에서의 모든 노드를 검사한 후에
     * 다음 깊이의 모든들을 검사한다.
     * FIFO 자료구조를 이용하며, 해당 코드에서는 Collection Queue를 이용하였다.
     * Queue의 원소로는 List<Integer>를 사용하였고, 이는 PATH와 같다.
     */
    static int[] bfs(int level, int n) {
        // FIFO 구조의 Queue 이고 비어있을때는 {} 원소를 하나 갖는다고 가정
        // {} 원소는 루트 노드 역할을 한다.
        // append 시 [0], [1], [2], ... [n - 1] 을 만들어내기 위한 것이다.
        Queue<List> q = new LinkedList<List>();
        // 1행에 놓일 수 있는 모든 위치 상태
        for (int i = 0; i < n; i++) {
            List<Integer> e = new LinkedList<>();
            e.add(i);
            q.add(e);
        }

        while (true) {
            // 깊이가 n 이상이 되면 반복 종료
            // 잎새노드에서 정답을 구하지 못하면 정답이 없는 것으로 간주한다.
            if (level >= n) {
                break;
            }

            for (int i = 0; i < Math.pow(n, level); i++) {
                // Queue의 가장 맨 앞 원소를 pop 하고 반환
                List<Integer> pop = q.remove();

                for (int j = 0; j < n; j++) {
                    // pop한 노드에 연결된 자식 노드들을 탐색
                    List<Integer> l = new LinkedList<Integer>(pop);
                    l.add(j);

                    // 모든 행에 하나씩 퀸이 놓여졌다면
                    if (l.size() >= n) {
                        int[] board = new int[l.size()];

                        for (int k = 0; k < l.size(); k++) {
                            board[k] = l.get(k);
                        }

                        // 정답인지 검사 
                        if (promising(board)) {
                            return board;
                        }
                    }
                    // 정답이 아니라면
                    q.add(l);
                }
            }
            // 깊이가 level인 모든 노드에 대해 탐색했다면 깊이 증가
            level++;
        }
        return null;
    }

    /**
     * 반복적 깊이 우선 증가 탐색으로,
     * 깊이를 0부터 n까지 하나씩 증가시키며
     * 깊이 우선 탐색을 통해정답을 찾아가는 알고리즘이다.
     */
    static int[] dfid(int n) {
        // 깊이를 한 단계씩 증가시키며 탐색
        for (int level = 0; level <= n; level++) {
            int[] board = new int[n];

            // 해당 깊이까지 노드를 구성하여 탐색
            board = dfs(0, board, level);
            if (null != board) {
                return board;
            }
        }
        return null;
    }

    /**
     * 인자로 넘겨진 board를 과제 형식에 맞게 출력한다.
     */
    static void printBoard(int[] board) {
        for (int i = 0; i < board.length; i++) {
            System.out.printf("%d ", board[i]);
        }
        System.out.println();
    }

    /**
     * path에 file 파일을 생성하고,
     * 파일에 contents 내용을 작성한다.
     */
    static void writeFile(String path, String file,
                          int[] dfs, int[] bfs, int[] dfid,
                          double dfsTime, double bfsTime, double dfidTime)
    {
        if (path.contains(" ")) {
            System.out.println("경로에 스페이스가 포함되었습니다.");
            System.out.println("전체 경로를 \"\"로 감싸거나, "
                             + "다른 경로를 지정하세요.");
            return;
        }

        String fileName = path + "\\" + file ;

        try {
            // 속도를 위해 BufferdWriter를 사용
            BufferedWriter fw = new BufferedWriter(new FileWriter(fileName, false));

            // 파일에 문자열 쓰기
            fw.write("> DFS\n");
            // dfs 배열의 각 원소
            if (null != dfs) {
                fw.write("Location : ");
                for (int e : dfs) {
                    fw.write(String.valueOf(e) + " ");
                }
            } else {
                fw.write("No solution");
            }

            fw.write("\n");
            fw.write("Time : ");
            fw.write(String.valueOf(dfsTime) + "\n\n");

            fw.write("> BFS\n");
            // bfs의 각 원소
            if (null != bfs) {
                fw.write("Location : ");
                for (int e : bfs) {
                    fw.write(String.valueOf(e) + " ");
                }
            } else {
                fw.write("No solution");
            }

            fw.write("\n");
            fw.write("Time : ");
            fw.write(String.valueOf(bfsTime) + "\n\n");

            fw.write("> DFID\n");
            // dfid의 각 원소
            if (null != dfid) {
                fw.write("Location : ");
                for (int e : dfid) {
                    fw.write(String.valueOf(e) + " ");
                }
            } else {
                fw.write("No solution");
            }

            fw.write("\n");
            fw.write("Time : ");
            fw.write(String.valueOf(dfidTime) + "\n");

            // 파일 스트림 보내기
            fw.flush();
            // 파일 객체 닫기
            fw.close();
        } catch (Exception e) {
            // 예외 처리
            e.printStackTrace();
        }
    }
}
