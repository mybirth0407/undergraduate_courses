import java.io.BufferedWriter;
import java.io.FileWriter;

/**
 * Created by Yedarm Seong on 2017-04-12.
 */
public class NQueens {
    public static void main(String args[]) {
        // 체스판의 크기, 행과 열의 크기는 동일하다.
        int n;
        // resultN 파일을 생성할 절대 경로
        String path;
        try {
            n = Integer.parseInt(args[0]);
            path = args[1];
            if (n <= 0) {
                return;
            }
        } catch (Exception e) {
            System.out.println("인자가 없거나 잘못되었습니다.");
            System.out.println("사용법: [java] [-jar] [2013011424.jar] "
                    + "[여왕의 개수] [출력 파일의 절대 경로]");
            e.printStackTrace();
            return;
        }

        // Standard CSP
        double start = System.currentTimeMillis();
        int[] standardBoard = doStandardCSP(new int[n], 0);
        double standardCSPTime =
                (System.currentTimeMillis() - start) / 1000.000;
        if (!isAnswer(standardBoard)) {
            standardBoard = null;
        }
        // 접근하지 않는 메모리에 대해 Garbage collector 작동을 제안한다.
        System.gc();
        printBoard(standardBoard);
        System.out.println(standardCSPTime);

        // CSP with forward checking
        start = System.currentTimeMillis();
        int[][] forwardCheckBoard = doForwardCheckCSP(new int[n][n], 0);
        double forwardCheckTime =
                (System.currentTimeMillis() - start) / 1000.000;
        if (!isAnswer(forwardCheckBoard)) {
            forwardCheckBoard = null;
        }
        System.gc();
        printBoard(forwardCheckBoard);
        System.out.println(forwardCheckTime);

        // CSP with arc consistency
        start = System.currentTimeMillis();
        int[][] arcConsistencyBoard = doArcConsistencyCSP(new int[n][n], 0);
        double arcConsistencyTime =
                (System.currentTimeMillis() - start) / 1000.000;
        if (!isAnswer(arcConsistencyBoard)) {
            arcConsistencyBoard = null;
        }
        System.gc();
        printBoard(arcConsistencyBoard);
        System.out.println(arcConsistencyTime);

        // 결과를 파일에 작성한다.
        writeFile(path, "result" + n +".txt",
                standardBoard, forwardCheckBoard, arcConsistencyBoard,
                standardCSPTime, forwardCheckTime, arcConsistencyTime);
    }

