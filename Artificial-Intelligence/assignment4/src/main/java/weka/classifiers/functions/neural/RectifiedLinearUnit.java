package weka.classifiers.functions.neural;

import weka.core.RevisionHandler;
import weka.core.RevisionUtils;

/**
 * Created by Yedarm Seong on 2017-05-02.
 * Assignment4 과제는 Weka Coding convention에 맞춰 작성했습니다.
 */
public class RectifiedLinearUnit
  implements NeuralMethod, RevisionHandler {

  /** 직렬화를 위한 임의의 숫자 */
  private static final long serialVersionUID = -4162958458177475652L;

  /**
   * 출력값을 계산하는 함수
   * @param node 출력값 계산을 위한 노드
   * @return 계산한 출력값
   */
  @Override
  public double outputValue(NeuralNode node) {
    // 해당 노드의 가중치 벡터
    double[] weights = node.getWeights();
    // 해당 노드의 입력 벡터
    NeuralConnection[] inputs = node.getInputs();
    // Bias
    double value = weights[0];

    // 입력 벡터와 가중치 벡터를 곱해 출력값을 계산한다.
    for (int noa = 0; noa < node.getNumInputs(); noa++) {
        value += (inputs[noa].outputValue(true) * weights[noa+1]);
    }

    // 출력부에서의 활성 함수를 ReLU로 한다.
    value = Math.max(0, value);
    return value;
  }

  /**
   * 오류값을 계산하는 함수
   * @param node 오류값 계산을 위한 노드
   * @return 계산한 오류값
   */
  @Override
  public double errorValue(NeuralNode node) {
    // 해당 노드의 출력 벡터
    NeuralConnection[] outputs = node.getOutputs();
    int[] oNums = node.getOutputNums();
    double error = 0;

    // 에러를 계산한다.
    for (int noa = 0; noa < node.getNumOutputs(); noa++) {
      error += (outputs[noa].errorValue(true)
             * outputs[noa].weightValue(oNums[noa]));
    }
    return error;
  }

  /**
   * 가중치 벡터를 업데이트하는 함수
   * @param node 가중치를 업데이트할 노드
   * @param learn 학습률(Learning rate, Step size)
   * @param momentum 변화의 방향에 대한 가속도
   */
  @Override
  public void updateWeights(NeuralNode node, double learn, double momentum) {
    // 해당 노드의 입력 벡터
    NeuralConnection[] inputs = node.getInputs();
    // 변화된 가중치 벡터
    double[] cWeights = node.getChangeInWeights();
    // 해당 노드의 가중치 벡터
    double[] weights = node.getWeights();
    double learnTimesError = 0;
    learnTimesError = learn * node.errorValue(false);
    double c = learnTimesError + momentum * cWeights[0];
    // Bias
    weights[0] += c;
    cWeights[0] = c;

    int stopValue = node.getNumInputs() + 1;
    for (int noa = 1; noa < stopValue; noa++) {
      c = learnTimesError * inputs[noa-1].outputValue(false);
      // 가속도를 곱해주어 더 빠르게 업데이트 할 수 있도록 한다.
      c += momentum * cWeights[noa];

      // 가중치를 업데이트한다.
      weights[noa] += c;
      cWeights[noa] = c;
    }
  }

  /**
   * Returns Revision 정보
   *
   * @return Revision
   */
  public String getRevision() {
    return RevisionUtils.extract("$Revision: 8034 $");
  }
}
