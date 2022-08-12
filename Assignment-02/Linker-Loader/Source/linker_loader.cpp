#include <bits/stdc++.h>
using namespace std;

struct esTab {
  string symbol;
  int controlSection;
  int address;
};
vector<esTab> estab;

struct modTab {
  int loc, val, length;
  int controlSection;
  string label;
  char operation;
};
vector<modTab> modtab;

map<int, string> objtab;

// convert a hexadecimal string to decimal integer
int hexToInt(string operand) {
  int answer = 0, power = 1;
  for(int i = operand.size() - 1; i >= 0; --i) {
    if(operand[i] >= '0' && operand[i] <= '9') {
      answer += (power * (operand[i] - '0'));
    } else {
      answer += (power * (operand[i] - 'A' + 10));
    }
    power *= 16;
  }
  return answer;
}

// convert decimal integer into hexadecimal string
string intToHex(int n) {
  string res = "";
  if(n > 0) {
    while(n) {
      int rem = n % 16;
      if(rem < 10) {
        res.push_back(rem + '0');
      } else {
        res.push_back(char(rem + 'A' - 10));
      }
      n /= 16;
    }
  } else if(n < 0) {
    unsigned int x = n;
    while(x) {
      int rem = x % 16;
      if(rem < 10) {
        res.push_back(rem + '0');
      } else {
        res.push_back(char(rem + 'A' - 10));
      }
      x /= 16;
    }
  }
  reverse(res.begin(), res.end());
  return res;
}

void parseRecords(string data, vector<string>& names, vector<string>& address) {
  bool ok = 0;
  for(int i = 0; i < data.size(); i = i + 6) {
    if(!ok) {
      string name = data.substr(i, 6);
      while(name.back() == ' ') name.pop_back();
      names.push_back(name);
    } else {
      string add = data.substr(i, 6);
      address.push_back(add);
    }
    ok = !ok;
  }
  assert(names.size() == address.size());
}

// convert a string in decimal into decimal integer
int convertToInt(string& s) {
  int res = 0, power = 1;
  for(int i = s.size() - 1; i >= 0; --i) {
    if(s[i] < '0' || s[i] > '9') {
      return 0;
    }
    res += (power * (s[i] - '0'));
    power *= 10;
  }
  return res;
}

// match the required size of string by adding a certain character at the beginning of the string
void matchSizeFront(string& s, int size, char waste) {
  reverse(s.begin(), s.end());
  while(s.size() < size) s.push_back(waste);
  reverse(s.begin(), s.end());
  return;
}

int programAddress, csAddress;

void pass1() {
  cout << "PASS 1 STARTED" << endl;

  cout << "Please Enter the Starting Address: ";
  cin >> programAddress;
  csAddress = programAddress;

  int curSection = 0, csLength = 0;
  string data;

  ifstream input("input.txt");
  while(getline(input, data)) {
    if(data == "") {
      continue;
    }

    if(data[0] == 'H') {
      curSection++;
      
      string label = data.substr(1, 6);
      while(label.back() == ' ') label.pop_back();
      string csL = data.substr(13, 6);
      csLength = hexToInt(csL);
      
      for(int i = 0; i < estab.size(); ++i) {
        if(estab[i].symbol == label) {
          cout << "ERROR PANIC" << endl;
          return;
        }
      }
      estab.resize(estab.size() + 1);
      estab.back().symbol = label;
      estab.back().controlSection = curSection;
      estab.back().address = csAddress;

      // cout << label << " " << csL << endl;
    }

    if(data[0] == 'E') {
      csAddress += csLength;
    }

    if(data[0] == 'D') {
      vector<string> names, address;
      parseRecords(data.substr(1, data.size() - 1), names, address);

      for(int i = 0; i < names.size(); ++i) {
        
        // search estab if found then error
        for(int id = 0; id < estab.size(); ++id) {
          if(estab[id].symbol == names[i]) {
            cout << "PANIC ERROR" << endl;
            return;
          }
        }

        estab.resize(estab.size() + 1);
        estab.back().symbol = names[i];
        estab.back().controlSection = curSection;
        estab.back().address = csAddress + hexToInt(address[i]);
      }
    }
  }

  input.close();

  cout << "PASS 1 ENDED" << endl;
}

