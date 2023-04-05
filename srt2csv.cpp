#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <windows.h>

using namespace std;

int main()
{
    //设置控制台编码为UTF-8
    SetConsoleOutputCP(CP_UTF8);

    string line;
    string line2;
    string line3;
    string line0;
    int timeh;
    int num;
    ifstream inFile("input.srt");
    ofstream outFile("out.csv");

    if (!inFile.is_open())
    {
        cout << "Unable to open input file." << endl;
        return 1;
    }

    if (!outFile.is_open())
    {
        cout << "Unable to open output file." << endl;
        return 1;
    }

    outFile << char(0xEF) << char(0xBB) << char(0xBF); // 写入BOM，防止中文乱码

    outFile << "时码输入,"
            << "时码输出,"
            << "对话" << endl; //写入标题

    while (getline(inFile, line))
    {
        // 如果是数字开头，且没有 "-->" 或 ":" 字符串，则跳过该行
        if (isdigit(line[0]) && line.find("-->") == string::npos && line.find(":") == string::npos)
        {
            continue;
        }

        // 替换所有的逗号为冒号
        size_t pos = 0;
        while ((pos = line.find(",", pos)) != string::npos)
        {
            line.replace(pos, 1, ":");

            pos++;
            // cout<<line<<endl;
        }

        // 替换所有的时间轴分隔符为一个空格//可以优化，尝试留空，减少空格
        while ((pos = line.find("-")) != string::npos)
        {
            line.replace(pos, 1, "");
        }

        // 替换所有的大于号为逗号
        while ((pos = line.find(">")) != string::npos)
        {
            line.replace(pos, 1, ",");
        }

        // 在字幕内容前面添加一个逗号 //需要修改，字幕内容也需要加“,”，
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

                //输出时间转换
                int otimeh = stoi(line.substr(arrow_pos + 13));
                int otimemin = stoi(line.substr(arrow_pos + 16));
                int otimes = stoi(line.substr(arrow_pos + 19));
                int otimems = stoi(line.substr(arrow_pos + 22));

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
            continue;
            // cout<< line <<endl ;
        }

        if (!isdigit(line[0]) && !isdigit(line.back()))
        {
            line3 = line2 + line;
        }

        // 跳过纯时间码的行
        if (isdigit(line3[0]) && line3.length() < 29)
        {
            continue;
        }

        // cout << line3 << endl;

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
