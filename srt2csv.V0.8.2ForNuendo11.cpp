#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <vector>

using namespace std;


// SRT字幕中单条字幕的结构体
struct SrtSubtitle {
    string actor;                // 角色
    int index;                // 字幕序号
    string startTime;    // 显示开始时间
    string endTime;      // 显示结束时间
    vector<string> textLines; // 字幕文本，支持多行

    // 构造函数
    SrtSubtitle(const string& actor,int idx, const string& start, const string& end, const vector<string>& text)
        : index(idx), startTime(start), endTime(end), textLines(text) {}
};

int inputframes = 24 ; //定义每秒24帧，后面需要改成可以自定义的

// 定义 转换时间单位毫秒到24帧数 的函数
string convertTimeToFrames(const string& srtTime) {
    int hours, minutes, seconds, milliseconds;
    char discard;
    istringstream iss(srtTime);
    iss >> hours >> discard >> minutes >> discard >> seconds >> discard >> milliseconds;

    // 将毫秒转换为帧，假设每秒24帧
    int frames = static_cast<int>((milliseconds / 1000.0) * inputframes);
    ostringstream oss;
    // 使用setw和setfill来确保时间格式正确（例如，保持小时、分钟和秒数始终为两位数字）
    oss << setw(2) << setfill('0') << hours << ":"
        << setw(2) << minutes << ":"
        << setw(2) << seconds << ":"
        << setw(2) << frames;
    return oss.str();
}


//读取字幕文件
bool parseSrtSubtitle(ifstream& inFile, SrtSubtitle& subtitle) {
    string line;
    string actor; 
    int index;
    string startTime, endTime;
    vector<string> textLines;
    
    // 跳过开始的任何空行
    while (getline(inFile, line) && line.empty()) {
        // Intentionally left blank to skip over any initial empty lines
    }

    // 如果直接到达文件末尾，则返回false
    if (inFile.eof()) {
        return false;
    }

/*
    // 从第一行读取序号
    try {
        index = stoi(line); // 将读取的行转换为序号
    } catch (const invalid_argument& ia) {         
        cerr << "无效的序号: " << line << endl;
        return false;
    }
*/

    // 读取时间戳
    if (!getline(inFile, line)) return false;
    istringstream iss(line);
    getline(iss, startTime, ' '); // 读取开始时间
    iss.ignore(5, '>'); // 忽略 " --> "
    getline(iss, endTime); // 读取结束时间
    
    // 读取字幕文本
    while (getline(inFile, line) && !line.empty()) {
        textLines.push_back(line);
    }

    subtitle = SrtSubtitle(actor, index, startTime, endTime, textLines);
    return true;
}


// 主函数
int main(int argc, char* argv[]) {
    // 设置控制台编码为UTF-8
    SetConsoleOutputCP(CP_UTF8);

    // 获取输入文件名称
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " filename\n";
        return 1;
    }

    ifstream inFile(argv[1]);
    if (!inFile) {
        cerr << "Failed to open file " << argv[1] << "\n";
        return 1;
    }

    string filename(argv[1]);
    string basename = filename.substr(0, filename.find_last_of("."));

    // 定义输出文件名称
    stringstream outfilename;
    outfilename << basename << ".csv";
    ofstream outFile(outfilename.str());

    if (!outFile) {
        cout << "Unable to open output file." << endl;
        return 1;
    }

    // 写入bom和标题
    outFile << char(0xEF) << char(0xBB) << char(0xBF); // 写入BOM，防止中文乱码
    //outFile << "ID,时码输入,时码输出,对话,描述,角色,配音员" << endl; // 写入标题
    outFile << "角色,时码输入,时码输出,对话,翻译,描述,配音员" << endl; // 写入标题

   
    SrtSubtitle subtitle("actor",0, "", "", {}); //初始化单行字幕结构

    // 执行帧数转换，并写入csv文件
    while (parseSrtSubtitle(inFile, subtitle)) {
        // 将时间转换为帧
        string startTime = convertTimeToFrames(subtitle.startTime);
        string endTime = convertTimeToFrames(subtitle.endTime);

        string actor = subtitle.actor; //演员名称传递过来

        //需要修改这里的多行字幕输出，有的可以，又的不行
        stringstream dialog;
        for (const auto& line : subtitle.textLines) {
            if (dialog.str().size() > 0) dialog << " "; // 添加空格分隔多行对话
            dialog << line;
        }
        //outFile << subtitle.index << "," << startTime << "," << endTime << ",\"" << dialog.str() << "\"" << endl;  
        outFile << actor << "," << startTime << "," << endTime << ",\"" << dialog.str() << "\"" << endl;  
    }

    inFile.close();
    outFile.close();
    cout << "转换成功" << endl;
    system("pause");

    return 0;
}