void pass2() {
  cout << "PASS 2 STARTED" << endl;
  ifstream input("input.txt");

  csAddress = programAddress;
  int execAddress = programAddress;
  string data;
  int curSection = 0, prevAddress = 0, csLength = 0, curAddress;

  while(getline(input, data)) {
    if(data == "") {
      continue;
    }

    if(data[0] == 'H') {
      string label = data.substr(1, 6);
      string address = data.substr(7, 6);
      csLength = hexToInt(data.substr(13, 6));
      curSection++;

      prevAddress = hexToInt(address) + csAddress;
      // cout << "H: " << csAddress << " " << label << " " << address << " " << prevAddress << " " << csLength << endl;
    }

    else if(data[0] == 'T') {
      int address = hexToInt(data.substr(1, 6)) + csAddress;
      curAddress = prevAddress;
      int recordLength = hexToInt(data.substr(7, 2));

      for(int i = 0; i < (address - prevAddress); ++i) {
        // objtab.push_back({"..", curAddress});
        objtab[curAddress++] = "..";
      }

      curAddress = address;

      for(int i = 9; i < data.size(); ++i) {
        // objtab.push_back({data.substr(i, 2), curAddress});
        objtab[curAddress++] = data.substr(i++, 2);
      }

      prevAddress = curAddress;
    }

    else if(data[0] == 'M') {
      modtab.resize(modtab.size() + 1);
      modtab.back().loc = hexToInt(data.substr(1, 6)) + csAddress;
      modtab.back().length = hexToInt(data.substr(7, 2));
      modtab.back().operation = data[9];
      modtab.back().label = data.substr(10, data.size() - 10);
      modtab.back().controlSection = curSection;
    }

    else if(data[0] == 'E' && data.size() > 1) {
      execAddress = csAddress + hexToInt(data.substr(1, 6));
    }

    if(data[0] == 'E') {
      csAddress += csLength;
    }

  }

  for(int i = 0; i < 29; ++i) {
    objtab[curAddress++] = "xx";
  }

  for(int i = 0; i < modtab.size(); ++i) {
    int loc = modtab[i].loc;
    int startLoc = loc;
    long long int newAddress = 0;
    
    int index = 0;
    for(int j = 0; j < estab.size(); ++j) {
      if(estab[j].symbol == modtab[i].label) {
        index = j;
        break;
      }
    }

    string temp = "";

    for(int j = 0; j < modtab[i].length; j += 2) {
      temp += objtab[loc];
      loc++;
    }
    // cout << temp << endl;
    if(temp[0] == 'F') {
      newAddress = (long long int) 0xFFFFFFFF000000;
    }

    if(modtab[i].operation == '+') {
      newAddress += ((long long int) hexToInt(temp) + (long long int) estab[index].address);
    }

    if(modtab[i].operation == '-') {
      newAddress += ((long long int) hexToInt(temp) - (long long int) estab[index].address);
    }

    temp = intToHex((int) newAddress);

    while(temp.size() < 6) {
      temp = "0" + temp;
    }

    if (temp.size() > 6 && temp[0] == 'F' && temp[1] == 'F') {
      temp = temp.substr(2, temp.size() - 2);
    }

    for(int it = 0; it < temp.size(); it += 2) {
      objtab[startLoc++] = temp.substr(it, 2);
    }
  }

  input.close();
  cout << "PASS 2 ENDED" << endl;
}

void handleOutput() {
  ofstream output("output.txt");

  int tempAddress = programAddress;
  
  if(tempAddress >= 32) {
    output << "0000";
    for(int i = 0; i < 4; ++i) 
      output << "\txxxxxxxx";
    output << endl;
    
    output << "....";
    for(int i = 0; i < 4; ++i) 
      output << "\t........";
    output << endl;
  }

  if(tempAddress >= 16) {
    string temp = intToHex(tempAddress - 16);
    matchSizeFront(temp, 4, '0');
    output << temp << "\t";
    int i = 4;
    while(i--) {
      output << "xxxxxxxx\t";
    }
    output << endl;
  }


  while(1) {
    bool exist = 0;
    for(auto t: objtab) 
      exist = (exist | (tempAddress == t.first));

    if(exist == 0) {
      break;
    }

    string temp = intToHex(tempAddress);
    matchSizeFront(temp, 4, '0');
    output << temp << "\t";

    for(int i = 0; i < 4; ++i) {
      for(int j = 0; j < 4; ++j) 
        output << objtab[tempAddress++];
      output << "\t";
    }
    output << endl;
  }

  output.close();
}

int main() {
  pass1();
  pass2();
  handleOutput();
}