#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <windows.h>

using namespace std;

int main(int argc, char *infilename[])
{
    //设置控制台编码为UTF-8
    SetConsoleOutputCP(CP_UTF8);

    string line;
    string line2;
    string line3;
    string line0;
    int timeh;
    int num;

    //获取输入文件名称
    if (argc < 2)
    {
        std::cerr << "Usage: " << infilename[0] << " filename\n";
        return 1;
    }

    std::ifstream inFile(infilename[1]);
    if (!inFile)
    {
        std::cerr << "Failed to open file " << infilename[1] << "\n";
        return 1;
    }

    std::string filename(infilename[1]);                                   // 将 char* 类型的字符串转换为 std::string
    std::string basename = filename.substr(0, filename.find_last_of(".")); // 截取文件名，不包括扩展名

    //把输入的名称传给输出文件，并把文件后缀改为csv
    stringstream outfilename1;
    outfilename1 << basename << ".csv";
    string outfilename = outfilename1.str();
    ofstream outFile(outfilename);

    if (!outFile.is_open())
    {
        cout << "Unable to open output file." << endl;
        return 1;
    }

    outFile << char(0xEF) << char(0xBB) << char(0xBF); // 写入BOM，防止中文乱码

    outFile << "时码输入,"
            << "时码输出,"
            << "对话,"
            << "描述" << endl; //写入标题

    //开始文件内容转换
    while (getline(inFile, line))
    {
        // 如果是数字开头，且没有 "-->" 或 ":" 字符串，则跳过该行
        if (isdigit(line[0]) && line.find("-->") == string::npos && line.find(":") == string::npos && line.find(",") == string::npos)
        {
            continue;
        }

        // 在字幕内容格式转换
        if (isdigit(line[0]) && isdigit(line.back()))
        {

            istringstream ss(line);
            stringstream isstemp;
            stringstream osstemp;
            getline(ss, line0);
            size_t arrow_pos = line0.find(":");
            if (arrow_pos != string::npos)
            {
                //输入时间转换
                int itimeh = stoi(line.substr(0, arrow_pos));
                int itimemin = stoi(line.substr(arrow_pos + 1));
                int itimes = stoi(line.substr(arrow_pos + 4));
                int itimems = stoi(line.substr(arrow_pos + 7));

                int itime24 = itimems / 41; // 毫秒转换为24帧的帧数

                //输出时间转换  13 16 19 22      15 18 21 24
                int otimeh = stoi(line.substr(arrow_pos + 15));
                int otimemin = stoi(line.substr(arrow_pos + 18));
                int otimes = stoi(line.substr(arrow_pos + 21));
                int otimems = stoi(line.substr(arrow_pos + 24));

                int otime24 = otimems / 41; // 毫秒转换为24帧的帧数

                //  cout << "timeh: " << timeh << endl;
                //   cout << "timemin: " << timemin << endl;
                //  cout << "times: " << times << endl;
                //  cout << "timems: " << timems << endl;

                isstemp << setw(2) << setfill('0') << itimeh << ":" << setw(2) << setfill('0') << itimemin << ":" << setw(2) << setfill('0') << itimes << ":" << setw(2) << setfill('0') << itime24 << ",";
                string timelin = isstemp.str();

                osstemp << setw(2) << setfill('0') << otimeh << ":" << setw(2) << setfill('0') << otimemin << ":" << setw(2) << setfill('0') << otimes << ":" << setw(2) << setfill('0') << otime24 << ",";
                string timeout = osstemp.str();

                // cout << "输入时间" << timelin << "输出时间" << timeout << endl;  //测试帧数转换

                line = timelin + timeout;
            }

            else
            {
                cerr << "Input line does not contain --> symbol." << endl;
            }

            line2 = line;

            // line = "" ;
            cout << line << endl;
            continue;
        }

        if (!isdigit(line[0]) && !isdigit(line.back()))
        {
            line3 = line2 + line;
        }

        // 跳过纯时间码的行和其他非正常内容的行

        if (line3.length() < 3)
        {
            continue;
        }

        if (isdigit(line3[0]) && line3.length() < 26)
        {
            continue;
        }

        cout << line3 << endl;

        // 写入输出文件
        outFile << line3 << endl;
    }

    inFile.close();
    outFile.close();
    cout << "转换成功" << endl;
    system("pause");
    // cin.get();
    // cout << "程序结束" << endl;
    return 0;
}
