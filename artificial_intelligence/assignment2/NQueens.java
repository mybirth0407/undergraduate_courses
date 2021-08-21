import java.io.BufferedWriter;
import java.io.FileWriter;
import java.util.Random;

/**
 * Created by Yedarm Seong on 2017-03-31.
 */

public class NQueens {
    public static void main(String[] args) {
        Random random = new Random();
        // n * n 체스판을 만들 n의 크기를 인자로 받음
        int n;
        // resultN 파일을 생성할 절대 경로
        String path;
        try {
            n = Integer.parseInt(args[0]);
            path = args[1];
        } catch (Exception e) {
            System.out.println("인자가 없거나 잘못되었습니다.");
            System.out.println("사용법: [java] [-jar] [2013011424.jar] "
                    + "[여왕의 개수] [출력 파일의 절대 경로]");
            e.printStackTrace();
            return;
        }

        System.out.println(">Hill Climbing");

        // 체스판 생성
        int[] board = new int[n];
        double startTime = System.currentTimeMillis();
        // Local minima에서 탈출하지 못하고
        boolean jail = true;

        // 대략 n ^ 2정도의 Random restart를 하면
        // n이 커지더라도 찾을 수 있다.
        for (int cnt = 0; cnt < n * n; cnt++) {
            System.out.println("Random restart");
            // 체스판의 Random start
            for (int i = 0; i < board.length; i++) {
                board[i] = Math.abs(random.nextInt()) % n;
            }

            // 정답을 찾는다면
            if (doHillClimbing(board, getHeuristicCost(board))) {
                jail = false;
                break;
            }
        }
        double time = (System.currentTimeMillis() - startTime) / 1000.000;
        if (jail) {
            System.out.println("Couldn't escape the local minima");
        }
        printBoard(board);
        System.out.println("Total Elapsed time: " + time);
        writeFile(path, "result" + n + ".txt", board, time, jail);
    }

    /**
     * 재귀적으로 Local minima를 찾아가는 함수
     * 현재 상태에서의 휴리스틱 값이 0이라면 정답을 반환한다.
     * @param currentBoard 현재 입력된 체스판에 각 행 마다 여왕이 놓인 위치
     * @param currentCost 현재 입력된 체스판의 휴리스틱 값
     */
    static boolean doHillClimbing(int[] currentBoard, int currentCost) {
        int minCost = currentCost;
        int row = 0, col = 0;

        for (int i = 0; i < currentBoard.length; i++) {
            int currentState = currentBoard[i];

            for (int j = 0; j < currentBoard.length; j++) {
                currentBoard[i] = j;
                int successorCost = getHeuristicCost(currentBoard);
                // 현재 상태보다 더 작은 휴리스틱 값을 갖는 후행자라면
                if (currentState != currentBoard[i]
                    && successorCost < minCost)
                {
                    // 가장 작은 후행자의 상태를 및 휴리스틱 값을 저장한다.
                    minCost = successorCost;
                    row = i;
                    col = j;
                }
            }
            currentBoard[i] = currentState;
        }

        // 휴리스틱 값
        if (minCost == 0) {
            currentBoard[row] = col;
            return true;
        // 후행자 중 가장 작은 휴리스틱 값이 현재 상태보다 작다면
        } else if (minCost < currentCost) {
            // 해당 후행자로 이동하여
            currentBoard[row] = col;
            return doHillClimbing(currentBoard, minCost);
        } else { // Local minima인 경우
            return false;
        }
    }

    /**
     * @param board 휴리스틱 값을 계산할 체스판의 상태
     */
    static int getHeuristicCost(int[] board) {
        int cost = 0;

        for (int i = 0; i < board.length; i++) {
            for (int j = i + 1; j < board.length; j++) {
                // 하나의 행에는 하나의 여왕만 놓여져 있으므로
                // 행끼리의 비교와 대각선 비교만 한다.
                if (board[i] == board[j]
                    || Math.abs(board[i] - board[j]) == j - i) {
                    cost++;
                }
            }
        }
        return cost;
    }

    /**
     * 인자로 넘겨진 board를 과제 형식에 맞게 출력한다.
     * @param board 출력할 체스판의 여왕의 위치 정보
     */
    static void printBoard(int[] board) {
        for (int e: board) {
        // for (int i = 0; i < board.length; i++) {
            System.out.printf("%d ", e);
        }
        System.out.println();
    }

    /**
     * path에 file 파일을 생성하고,
     * 파일에 과제 형식에 맞는 내용을 작성한다.
     * @param path resultN.txt 파일을 생성할 위치의 '절대 경로'
     * @param board 파일에 작성할 여왕의 위치 정보를 담은 체스판
     * @param file 생성할 파일명, resultN.txt
     * @param time 알고리즘이 작동한 시간
     */
    static void writeFile(String path, String file, int[] board,
                          double time, boolean jail) {
        if (path.contains(" ")) {
            System.out.println("경로에 스페이스가 포함되었습니다.");
            System.out.println("전체 경로를 \"\"로 감싸거나, "
                    + "다른 경로를 지정하세요.");
            return;
        }

        String fileName = path + "\\" + file ;

        try {
            // 속도를 위해 BufferdWriter를 사용
            BufferedWriter fw = new BufferedWriter(
                    new FileWriter(fileName, false));

            // 파일에 문자열 쓰기
            fw.write(">Hill Climbing\n");
            // board의 각 원소
            if (null != board) {
                fw.write("Location: ");
                for (int e : board) {
                    fw.write(String.valueOf(e) + " ");
                }
            } else {
                fw.write("No solution");
            }
            fw.write("\n");
            if (jail) {
                fw.write("Couldn't escape the local minima\n");
            }

            fw.write("Total Elapsed Time: ");
            fw.write(String.valueOf(time));

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
