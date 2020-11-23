#include <algorithm>
#include <iostream>
#include <queue>
#include <stack>
#include <vector>

using std::vector;

vector<vector<bool>> matrix_or(const vector<vector<bool>>& lhs,
                               const vector<vector<bool>>& rhs) {
  int size = lhs.size();
  vector<vector<bool>> res =
      vector<vector<bool>>(size, vector<bool>(size, false));
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      res[i][j] = lhs[i][j] || rhs[i][j];
    }
  }
  return res;
}

vector<vector<bool>> matrix_multiply(const vector<vector<bool>>& lhs,
                                     const vector<vector<bool>>& rhs) {
  int size = lhs.size();
  vector<vector<bool>> res =
      vector<vector<bool>>(size, vector<bool>(size, false));
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = i; k <= j; k++) {
        if (lhs[i][k] && rhs[k][j]) {
          res[i][j] = true;
          break;
        }
      }
    }
  }
  return res;
}

vector<vector<bool>> matrix_kleene(const vector<vector<bool>>& input_matrix) {
  int size = input_matrix.size();
  vector<vector<bool>> res =
      vector<vector<bool>>(size, vector<bool>(size, false));
  for (int i = 0; i < size; i++) {
    for (int j = i; j < size; j++) {
      if (i == j) {
        res[i][i] = true;
      }
      for (int k = i; k <= j; k++) {
        if (!res[i][k]) continue;
        if (input_matrix[k][j]) {
          res[i][j] = true;
          break;
        }
      }
    }
  }
  return res;
}

struct StackElement {
  vector<vector<bool>> is_some_word;

  StackElement() = default;

  StackElement(char c, const std::string& input_word) {
    size_t size = input_word.size() + 1;
    is_some_word = vector<vector<bool>>(size, vector<bool>(size, false));

    if (c == '1') {
      for (size_t i = 0; i < size; ++i) {
        is_some_word[i][i] = true;
      }
    }

    for (size_t i = 0; i < input_word.size(); ++i) {
      if (input_word[i] == c) {
        is_some_word[i][i + 1] = true;
      }
    }
  }
};

StackElement Union(const StackElement& left, const StackElement& right) {
  StackElement result;
  result.is_some_word = matrix_or(left.is_some_word, right.is_some_word);
  return result;
}

StackElement Concat(const StackElement& left, const StackElement& right) {
  StackElement result;
  result.is_some_word = matrix_multiply(left.is_some_word, right.is_some_word);
  return result;
}

StackElement Kleene(const StackElement& operand) {
  StackElement result;
  result.is_some_word = matrix_kleene(operand.is_some_word);
  return result;
}

bool IsStackSizeValid(const std::stack<StackElement>& st, char c) {
  switch (c) {
    case '+':
    case '.':
      return st.size() >= 2;
    case '*':
      return st.size() >= 1;
    case '$':
      return st.size() == 1;
    default:
      return true;
  }
}

std::pair<bool, size_t> Find_max_prefix_lens(const std::string& regular,
                                             const std::string& word) {
  size_t answer = 0;
  std::stack<StackElement> sstack;
  StackElement left_op, right_op, operand, last_element;
  for (char symbol : regular) {
    switch (symbol) {
      case 'a':
      case 'b':
      case 'c':
      case '1':
        sstack.emplace(symbol, word);
        break;
      case '+':
        if (!IsStackSizeValid(sstack, symbol)) {
          return std::make_pair(false, 0);
        }
        left_op = sstack.top();
        sstack.pop();
        right_op = sstack.top();
        sstack.pop();
        sstack.push(Union(left_op, right_op));
        break;
      case '.':
        if (!IsStackSizeValid(sstack, symbol)) {
          return std::make_pair(false, 0);
        }
        right_op = sstack.top();
        sstack.pop();
        left_op = sstack.top();
        sstack.pop();
        sstack.push(Concat(left_op, right_op));
        break;
      case '*':
        if (!IsStackSizeValid(sstack, symbol)) {
          return std::make_pair(false, 0);
        }
        operand = sstack.top();
        sstack.pop();
        sstack.push(Kleene(operand));
        break;
      default:
        break;
    }
  }
  if (!IsStackSizeValid(sstack, '$')) {
    return std::make_pair(false, 0);
  }
  last_element = sstack.top();

  for (size_t i = 0; i < last_element.is_some_word.size(); ++i) {
    if (last_element.is_some_word[0][i]) {
      if (i > answer) {
        answer = i;
      }
    }
  }
  return std::make_pair(true, answer);
}

int main() {
  std::string regular;
  std::cin >> regular;
  std::string word;
  std::cin >> word;
  auto result = Find_max_prefix_lens(regular, word);
  if (result.first) {
    std::cout << result.second << std::endl;
  } else {
    std::cout << "ERROR\n";
  }
}
