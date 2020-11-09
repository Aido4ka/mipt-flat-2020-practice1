#include <algorithm>
#include <iostream>
#include <queue>
#include <stack>
#include <vector>

bool checking_for_correctness(const std::string& regular) {
  size_t stack_sz = 0;
  for (const auto& c : regular) {
    if (c == 'a' || c == 'b' || c == 'c' || c == '1') {
      ++stack_sz;
    } else if (c == '+' || c == '.') {
      if (stack_sz < 2) {
        return false;
      }
      --stack_sz;
    } else if (c == '*') {
      if (stack_sz < 1) {
        return false;
      }
    } else {
      return false;
    }
  }
  return stack_sz == 1;
}

using std::vector;

vector<vector<bool>> matrix_or(const vector<vector<bool>>& first_matrix,
                               const vector<vector<bool>>& second_matrix) {
  int size = first_matrix.size();
  vector<vector<bool>> res =
      vector<vector<bool>>(size, vector<bool>(size, false));
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (first_matrix[i][j] || second_matrix[i][j]) res[i][j] = true;
    }
  }
  return res;
}

vector<vector<bool>> matrix_and(const vector<vector<bool>>& first_matrix,
                                const vector<vector<bool>>& second_matrix) {
  int size = first_matrix.size();
  vector<vector<bool>> res =
      vector<vector<bool>>(size, vector<bool>(size, false));
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = i; k <= j; k++) {
        if (first_matrix[i][k] && second_matrix[k][j]) {
          res[i][j] = true;
          break;
        }
      }
    }
  }
  return res;
}

vector<vector<bool>> matrix_star(const vector<vector<bool>>& first_matrix) {
  int size = first_matrix.size();
  vector<vector<bool>> res =
      vector<vector<bool>>(size, vector<bool>(size, false));
  for (int i = 0; i < size; i++) {
    for (int j = i; j < size; j++) {
      if (i == j) {
        res[i][i] = true;
      }
      for (int k = i; k <= j; k++) {
        if (!res[i][k]) continue;
        if (first_matrix[k][j]) {
          res[i][j] = true;
          break;
        }
      }
    }
  }
  return res;
}

struct StackElement {
  vector<vector<bool>> whole_;

  StackElement() = default;

  StackElement(char c, const std::string& input_word) {
    size_t size = input_word.size() + 1;
    whole_ = vector<vector<bool>>(size, vector<bool>(size, false));

    for (size_t i = 0; i < size; ++i) {
      if (c == '1') {
        whole_[i][i] = true;
      }
    }

    for (size_t i = 0; i < input_word.size(); ++i) {
      if (input_word[i] == c) {
        whole_[i][i + 1] = true;
      }
    }
  }
};

StackElement Union(const StackElement& left, const StackElement& right) {
  StackElement result;
  result.whole_ = matrix_or(left.whole_, right.whole_);
  return result;
}

StackElement Concat(const StackElement& left, const StackElement& right) {
  StackElement result;
  result.whole_ = matrix_and(left.whole_, right.whole_);
  return result;
}

StackElement Kleene(const StackElement& operand) {
  StackElement result;
  result.whole_ = matrix_star(operand.whole_);
  return result;
}

size_t Find_max_prefix_lens(const std::string& regular,
                            const std::string& word) {
  size_t answer = 0;
  std::stack<StackElement> sstack;
  StackElement left_op, right_op, operand, last_element;
  for (char symbol : regular) {
    switch (symbol) {
      case 'a':
        sstack.emplace(symbol, word);
        break;
      case 'b':
        sstack.emplace(symbol, word);
        break;
      case 'c':
        sstack.emplace(symbol, word);
        break;
      case '1':
        sstack.emplace(symbol, word);
        break;
      case '+':
        left_op = sstack.top();
        sstack.pop();
        right_op = sstack.top();
        sstack.pop();
        sstack.push(Union(left_op, right_op));
        break;
      case '.':
        right_op = sstack.top();
        sstack.pop();
        left_op = sstack.top();
        sstack.pop();
        sstack.push(Concat(left_op, right_op));
        break;
      case '*':
        operand = sstack.top();
        sstack.pop();
        sstack.push(Kleene(operand));
        break;
      default:
        break;
    }
  }
  last_element = sstack.top();

  for (size_t i = 0; i < last_element.whole_.size(); ++i) {
    if (last_element.whole_[0][i]) {
      if (i > answer) {
        answer = i;
      }
    }
  }
  return answer;
}

int main() {
  std::string regular;
  std::cin >> regular;
  std::string word;
  std::cin >> word;

  if (checking_for_correctness(regular)) {
    std::cout << Find_max_prefix_lens(regular, word) << std::endl;
  } else {
    std::cout << "INF\n";
  }
}