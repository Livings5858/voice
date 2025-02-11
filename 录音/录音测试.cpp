
#include<iostream>
#include <windows.h>
#include"mmsystem.h"
#include <SDKDDKVer.h>
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) //隐藏窗口
#pragma comment(lib, "winmm.lib") 
#define WIN32_LEAN_AND_MEAN
using namespace std;


//int _t(/*int argc, _TCHAR* argv[]*/);
HWAVEIN hWaveIn;  //输入设备  
WAVEFORMATEX waveform; //采集音频的格式，结构体  
BYTE *pBuffer1;//采集音频时的数据缓存  
WAVEHDR wHdr1; //采集音频时包含数据缓存的结构体  
FILE *pf;  
int main()  
{  
	int time = 8;
	string filename = "";
	char pName[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, pName, MAX_PATH);
	int iName = 0;
	for (; pName[iName] != '\0'; iName++);
	for (; pName[iName] != '\\'; iName--);
	for(int i=0;i<iName;i++)
	{
		filename.push_back(pName[i]);
	}
	filename += "\\02.pcm";
	//cout <<"文件存储路径： "<<filename << endl;
    HANDLE          wait;  
    waveform.wFormatTag = WAVE_FORMAT_PCM;//声音格式为PCM  
    waveform.nSamplesPerSec = 8000;//采样率，16000次/秒  
    waveform.wBitsPerSample = 16;//采样比特，16bits/次  
    waveform.nChannels = 1;//采样声道数，2声道  
    waveform.nAvgBytesPerSec = 16000;//每秒的数据率，就是每秒能采集多少字节的数据  
    waveform.nBlockAlign = 2;//一个块的大小，采样bit的字节数乘以声道数  
    waveform.cbSize = 0;//一般为0  
  
    wait = CreateEvent(NULL, 0, 0, NULL);  
    //使用waveInOpen函数开启音频采集  
    waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform,(DWORD_PTR)wait, 0L, CALLBACK_EVENT);  
  
    //建立两个数组（这里可以建立多个数组）用来缓冲音频数据  
    DWORD bufsize = 1024*100;//每次开辟10k的缓存存储录音数据  
    int r=fopen_s(&pf, filename.c_str(), "wb");
	if (r != 0)
	{
		//cout << "文件路径出现问题" << endl;
		return -1;
	}
    while (time--)//录制20左右秒声音，结合音频解码和网络传输可以修改为实时录音播放的机制以实现对讲功能  
    {  
        pBuffer1 = new BYTE[bufsize];  
        wHdr1.lpData = (LPSTR)pBuffer1;  
        wHdr1.dwBufferLength = bufsize;  
        wHdr1.dwBytesRecorded = 0;  
        wHdr1.dwUser = 0;  
        wHdr1.dwFlags = 0;  
        wHdr1.dwLoops = 1;  
        waveInPrepareHeader(hWaveIn, &wHdr1, sizeof(WAVEHDR));//准备一个波形数据块头用于录音  
        waveInAddBuffer(hWaveIn, &wHdr1, sizeof (WAVEHDR));//指定波形数据块为录音输入缓存  
        waveInStart(hWaveIn);//开始录音  
        Sleep(1000);//等待声音录制1s  
        waveInReset(hWaveIn);//停止录音  
        fwrite(pBuffer1, 1, wHdr1.dwBytesRecorded, pf);  
        delete pBuffer1; 
		//printf("%ds  ", time);
    }  
    fclose(pf);  
    waveInClose(hWaveIn);  
	//_t();
	return 0;  
}  

//char buf[1024 * 1024 * 4];  
//int _t(/*int argc, _TCHAR* argv[]*/) {  
//    FILE*           thbgm;//文件  
//    int             cnt;  
//    HWAVEOUT        hwo;  
//    WAVEHDR         wh;  
//    WAVEFORMATEX    wfx;  
//    HANDLE          wait;  
//  
//    wfx.wFormatTag = WAVE_FORMAT_PCM;//设置波形声音的格式  
//    wfx.nChannels = 1;//设置音频文件的通道数量  
//    wfx.nSamplesPerSec = 8000;//设置每个声道播放和记录时的样本频率  
//    wfx.nAvgBytesPerSec = 16000;//设置请求的平均数据传输率,单位byte/s。这个值对于创建缓冲大小是很有用的  
//    wfx.nBlockAlign = 2;//以字节为单位设置块对齐  
//    wfx.wBitsPerSample = 16;  
//    wfx.cbSize = 0;//额外信息的大小  
//    wait = CreateEvent(NULL, 0, 0, NULL);  
//    waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);//打开一个给定的波形音频输出装置来进行回放  
//    fopen_s(&thbgm, "01.pcm", "rb");  
//    cnt = fread(buf, sizeof(char), 1024 * 1024 * 4, thbgm);//读取文件4M的数据到内存来进行播放，通过这个部分的修改，增加线程可变成网络音频数据的实时传输。当然如果希望播放完整的音频文件，也是要在这里稍微改一改  
//    int dolenght = 0;  
//    int playsize = 1024;  
//    while (cnt) {//这一部分需要特别注意的是在循环回来之后不能花太长的时间去做读取数据之类的工作，不然在每个循环的间隙会有“哒哒”的噪音  
//        wh.lpData = buf + dolenght;  
//        wh.dwBufferLength = playsize;  
//        wh.dwFlags = 0L;  
//        wh.dwLoops = 1L;  
//        waveOutPrepareHeader(hwo, &wh, sizeof(WAVEHDR));//准备一个波形数据块用于播放  
//        waveOutWrite(hwo, &wh, sizeof(WAVEHDR));//在音频媒体中播放第二个函数wh指定的数据  
//        WaitForSingleObject(wait, INFINITE);//用来检测hHandle事件的信号状态，在某一线程中调用该函数时，线程暂时挂起，如果在挂起的INFINITE毫秒内，线程所等待的对象变为有信号状态，则该函数立即返回  
//        dolenght = dolenght + playsize;  
//        cnt = cnt - playsize;  
//    }  
//    waveOutClose(hwo);  
//    fclose(thbgm);  
//    return 0;  
//}  
    