import java.io.BufferedWriter;
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Random;

/**
 * Created by Yedarm Seong on 2017-05-26.
 */

public class NQueens {
    // 랜덤 변수를 위한 Random 인스턴스
    public static Random random = new Random();

    public static void main(String[] args) {
        // n * n 체스판을 만들 n의 크기를 인자로 받음
        int n;
        // resultN 파일을 생성할 절대 경로
        String path;
        try {
            n = Integer.parseInt(args[0]);
            path = args[1];
        } catch (Exception e) {
            System.out.println("인자가 없거나 잘못되었습니다.");
            System.out.println("사용법: [java] [-jar] [NQueens.jar] "
                    + "[여왕의 개수] [출력 파일의 절대 경로]");
            e.printStackTrace();
            return;
        }

        System.out.println("> Genetic Algorithm");

        double startTime = System.currentTimeMillis();

        // 유전 알고리즘 실행
        int[] solution =
                doGeneticAlgorithm(n, n * n, 1000000,
                0.01, n * n / 5, n);
        double time = (System.currentTimeMillis() - startTime) / 1000.0;

        // 유전알고리즘의 반환된 결과를 출력한다.
        // Epoch가 끝났음에도 답을 찾지 못한 경우,
        // 마지막 Epoch에서 가장 Fitness가 높은 유전자를 반환한다.
        printBoard(solution);
        System.out.println("Total Elapsed time: " + time);
        writeFile(path, "result" + n + ".txt", solution, time);
    }

    /**
     * Sort를 위한 Comparable 인터페이스를 구현한 염색체 클래스
     * @fitness 서로 공격하는 여왕의 쌍의 수
     * @information 염색체 정보
     */
    public static class Chromosome implements Comparable<Chromosome>{
        private int fitness;
        private int[] information;

        /**
         * 생성자
         */
        public Chromosome(int fitness, int[] information) {
            this.fitness = fitness;
            this.information = information;
        }

        /**
         * Getter and Setter
         */
        public int getFitness() {
            return fitness;
        }

        public int[] getInformation() {
            return information;
        }

        public void setFitness(int fitness) {
            this.fitness = fitness;
        }

        public void setInformation(int[] information) {
            this.information = information;
        }

        @Override
        // Fitness가 낮은 순서대로 정렬하기 위한 Comparable 인터페이스 구현
        public int compareTo(Chromosome chromosome) {
            if (this.fitness > chromosome.getFitness()) {
                return 1;
            }
            else if (this.fitness < chromosome.getFitness()) {
                return -1;
            }
            else {
                return 0;
            }
        }
    }

    /**
     * 유전 알고리즘을 수행한다.
     * @param size 하나의 염색체 사이즈
     * @param mutationProbability 돌연변이 확률
     * @param k K토너먼트의 K
     * @param maxEpoch 최대 Epoch 수, 세대 수와 같다.
     * @param numofParents 한 세대에서 다음 세대로 넘겨질 부모 염색체 수
     * @param population 한 세대의 전체 염색체 수
     */
    static int[] doGeneticAlgorithm(int size, int population,
                                    int maxEpoch,
                                    double mutationProbability,
                                    int numofParents, int k)
    {
        // 임의의 초기 염색체 집단 생성
        ArrayList<Chromosome> chromosomes =
                generateInitChromosomes(size, population);
        for (int epoch = 1; epoch <= maxEpoch; epoch++) {
            System.out.println("epoch: " + epoch);
            for (Chromosome chromosome: chromosomes) {
                // 정답과 일치하는 Fitness를 가진 염색체가 있다면
                // 염색체 정보를 반환한다.
                if (0 == chromosome.getFitness()) {
                    return chromosome.getInformation();
                }
            }

            // Fitness를 기준으로 정렬한다.
            Collections.sort(chromosomes);
            // 토너먼트 방식을 통해 numofParents 개수 만큼의 유전자를
            // 부모 염색체로 선택한다.
            ArrayList<Chromosome> parents =
                    kTournamentSelection(chromosomes, k, numofParents);
            chromosomes.removeAll(chromosomes);
            // 부모 직속 전달, 교배, 돌연변이 연산을 통해 다음 세대의
            // 염색체를 생성한다.
            chromosomes = generateNextChromosomes(
                    parents, mutationProbability, population, size);
        }
        // 마지막 Epoch에서도 정답을 찾지 못한다면
        // 가장 Fitness가 높은 염색체의 정보를 반환한다.
        return chromosomes.get(0).getInformation();
    }

