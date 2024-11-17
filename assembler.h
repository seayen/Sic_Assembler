#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <map>
#include <vector>
#include "Line.h"

using namespace std;

class Assembler {
private:
    map<string, string> optab;  // op 테이블
    map<string, int> symtab;    // symbol 테이블
    vector<LINE> lines;         // 소스 프로그램의 각 LINE 객체들

public:

    // OPTAB.txt 파일을 로드하는 함수
    bool loadOptab(const string& filename);

    // pass1:  operation 체크 / symbol 테이블 작성
    void pass1(const vector<string>& srcFile);

    // pass2:  symbol 테이블 체크 / 목적 코드 생성
    void pass2();

    // OPTAB에서 opcode를 조회하는 메서드
    string findOpcode(const string& operation);
    // resw나 resb에서, c'~~' 의 상수 크기를 계산하는 함수
    int calculateByteSize(const string& operand);
    // srcfile 이 입력되면 공백 공간이 생성되는데, 이를 정리하는 함수
    string trim(const string& str);

    // text 파일 쓰기 함수
    void writeObjectCode(ofstream& outputFile, const LINE& line);
    void writeAllObjectCode(ofstream& outputFile);
};

#endif 
