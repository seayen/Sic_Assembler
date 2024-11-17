#include "Assembler.h"
#include "LINE.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>

using namespace std;
// optab을 읽어와 변수에 저장하는 함수 
// 입력 : 파일 이름
// 출력 : 성공 여부 (bool 형태)
bool Assembler::loadOptab(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error opening " << filename << endl;
        return false;
    }

    string operation, code;
    while (file >> operation >> code) {
        optab[operation] = code;
    }
    file.close();
    return true;
}

//패스 1 과정
void Assembler::pass1(const vector<string>& SRCFILE) {
    int locCounter = 0; //loc 부여를 위한 카운터

    for (const string& lineStr : SRCFILE) {
        //라인 정보 가져옴
        string label = trim(lineStr.substr(0, 9));
        string operationCode = trim(lineStr.substr(9, 6));
        string operand = trim(lineStr.substr(17));
        

        LINE line(label, operationCode, operand, locCounter);
        // START 또는 END 의 줄의 경우 예외처리해주는 부분
        // 특히 시작 주소 loc를 설정한다.
        if (operationCode == "START") {
            locCounter = stoi(operand, nullptr, 16); // 16진수로 변환
            if (!label.empty()) {
                symtab[label] = locCounter;
            }
            lines.push_back(line);
            continue;
        } else if (operationCode == "END") {
            lines.push_back(line);
            continue;
        }
        
        // 라벨이 중복됬을 경우를 처리하는 부분
        // 오류코드 1 설정
        if (!label.empty()) {
            if (symtab.find(label) != symtab.end()) {
                cerr << "Error: Duplicate symbol - " << label << endl;
                line.setErrorCode(1);
            } else {
                symtab[label] = locCounter;
            }
        }

        // OPERATION을 검사하는 부분
        //      변수 키워드의 경우를 처리하는 부분
        //      해당하는 크기만큼의 loc를 증가시킨다.
        if (operationCode == "WORD") {
            locCounter += 3;
        } else if (operationCode == "BYTE") {
            locCounter += calculateByteSize(operand);
        } else if (operationCode == "RESW") {
            locCounter += 3 * stoi(operand);
        } else if (operationCode == "RESB") {
            locCounter += stoi(operand);
        } else {
            // 변수 예약어가 아닌 경우 OPTAB을 확인하고 이것도 아니면 오류를 생성한다.
            // 오류코드 2 설정
        string opcode = findOpcode(operationCode);
            if (opcode.empty()) {
                cerr << "Error: Undefined operation code - " << operationCode << endl;
                line.setErrorCode(2);
            } else {
                locCounter += 3;
            }
        }
        
        lines.push_back(line);
    }
}

//패스 2 과정
void Assembler::pass2() {
    // lines를 가져와서 처리
    for (LINE& line : lines) {

        // 라인 정보 가져옴
        string operationCode = line.getOperation().getCode();
        string opcode = findOpcode(operationCode);
        string operand = line.getOperation().getText();
        // str,x 같은 경우의 컴마 이후의 것 제거
        size_t commaPos = operand.find(',');
        if (commaPos != string::npos) {
            operand = operand.substr(0, commaPos); // Pass 2에서는 ',' 이후 부분 제거
        }

        //objcode(OPCODE) 생성 
        //opration이 없는 경우 처리
        stringstream objCode;
        if (opcode.empty() && operationCode != "END") {
            line.setOpcode("      ");
            continue;
        }
        
        // opcode를 objcode에 먼저 삽입(앞 두자리)
        objCode << opcode;
        // operand가 없는 경우 뒤를 0으로 채움.
        // 있는 경우 해당 심볼이 저장된 symbol 테이블에서 값을 가져와 objcode를 생성함
        // 해당 심볼이 없는 경우 오류를 발생시킴 오류 코드 3 설정
        if (operand.empty()) {
            objCode << "0000";
        } else {
            if (symtab.find(operand) != symtab.end()) {
                objCode << setw(4) << setfill('0') << hex << symtab[operand];
            } else {
                cerr << "Error: Undefined symbol - " << operand << endl;
                line.setErrorCode(3);
                continue;
            }
        }

        line.setOpcode(objCode.str());
    }
}

void Assembler::writeAllObjectCode(ofstream& outputFile) {
    for (const LINE& line : lines) {
        writeObjectCode(outputFile, line);
    }
}

void Assembler::writeObjectCode(ofstream& outputFile, const LINE& line) {
    outputFile << "Loc: " << setw(4) << setfill('0') << hex << uppercase << line.getLoc() << ", "
               << "Label: " << left << setw(8) << setfill(' ') << line.getLabel() << ", "
               << "Operation: " << left << setw(6) << setfill(' ') << line.getOperation().getCode() << ", "
               << "Operand: " << left << setw(18) << setfill(' ') << line.getOperation().getText() << ", "
               << "Opcode: " << setw(6) << line.getOpcode() << endl;

    if (line.getErrorCode() == 2) {
        outputFile << "**undefined operation code**" << endl;
    } else if (line.getErrorCode() == 3) {
        outputFile << "**undefined symbol**" << endl;
    }
}

string Assembler::findOpcode(const string& operation) {
    auto it = optab.find(operation);
    return (it != optab.end()) ? it->second : "";
}

int Assembler::calculateByteSize(const string& operand) {
    if (operand[0] == 'C' && operand[1] == '\'') {
        return operand.length() - 3;
    } else if (operand[0] == 'X' && operand[1] == '\'') {
        return (operand.length() - 3) / 2;
    }
    return 0;
}

string Assembler::trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

int main() {
    ifstream srcFileStream("SRCFILE");
    if (!srcFileStream) {
        cerr << "Error opening SRCFILE." << endl;
        return 1;
    }

    vector<string> SRCFILE;
    string line;
    while (getline(srcFileStream, line)) {
        transform(line.begin(), line.end(), line.begin(), ::toupper);
        if (!line.empty()) {
            SRCFILE.push_back(line);
        }
    }
    srcFileStream.close();

    Assembler assembler;

    if (!assembler.loadOptab("optab.txt")) {
        cerr << "Error loading optab.txt." << endl;
        return 1;
    }

    cout << "Running pass1..." << endl;
    assembler.pass1(SRCFILE);

    cout << "Running pass2..." << endl;
    assembler.pass2();

    ofstream outputFile("object_code_output.txt");
    if (!outputFile) {
        cerr << "Error opening output file for writing object code." << endl;
        return 1;
    }
    assembler.writeAllObjectCode(outputFile);
    outputFile.close();

    cout << "Assembler pass completed. Check 'object_code_output.txt' for results." << endl;

    return 0;
}