    /**
     * 다음 세대의 염색체를 발현한다.
     * 직속 전달, 교배 및 돌연변이 연산으로 진행된다.
     * @param parents 교배할 부모 염색체
     * @param mutationProbability 돌연변이 확률
     * @param population 한 세대의 인구수
     * @param size 하나의 염색체 크기
     */
    static ArrayList generateNextChromosomes(ArrayList parents,
                                             double mutationProbability,
                                             int population, int size)
    {
        ArrayList<Chromosome> next = new ArrayList<>();

        // 교배를 통한 자식 염색체 생성, 해당 과정 내에서 돌연변이도 발생한다.
        // 다음 세대의 자식은 현재 인구수의 2배로 정한다.
        ArrayList<Chromosome> childs =
                crossover(parents, 2 * population,
                    size, mutationProbability);

        // 부모는 그대로 다음 세대로 전달한다.
        next.addAll(parents);
        // 생성된 자식 염색체도 전달한다.
        next.addAll(childs);
        return next;
    }

    /**
     * 교배 연산을 진행한다. 해당 과정에서 돌연변이도 발생한다.
     * @param parents 부모 염색체
     * @param numofChild 교배하여 생성할 자식 염색체의 개수
     * @param size 하나의 염색체 크기
     * @param mutationProbability 돌연변이 확률
     */
    static ArrayList crossover(ArrayList parents, int numofChild,
                               int size, double mutationProbability)
    {
        ArrayList<Chromosome> childs = new ArrayList();

        for (int i = 0; i < numofChild; i++) {
            // 모 염색체
            int randomMother = random.nextInt(parents.size());
            Chromosome mother = (Chromosome) parents.get(randomMother);

            // 부 염색체
            int randomFather = random.nextInt(parents.size());
            Chromosome father = (Chromosome) parents.get(randomFather);

            // 부모가 같은 염색체일 수 없다.
            while (true) {
                if (mother.getInformation().equals(father.getInformation())) {
                    break;
                }
                randomFather = random.nextInt(parents.size());
                father = (Chromosome) parents.get(randomFather);
            }

            // 임의로 생성된 서로 다른 2점을 교차한다.
            int first = randomMother % mother.getInformation().length;
            int second = randomMother % father.getInformation().length;
            while (true) {
                if (first != second) {
                    break;
                }
                second = random.nextInt(father.getInformation().length);
            }

            // first < second
            if (first > second) {
                int t = first;
                first = second;
                second = t;
            }

            // index 0부터 first까지는 모 염색체
            int[] temp = new int[mother.getInformation().length];
            for (int j = 0; j < first; j++) {
                temp[j] = mother.getInformation()[j];
            }

            // index first부터 second까지는 부 염색체
            for (int j = first; j < second; j++) {
                temp[j] = father.getInformation()[j];
            }

            // index second부터 마지막까지는 모 염색체
            for (int j = second; j < mother.getInformation().length; j++) {
                temp[j] = mother.getInformation()[j];
            }

            // 확률적으로 돌변연이를 발생시킨다.
            temp = mutation(temp, mutationProbability);
            Chromosome child = new Chromosome(calculateFitness(temp), temp);
            childs.add(child);
        }
        return childs;
    }