    /**
     * 인자로 받은 정수형 배열에 담긴 퀸들의 위치를 가지고,
     * 현재 퀸들의 배치가 적합한지 확인한다.
     * @param board 검사할 체스판의 상태이다.
     * @param level 몇 번째 레벨까지 검사할지를 정해준다.
     * @return 유망하다면 true, 그렇지 않다면 false를 반환한다.
     */
    static boolean standardPromising(int[] board, int level) {
        for (int i = 0; i < level + 1; i++) {
            for (int j = i + 1; j < level + 1; j++) {
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
     * Standard CSP를 이용하여 N-여왕 문제를 해결한다.
     * @param board doStandardCSP를 호출할 당시의 체스판의 상태이다.
     * @param level 재귀 호출의 깊이, 몇번째 행인지를 나타낸다.
     * @return level에서의 유망한 board의 상태를 반환한다.
     */
    public static int[] doStandardCSP(int[] board, int level) {
        if (level >= board.length) {
            return board;
        }

        int[] returnBoard = new int[board.length];

        for (int i = 0; i < board.length; i++) {
            board[level] = i;
            // 깊이를 하나 증가시켜 재귀 호출
            if (standardPromising(board, level)) {
                returnBoard = doStandardCSP(board, level + 1);

                // 정답을 찾는다면, 해당 정답을 반환한다.
                if (null != returnBoard) {
                    return returnBoard;
                }
            }
        }
        // 정답이 아닌 경우 null을 반환한다.
        return null;
    }

    /**
     * 인자로 받은 정수형 배열에 담긴 퀸들의 위치를 가지고,
     * 현재 퀸들의 배치가 적합한지 확인한다.
     * @param board 검사할 체스판의 상태이다.
     *              -1: 유망하지 않은 위치
     *              0: 유망한 위치
     *              1: 이미 퀸을 배정한 위치
     * @param row 몇 번째 행을 검사할지를 정해준다.
     * @return 유망하다면 true, 그렇지 않다면 false
     */
    public static boolean forwardCheckPromising(int[][] board, int row) {
        for (int i = 0; i < board.length; i++) {
            if (0 == board[row][i]) {
                return true;
            }
        }
        return false;
    }

    /**
     * 인자로 받은 board의 row, column의 위치를 기준으로 forward checking을 한다.
     * @param board forward checking을 할 체스판의 상태
     * @param row forward checking을 할 열의 위치
     * @param column forward checking을 할 열의 위치
     */
    public static void forwardCheck(int[][] board, int row, int column) {
        for (int i = 0; i < board.length; i++) {
            if (0 == board[row][i]) {
                board[row][i] = -1;
            }
        }

        for (int i = 0; i < board.length; i++) {
            if (0 == board[i][column]) {
                board[i][column] = -1;
            }
        }

        // 입력된 행보다 위의 행들에 대해서 forward checking
        for (int i = 0; i < row; i++) {
            if (-1 < (column - row + i)
                && board.length > (column - row + i)
                && board[i][column - row + i] != 1)
            {
                board[i][column - row + i] = -1;
            }

            if (-1 < (column + row - i)
                && board.length > (column + row - i)
                && board[i][column + row - i] != 1)
            {
                board[i][column + row - i] = -1;
            }
        }

        // 입력된 행보다 아래의 행들에 대해서 forward checking
        for (int i = row + 1; i < board.length; i++) {
            if (-1 < (column - row + i - 1)
                    && board.length > (column - row + i)
                    && board[i][column - row + i] != 1)
            {
                board[i][column - row + i] = -1;
            }

            if (-1 < (column + row - i - 1)
                    && board.length > (column + row - i)
                    && board[i][column + row - i] != 1)
            {
                board[i][column + row - i] = -1;
            }
        }
    }

    /**
     * @param board 현재 board의 상태
     * @param level forward checking을 이용한 CSP를 수행할 단계, 행의 위치이다.
     * @return level에서의 유망한 board를 반환한다. 그렇지 않다면 null을 반환한다.
     */
    public static int[][] doForwardCheckCSP(int[][] board, int level) {
        if (level >= board.length) {
            return board;
        }

        if (!forwardCheckPromising(board, level)) {
            return null;
        }

        int[][] newBoard = new int[board.length][board.length];
        copyArray(board, newBoard);

        for (int i = 0; i < board.length; i++) {
            if (0 == newBoard[level][i]) {
                newBoard[level][i] = 1;
                forwardCheck(newBoard, level, i);
                newBoard = doForwardCheckCSP(newBoard, level + 1);

                if (null != newBoard) {
                    return newBoard;
                }
                newBoard = new int[board.length][board.length];
                copyArray(board, newBoard);
                newBoard[level][i] = -1;
            }
        }
        return null;
    }

    /**
     * forward checking 이후 모든 간선에 대해 consistency를 확인한다.
     * N-여왕 문제의 경우, 모든 상태는 서로 연결되어 있기 때문에
     * level 이후의 consistency만 고려하면 된다.
     * @param board
     * @param level
     * @return
     */
    public static int[][] doArcConsistencyCSP(int[][] board, int level) {
        if (level >= board.length) {
            return board;
        }

        int[][] newBoard = new int[board.length][board.length];
        copyArray(board, newBoard);

        for (int i = 0; i < newBoard.length; i++) {
            if (-1 == newBoard[level][i]) {
                continue;
            }
            // [level][i]의 consistency를 확인하기 위해
            // forward checking을 선행한다.
            forwardCheck(newBoard, level, i);

            if (consistencyCheck(newBoard, level + 1)) {
                // [level][i]가 유망하고,
                // 모든 행에 대해 Consistency를 만족한다면 여왕을 배치한다.
                newBoard[level][i] = 1;
                newBoard = doArcConsistencyCSP(newBoard, level + 1);

                // 유망한 상태라면 해당 체스판을 반환한다
                if (null != newBoard) {
                    return newBoard;
                } else {
                    // 그렇지 않다면 이전 체스판으로 복귀한다.
                    newBoard = new int[board.length][board.length];
                    copyArray(board, newBoard);
                }
            } else {
                copyArray(board, newBoard);
                newBoard[level][i] = -1;
            }
        }
        return null;
    }

    /**
     * level 이전의 행은 이미 consistency 하다. 따라서 이후의 level만 확인한다.
     * @param level
     * @param board
     * @return level 이후부터의 모든 간선이 Consistency하면 true를 반환한다.
     */
    public static boolean consistencyCheck(int[][] board, int level) {
        for (int i = level; i < board.length; i++) {
            boolean isConsistency = false;
            for (int j = 0; j < board.length; j++) {
                if (0 == board[level][j]) {
                    isConsistency = isConsistency | true;
                    break;
                }
            }

            if (!isConsistency) {
                return false;
            }
        }
        return true;
    }

    /**
     * 입력받은 일차원 배열 board를 검사하여 정답인지 아닌지를 판별한다.
     * @param board 정답인지 검사할 체스판
     * @return 정답이면 true, 그렇지 않다면 false를 밴환한다.
     */
    static boolean isAnswer(int[] board) {
        if (null == board || board.length <= 1) {
            return false;
        }

        for (int i = 0; i < board.length; i++) {
            for (int j = i + 1; j < board.length; j++) {
                // 같은 열 검사
                if (board[i] == board[j]) {
                    return false;
                    // 대각선 검사
                } else if (j - i == Math.abs(board[j] - board[i])) {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * 일차원 배열인 board를 출력한다.
     * @param board 출력할 체스판
     */
    static void printBoard(int[] board) {
        if (null == board) {
            System.out.println("No solution");
            return;
        }

        for (int i = 0; i < board.length; i++) {
            System.out.printf("%d ", board[i]);
        }
        System.out.println();
    }

    /**
     * 이차원 배열인 board를 출력한다.
     * @param board 출력할 체스판
     */
    static void printBoard(int[][] board) {
        if (null == board) {
            System.out.println("No solution");
            return;
        }

        for (int i = 0; i < board.length; i++) {
            for (int j = 0; j < board.length; j++) {
                if (1 == board[i][j]) {
                    System.out.printf("%d ", j);
                }
            }
        }
        System.out.println();
    }

    /**
     * 정수형 이차원 배열 src를 같은 형식, 같은 크기의 des에 복사한다.
     * @param src
     * @param des
     */
    static void copyArray(int[][] src, int[][] des) {
        for (int j = 0; j < src.length; j++) {
            for (int k = 0; k < src.length; k++) {
                des[j][k] = src[j][k];
            }
        }
    }

    /**
     * 입력받은 이차원 배열 board를 검사하여 정답인지 아닌지를 판별한다.
     * @param board
     * @return
     */
    static boolean isAnswer(int[][] board) {
        if (null == board || board.length <= 1) {
            return false;
        }

        int[] testBoard = new int[board.length];
        for (int i = 0; i < board.length; i++) {
            for (int j = 0; j < board.length; j++) {
                if (1 == board[i][j]) {
                    testBoard[i] = j;
                }
            }
        }

        for (int i = 0; i < testBoard.length; i++) {
            for (int j = i + 1; j < testBoard.length; j++) {
                // 같은 열 검사
                if (testBoard[i] == testBoard[j]) {
                    return false;
                    // 대각선 검사
                } else if (j - i == Math.abs(testBoard[j] - testBoard[i])) {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * 입력받은 절대경로 path에 file 파일을 생성하고, 각 내용들을 작성한다.
     * @param path file 파일을 생성할 절대경로
     * @param file 생성할 파일 이름
     * @param standardBoard 파일에 출력할 CSP 체스판
     * @param forwardCheckBoard 파일에 출력할 forward check 체스판
     * @param arcConsistencyBoard 파일에 출력할 arc consistency 체스판
     * @param standardTime CSP로 정답을 찾는데 걸린 시간
     * @param forwardCheckTime forward check로 정답을 찾는데 걸린 시간
     * @param arcConsistencyTime arc consistency로 정답을 찾는데 걸린 시간
     */
    static void writeFile(String path, String file,
                          int[] standardBoard,
                          int[][] forwardCheckBoard,
                          int[][] arcConsistencyBoard,
                          double standardTime,
                          double forwardCheckTime,
                          double arcConsistencyTime)
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
            BufferedWriter fw = new BufferedWriter(
                    new FileWriter(fileName, false));

            // 파일에 문자열 쓰기
            fw.write(">Standard CSP\n");
            // board의 각 원소
            if (null != standardBoard) {
                fw.write("Location: ");
                for (int e : standardBoard) {
                    fw.write(String.valueOf(e) + " ");
                }
            } else {
                fw.write("No solution");
            }
            fw.write("\n");

            fw.write("Total Elapsed Time: ");
            fw.write(String.valueOf(standardTime) + "\n\n");

            // 파일에 문자열 쓰기
            fw.write(">Forward Checking\n");
            // board의 각 원소
            if (null != forwardCheckBoard) {
                fw.write("Location: ");
                for (int i = 0; i < forwardCheckBoard.length; i++) {
                    for (int j = 0; j < forwardCheckBoard.length; j++) {
                        if (1 == forwardCheckBoard[i][j]) {
                            fw.write(String.valueOf(j) + " ");
                        }

                    }
                }
            } else {
                fw.write("No solution");
            }
            fw.write("\n");

            fw.write("Total Elapsed Time: ");
            fw.write(String.valueOf(forwardCheckTime) + "\n\n");

            // 파일에 문자열 쓰기
            fw.write(">Arc consistency\n");
            // board의 각 원소
            if (null != arcConsistencyBoard) {
                fw.write("Location: ");
                for (int i = 0; i < arcConsistencyBoard.length; i++) {
                    for (int j = 0; j < arcConsistencyBoard.length; j++) {
                        if (1 == arcConsistencyBoard[i][j]) {
                            fw.write(String.valueOf(j) + " ");
                        }

                    }
                }
            } else {
                fw.write("No solution");
            }
            fw.write("\n");

            fw.write("Total Elapsed Time: ");
            fw.write(String.valueOf(arcConsistencyTime) + '\n');

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
