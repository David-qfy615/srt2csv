#include <iostream>
#include <fstream>
#include <string>
#include <regex>
using namespace std;

int main() {
    string srtFileName = "input.srt";
    string csvFileName = "output.csv";

    ifstream srtFile(srtFileName);
    ofstream csvFile(csvFileName);

    string line;
    int subtitleCount = 0;
    while (getline(srtFile, line)) {
        subtitleCount++;

        // 解析字幕起始时间和结束时间
        regex timeRegex("\\d{2}:\\d{2}:\\d{2},\\d{3}\\s-->\\s\\d{2}:\\d{2}:\\d{2},\\d{3}");
        smatch timeMatch;
        if (regex_search(line, timeMatch, timeRegex)) {
            string startTimeStr = timeMatch.prefix().str();
            string endTimeStr = timeMatch.suffix().str();

            // 将时间格式转换成Nuendo可以识别的格式
            string startTime = startTimeStr.substr(0, 8) + ":" + startTimeStr.substr(9, 3);
            string endTime = endTimeStr.substr(0, 8) + ":" + endTimeStr.substr(9, 3);

            // 解析字幕内容
            getline(srtFile, line);
            string subtitle = line;

            // 将字幕项转换成csv格式
            string csvLine = to_string(subtitleCount) + "," + startTime + "," + endTime + "," + subtitle + "\n";

            // 写入csv文件
            csvFile << csvLine;
        }
    }

    srtFile.close();
    csvFile.close();

    cout << "Conversion complete." << endl;
    return 0;
}
