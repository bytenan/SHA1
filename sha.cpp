#include <iostream>
#include <string>
#include <cstring>

unsigned int BASIC_H[] = { 0x67452301,0xefcdab89,0x98badcfe,0x10325476,0xc3d2e1f0 };
unsigned char *message = nullptr;
size_t message_length = 0;

void expandWord (const unsigned char *group, unsigned int *W) 
{
    for (int i = 0; i < 16; i++)
    {
        int j = 4 * i;
        W[i] = ((unsigned int)group[j]) << 24     | 
               ((unsigned int)group[1 + j]) << 16 | 
               ((unsigned int)group[2 + j]) << 8  |
               ((unsigned int)group[3 + j]) << 0;
    }
    for (int i = 16; i < 80; i++)
    {
        W[i] = W[i - 16] ^ W[i - 14] ^ W[i - 8] ^ W[i - 3];
        W[i] = (W[i] << 1) | (W[i] >> 31);
    }
}

void fillMessage(const std::string &input) {
    message_length = ((input.size() + 8) / 64) * 64 + 56 + 8;
    message = (unsigned char *)malloc(message_length);
    memset(message, 0x00, message_length);
    memcpy(message, input.c_str(), input.size());// 将输入填充进消息
    message[message_length] = 0x80;// 将1填充进消息

    unsigned char input_length[8];
    memset(input_length, 0x00, 8);
    unsigned int temp_length = (unsigned int)(input.size() * 8);
    memcpy(input_length, &temp_length, sizeof(unsigned long));

    // 颠倒数据长度存储的端序
    for (int i = 7; i >= 4; --i) {
      int temp = input_length[i];
      input_length[i] = input_length[7-i];
      input_length[7 - i] = temp;
    }
    memcpy(message + (message_length - 8), input_length, 8);// 将长度填充进消息
}

void groupHandler() {
    // 每次循环都是以一个分组(64byte)为单位
    for (int i = 0; i < message_length; i += 64)
    {
        //A=H[0],B=H[1],C=H[2],D=H[3],E=H[4]
        unsigned int H[5] = { 0 };// 五个缓冲区
        unsigned char group[64] = { 0 };// 一个分组
        unsigned int W[80] = { 0 };// 用来将分组中的16个字拓展成80个字

        memcpy(H, BASIC_H, 5 * sizeof(unsigned int));// 用基础缓冲区赋值给工作缓冲区
        memcpy(group, (message + i), 64);// 从消息中取出分组
        expandWord(group, W);// 拓展字函数

        int K = 0;// 加法常量
        int f = 0;// 基本逻辑函数“值”
        for (int t = 0; t < 80; ++t)
        {
            switch ((int)t / 20)
            {
            case 0:
                K = 0x5a827999;
                f = (H[1] & H[2]) | ((~H[1]) & H[3]);
                break;
            case 1:
                K = 0x6ed9eba1;
                f = H[1] ^ H[2] ^ H[3];
                break;
            case 2:
                K = 0x8f1bbcdc;
                f = (H[1] & H[2]) | (H[1] & H[3]) | (H[2] & H[3]);
                break;
            case 3:
                K = 0xca62c1d6;
                f = H[1] ^ H[2] ^ H[3];
                break;
            default:
                break;
            }
            //ABCDE位置移动
            int temp_A = ((H[0] << 5) | (H[0] >> 27)) + f + H[4] + W[t] + K;
            H[4] = H[3];
            H[3] = H[2];
            H[2] = (H[1] << 30) | (H[1] >> 2);
            H[1] = H[0];
            H[0] = temp_A;
        }
        //ABCDE累加
        for (int j = 0; j < 5; ++j) {
            BASIC_H[j] += H[j];
        }
    }   
}

unsigned int *getHashDigest() {
    unsigned int *result = (unsigned int *)malloc(5 * sizeof(unsigned int));
    for (int i = 0; i < 5; ++i) {
        *(result + i) |= BASIC_H[i];
    }
    return result;
}

int main() {
    std::cout << "Please input# ";
    std::string input;
    std::cin >> input;

    fillMessage(input);
    groupHandler();
    std::cout << "HashDigest--> " << getHashDigest() << std::endl;

    return 0;
}