#ifndef LINE_H
#define LINE_H

#include <string>

using namespace std; // std:: 생략을 위해 추가

// OPERATION 클래스 정의
class OPERATION {
private:
    string code;   // 코드 데이터
    string text;   // 텍스트 데이터

public:
    OPERATION(const string& codeData = "", const string& textData = "") 
        : code(codeData), text(textData) {}

    // Getter와 Setter 함수들
    string getCode() const { return code; }
    string getText() const { return text; }
    void setCode(const string& codeData) { code = codeData; }
    void setText(const string& textData) { text = textData; }
};

// LINE 클래스 정의
class LINE {
private:
    string label;
    OPERATION operation; // OPERATION 객체
    string operand; 
    int loc;
    string opcode;
    int errorCode; // 오류 코드를 저장하는 변수

public:
    // 생성자
    LINE(const string& labelData, const string& operationCode, const string& operandData, int startLoc)
    : label(labelData), operation(operationCode, operandData), operand(operandData), loc(startLoc), errorCode(0) {}

    // Getter와 Setter 함수들
    string getLabel() const { return label; }
    OPERATION getOperation() const { return operation; }
    string getOperand() const { return operand; }
    int getLoc() const { return loc; }
    string getOpcode() const { return opcode; }
    int getErrorCode() const { return errorCode; }

    void setLabel(const string& lbl) { label = lbl; }
    void setOperation(const OPERATION& op) { operation = op; }
    void setOperand(const string& opnd) { operand = opnd; }
    void setLoc(int location) { loc = location; }
    void setOpcode(const string& opcd) { opcode = opcd; }
    void setErrorCode(int code) { errorCode = code; }
};

#endif // LINE_H