    /**
     * 확률적으로 돌연변이를 발생시킨다.
     * 염색체 하나에 대해 유전자를 하나씩 확률적으로 임의로 변환시킨다.
     * @param chromosome 돌연변이를 발생시킬 염색체
     * @param mutationProbability 돌연변이가 발생할 확률
     */
    static int[] mutation(int[] chromosome, double mutationProbability) {
        // 돌연변이 발생 여부
        boolean isMutation = false;
        for (int i = 0; i < chromosome.length; i++) {
            int r = random.nextInt((int) (1 / mutationProbability));
            if (r < 1) {
                isMutation = true;
                chromosome[i] = Math.abs(random.nextInt(chromosome.length));
            }
        }

        if (isMutation) {
            System.out.println("Mutation!");
        }
        return chromosome;
    }

    /**
     * 첫 세대의 염색체를 임의로 발현한다.
     * @param population 첫 세대의 인구수
     * @param size 하나의 염색체 크기
     */
    static ArrayList generateInitChromosomes(int size, int population) {
        ArrayList<Chromosome> chromosomes = new ArrayList<>();

        for (int i = 0; i < population; i++) {
            Chromosome chromosome;

            while (true) {
                chromosome = generateChromosome(size);
                if (!chromosomes.contains(chromosome)) {
                    break;
                }
            }
            chromosomes.add(chromosome);
        }
        return chromosomes;
    }

    /**
     * 염색체를 임의로 발현한다.
     * @param size 하나의 염색체 크기
     */
    static Chromosome generateChromosome(int size) {
        Random random = new Random();
        int[] information = new int[size];

        for (int i = 0; i < size; i++) {
            information[i] = Math.abs(random.nextInt(size));
        }
        Chromosome chromosome =
                new Chromosome(calculateFitness(information), information);

        return chromosome;
    }

    /**
     * Fitness를 계산한다. 0에 근접할수록 우수하다고 할 수 있다.
     * @param information 계산할 염색체 정보
     */
    static int calculateFitness(int[] information) {
        int fitness = 0;

        for (int i = 0; i < information.length; i++) {
            for (int j = i + 1; j < information.length; j++) {
                if (information[i] == information[j]
                    || Math.abs(information[i] - information[j]) == j - i)
                {
                    fitness++;
                }
            }
        }
        return fitness;
    }

    /**
     * 임의의 k개의 염색체를 선택해서 k개 중 가장 Fitness가 높은 염색체를
     * 부모 염색체로 선택한다.
     * 위의 과정을 n번 반복하여 n개의 부모 염색체를 생성한다.\
     * @param chromosomes 전체 염색체 집단
     * @param k 부모를 선택하기 위해 임의로 선택할 염색체의 수
     * @param n 반환할 부모 염색체의 개수
     */
    static ArrayList kTournamentSelection(ArrayList chromosomes,
                                          int k, int n)
    {
        ArrayList<Chromosome> parents = new ArrayList<>();
        Random random = new Random();
        ArrayList<Chromosome> candidates = new ArrayList<>();

        // n개의 부모를 선택하기 위해
        for (int parent = 0; parent < n; parent++) {
            // k개 선택
            for (int i = 0; i < k; i++) {
                candidates.add((Chromosome)
                        chromosomes.get(random.nextInt(chromosomes.size())));
            }
            Collections.sort(candidates);
            parents.add(candidates.get(0));
            candidates.removeAll(candidates);
        }
        return parents;
    }

    /**
     * 유전자 정보에 대해 출력한다.
     * 유전 알고리즘에서는 하나의 염색체와 같다.
     * @param board 출력할 체스판
     */
    static void printBoard(int[] board) {
        // 잘못된 인자라면
        if (null == board) {
            System.out.println("No solution");
        } else {
            for (int e : board) {
                // for (int i = 0; i < board.length; i++) {
                System.out.printf("%d ", e);
            }
            System.out.println();
        }
    }

    /**
     * path에 file 파일을 생성하고,
     * 파일에 과제 형식에 맞는 내용을 작성한다.
     * @param path resultN.txt 파일을 생성할 위치의 '절대 경로'
     * @param board 파일에 작성할 여왕의 위치 정보를 담은 체스판
     * @param file 생성할 파일명, resultN.txt
     * @param time 알고리즘이 작동한 시간
     */
    static void writeFile(String path, String file, int[] board, double time)
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
            fw.write("> Genetic Algorithm\n");
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
